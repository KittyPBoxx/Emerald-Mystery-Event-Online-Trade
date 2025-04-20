/*
 * Example Gen3-multiboot payload by slipstream/RoL 2017.
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 *
 * libpayload.h: header for payload helper functions
 */

/**
 * Decrypts the substructures of a Pok�mon structure, so they can be viewed or modified easily.
 * Remember to call EncryptPokemon() afterwards.
 * @param struct Pokemon* pkm The Pok�mon to decrypt the substructures of.
*/
// void DecryptPokemon(struct Pokemon* pkm);

/**
 * Decrypts the substructures of a core Pok�mon structure, so they can be viewed or modified easily.
 * Used by DecryptPokemon().
 * Remember to call EncryptPokemon() afterwards.
 * @param struct BoxPokemon* pkm The BoxPokemon to decrypt the substructures of.
*/
// void DecryptBoxPokemon(struct BoxPokemon* pkm);

/**
 * Encrypts the substructures of a Pok�mon structure, and fixes the checksum.
 * Must be used after DecryptPokemon() has been called, otherwise the Pok�mon you decrypted and forgot to re-encrypt will become a Bad Egg.
 * @param struct Pokemon* pkm The Pok�mon to encrypt the substructures and fix the checksum of.
*/
// void EncryptPokemon(struct Pokemon* pkm);

/**
 * Encrypts the substructures of a core Pok�mon structure, and fixes the checksum.
 * Must be used after DecryptBoxPokemon() has been called, otherwise the Pok�mon you decrypted and forgot to re-encrypt will become a Bad Egg.
 * @param struct BoxPokemon* pkm The BoxPokemon to encrypt the substructures and fix the checksum of.
*/
// void EncryptBoxPokemon(struct BoxPokemon* pkm);

/**
 * Gets a substructure of a Pok�mon structure.
 * Call DecryptPokemon() first or the substructure data will be encrypted.
 * @param struct Pokemon* pkm The Pokemon to get a substructure of.
 * @param u8 substructId The substructure to get.
 * @return union PokemonSubstruct* The substructure.
*/
// union PokemonSubstruct* GetPokemonSubstruct(struct Pokemon* pkm,u8 substructId);

/**
 * Gets a substructure of a core Pok�mon structure.
 * Call DecryptBoxPokemon() first or the substructure data will be encrypted.
 * @param struct Pokemon* pkm The Pokemon to get a substructure of.
 * @param u8 substructId The substructure to get.
 * @return union PokemonSubstruct* The substructure.
*/
// union PokemonSubstruct* GetBoxPokemonSubstruct(struct BoxPokemon* pkm,u8 substructId);

/**
 * Gets the checksum of a core Pok�mon structure.
 * @param struct BoxPokemon* pkm The BoxPokemon to calculate the checksum of.
 * @return u16 The checksum.
*/
// u16 CalculateBoxPokemonChecksum(struct BoxPokemon* pkm);

/**
 * Fixes the checksum of a core Pok�mon structure.
 * @param struct BoxPokemon* pkm The BoxPokemon to fix the checksum of.
*/
// void FixBoxPokemonChecksum(struct BoxPokemon* pkm);

/**
 * Gets the zeroth substructure ("Growth") of a Pok�mon structure.
 * Call DecryptPokemon() first or the substructure data will be encrypted.
 * @param struct Pokemon* pkm The Pok�mon to get a substructure of.
 * @return struct PokemonSubstruct0* The substructure.
*/
// struct PokemonSubstruct0* GetPokemonSubstruct0(struct Pokemon* pkm);

/**
 * Gets the zeroth substructure ("Growth") of a core Pok�mon structure.
 * Call DecryptBoxPokemon() first or the substructure data will be encrypted.
 * @param struct BoxPokemon* pkm The BoxPokemon to get the substructure of.
 * @return struct PokemonSubstruct0* The substructure.
*/
// struct PokemonSubstruct0* GetBoxPokemonSubstruct0(struct BoxPokemon* pkm);

/**
 * Gets the first substructure ("Attacks") of a Pok�mon structure.
 * Call DecryptPokemon() first or the substructure data will be encrypted.
 * @param struct Pokemon* pkm The Pok�mon to get a substructure of.
 * @return struct PokemonSubstruct0* The substructure.
*/
// struct PokemonSubstruct1* GetPokemonSubstruct1(struct Pokemon* pkm);

/**
 * Gets the first substructure ("Attacks") of a core Pok�mon structure.
 * Call DecryptBoxPokemon()  first or the substructure data will be encrypted.
 * @param struct BoxPokemon* pkm The BoxPokemon to get the substructure of.
 * @return struct PokemonSubstruct1* The substructure.
*/
// struct PokemonSubstruct1* GetBoxPokemonSubstruct1(struct BoxPokemon* pkm);

/**
 * Gets the second substructure ("EVs & Condition") of a Pok�mon structure.
 * Call DecryptPokemon() first or the substructure data will be encrypted.
 * @param struct Pokemon* pkm The Pok�mon to get a substructure of.
 * @return struct PokemonSubstruct0* The substructure.
*/
// struct PokemonSubstruct2* GetPokemonSubstruct2(struct Pokemon* pkm);

/**
 * Gets the second substructure ("EVs & Condition") of a core Pok�mon structure.
 * Call DecryptBoxPokemon()  first or the substructure data will be encrypted.
 * @param struct BoxPokemon* pkm The BoxPokemon to get the substructure of.
 * @return struct PokemonSubstruct2* The substructure.
*/
// struct PokemonSubstruct2* GetBoxPokemonSubstruct2(struct BoxPokemon* pkm);

/**
 * Gets the third substructure ("Miscellaneous")  of a Pok�mon structure.
 * Call DecryptPokemon() first or the substructure data will be encrypted.
 * @param struct Pokemon* pkm The Pok�mon to get a substructure of.
 * @return struct PokemonSubstruct0* The substructure.
*/
// struct PokemonSubstruct3* GetPokemonSubstruct3(struct Pokemon* pkm);

/**
 * Gets the third substructure ("Miscellaneous") of a core Pok�mon structure.
 * Call DecryptBoxPokemon()  first or the substructure data will be encrypted.
 * @param struct BoxPokemon* pkm The BoxPokemon to get the substructure of.
 * @return struct PokemonSubstruct3* The substructure.
*/
// struct PokemonSubstruct3* GetBoxPokemonSubstruct3(struct BoxPokemon* pkm);

/**
 * Calculates the checksum for an R/S-specific Enigma Berry structure in SaveBlock1.
 * @param struct EnigmaBerry* enigmaBerry The R/S-specific Enigma Berry to calculate the checksum for.
 * @return u32 The checksum.
*/
// u32 CalculateEnigmaBerryChecksumRS(struct EnigmaBerry* enigmaBerry);

/**
 * Calculates the checksum for an FR/LG/Emerald-specific Enigma Berry structure in SaveBlock1.
 * @param struct EnigmaBerryFRLGE* enigmaBerry The FR/LG/Emerald-specific Enigma Berry to calculate the checksum for.
 * @return u32 The checksum.
*/
// u32 CalculateEnigmaBerryChecksumFRLGE(struct EnigmaBerryFRLGE* enigmaBerry);

/**
 * Calculates the checksum for an unspecified Enigma Berry structure in SaveBlock1. (detected by current game)
 * @param void* enigmaBerry The Enigma Berry structure to calculate the checksum for.
 * @return u32 The checksum.
 */
// u32 CalculateEnigmaBerryChecksum(void* enigmaBerry);

/**
 * Calculates the checksum for a RAM script structure in SaveBlock1.
 * @param struct RamScript* ramScript The RAM script structure to calculate the checksum for.
 * @return u32 The checksum.
 */
u32 CalculateRamScriptChecksum1(struct RamScript* ramScript);