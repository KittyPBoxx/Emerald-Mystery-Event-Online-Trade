/*
 * Example Gen3-multiboot payload by slipstream/RoL 2017.
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 *
 * main.c: setup, call payload, return gracefully back to game
 */

#include <gba.h>
#include <stddef.h>
#include "payload.h"
#include "generated_symbols.h" 
#include "symbols.h"

#define OFFSET_IN_AGB_MAIN_TO_SKIP_RAM_RESET 0x86
#define OFFSET_IN_RELOAD_SAVE_TO_SKIP_EWRAM_RESET 0x54

// MEM
#define ROM_OFFSET 0x8000000
#define ROM_CODE_OFFSET 0xAC
#define IWRAM_OFFSET 0x02000000
#define MB_HEADER_SIZE 0xE0
#define MB_PADDING_SIZE 0x2000

// GRPAHICS
#define VRAM_SIZE         0x18000
#define BG_VRAM           VRAM
#define BG_VRAM_SIZE      0x10000
#define BG_CHAR_SIZE      0x4000
#define BG_SCREEN_SIZE    0x800
#define BG_CHAR_ADDR(n)   (BG_VRAM + (BG_CHAR_SIZE * (n)))
#define BG_SCREEN_ADDR(n) (BG_VRAM + (BG_SCREEN_SIZE * (n)))
#define PLTT_ID(n) ((n) * 16)
#define BG_PLTT_OFFSET 0x000
#define OBJ_PLTT_OFFSET 0x100
#define BG_PLTT_ID(n) (BG_PLTT_OFFSET + PLTT_ID(n))

// SERIAL
#define REG_OFFSET_TMCNT_H     0x102
#define REG_ADDR_TMCNT_H     (REG_BASE + REG_OFFSET_TMCNT_H)
#define REG_TMCNT_H(n)  (*(vu16 *)(REG_ADDR_TMCNT_H + ((n) * 4)))
#define INTR_FLAG_VBLANK  (1 <<  0)
#define INTR_FLAG_VCOUNT  (1 <<  2)
#define INTR_FLAG_TIMER3  (1 <<  6)
#define INTR_FLAG_SERIAL  (1 <<  7)
#define SIO_MULTI_MODE     0x2000 
#define REG_OFFSET_JOYCNT      0x140
#define REG_OFFSET_JOYSTAT     0x158
#define REG_OFFSET_JOY_RECV    0x150
#define REG_OFFSET_JOY_TRANS   0x154
#define REG_ADDR_JOY_TRANS   (REG_BASE + REG_OFFSET_JOY_TRANS)
#define REG_ADDR_JOYCNT      (REG_BASE + REG_OFFSET_JOYCNT)
#define REG_ADDR_JOYSTAT     (REG_BASE + REG_OFFSET_JOYSTAT)
#define REG_ADDR_JOY_RECV    (REG_BASE + REG_OFFSET_JOY_RECV)
#define JOY_CNT         (*(vu16 *)REG_ADDR_JOYCNT)
#define JOY_TRANS	    (*(vu32*)(REG_ADDR_JOY_TRANS))
#define JOY_RECV        (*(vu32 *)REG_ADDR_JOY_RECV) 

// NETWORK
#define MAX_CONNECTION_LOOPS 5000
#define SEND_DATA_TO_GC_BUFFER 0x15
#define POST_GC_BUFFER_TO_SERVER 0x13
#define RECV_DATA_FROM_GC_BUFFER 0x25
#define NET_CMD_HEADER(cmd, buf) (((cmd) << 8) | (buf))
#define NET_CONN_CHCK_RES 0x1101 // Returning check bytes for the last data sent 
#define NET_CONN_PINF_REQ 0x1201 // Tell wii to use current data as player info 
#define NET_CONN_CINF_REQ 0x1202 // Tell wii to use current data as server info 
#define NET_CONN_LIFN_REQ 0x2005 // Return information about this devices network connection 
#define NET_SERVER_ADDR_LENGTH 14
#define JOY_WRITE 0x2
#define JOY_READ  0x4
#define JOY_RW    0x6
#define TRAINER_INFO_LENGTH 14 // Player name + 1 + Gender + Special Warp Flag + Trainer ID)
#define SERIAL_MODE_SEND 0 
#define SERIAL_MODE_RECV 1
#define SERIAL_VERIFY_ON 0 
#define SERIAL_VERIFY_OFF 1
#define CHUNK_SIZE 16

#define TX(c) \
    ((c) == '0' ? 0xA1 : \
    (c) == '1' ? 0xA2 : \
    (c) == '2' ? 0xA3 : \
    (c) == '3' ? 0xA4 : \
    (c) == '4' ? 0xA5 : \
    (c) == '5' ? 0xA6 : \
    (c) == '6' ? 0xA7 : \
    (c) == '7' ? 0xA8 : \
    (c) == '8' ? 0xA9 : \
    (c) == '9' ? 0xAA : \
	(c) == 'B' ? 0xBC : \
	(c) == 'P' ? 0xCA : \
	(c) == 'E' ? 0xBF : \
    (c) == ':' ? 0xF0 : \
    (c) == '.' ? 0xAD : 0xFF)

// ROM FUNCS - If a standard function is present in the rom we can (and should) use that to save space
typedef void (*RunTasksFunc)(void);
typedef void (*CB2_InitTitleScreen)(void);
typedef void (*UpdatePaletteFade)(void);
typedef void (*m4aSoundInit)(void);
typedef void (*LZDecompressVram)(u32 *src, void *dest);
typedef void (*LoadPalette)(void *src, u16 offset, u16 size);
typedef void (*PlaySE)(u16 songNum);
typedef void (*EnableInterrupts)(u16 mask);
typedef void (*DisableInterrupts)(u16 mask);
typedef void *(*MemSet)(void *s, int c, size_t n);

void call_into_middle_of_titlescreen_func(u32 addr,u32 stackspace);

void decrypt_save_structures(pSaveBlock1 SaveBlock1,pSaveBlock2 SaveBlock2,pSaveBlock3 SaveBlock3) {
	if (GAME_RS || GAME_FRLG) {
		// R/S doesn't have save crypto.
		// We arn't supporting FRLG here
		return;
	}
	u8* sb1raw = (u8*)SaveBlock1;
	u8* sb2raw = (u8*)SaveBlock2;
	//u8* sb3raw = (u8*)SaveBlock3; // unused
	
	u32* xor_key_ptr = (u32*)(&sb2raw[( GAME_EM ? 0xA8 : 0xF20 )]);
	
	u32 xor_key = *xor_key_ptr;
	u16 xor_key16 = (u16)xor_key;
	if (!xor_key) {
		// xor key is zero, nothing needs to be done.
		return;
	}
	
	u32* ptr_to_xor;
	u32 save_offset;
	int i;
	u32* bag_pocket_offsets;
	u32* bag_pocket_counts;

	// loop over and decrypt various things
	for (i = 0; i <= 0x3f; i++) {
		save_offset = 0x159c + (i*sizeof(u32));
		ptr_to_xor = (u32*)(&sb1raw[save_offset]);
		*ptr_to_xor ^= xor_key;
	}
	// loop over each of the bag pockets and decrypt decrypt decrypt
	bag_pocket_offsets = (u32[5]) { 0x560, 0x5D8, 0x650, 0x690, 0x790 };
	bag_pocket_counts = (u32[5]) { 30, 30, 16, 64, 46 };
	for (i = 0; i < 5; i++) {
		for (int bag_i = 0; bag_i < bag_pocket_counts[i]; bag_i++) {
			save_offset = bag_pocket_offsets[i] + (sizeof(u32) * bag_i) + 2;
			*(u16*)(&sb1raw[save_offset]) ^= xor_key16;
		}
	}
	// decrypt some more stuff
	save_offset = 0x1F4;
	ptr_to_xor = (u32*)(&sb1raw[save_offset]);
	*ptr_to_xor ^= xor_key;
	save_offset = 0x490;
	ptr_to_xor = (u32*)(&sb1raw[save_offset]);
	*ptr_to_xor ^= xor_key;
	save_offset = 0x494;
	*(u16*)(&sb1raw[save_offset]) ^= xor_key16;
	
	*xor_key_ptr = 0;
	return;
	
}

void drawNetworkLoadingScreen()
{
	RunTasksFunc RunTasks = (RunTasksFunc)ROM_RunTasks;
	UpdatePaletteFade updatePaletteFade = (UpdatePaletteFade)ROM_UpdatePaletteFade;
	LZDecompressVram decompVram = (LZDecompressVram)ROM_LZDecompressVram;
	LoadPalette loadPal = (LoadPalette)ROM_LoadPalette;

	// The current task should be playing the first part of the intro cutscene (with the leaves and water drops) Task_Scene1_Load
	// Normally this would loop over: RunTasks(); AnimateSprites(); BuildOamBuffer(); UpdatePaletteFade();
	// By skipping the middle two we can avoid sprites loading and just override the background graphics in vram

	u8 i = 0;
	while(i < 20) 
	{
		RunTasks();
		updatePaletteFade();
		VBlankIntrWait();
		i++;
		if (i == 5) 
		{
			// On the 5th loop their graphics is already loaded so we can substitute it with our own

			// Replace sIntro1Bg_Gfx with gEasyChatWindow_Gfx. This happens to create a repeated tile background with two textboxes with the tiles already loaded for the intro
			decompVram((u32 *) EASY_CHAT_WINDOW_GFX, (void *)VRAM); 
			decompVram((u32 *) EASY_CHAT_WINDOW_TILEMAP, (void *)(BG_CHAR_ADDR(2))); 
			loadPal((u16 *) WAVE_MAIL_PALETTE, BG_PLTT_ID(0), 32); // 0x08dbe818 orange mail palette looks good too but the text is too light

			// Remove the lower text box which has slightly broken graphics anyway
			u16 *tilemap = (u16 *)(BG_CHAR_ADDR(2)); 
			for (volatile int k = 20; k < 25; k++) 
			{
				for (volatile int j = 0; j < 30; j++) 
				{
					tilemap[j + (k * 32)] = 33; 
				}
			}

			// Load a (previously unused) tilemap at a later point in vram that lets us write text
			decompVram((u32 *) OLD_CHAR_MAP, (void *)VRAM + 1400 + 4); 
			decompVram((u32 *) OLD_CHAR_TILEMAP, (void *)(BG_SCREEN_ADDR(18))); 

			u8 *text = (u8 *) "CONNECTING TO NETWORK";
			int textIndex = 0;
			for (volatile int y = 12; y < 16; y++) {
				for (volatile int x = 4; x < 32; x++) 
				{
					if (text[textIndex] == '\0') 
					{
						break; 
					}
					char c = text[textIndex];
					u16 tileIndex = c + 159 + 44; 
					tilemap[x + (y * 32)] = tileIndex; 
					textIndex++;
				}
			}
		}
	}

}

void NetConnEnableSerial(void)
{
	DisableInterrupts disableInterrupts = (DisableInterrupts)ROM_DisableInterrupts;
	EnableInterrupts enableInterrupts = (EnableInterrupts)ROM_EnableInterrupts;

    disableInterrupts(1 | INTR_FLAG_SERIAL);
    REG_RCNT = R_JOYBUS;
    enableInterrupts(INTR_FLAG_VBLANK | INTR_FLAG_VCOUNT | INTR_FLAG_TIMER3 | INTR_FLAG_SERIAL);
}

void NetConnDisableSerial(void)
{
	DisableInterrupts disableInterrupts = (DisableInterrupts)ROM_DisableInterrupts;

    disableInterrupts(INTR_FLAG_TIMER3 | INTR_FLAG_SERIAL);
    REG_SIOCNT = SIO_MULTI_MODE;
    REG_TMCNT_H(3) = 0;
    REG_IF = INTR_FLAG_TIMER3 | INTR_FLAG_SERIAL;
    JOY_TRANS = 0;
    REG_RCNT = 0; 
}


void waitForTransmissionFinish(u16 readOrWriteFlag)
{
    u32 i = 0;

    for (i = 0; (JOY_CNT&readOrWriteFlag) == 0; i++)
    {
        if (i > MAX_CONNECTION_LOOPS)
        {
            return;
        }
    }
}

void send32(u32 data) 
{
    JOY_CNT |= JOY_RW;
    JOY_TRANS = data;
    waitForTransmissionFinish(JOY_READ);
}

u32 recv32() 
{
    JOY_CNT |= JOY_RW;
    waitForTransmissionFinish(JOY_WRITE);
    return JOY_RECV;
}

void DoSerialDataBlock(u8 * dataStart, u8 xferMode, u16 cmd, u8 dataLength, u8 disabledChecked)
{
    u32 i;
    u16 checkBytes;
    u8 transBuff[4];
    u32 resBuff;

	u8 complete = 0;

	while (!complete)
	{
		i = 0;
		checkBytes = 0xFFFF;
		resBuff = 0;

		JOY_CNT |= JOY_RW;
		for (i = 0; JOY_CNT&JOY_READ && i <= MAX_CONNECTION_LOOPS; i++) 
		{ 
		}

		send32(((u32)dataLength << 16) | cmd);

		if (xferMode == SERIAL_MODE_SEND)
		{ 
			checkBytes ^= cmd;
			checkBytes ^= dataLength;
		}

		for(i = 0; i < dataLength; i+=4)
		{
			if (xferMode == SERIAL_MODE_SEND)
			{
				transBuff[0] = dataStart[i];
				transBuff[1] = i + 1 < dataLength ? dataStart[i + 1] : 0;
				transBuff[2] = i + 2 < dataLength ? dataStart[i + 2] : 0;
				transBuff[3] = i + 3 < dataLength ? dataStart[i + 3] : 0;

				send32((u32) (transBuff[0] + (transBuff[1] << 8) + (transBuff[2] << 16) + (transBuff[3] << 24)));
			}
			else 
			{
				resBuff = recv32();

				transBuff[0] = (resBuff >> 24) & 0xFF;
				transBuff[1] = (resBuff >> 16) & 0xFF;
				transBuff[2] = (resBuff >> 8) & 0xFF;
				transBuff[3] = resBuff & 0xFF;

				dataStart[i] = transBuff[0];
				if (i + 1 < dataLength) dataStart[i + 1] = transBuff[1];
				if (i + 2 < dataLength) dataStart[i + 2] = transBuff[2];
				if (i + 3 < dataLength) dataStart[i + 3] = transBuff[3];
			}

			checkBytes ^= (u16) (transBuff[0] + (transBuff[1] << 8));
			checkBytes ^= (u16) (transBuff[2] + (transBuff[3] << 8));

		}
		
		resBuff = recv32();

		if (disabledChecked || (resBuff ^ (u32) ((NET_CONN_CHCK_RES << 16) | (checkBytes & 0xFFFF))) == 0)
		{
			if (xferMode == SERIAL_MODE_SEND)
			{
				JOY_TRANS = 0;
			}
			else 
			{
				JOY_RECV = 0;
			}

			complete = 1;
		}
		else
		{
			for (i = 0; i < 300; i++) {}
		}
	}

}

void DoChunkedSerialDataBlock(u8 *dataStart, u8 xferMode, u16 cmd, u8 dataLength, u8 disabledChecked) 
{
    u8 remaining = dataLength;
    u8 currentChunk = 0;

    while (remaining > 0) 
    {
        u8 length = (remaining > CHUNK_SIZE) ? CHUNK_SIZE : remaining;
        DoSerialDataBlock(dataStart + (CHUNK_SIZE * currentChunk), xferMode, cmd + currentChunk, currentChunk == 0 ? length : 16, disabledChecked);

        remaining -= length;
        currentChunk++;
    }
}

void NetConnResetSerial(void) 
{
    NetConnDisableSerial();
    NetConnEnableSerial();
}

int main(void) 
{
	// check the ROM code, make sure this game is supported.
	u8* ROM = (u8*) ROM_OFFSET;
	
	u32 gamecode = (*(u32*)(&ROM[ROM_CODE_OFFSET]));
	
	void(*loadsave)(char a1);
	void(*mainloop)();
	void(*load_pokemon)();

	pSaveBlock1 saveBlock1;
	pSaveBlock2 saveBlock2;
	pSaveBlock3 saveBlock3;
	u32 titlemid = 0;
	// get the address of the save loading function.
	switch (gamecode) 
	{
		case 'EEPB': // Emerald English
			saveBlock1 = (pSaveBlock1) SAVE_BLOCK_1;
			saveBlock2 = (pSaveBlock2) SAVE_BLOCK_2;
			saveBlock3 = (pSaveBlock3) SAVE_BLOCK_3;
			*(pSaveBlock1*)(SAVE_BLOCK_1_PTR) = saveBlock1;
			loadsave = (void(*)(char)) LOAD_GAME_SAVE;
			mainloop = (void(*)()) (AGB_MAIN + OFFSET_IN_AGB_MAIN_TO_SKIP_RAM_RESET);
			titlemid = (RELOAD_SAVE + OFFSET_IN_RELOAD_SAVE_TO_SKIP_EWRAM_RESET);
			load_pokemon = (void(*)()) LOAD_PLAYER_PARTY;
			break;
		default:
			return 0; // this game isn't supported
	}

	// Before loading the save we need to make sure the ram we are loading it into is clear
	MemSet memsetFunc = (MemSet)ROM_memset;
	memsetFunc((void *)(IWRAM_OFFSET + MB_HEADER_SIZE), 0, MB_PADDING_SIZE - MB_HEADER_SIZE);

	*(volatile u32*)CONNECTION_WORKING = 0;
	
	loadsave(0);
	
	// now the save is loaded, we can do what we want with the loaded blocks.
	// first, we're going to want to decrypt the parts that are crypted, if applicable.
	decrypt_save_structures(saveBlock1,saveBlock2,saveBlock3);
	
	// time to call the payload. This is mostly for injecting data into the save block but we can also fiddle with ram values
	payload(saveBlock1,saveBlock2,saveBlock3);
	
	// Now, we better call the function that sets the pokemon-related stuff from the structure elements of the loaded save again.
	// Just in case the payload did something with that.
	load_pokemon();

	// Replace the title screen graphics with our own custom screen
	drawNetworkLoadingScreen();

	m4aSoundInit resetSound = (m4aSoundInit)ROM_m4aSoundInit;
	PlaySE playSe = (PlaySE)ROM_PlaySE;
	
	resetSound();
	playSe(109);

	u16 i;

	// Setup the network connection
	NetConnResetSerial();
	SaveBlock2_E* sb2e = &saveBlock2->e;

	// Give the GC the players info
	u8 sNetGameName[] = { TX('B'), TX('P'), TX('E'), TX('E'), 0xFF };
	DoChunkedSerialDataBlock((u8 *) &sb2e->playerName[0], SERIAL_MODE_SEND, NET_CMD_HEADER(SEND_DATA_TO_GC_BUFFER, 0), TRAINER_INFO_LENGTH, SERIAL_VERIFY_ON);
	DoChunkedSerialDataBlock((u8 *) &sNetGameName[0], SERIAL_MODE_SEND, NET_CMD_HEADER(SEND_DATA_TO_GC_BUFFER, 2), 5, SERIAL_VERIFY_ON);
	DoSerialDataBlock(0, SERIAL_MODE_SEND, NET_CONN_PINF_REQ, 0, SERIAL_VERIFY_ON);

	// Give the GC the network info
	u8 sNetServerAddr[] = { TX('1'),  TX('2'), TX('7'), TX('.'), TX('0'), TX('.'), TX('0'), TX('.'), TX('1'), TX(':'), TX('9'), TX('0'), TX('0'), TX('0'), 0xFF};
	DoChunkedSerialDataBlock((u8 *) &sNetServerAddr[0], SERIAL_MODE_SEND, NET_CMD_HEADER(SEND_DATA_TO_GC_BUFFER, 0), NET_SERVER_ADDR_LENGTH, SERIAL_VERIFY_ON);
	DoSerialDataBlock(0, SERIAL_MODE_SEND, NET_CONN_CINF_REQ, 0, SERIAL_VERIFY_ON);

	
	i = 0;
	u8 attempts = 0;
	while (1)
	{
		i = 0;
		// Wait for a bit
		while(i < 200)
		{
			VBlankIntrWait();
			i++;
		}

		// Get the connection status
		u8 sNetStateResponse[4] = {0, 0, 0, 0};
		DoSerialDataBlock((u8 *) &sNetStateResponse[0], SERIAL_MODE_RECV, NET_CONN_LIFN_REQ, 4, SERIAL_VERIFY_OFF);
		if (sNetStateResponse[3] == 1 && sNetStateResponse[2] == 1)
		{
			playSe(31);
			*(volatile u32*)CONNECTION_WORKING = 1;
			break;
		}
		else if (sNetStateResponse[3] >= 3 || attempts > 10)
		{
			playSe(32);
			break;
		}
		
		attempts++;
	}

	// In FR/LG/Emerald, just returning to the game is unwise.
	// The game reloads the savefile.
	// In FR/LG, this is done at the title screen after setting ASLR/saveblock-crypto up. (probably because at initial save-load, SaveBlock3 ptr isn't set up lol)
	// So, better bypass the title screen and get the game to return directly to the Continue/New Game screen.
	// In Emerald, the save reload happens after the Continue option was chosen, so we have no choice but to bypass everything and get the game to go straight to the overworld.
	// Easiest way to do this is to call into the middle of the function we want, using an ASM wrapper to set up the stack.
	// Here goes...
	if (titlemid) 
	{
		// Function reserves an extra 4 bytes of stack space in FireRed/LeafGreen, and none in Emerald.
		call_into_middle_of_titlescreen_func(titlemid,(GAME_EM ? 0 : 4));
	}
	// Now we've done what we want, time to return to the game.
	// Can't just return, the game will reload the save.
	// So let's just call the main-loop directly ;)
	// turn the sound back on before we head back to the game
	*(vu16 *)(REG_BASE + 0x84) = 0x8f;
	// re-enable interrupts
	REG_IME = 1;
	mainloop();
	// Anything past here will not be executed.
	return 0;
}


