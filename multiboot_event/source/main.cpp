/****************************************************************************
 * Multiboot Script Loader
 * KittyPBoxx 2024
 *
 * Based on slipstream/RoL's Multiboot PoC and FIX94's multiboot loader
 * Copyright (c) 2017 slipstream/RoL
 * Copyright (C) 2016 FIX94
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 *
 * main.c
 * This code pretends to be colosseum, sending over a multiboot image 
 * during the copyright screen. 
 ***************************************************************************/

#include <gccore.h>
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "gba_mb_gba.h"

#include "FreeTypeGX.h"
#include "video.h"
#include "audio.h"
#include "menu.h"
#include "input.h"
#include "filelist.h"
#include "main.h"
#include "gettext.h"

extern "C" {
	#include "seriallink.h"
	#include "pokekeys.h"
	#include "log.h"
	#include "linkcableclient.h"
}

#define USE_UI TRUE

int ExitRequested = 0;

void ExitApp()
{
	VIDEO_SetBlack(true);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	VIDEO_WaitVSync();
	ShutdownAudio();
	StopGX();
	SYS_ResetSystem(SYS_HOTRESET, 0, 0);
}

#define GAMECUBE_PORT1 0
#define GAMECUBE_PORT2 1

#define SI_STATUS_CONNECTED 0x10

// Session Key Algorithm used by both the game and the GBA BIOS multiboot: 
// sessionkey = (initialvalue * 0x6177614b) + 1
#define MB_SESSION_KEY_MULTIPLIER 0x6177614b

extern const uint8_t gba_mb_gba[];
extern const size_t gba_mb_gba_size;

static	lwp_t serial_thread_handle = (lwp_t)LWP_THREAD_NULL;

static void *doGBALink (void * port);


enum {
	LINK_STATE_INIT,                   // Reseting variables before looking for a connection 
    LINK_STATE_SEARCHING_FOR_GBA,      // No GBA has been detected, the device needs to be connected while powered off, then switched on 
	LINK_STATE_IN_BIOS,                // The GBA has been detected, it is currently displaying the bios boot screen
	LINK_STATE_RECEIVE_GAME_CODE,      // The cartrige has loaded onto the copywrite screen where the communication starts, here a device reset command must be sent or GameCubeMultiBoot_Init will terminate early. On reset the game will send it's code, Note that Emerald (U) sends "AXVE" (Pokemon Ruby's rom code), rather than it's code (BPEE)
	LINK_STATE_TRADE_KEYS,             // Now we have the code we need to send it back followed by our generated KeyA. The game should respond with KeyB, which we validate and generate the CRC and session key
	LINK_STATE_SEND_MB_HEADER,         // Multiboot rom header is sent and the game validates the nintendo logo is correct before accepting more data
	LINK_STATE_SEND_MB_ROM,            // The rest of the multiboot rom is sent over
	LINK_STATE_VALIDATE_MB_CHECKSUM,   // The game sends back a KeyC value (based on the rom data it got), we use that to derive a bootkey.
	LINK_STATE_SEND_BOOT_KEY,          // We send the bootkey back. If the bootkey matches what the game thinks it should be the mb rom is loaded. If not the game jumps to the intro sequence.  
	LINK_STATE_FINISHED,
	LINK_STATE_ERROR,
};

int main(int argc, char **argv) 
{
	(void)argc;
	(void)argv;

	InitVideo(USE_UI); // Initialize video
	InitAudio(); // Initialize audio
	InitFreeType((u8*)font_ttf, font_ttf_size); // Initialize font system
	InitGUIThreads(); // Initialize GUI
	LoadLanguage(); // Load localised text

	PAD_Init();

	// Start a new thread to handle the GBA serial communication on a specific gamecube controller port
	u8 *port = (u8 *) malloc(sizeof(*port));
	*port = GAMECUBE_PORT2;
	LWP_CreateThread(&serial_thread_handle,	        /* thread handle */
			         doGBALink,                     /* code */
			         (void *) port,		            /* arg pointer for thread */
			         NULL,			                /* stack base */
			         16*1024,		                /* stack size */
			         250          			        /* thread priority */ );


	// On the main thread listen 
	if (USE_UI)
	{
		MainMenu();
	}
	else
	{
		printf ("Starting Channel\nSTART/HOME = Reset Cube \nZ/+ = Reset GBA Connection \n");
		while(1)
		{
			VIDEO_WaitVSync();
			PAD_ScanPads();

			int buttonsHeld = PAD_ButtonsHeld(GAMECUBE_PORT1);
			
			if (buttonsHeld & PAD_BUTTON_START) 
			{
				ExitApp();
			}
			else if (buttonsHeld & PAD_TRIGGER_Z) 
			{
				resetLink();
			}

			#ifdef TARGET_WII
				WPAD_ScanPads();

				int buttonsDown = WPAD_ButtonsDown(0);

				if (buttonsDown & WPAD_BUTTON_HOME) 
				{
					ExitApp();
				}
				else if (buttonsHeld & WPAD_BUTTON_PLUS) 
				{
					resetLink();
				}
	
			#endif
		}
	}

	return 0;				 
}


static void *doGBALink (void * portPtr)
{
	const int sendsize = ((gba_mb_gba_size+7)&~7);
	const u32 hackedupsize = (sendsize >> 3) - 1;

	int port = *(u8 *) portPtr;
	int commResult = 0;
	int connectionState = LINK_STATE_INIT;
	
	u8 pkt[4];
	u32 keyA = 0;
	u32 keyB = 0;
	u32 keyC = 0;
	u32 gamecode = 0;
	u32 sessionkey = 0;
	u32 kcrc = 0;

	while(1)
	{
		if (connectionState == LINK_STATE_INIT)
		{
			commResult = 0;
			connectionState = LINK_STATE_SEARCHING_FOR_GBA;

			pkt[0] = 0; pkt[1] = 0; pkt[2] = 0; pkt[3] = 0;	
			keyA = genKeyA();
			keyB = 0;
			keyC = 0;
			gamecode = 0;
			sessionkey = 0;
			kcrc = 0;
			
			printf("Waiting for a GBA in port %x...\n", port + 1);
		}

		while (connectionState == LINK_STATE_SEARCHING_FOR_GBA)
		{
			SL_reset(port, pkt);
			SL_resetDeviceType(port);
			usleep(10000);
			commResult = SL_getstatus(port, pkt);
			SI_GetTypeAsync(port, SL_getDeviceTypeCallback(port));
			usleep(10000);

			if (commResult < 0)
			{
				connectionState = LINK_STATE_ERROR;
			}
            else if (pkt[2] & SI_STATUS_CONNECTED && SL_getDeviceType(port) & SI_GBA)
            {
				printf("GBA Found! Waiting on BIOS\n");
				connectionState = LINK_STATE_IN_BIOS; 
				playerConnected();
            }
            else
            {
                VIDEO_WaitVSync();
            }

		} 


		while (connectionState == LINK_STATE_IN_BIOS)
		{
			SL_reset(port, pkt);
			usleep(10000);

			if (pkt[1] != 4) 
			{
				printf("BIOS handed over to game, waiting on game\n");
				connectionState = LINK_STATE_RECEIVE_GAME_CODE;

				// Even through the game has now started. If we try and interact too quickly we can get issues
				usleep(2000000);
				printf("Starting handshake...\n");
			}
            else
            {
                VIDEO_WaitVSync();
            }

		} 

		while (connectionState == LINK_STATE_RECEIVE_GAME_CODE)
		{
			SL_reset(port, pkt);

			if ((pkt[0] != 0) || !(pkt[2]&0x10)) 
			{
				SL_recv(port, pkt);
				gamecode = (pkt[0] << 24) | (pkt[1] << 16) | (pkt[2] << 8) | pkt[3];

				if (pkt[0] == 0 || pkt[1] == 0 || pkt[2] == 0 || pkt[3] == 0)
				{
					printf("Game code invalid, if emulating make sure the bios screen is enabled.\n");
					connectionState = LINK_STATE_ERROR;
				}
				else 
				{
					printf("Game code: %c%c%c%c\n", (char) pkt[0], (char) pkt[1], (char) pkt[2], (char) pkt[3]);    
					connectionState = LINK_STATE_TRADE_KEYS;
				}
			}
            else
            {
				usleep(10000);
                VIDEO_WaitVSync();
            }
		} 



		if (connectionState == LINK_STATE_TRADE_KEYS) 
		{
			// send the game code back, then KeyA.
			SL_send(port, __builtin_bswap32(gamecode));
			SL_send(port, keyA);
			
		}

		while (connectionState == LINK_STATE_TRADE_KEYS)
		{
			SL_recv(port, pkt);
			keyB = (pkt[0] << 24) | (pkt[1] << 16) | (pkt[2] << 8) | pkt[3];

			if (keyB != gamecode)
			{
				// get KeyB from GBA, check it to make sure its valid 
				keyB = checkKeyB(__builtin_bswap32(keyB));
				if (!keyB)
				{
					connectionState = LINK_STATE_ERROR;
				}
				else 
				{
					//  xor with KeyA to derive the initial CRC value and the sessionkey
					sessionkey = keyB ^ keyA;
					kcrc = sessionkey;
					printf("start kCRC=%08x\n",kcrc);
					sessionkey = (sessionkey * MB_SESSION_KEY_MULTIPLIER) +1;
					connectionState = LINK_STATE_SEND_MB_HEADER;
				}
				
			}
			else 
			{
				usleep(10000);
                VIDEO_WaitVSync();
			}

		}

		if (connectionState == LINK_STATE_SEND_MB_HEADER) 
		{
			// send hacked up send-size in uint32s
			printf("Sending hacked up size 0x%08x\n",hackedupsize);
			SL_send(port, hackedupsize);
			usleep(10000);
			//unsigned int fcrc = 0x00bb;
			// send over multiboot binary header, in the clear until the end of the nintendo logo.
			// GBA checks this, if nintendo logo does not match the one in currently inserted cart's ROM, it will not accept any more data.
			for(int i = 0; i < 0xA0; i+=4) 
			{
				vu32 rom_dword = *(vu32*)(gba_mb_gba+i);
				SL_send(port, __builtin_bswap32(rom_dword));
				usleep(2000);
			}
			printf("\n");
			connectionState = LINK_STATE_SEND_MB_ROM;
		}

		if (connectionState == LINK_STATE_SEND_MB_ROM) 
		{	
			printf("Header done! Sending ROM...\n");
			// Add each uint32 of the multiboot image to the checksum, encrypt the uint32 with the session key, increment the session key, send the encrypted uint32.
			for(int i = 0xA0; i < sendsize; i+=4)
			{
				u32 dec = (
					(((gba_mb_gba[i+3]) << 24) & 0xff000000)|
					(((gba_mb_gba[i+2]) << 16) & 0x00ff0000)|
					(((gba_mb_gba[i+1]) << 8)  & 0x0000ff00)|
					(((gba_mb_gba[i])   << 0)  & 0x000000ff)
				);
				u32 enc = (dec - kcrc) ^ sessionkey;
				kcrc = docrc(kcrc, dec);
				sessionkey = (sessionkey*0x6177614B)+1;
				//enc^=((~(i+(0x20<<20)))+1);
				//enc^=0x6f646573;//0x20796220;
				SL_send(port, enc);
				usleep(2000);
			}
			//fcrc |= (sendsize<<16);
			printf("ROM done! CRC: %08x\n", kcrc);		

			connectionState = LINK_STATE_VALIDATE_MB_CHECKSUM;
		}

		while (connectionState == LINK_STATE_VALIDATE_MB_CHECKSUM)
		{
			//get crc back (unused)
			// Get KeyC derivation material from GBA (eventually)
			u32 keyCderive = 0;
			SL_recv(port, pkt);
			keyCderive = (pkt[0] << 24) | (pkt[1] << 16) | (pkt[2] << 8) | pkt[3];
			
			if (keyCderive <= 0xfeffffff)
			{
				usleep(1000);
                VIDEO_WaitVSync();
			}
			else 
			{
				keyCderive = __builtin_bswap32(keyCderive);
				keyCderive >>= 8;
				printf("KeyC derivation material: %08x\n",keyCderive);
				// (try to) find the KeyC, using the checksum of the multiboot image, and the derivation material that GBA sent to us
				keyC = deriveKeyC(keyCderive, kcrc);
				if (keyC == 0) printf("Could not find keyC - kcrc=0x%08x\n",kcrc);
				connectionState = LINK_STATE_SEND_BOOT_KEY;
			}

		}
		
		if (connectionState == LINK_STATE_SEND_BOOT_KEY)
		{
			// derive the boot key from the found KeyC, and send to GBA. if this is not correct, GBA will not jump to the multiboot image it was sent. 
			u32 bootkey = docrc(0xBB,keyC) | 0xbb000000;
			printf("BootKey = 0x%08x\n",bootkey);
			SL_send(port, bootkey);
		}

		if (connectionState == LINK_STATE_ERROR)
		{
			printf("Something went wrong! Press A to restart or START to exit.\n");	
		}
		else 
		{
			printf("Done! Press 'Z' or '+' to restart\nPress 'START' or 'HOME' to exit.\n");
		}

		connectionState = LINK_STATE_FINISHED;

		usleep(80000);
        VIDEO_WaitVSync();
		handeConnectionLoop(port);
		connectionState = LINK_STATE_INIT;
	}

	return NULL;

}
