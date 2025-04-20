/****************************************************************************
* Multiboot Script Loader
* KittyPBoxx 2024
*
* pokekeys.h
* All the key generation functions used to spoof a colosseum multiboot rom
***************************************************************************/
#include <ogcsys.h>
#include <gccore.h>
#include <string.h>

unsigned int docrc(u32 crc,u32 val) 
{
	u32 result;
	
	result = val ^ crc;
	
    for (int i = 0; i < 0x20; i++) 
    {
		if (result & 1) 
        {
			result >>= 1;
			result ^= 0xA1C1;
		} 
        else 
        {
            result >>= 1;
        }
	}

	return result;
}

/*
* KeyA, we send, is a random key that must conform to the rule:
* keyA >> 24 == 0xDD
*/
u32 genKeyA() 
{
	u32 retries = 0;
	while (true) 
    {
		u32 key = 0;
		
        if (retries > 32) 
        {
			key = 0xDD654321;
		} 
        else 
        {
			key = (rand() & 0x00ffffff) | 0xDD000000;
		}
		
        u32 unk = (key % 2 != 0);
		u32 v12 = key;
		
        for (u32 v13 = 1; v13 < 32; v13++) 
        {
			v12 >>= 1;
			unk += (v12 % 2 != 0);
		}
		
        if ((unk >= 10 && unk <= 24)) 
        {
			if (retries > 4) printf("KeyA retries = %d",retries);
			printf("KeyA = 0x%08x\n",key);
			return key;
		}

		retries++;
	}
}

/*
* KeyB, is sent to us, is a random key, generated base on frames run, it must conform to the following:
* keyB >> 24 == 0xEE
*/
u32 checkKeyB(u32 KeyBRaw) 
{
	if ((KeyBRaw & 0xFF) != 0xEE) 
    {
		printf("Invalid KeyB - lowest 8 bits should be 0xEE, actually 0x%02x\n",((u8)(KeyBRaw)));
		return 0;
	}

	u32 KeyB = KeyBRaw & 0xffffff00;
	u32 val = KeyB;
	// u32 unk = (val < 0); // val is unsigned so unk is always 0 

	for (u32 i = 1; i < 24; i++) 
    {
		val <<= 1;
		// unk += (val < 0);
	}

	// if (unk > 14) 
    // {
	// 	printf("Invalid KeyB - high 24 bits bad: 0x%08x\n",KeyB);
	// 	return 0;
	// }

	printf("Valid KeyB: 0x%08x\n",KeyB);
	return KeyB;
}

u32 deriveKeyC(u32 keyCderive, u32 kcrc) 
{
	u32 keyc = 0;
	u32 keyCi = 0;
	do 
    {
		u32 v5 = 0x1000000 * keyCi - 1;
		u32 keyCattempt = docrc(kcrc,v5);
		//printf("i = %d; keyCderive = %08x; keyCattempt = %08x\n",keyCi,keyCderive,keyCattempt);
		
        if (keyCderive == keyCattempt) 
        {
			keyc = v5;
			printf("Found keyC: %08x\n",keyc);
			return keyc;
		}
		keyCi++;

	} while (keyCi < 256);
	
    return keyc;
}
