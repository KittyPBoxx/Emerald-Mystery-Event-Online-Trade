/*
 * Example Gen3-multiboot payload by slipstream/RoL 2017.
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 *
 * payload.c: place where user payload should go :)
 */

#include <gba.h>
#include "payload.h"
#include "libpayload.h"
#include "event_script_bin.h"
#include "ram_script_bin.h"
#include "generated_ram_header.h" 
#include "symbols.h"

#define RAM_SCRIPT_MAGIC 51

typedef void *(*MemCpy)(void *dest, const void *src, size_t n);

extern const unsigned char event_script_bin[];
extern const unsigned int event_script_bin_size;

// Your payload code should obviously go into the body of this, the payload function.
void payload(pSaveBlock1 SaveBlock1,pSaveBlock2 SaveBlock2,pSaveBlock3 SaveBlock3) {

	// Structure offsets are different between R/S, FR/LG, and Emerald.
	// The beginning of the structures are the same but do NOT take shortcuts here, it's not good practise.
	// If you want to support multiple games, make specific checks for games, use the right structure for each game.
	// SaveBlock1_RS* sb1rs = &SaveBlock1->rs;
	// SaveBlock1_FRLG* sb1frlg = &SaveBlock1->frlg;
	SaveBlock1_E* sb1e = &SaveBlock1->e;
	if (GAME_FRLG) {
		return;
	} else if (GAME_RS) {
		return;
	} else if (GAME_EM) {

		struct RamScript *script = &sb1e->ramScript;
		struct RamScriptData *scriptData = &sb1e->ramScript.data;

		MemCpy memcpyFunc = (MemCpy)ROM_memcpy;

		scriptData->magic = RAM_SCRIPT_MAGIC;
		scriptData->mapGroup = 0;
		scriptData->mapNum = 9;
		scriptData->objectId = 1;

		memcpyFunc((void *)RAM_SCRIPT_ADDR, ram_script_bin, ram_script_bin_size);

		// Example of of event script data
		// unsigned char script_data[] = {
		// 	0x23,                    // callasm
		// 	0xE5, 0x47, 0x0D, 0x08,  // address 0x080d47e4 (E5 because it's thumb code) 
		// 	0x0F, 0x00,              // msgbox
		// 	0xE5, 0x8C, 0x1E, 0x08,  // text pointer
		// 	0x09, 0x0A,              // Second byte msg box type MSGBOX_DEFAULT = 4 
		// 	0x6C, 0x02               // release end 
		// };
		// memcpyFunc(scriptData->script, script_data, sizeof(script_data));


		memcpyFunc(scriptData->script, event_script_bin, event_script_bin_size);
		sb1e->ramScript.checksum = CalculateRamScriptChecksum1(script);

		return;
	}
}