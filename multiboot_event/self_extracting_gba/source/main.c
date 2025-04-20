#include "gba_mb_functions_lz_bin.h"

#define DEST_ADDR ((void *)0x020020E0) 

__attribute__((naked))
void DecompressLZ77_BIOS(const void *src, void *dst) 
{
    __asm__("swi 0x11\n bx lr\n");
}

int main(void) {
    DecompressLZ77_BIOS(gba_mb_functions_lz_bin, DEST_ADDR);

    void (*entry_point)(void) = (void (*)(void)) DEST_ADDR;
    entry_point();

    return 0;
}