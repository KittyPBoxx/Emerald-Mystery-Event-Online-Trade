/*
 * Example Gen3-multiboot payload by slipstream/RoL 2017.
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 *
 * libpayload.c: contains helper functions for the payload
 */
 
#include <gba.h>
#include "payload.h"

// private functions
// static void CryptBoxPokemon(struct BoxPokemon* pkm);
static u16 crc16(const u8* data, u16 len);
static u32 CalculateRamScriptDataChecksum(struct RamScriptData* ramScript);

/**
 * Decrypts the substructures of a Pok�mon structure, so they can be viewed or modified easily.
 * Remember to call EncryptPokemon() afterwards.
 * @param struct Pokemon* pkm The Pok�mon to decrypt the substructures of.
*/
// void DecryptPokemon(struct Pokemon* pkm) {
// 	struct BoxPokemon* boxMon = &(pkm->box);
// 	CryptBoxPokemon(boxMon);
// }

/**
 * Private function that does the actual work of crypting the substructures of a BoxPokemon.
 * @param struct BoxPokemon* pkm The BoxPokemon whose substructures will be crypted.
*/
// static void CryptBoxPokemon(struct BoxPokemon* pkm) {
// 	for (u32 i = 0; i < 12; i++) {
// 		pkm->secure.raw[i] ^= (pkm->otId ^ pkm->personality);
// 	}
// }

/**
 * Decrypts the substructures of a core Pok�mon structure, so they can be viewed or modified easily.
 * Used by DecryptPokemon().
 * Remember to call EncryptPokemon() afterwards.
 * @param struct BoxPokemon* pkm The BoxPokemon to decrypt the substructures of.
*/
// void DecryptBoxPokemon(struct BoxPokemon* pkm) {
// 	CryptBoxPokemon(pkm);
// }

/**
 * Encrypts the substructures of a Pok�mon structure, and fixes the checksum.
 * Must be used after DecryptPokemon() has been called, otherwise the Pok�mon you decrypted and forgot to re-encrypt will become a Bad Egg.
 * @param struct Pokemon* pkm The Pok�mon to encrypt the substructures and fix the checksum of.
*/
// void EncryptPokemon(struct Pokemon* pkm) {
// 	struct BoxPokemon* boxMon = &(pkm->box);
// 	EncryptBoxPokemon(boxMon);
// }

/**
 * Encrypts the substructures of a core Pok�mon structure, and fixes the checksum.
 * Must be used after DecryptBoxPokemon() has been called, otherwise the Pok�mon you decrypted and forgot to re-encrypt will become a Bad Egg.
 * @param struct BoxPokemon* pkm The BoxPokemon to encrypt the substructures and fix the checksum of.
*/
// void EncryptBoxPokemon(struct BoxPokemon* pkm) {
// 	FixBoxPokemonChecksum(pkm);
// 	CryptBoxPokemon(pkm);
// }

/**
 * Gets a substructure of a Pok�mon structure.
 * Call DecryptPokemon() first or the substructure data will be encrypted.
 * @param struct Pokemon* pkm The Pokemon to get a substructure of.
 * @param u8 substructId The substructure to get.
 * @return union PokemonSubstruct* The substructure.
*/
// union PokemonSubstruct* GetPokemonSubstruct(struct Pokemon* pkm,u8 substructId) {
// 	struct BoxPokemon* boxMon = &(pkm->box);
// 	return GetBoxPokemonSubstruct(boxMon,substructId);
// }

/**
 * Gets a substructure of a core Pok�mon structure.
 * Call DecryptBoxPokemon() first or the substructure data will be encrypted.
 * @param struct Pokemon* pkm The Pokemon to get a substructure of.
 * @param u8 substructId The substructure to get.
 * @return union PokemonSubstruct* The substructure.
*/
// union PokemonSubstruct* GetBoxPokemonSubstruct(struct BoxPokemon* pkm,u8 substructId) {
// 	if (substructId > 3) return NULL;
// 	u32 personality = pkm->personality;
// 	// Staring at the substruct indexes, I noticed the last two columns are the reverse of the first two!
// 	// that is, substructId==2 column is the reverse of substructId==1, substructId==3 is the reverse of substructId==0
// 	// At least that means there's no need to hardcode all four.
// 	u8 substruct_idxes[2][24] = {
// 		{ 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 2, 3, 1, 1, 2, 3, 2, 3, 1, 1, 2, 3, 2, 3 },
// 		{ 1, 1, 2, 3, 2, 3, 0, 0, 0, 0, 0, 0, 2, 3, 1, 1, 3, 2, 2, 3, 1, 1, 3, 2 }
// 	};
// 	u32 modulo = (personality % 24);
// 	if (substructId < 2) {
// 		return &( pkm->secure.substructs[ substruct_idxes[substructId][modulo] ] );
// 	}
// 	return &( pkm->secure.substructs[ substruct_idxes[3 - substructId][23 - modulo] ] );
// }

/**
 * Gets the checksum of a core Pok�mon structure.
 * @param struct BoxPokemon* pkm The BoxPokemon to calculate the checksum of.
 * @return u16 The checksum.
*/
// u16 CalculateBoxPokemonChecksum(struct BoxPokemon* pkm) {
// 	u16 checksum = 0;
// 	union PokemonSubstruct* substructs[4] = {
// 		GetBoxPokemonSubstruct(pkm,0),
// 		GetBoxPokemonSubstruct(pkm,1),
// 		GetBoxPokemonSubstruct(pkm,2),
// 		GetBoxPokemonSubstruct(pkm,3)
// 	};
	
// 	for (int substruct = 0; substruct < 4; substruct++) {
// 		for (int i = 0; i < 6; i++) {
// 			checksum += substructs[substruct]->raw[i];
// 		}
// 	}
// 	return checksum;
// }

/**
 * Fixes the checksum of a core Pok�mon structure.
 * @param struct BoxPokemon* pkm The BoxPokemon to fix the checksum of.
*/
// void FixBoxPokemonChecksum(struct BoxPokemon* pkm) {
// 	pkm->checksum = CalculateBoxPokemonChecksum(pkm);
// }

/**
 * Gets the zeroth substructure ("Growth") of a Pok�mon structure.
 * Call DecryptPokemon() first or the substructure data will be encrypted.
 * @param struct Pokemon* pkm The Pok�mon to get a substructure of.
 * @return struct PokemonSubstruct0* The substructure.
*/
// struct PokemonSubstruct0* GetPokemonSubstruct0(struct Pokemon* pkm) {
// 	struct BoxPokemon* boxMon = &(pkm->box);
// 	return GetBoxPokemonSubstruct0(boxMon);
// }

/**
 * Gets the zeroth substructure ("Growth") of a core Pok�mon structure.
 * Call DecryptBoxPokemon() first or the substructure data will be encrypted.
 * @param struct BoxPokemon* pkm The BoxPokemon to get the substructure of.
 * @return struct PokemonSubstruct0* The substructure.
*/
// struct PokemonSubstruct0* GetBoxPokemonSubstruct0(struct BoxPokemon* pkm) {
// 	return &( GetBoxPokemonSubstruct(pkm,0)->type0 );
// }

/**
 * Gets the first substructure ("Attacks") of a Pok�mon structure.
 * Call DecryptPokemon() first or the substructure data will be encrypted.
 * @param struct Pokemon* pkm The Pok�mon to get a substructure of.
 * @return struct PokemonSubstruct0* The substructure.
*/
// struct PokemonSubstruct1* GetPokemonSubstruct1(struct Pokemon* pkm) {
// 	struct BoxPokemon* boxMon = &(pkm->box);
// 	return GetBoxPokemonSubstruct1(boxMon);
// }

/**
 * Gets the first substructure ("Attacks") of a core Pok�mon structure.
 * Call DecryptBoxPokemon()  first or the substructure data will be encrypted.
 * @param struct BoxPokemon* pkm The BoxPokemon to get the substructure of.
 * @return struct PokemonSubstruct1* The substructure.
*/
// struct PokemonSubstruct1* GetBoxPokemonSubstruct1(struct BoxPokemon* pkm) {
// 	return &( GetBoxPokemonSubstruct(pkm,1)->type1 );
// }

/**
 * Gets the second substructure ("EVs & Condition") of a Pok�mon structure.
 * Call DecryptPokemon() first or the substructure data will be encrypted.
 * @param struct Pokemon* pkm The Pok�mon to get a substructure of.
 * @return struct PokemonSubstruct0* The substructure.
*/
// struct PokemonSubstruct2* GetPokemonSubstruct2(struct Pokemon* pkm) {
// 	struct BoxPokemon* boxMon = &(pkm->box);
// 	return GetBoxPokemonSubstruct2(boxMon);
// }

/**
 * Gets the second substructure ("EVs & Condition") of a core Pok�mon structure.
 * Call DecryptBoxPokemon()  first or the substructure data will be encrypted.
 * @param struct BoxPokemon* pkm The BoxPokemon to get the substructure of.
 * @return struct PokemonSubstruct2* The substructure.
*/
// struct PokemonSubstruct2* GetBoxPokemonSubstruct2(struct BoxPokemon* pkm) {
// 	return &( GetBoxPokemonSubstruct(pkm,2)->type2 );
// }

/**
 * Gets the third substructure ("Miscellaneous")  of a Pok�mon structure.
 * Call DecryptPokemon() first or the substructure data will be encrypted.
 * @param struct Pokemon* pkm The Pok�mon to get a substructure of.
 * @return struct PokemonSubstruct0* The substructure.
*/
// struct PokemonSubstruct3* GetPokemonSubstruct3(struct Pokemon* pkm) {
// 	struct BoxPokemon* boxMon = &(pkm->box);
// 	return GetBoxPokemonSubstruct3(boxMon);
// }

/**
 * Gets the third substructure ("Miscellaneous") of a core Pok�mon structure.
 * Call DecryptBoxPokemon()  first or the substructure data will be encrypted.
 * @param struct BoxPokemon* pkm The BoxPokemon to get the substructure of.
 * @return struct PokemonSubstruct3* The substructure.
*/
// struct PokemonSubstruct3* GetBoxPokemonSubstruct3(struct BoxPokemon* pkm) {
// 	return &( GetBoxPokemonSubstruct(pkm,3)->type3 );
// }

/**
 * Calculates the checksum for an R/S-specific Enigma Berry structure in SaveBlock1.
 * @param struct EnigmaBerry* enigmaBerry The R/S-specific Enigma Berry to calculate the checksum for.
 * @return u32 The checksum.
*/
// u32 CalculateEnigmaBerryChecksumRS(struct EnigmaBerry* enigmaBerry) {
// 	if (!GAME_RS) return 0; // Enigma Berry structure changed in FR/LG, use CalculateEnigmaBerryChecksumFRLGE() instead.
// 	u32 checksum = 0;
// 	// Save off and zero out the original Enigma Berry description pointers.
// 	const u8* description[2] = { enigmaBerry->berry.description1, enigmaBerry->berry.description2 };
// 	enigmaBerry->berry.description1 = enigmaBerry->berry.description2 = NULL;
// 	u8* enigmaBerryBytes = (u8*)enigmaBerry;
// 	// Calculate the checksum.
// 	for (u32 i = 0; i < 1324; i++) {
// 		checksum += enigmaBerryBytes[i];
// 	}
// 	// Restore the saved description.
// 	enigmaBerry->berry.description1 = description[0];
// 	enigmaBerry->berry.description2 = description[1];
// 	return checksum;
// }

/**
 * Calculates the checksum for an FR/LG/Emerald-specific Enigma Berry structure in SaveBlock1.
 * @param struct EnigmaBerryFRLGE* enigmaBerry The FR/LG/Emerald-specific Enigma Berry to calculate the checksum for.
 * @return u32 The checksum.
*/
// u32 CalculateEnigmaBerryChecksumFRLGE(struct EnigmaBerryFRLGE* enigmaBerry) {
// 	if (GAME_RS) return 0; // Enigma Berry structure is different in R/S, use CalculateEnigmaBerryChecksumRS() instead.
// 	u32 checksum = 0;
// 	u8* enigmaBerryBytes = (u8*)enigmaBerry;
// 	for (int i = 0; i < 0x30; i++) {
// 		checksum += enigmaBerryBytes[i];
// 	}
// 	return checksum;
// }

/**
 * Calculates the checksum for an unspecified Enigma Berry structure in SaveBlock1. (detected by current game)
 * @param void* enigmaBerry The Enigma Berry structure to calculate the checksum for.
 * @return u32 The checksum.
 */
// u32 CalculateEnigmaBerryChecksum(void* enigmaBerry) {
// 	if (GAME_RS) return CalculateEnigmaBerryChecksumRS( (struct EnigmaBerry*) enigmaBerry );
// 	return CalculateEnigmaBerryChecksumFRLGE( (struct EnigmaBerryFRLGE*) enigmaBerry );
// }

/**
 * Calculates the checksum for a RAM script structure in SaveBlock1.
 * @param struct RamScript* ramScript The RAM script structure to calculate the checksum for.
 * @return u32 The checksum.
 */
u32 CalculateRamScriptChecksum1(struct RamScript* ramScript) {
	asm(""); // make sure the call to CalculateRamScriptDataChecksum() is *not* inlined.
	// For some reason, if it gets inlined, something happens, and the wrong length is used when checksumming...
	return CalculateRamScriptDataChecksum(&ramScript->data);
}

/**
 * Calculates the checksum for a RAM script structure in SaveBlock1.
 * @param struct RamScript* ramScript The RAM script structure to calculate the checksum for.
 * @return u32 The checksum.
 */
static __attribute__ ((noinline)) u32 CalculateRamScriptDataChecksum(struct RamScriptData* ramScript) {
	u32 checksum = 0;
	u32 i = 0;
	u8* ramScriptBytes = (u8*)(ramScript);
	if (GAME_RS) {
		for (i = 0; i < sizeof(struct RamScriptData); i++) checksum += ramScriptBytes[i];
		return checksum;
	}
	
	return (u32)crc16(ramScriptBytes,sizeof(struct RamScriptData) + 1);
}

/**
 * Private function to CRC-16, (used by FR/LG/Emerald ramscript checksum). Adapted from http://forums.glitchcity.info/index.php?topic=7114.0
 * @param u8* Data to checksum
 * @param u16 Length of data
 * @return u16 The checksum
*/
static u16 crc16(const u8* data, u16 len) {
	u16 crc = 0x1121;
	for (u16 i = 0; i < len; ++i) {
		crc ^= data[i];
		for (u16 j = 0; j < 8; ++j) {
			if(crc & 1) {
				crc = (crc >> 1) ^ 0x8408;
			} else {
				crc >>= 1;
			}
		}
	}
	return ~crc;
}