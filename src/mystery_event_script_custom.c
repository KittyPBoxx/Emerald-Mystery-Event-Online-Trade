/**
* Begining of custom code for the mystery event
**/
#include "global.h"
#include "berry.h"
#include "battle_tower.h"
#include "easy_chat.h"
#include "event_data.h"
#include "mail.h"
#include "mystery_event_script.h"
#include "pokedex.h"
#include "pokemon.h"
#include "pokemon_size_record.h"
#include "script.h"
#include "strings.h"
#include "string_util.h"
#include "text.h"
#include "util.h"
#include "mystery_event_msg.h"
#include "pokemon_storage_system.h"

#include "gpu_regs.h"
#include "map_name_popup.h"
#include "overworld.h"
#include "task.h"
#include "item_use.h"
#include "menu.h"
#include "sound.h"
#include "constants/songs.h"
#include "field_message_box.h"
#include "shop.h"

#define ROM_FUNCTION __attribute__((section(".rom_function")))
#define RAM_FUNCTION __attribute__((section(".ram_function")))

#ifndef M_EVENTS
    #error "M_EVENTS is NOT defined!"
#endif

#if M_EVENTS
    // Build a broken rom, with functions that can be extracted and injected into a vanilla game
    #define FUNCTION_SECTION RAM_FUNCTION
#else
    // Build a rom that can be tested locally
    #define FUNCTION_SECTION ROM_FUNCTION
#endif

extern void Task_NetworkTaskLoop(u8 taskId);
extern void myEvent();

/* 
* This defines the mystery event function. It is stored in rom, but we are telling the compiler that it will be executed from ram.
* This is because when loaded as an event on a vanilla rom it will be written directly to ram 
*/
FUNCTION_SECTION void myEvent() 
{
    CreateTask(Task_NetworkTaskLoop, 80);
}

// FUNCTION_SECTION const u8 gText_SomeCustomText[] = _("Custom code was run!");
// DrawDialogFrameWithCustomTileAndPalette(0, FALSE, 0x100, 0xF);
// AddTextPrinterParameterized(0, FONT_NORMAL, gText_SomeCustomText, 0, 1, 0, 0);
// ScheduleBgCopyTilemapToVram(0);

// Start of networking code
#define JOY_CNT         (*(vu16 *)REG_ADDR_JOYCNT)
#define JOY_TRANS	    (*(vu32*)(REG_ADDR_JOY_TRANS))
#define JOY_RECV        (*(vu32 *)REG_ADDR_JOY_RECV) 
#define MAX_CONNECTION_LOOPS 500000
#define JOY_WRITE 0x2
#define JOY_READ  0x4
#define JOY_RW    0x6

#define NET_CONN_CHCK_RES 0x1101

#define SERIAL_MODE_SEND 0 
#define SERIAL_MODE_RECV 1
#define SERIAL_VERIFY_ON 0 
#define SERIAL_VERIFY_OFF 1
#define CHUNK_SIZE 16
#define R_JOYBUS  0xC000

#define SEND_DATA_TO_GC_BUFFER 0x15
#define POST_GC_BUFFER_TO_SERVER 0x13
#define RECV_DATA_FROM_GC_BUFFER 0x25
#define NET_CMD_HEADER(cmd, buf) (((cmd) << 8) | (buf))

FUNCTION_SECTION static const u8 sDot[] = _("·");
FUNCTION_SECTION static const u8 sWaitingMessage[] = _("Waiting for server:");

#define LINK_PARTNER_NONE 0
#define LINK_PARTNER_SUCCESS 1

#define netState data[0]
#define netLoopCounter data[1]
#define netStateAfterWait data[2]
#define netWaitLength data[3]

FUNCTION_SECTION enum {
    NET_RESET_SERIAL= 0,
    NET_CLEAR_PAST_DATA,
    NET_PREPARE_SEND_REQ_TO_GC,
    NET_PREPARE_SEND_MON_TO_GC,
    NET_PREPARE_PUSH_TO_SERVER,
    NET_WAIT_FOR_READ,
    NET_READ_NAME_RESPONSE,
    NET_READ_MON_RESPONSE,
    NET_ERROR,
    NET_DONE
};


FUNCTION_SECTION void waitForTransmissionFinish(u16 readOrWriteFlag)
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

FUNCTION_SECTION void send32(u32 data) 
{
    JOY_CNT |= JOY_RW;
    JOY_TRANS = data;
    waitForTransmissionFinish(JOY_READ);
}

FUNCTION_SECTION u32 recv32() 
{
    JOY_CNT |= JOY_RW;
    waitForTransmissionFinish(JOY_WRITE);
    return JOY_RECV;
}

FUNCTION_SECTION s32 DoSerialDataBlock(u8 * dataStart, u8 xferMode, u16 cmd, u8 dataLength, u8 disabledChecked)
{
    u32 i;
    u16 checkBytes;
    u8 transBuff[4];
    u32 resBuff;
    u32 attempts = 50;

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

        if ((cmd >> 8) != POST_GC_BUFFER_TO_SERVER)
        {
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

        attempts--; 
        if (attempts == 0)
        {
            return 0;
        }
	}

    return 1;
}

FUNCTION_SECTION s32 DoChunkedSerialDataBlock(u8 *dataStart, u8 xferMode, u16 cmd, u8 dataLength, u8 disabledChecked) 
{
    u8 remaining = dataLength;
    u8 currentChunk = 0;
    u32 attempts = 10;
    u32 i;

    while (remaining > 0) 
    {
        u8 length = (remaining > CHUNK_SIZE) ? CHUNK_SIZE : remaining;
        if (DoSerialDataBlock(dataStart + (CHUNK_SIZE * currentChunk), xferMode, cmd + currentChunk, length, disabledChecked))
        {
            remaining -= length;
            currentChunk++;
        }
        else 
        {
            attempts--;
            if(attempts == 0)
            {
                return 0;
            }
        }

        for (i = 0; i < 300; i++) {}
    }

    return 1;
}


FUNCTION_SECTION void Task_NetworkTaskLoop(u8 taskId)
{
    if (JOY_HELD(B_BUTTON))
    {
        gSpecialVar_0x8003 = LINK_PARTNER_NONE; 
        gTasks[taskId].netState = NET_ERROR;
    }

    switch (gTasks[taskId].netState)
    {
        case NET_RESET_SERIAL:
            if (REG_RCNT != R_JOYBUS)
            {
                // Disable Serial
                DisableInterrupts(INTR_FLAG_TIMER3 | INTR_FLAG_SERIAL);
                REG_SIOCNT = SIO_MULTI_MODE;
                REG_TMCNT_H(3) = 0;
                REG_IF = INTR_FLAG_TIMER3 | INTR_FLAG_SERIAL;
                JOY_TRANS = 0;
                REG_RCNT = 0; 

                // Enable Serial
                REG_RCNT = R_JOYBUS;
                EnableInterrupts(INTR_FLAG_VBLANK | INTR_FLAG_VCOUNT | INTR_FLAG_TIMER3 | INTR_FLAG_SERIAL);
            }
            if (gSpecialVar_0x8003 == 2)
            {
                gTasks[taskId].netState = NET_PREPARE_PUSH_TO_SERVER;
            }
            else
            {
                gTasks[taskId].netState = NET_CLEAR_PAST_DATA;
            }
            gSpecialVar_0x8003 = LINK_PARTNER_NONE;
            break;
        case NET_CLEAR_PAST_DATA: 
            gStringVar3[0] = 0; gStringVar3[1] = 0; gStringVar3[2] = 0; gStringVar3[3] = 0;
            if (DoChunkedSerialDataBlock((u8 *) &gStringVar3[0], SERIAL_MODE_SEND, NET_CMD_HEADER(SEND_DATA_TO_GC_BUFFER, 0xF0), 4, SERIAL_VERIFY_ON))
                gTasks[taskId].netState = NET_PREPARE_SEND_REQ_TO_GC;

            break;
        case NET_PREPARE_SEND_REQ_TO_GC: 
            gStringVar3[0] = 'T'; gStringVar3[1] = 'R'; gStringVar3[2] = '_'; gStringVar3[3] = '0';
            if (DoChunkedSerialDataBlock((u8 *) &gStringVar3[0], SERIAL_MODE_SEND, NET_CMD_HEADER(SEND_DATA_TO_GC_BUFFER, 0), 4, SERIAL_VERIFY_ON))
                gTasks[taskId].netState = NET_PREPARE_SEND_MON_TO_GC;

            break;
        case NET_PREPARE_SEND_MON_TO_GC: 
            if (DoChunkedSerialDataBlock((u8 *) &gPlayerParty[gSpecialVar_0x8005], SERIAL_MODE_SEND, NET_CMD_HEADER(SEND_DATA_TO_GC_BUFFER, 1), sizeof(struct Pokemon), SERIAL_VERIFY_ON))
                gTasks[taskId].netState = NET_PREPARE_PUSH_TO_SERVER;

            break;
        case NET_PREPARE_PUSH_TO_SERVER:
            if (DoSerialDataBlock(0, SERIAL_MODE_SEND, NET_CMD_HEADER(POST_GC_BUFFER_TO_SERVER, 0), 16 + sizeof(struct Pokemon), SERIAL_VERIFY_OFF))
            {
                gTasks[taskId].netState = NET_WAIT_FOR_READ;
                CpuFill32(0, &gStringVar3, sizeof(gStringVar3));
                CpuFill32(0, &gEnemyParty, sizeof(gEnemyParty)); 
                gTasks[taskId].netLoopCounter = 0;
                gTasks[taskId].netState = NET_WAIT_FOR_READ;
                gTasks[taskId].netStateAfterWait = NET_READ_NAME_RESPONSE;
                gTasks[taskId].netWaitLength = 120;
            }
            break;
        case NET_WAIT_FOR_READ:
        {
            if (gTasks[taskId].netLoopCounter == 0)
            {
                LoadMessageBoxAndFrameGfx(0, TRUE);
                VBlankIntrWait(); VBlankIntrWait();
                AddTextPrinterParameterized(0, FONT_NORMAL, sWaitingMessage, 0, 1, 0, NULL);
                gTasks[taskId].netLoopCounter++;
            }
            else if (gTasks[taskId].netLoopCounter <= gTasks[taskId].netWaitLength)
            {
                VBlankIntrWait(); VBlankIntrWait();
                if (gTasks[taskId].netLoopCounter % 10 == 0)
                {
                    AddTextPrinterParameterized(0, FONT_NORMAL, sDot, ((gTasks[taskId].netLoopCounter - 1) * 8) / 10, 16, 0, NULL);
                }
                gTasks[taskId].netLoopCounter++;
            }
            else
            {                
                gTasks[taskId].netState = gTasks[taskId].netStateAfterWait;
            }
            break;  
        }
        case NET_READ_NAME_RESPONSE: 
        {
            u8 i;
            if (DoSerialDataBlock((u8 *) &gStringVar3[0], SERIAL_MODE_RECV, NET_CMD_HEADER(RECV_DATA_FROM_GC_BUFFER, 0xF0), 4, SERIAL_VERIFY_OFF))
            {
                gTasks[taskId].netLoopCounter = 0;
                gTasks[taskId].netState = NET_DONE;
                for (i = 0; i < 4; i++)
                {
                    if (gStringVar3[i] != 0)
                    {
                        gSpecialVar_0x8003 = LINK_PARTNER_SUCCESS; 
                        gTasks[taskId].netState = NET_WAIT_FOR_READ;
                        gTasks[taskId].netStateAfterWait = NET_READ_MON_RESPONSE;
                        gTasks[taskId].netWaitLength = 60;
                        break;
                    }
                }
            }
            break;
        }
        case NET_READ_MON_RESPONSE: 
        {
            u16 species;
            if (DoChunkedSerialDataBlock((u8 *) &gEnemyParty[0], SERIAL_MODE_RECV, NET_CMD_HEADER(RECV_DATA_FROM_GC_BUFFER, 0xF1), sizeof(struct Pokemon), SERIAL_VERIFY_ON))
            {
                species = GetMonData(&gEnemyParty[0], MON_DATA_SPECIES);
                if (!gEnemyParty[0].box.isBadEgg && !(species > SPECIES_EGG))
                {
                    gSpecialVar_0x8003 = LINK_PARTNER_SUCCESS; 
                    gTasks[taskId].netState = NET_DONE;
                }
                else
                {
                    gTasks[taskId].netLoopCounter = 0;
                    gTasks[taskId].netState = NET_WAIT_FOR_READ;
                }
            }

            break;
        }
        case NET_ERROR:
        case NET_DONE:
        {
            JOY_TRANS = 0;
            DisableInterrupts(INTR_FLAG_TIMER3 | INTR_FLAG_SERIAL);
            REG_IF = INTR_FLAG_TIMER3 | INTR_FLAG_SERIAL;
            HideFieldMessageBox();
            ScriptContext_Enable();
            DestroyTask(taskId);
            break;
        }
    }
}

#undef netState
#undef netLoopCounter
#undef netStateAfterWait
#undef netWaitLength
