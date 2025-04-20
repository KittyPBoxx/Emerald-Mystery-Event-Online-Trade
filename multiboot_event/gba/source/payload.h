/*
 * Example Gen3-multiboot payload by slipstream/RoL 2017.
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 *
 * payload.h: header file describing payload function
 */

#include "saveblocks.h"
#include "libpayload.h"

#define GAME_RUBY (((*(u32*)(0x80000AC)) << 8) == 'VXA\x00')
#define GAME_SAPP (((*(u32*)(0x80000AC)) << 8) == 'PXA\x00')
#define GAME_RS   ((GAME_RUBY) || (GAME_SAPP))
#define GAME_FR   (((*(u32*)(0x80000AC)) << 8) == 'RPB\x00')
#define GAME_LG   (((*(u32*)(0x80000AC)) << 8) == 'GPB\x00')
#define GAME_FRLG ((GAME_FR) || (GAME_LG))
#define GAME_EM   (((*(u32*)(0x80000AC)) << 8) == 'EPB\x00')

// #define LANG_JAPAN ((*(u8*)(0x80000AF)) == 'J')

void payload(pSaveBlock1 SaveBlock1,pSaveBlock2 SaveBlock2,pSaveBlock3 SaveBlock3);
void  ram_function(void);