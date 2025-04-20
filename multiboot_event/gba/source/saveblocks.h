/*
 * Example Gen3-multiboot payload by slipstream/RoL 2017.
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 *
 * saveblocks.h: describes saveblock structures for all of Gen 3 (yay!)
 */

// Most of the structures come from pokeruby, FR/LG changes come from my own research / the firered IDB on pokecommunity
#include "savestructs.h"

typedef struct
{
    /*0x00*/ struct Coords16 pos;
    /*0x04*/ struct WarpData location;
    /*0x0C*/ struct WarpData warp[4];
    /*0x2C*/ u16 battleMusic;
    /*0x2E*/ u8 weather;
    /*0x2F*/ u8 filler_2F;
    /*0x30*/ u8 flashUsed;
    /*0x32*/ u16 mapDataId;
    /*0x34*/ u16 mapView[0x100];
    /*0x234*/ u8 playerPartyCount;
    /*0x238*/ struct Pokemon playerParty[6];
    /*0x490*/ u32 money;
    /*0x494*/ u16 coins;
    /*0x496*/ u16 registeredItem; // registered for use with SELECT button
    /*0x498*/ struct ItemSlot pcItems[50];
    /*0x560*/ struct ItemSlot bagPocket_Items[20];
    /*0x5B0*/ struct ItemSlot bagPocket_KeyItems[20];
    /*0x600*/ struct ItemSlot bagPocket_PokeBalls[16];
    /*0x640*/ struct ItemSlot bagPocket_TMHM[64];
    /*0x740*/ struct ItemSlot bagPocket_Berries[46];
    /*0x7F8*/ struct Pokeblock pokeblocks[40];
    /*0x938*/ u8 unk938[52];  // pokedex related
    /*0x96C*/ u16 berryBlenderRecords[3];
    /*0x972*/ u8 filler_972[0x6];
    /*0x978*/ u16 trainerRematchStepCounter;
    /*0x97A*/ u8 trainerRematches[100];
    /*0x9E0*/ struct ObjectEvent objectEvents[16];
    /*0xC20*/ struct ObjectEventTemplate objectEventTemplates[64];
    /*0x1220*/ u8 flags[0x120];
    /*0x1340*/ u16 vars[0x100];
    /*0x1540*/ u32 gameStats[50];
    /*0x1608*/ struct BerryTree berryTrees[128];
    /*0x1A08*/ struct SecretBaseRecord secretBases[20];
    /*0x2688*/ u8 playerRoomDecor[12];
    /*0x2694*/ u8 playerRoomDecorPos[12];
    /*0x26A0*/ u8 decorDesk[10];
    /*0x26AA*/ u8 decorChair[10];
    /*0x26B4*/ u8 decorPlant[10];
    /*0x26BE*/ u8 decorOrnament[30];
    /*0x26DC*/ u8 decorMat[30];
    /*0x26FA*/ u8 decorPoster[10];
    /*0x2704*/ u8 decorDoll[40];
    /*0x272C*/ u8 decorCushion[10];
    /*0x2736*/ u8 padding_2736[2];
    /*0x2738*/ TVShow tvShows[24];
    /*0x2A98*/ u8 filler_2A98[0x64];
    /*0x2AFC*/ u16 outbreakPokemonSpecies;
    /*0x2AFE*/ u8 outbreakLocationMapNum;
    /*0x2AFF*/ u8 outbreakLocationMapGroup;
    /*0x2B00*/ u8 outbreakPokemonLevel;
    /*0x2B01*/ u8 outbreakUnk1;
    /*0x2B02*/ u16 outbreakUnk2;
    /*0x2B04*/ u16 outbreakPokemonMoves[4];
    /*0x2B0C*/ u8 outbreakUnk4;
    /*0x2B0D*/ u8 outbreakPokemonProbability;
    /*0x2B0E*/ u16 outbreakUnk5;
    /*0x2B10*/ u8 filler_2B0E[0xC];
    /*0x2B1C*/ u16 unk2B1C[4];
    /*0x2B24*/ u8 filler_2B24[0x28];
    /*0x2B4C*/ struct MailStruct mail[16];
    /*0x2D8C*/ u8 filler_2D8C[0x8];
    /*0x2D94*/ OldMan oldMan;
    /*0x2DC0*/ u8 unk_2DC0[0x14];
    /*0x2DD4*/ struct EasyChatPair easyChatPairs[5]; //Dewford trend [0] and some other stuff
    /*0x2DFC*/ u8 filler_2DFC[0x100];
    /*0x2EFC*/ struct SB1_2EFC_Struct sb1_2EFC_struct[5];
    /*0x2F9C*/ u8 filler_2F9C[0xA0];
    /*0x303C*/ u8 filler_303C[0x38];
    /*0x3074*/ u8 filler_3074[0x42];
    /*0x30B6*/ u8 filler_30B6;
    /*0x30B7*/ u8 filler_30B7[0x59];
    /*0x3110*/ u8 giftRibbons[7];
    /*0x3117*/ u8 filler_311B[0x2D];
    /*0x3144*/ struct Roamer roamer;
    /*0x3158*/ u8 filler_3158[0x8];
    /*0x3160*/ struct EnigmaBerry enigmaBerry; // this is actually offset by 0x98 ...
    /*0x3690*/ struct RamScript ramScript;
    /*0x3A7C*/ u8 filler_3A7C[0x10];
    /*0x3A8C*/ u8 unk3A8C[52]; //pokedex related
} SaveBlock1_RS;

typedef struct // Don't rely on the commented offsets, they'll be wrong due to elements removed in FR/LG...
{
    /*0x00*/ struct Coords16 pos;
    /*0x04*/ struct WarpData location;
    /*0x0C*/ struct WarpData warp[4];
    /*0x2C*/ u16 battleMusic;
    /*0x2E*/ u8 weather;
    /*0x2F*/ u8 filler_2F;
    /*0x30*/ u8 flashUsed;
    /*0x32*/ u16 mapDataId;
//    /*0x34*/ u16 mapView[0x100]; // Not in fr/lg
    /*0x234*/ u8 playerPartyCount;
    /*0x238*/ struct Pokemon playerParty[6];
    /*0x490*/ u32 money;
    /*0x494*/ u16 coins;
    /*0x496*/ u16 registeredItem; // registered for use with SELECT button
    /*0x498*/ struct ItemSlot pcItems[30];
    /*0x560*/ struct ItemSlot bagPocket_Items[42];
    /*0x5B0*/ struct ItemSlot bagPocket_KeyItems[30];
    /*0x600*/ struct ItemSlot bagPocket_PokeBalls[13];
    /*0x640*/ struct ItemSlot bagPocket_TMHM[58];
    /*0x740*/ struct ItemSlot bagPocket_Berries[43];
//    /*0x7F8*/ struct Pokeblock pokeblocks[40]; // Not in fr/lg
    /*0x938*/ u8 unk938[52];  // pokedex related
    /*0x96C*/ u8 unk_62C[12];
    /*0x972*/ u8 filler_972[0x6];
    /*0x97A*/ u8 unk_63E[98];
    /*0x9E0*/ struct ObjectEvent objectEvents[16];
    /*0xC20*/ struct ObjectEventTemplate objectEventTemplates[64];
    /*0x1220*/ u8 flags[0x120];
    /*0x1340*/ u16 vars[0x100];
	/*0x1540*/ u32 gameStats[64]; // encrypted with saveblock2 xor-key
    struct QuestStory questlog[4];
    u8 messages[12][4];
	struct NPCState npc_states[0x10];
	u8 unk_2f10[112];
	struct DaycarePokemon daycare[2];
	u8 unk_3098[56];
	struct Roamer roamer;
	u8 unk_30e4[8];
	/*0x3160*/ struct EnigmaBerryFRLGE enigmaBerry;
	u8 unk_3120[0x1C0]; // 4 bytes of CRC16, then 444 bytes of unknown. Mystery Gift related.
	u8 unk_32E0[0x150]; // 4 bytes of CRC16, then 332 bytes of unknown. Mystery Gift related. "mevent_buffer_1"
	u8 unk_3430[0x150]; // 4 bytes of CRC16, then 332 bytes of unknown. Mystery Gift related. "mevent_buffer_2"
	u8 unk_368C[0x9C]; // padding? doesn't seem to be actually used
	struct RamScript ramScript;
	u8 unk_3A07[17];
	u8 pokemon_flags_2[52];
	u8 rivalName[8];
	u8 unk_3a54[128];
	u8 words[21][10];
	u8 unk_3ba6[570];
} __attribute__((aligned(1))) SaveBlock1_FRLG;

typedef struct // Don't rely on the commented offsets, they'll be wrong due to elements changed/added in Emerald...
{
    /*0x00*/ struct Coords16 pos;
    /*0x04*/ struct WarpData location;
    /*0x0C*/ struct WarpData warp[4];
    /*0x2C*/ u16 battleMusic;
    /*0x2E*/ u8 weather;
    /*0x2F*/ u8 filler_2F;
    /*0x30*/ u8 flashUsed;
    /*0x32*/ u16 mapDataId;
    /*0x34*/ u16 mapView[0x100];
    /*0x234*/ u8 playerPartyCount;
    /*0x238*/ struct Pokemon playerParty[6];
    /*0x490*/ u32 money;
    /*0x494*/ u16 coins;
    /*0x496*/ u16 registeredItem; // registered for use with SELECT button
    /*0x498*/ struct ItemSlot pcItems[50];
    /*0x560*/ struct ItemSlot bagPocket_Items[30];
    /*0x5D8*/ struct ItemSlot bagPocket_KeyItems[30];
    /*0x650*/ struct ItemSlot bagPocket_PokeBalls[16];
    /*0x690*/ struct ItemSlot bagPocket_TMHM[64];
    /*0x790*/ struct ItemSlot bagPocket_Berries[46];
    /*0x7F8*/ struct Pokeblock pokeblocks[40]; // every offset is shifted by 0x50 from here on thanks to changed bag-counts
    /*0x938*/ u8 unk938[52];  // pokedex related
    /*0x96C*/ u16 berryBlenderRecords[3];
    /*0x972*/ u8 filler_972[0x6];
    /*0x978*/ u16 trainerRematchStepCounter;
    /*0x97A*/ u8 trainerRematches[100];
    /*0x9E0*/ struct ObjectEvent objectEvents[16];
    /*0xC20*/ struct ObjectEventTemplate objectEventTemplates[64];
    /*0x1220*/ u8 flags[0x12C];
    /*0x1340*/ u16 vars[0x100]; // offsets shifted by 0x5C from here on thanks to added flags
    /*0x1540*/ u32 gameStats[64]; // encrypted with saveblock2 xor-key
    /*0x1608*/ struct BerryTree berryTrees[128]; // offsets shifted by 0x94 from here on thanks to added 14 gamestats
    /*0x1A08*/ struct SecretBaseRecord secretBases[20];
    /*0x2688*/ u8 playerRoomDecor[12];
    /*0x2694*/ u8 playerRoomDecorPos[12];
    /*0x26A0*/ u8 decorDesk[10];
    /*0x26AA*/ u8 decorChair[10];
    /*0x26B4*/ u8 decorPlant[10];
    /*0x26BE*/ u8 decorOrnament[30];
    /*0x26DC*/ u8 decorMat[30];
    /*0x26FA*/ u8 decorPoster[10];
    /*0x2704*/ u8 decorDoll[40];
    /*0x272C*/ u8 decorCushion[10];
    // /*0x2736*/ u8 padding_2736[2];
    /*0x2738*/ TVShow tvShows[24];
    /*0x2A98*/ u8 filler_2A98[0x64];
    /*0x2AFC*/ u16 outbreakPokemonSpecies; // offset by 0x94
    /*0x2AFE*/ u8 outbreakLocationMapNum;
    /*0x2AFF*/ u8 outbreakLocationMapGroup;
    /*0x2B00*/ u8 outbreakPokemonLevel;
    /*0x2B01*/ u8 outbreakUnk1;
    /*0x2B02*/ u16 outbreakUnk2;
    /*0x2B04*/ u16 outbreakPokemonMoves[4];
    /*0x2B0C*/ u8 outbreakUnk4;
    /*0x2B0D*/ u8 outbreakPokemonProbability;
    /*0x2B0E*/ u16 outbreakUnk5;
    /*0x2B10*/ u8 filler_2B0E[0xC];
    /*0x2B1C*/ u16 unk2B1C[4];
    /*0x2B24*/ u8 filler_2B24[0x28];
    /*0x2B4C*/ struct MailStruct mail[16]; // offset by 0x94
    /*0x2D8C*/ u8 filler_2D8C[0x8];
    /*0x2D94*/ OldMan oldMan;
    /*0x2DC0*/ u8 unk_2DC0[0x14];
    /*0x2DD4*/ struct EasyChatPair easyChatPairs[5]; //Dewford trend [0] and some other stuff
    // /*0x2DFC*/ u8 filler_2DFC[0x100];
    /*0x2EFC*/ struct SB1_2EFC_Struct sb1_2EFC_struct[12];
	u8 unk_3010[0x198]; // no idea if any of this is actually used.
    /*0x3110*/ u8 giftRibbons[7];
    /*0x3117*/ u8 filler_311B[0x2D];
    /*0x3144*/ struct Roamer roamer;
    /*0x3158*/ u8 filler_3158[0x8];
    /*0x3160*/ struct EnigmaBerryFRLGE enigmaBerry;
	u8 unk_322C[0x1C0]; // 4 bytes of CRC16, then 444 bytes of unknown. Mystery Gift related.
	u8 unk_33EC[0x150]; // 4 bytes of CRC16, then 332 bytes of unknown. Mystery Gift related. "mevent_buffer_1"
	u8 unk_353C[0x150]; // 4 bytes of CRC16, then 332 bytes of unknown. Mystery Gift related. "mevent_buffer_2"
	u8 unk_368C[0x9C]; // padding? doesn't seem to be actually used
    /*0x3690*/ struct RamScript ramScript;
    /*0x3A7C*/ u8 filler_3A7C[0x10];
    /*0x3A8C*/ u8 unk3A8C[52]; //pokedex related
} SaveBlock1_E;

// ---

struct SaveBlock2_Sub
{
    /*0x0000, 0x00A8*/ u8 filler_000[0x4AE];
    /*0x04AE, 0x0556*/ u8 var_4AE;
    /*0x04AF, 0x0557*/ u8 var_4AF;
    /*0x04B0, 0x0558*/ u16 var_4B0;
    /*0x04B2, 0x055A*/ u16 var_4B2;
    /*0x04B4, 0x055C*/ u16 var_4B4;
    /*0x04B6, 0x055E*/ u16 var_4B6;
    /*0x04B8, 0x0560*/ u8 filler_4B8[0x10];
    /*0x04C8, 0x0570*/ u16 var_4C8;
    /*0x04CA, 0x0572*/ u16 var_4CA;
    /*0x04CC, 0x0574*/ u8 filler_4CC[0x31C];
};

typedef struct
{
    /*0x00*/ u8 playerName[8];
    /*0x08*/ u8 playerGender; // MALE, FEMALE
    /*0x09*/ u8 specialSaveWarp;
    /*0x0A*/ u8 playerTrainerId[4];
    /*0x0E*/ u16 playTimeHours;
    /*0x10*/ u8 playTimeMinutes;
    /*0x11*/ u8 playTimeSeconds;
    /*0x12*/ u8 playTimeVBlanks;
    /*0x13*/ u8 optionsButtonMode;  // OPTIONS_BUTTON_MODE_[NORMAL/LR/L_EQUALS_A]
    /*0x14*/ u16 optionsTextSpeed:3; // OPTIONS_TEXT_SPEED_[SLOW/MID/FAST]
             u16 optionsWindowFrameType:5; // Specifies one of the 20 decorative borders for text boxes
             u16 optionsSound:1; // OPTIONS_SOUND_[MONO/STEREO]
             u16 optionsBattleStyle:1; // OPTIONS_BATTLE_STYLE_[SHIFT/SET]
             u16 optionsBattleSceneOff:1; // whether battle animations are disabled
             u16 regionMapZoom:1; // whether the map is zoomed in
    /*0x18*/ struct Pokedex pokedex;
    /*0x90*/ u8 filler_90[0x8];
    /*0x98*/ struct Time localTimeOffset;
    /*0xA0*/ struct Time lastBerryTreeUpdate;
    /*0xA8*/ struct SaveBlock2_Sub filler_A8;
} SaveBlock2_RS;

typedef struct
{
    /*0x00*/ u8 playerName[8];
    /*0x08*/ u8 playerGender; // MALE, FEMALE
    /*0x09*/ u8 specialSaveWarp;
    /*0x0A*/ u8 playerTrainerId[4];
    /*0x0E*/ u16 playTimeHours;
    /*0x10*/ u8 playTimeMinutes;
    /*0x11*/ u8 playTimeSeconds;
    /*0x12*/ u8 playTimeVBlanks;
    /*0x13*/ u8 optionsButtonMode;  // OPTIONS_BUTTON_MODE_[NORMAL/LR/L_EQUALS_A]
    /*0x14*/ u16 optionsTextSpeed:3; // OPTIONS_TEXT_SPEED_[SLOW/MID/FAST]
             u16 optionsWindowFrameType:5; // Specifies one of the 20 decorative borders for text boxes
             u16 optionsSound:1; // OPTIONS_SOUND_[MONO/STEREO]
             u16 optionsBattleStyle:1; // OPTIONS_BATTLE_STYLE_[SHIFT/SET]
             u16 optionsBattleSceneOff:1; // whether battle animations are disabled
             u16 regionMapZoom:1; // whether the map is zoomed in
    /*0x18*/ struct Pokedex pokedex;
    /*0x90*/ u8 filler_90[0x8];
    /*0x98*/ struct Time localTimeOffset;
    /*0xA0*/ struct Time lastBerryTreeUpdate;
    /*0xA8*/ struct SaveBlock2_Sub filler_A8;
	/*0x890*/ u8 unk_890[8];
	/*0x898*/ u8 mapdata[0x258];
	/*0xaf0*/ u16 field_af0;
	/*0xaf2*/ u16 field_af2;
	/*0xaf4*/ u16 field_af4;
	/*0xaf6*/ u16 field_af6;
	/*0xaf8*/ u8 unk_af8[0x428];
	/*0xf20*/ u32 xor_key;
} SaveBlock2_FRLG;

typedef struct
{
    /*0x00*/ u8 playerName[8];
    /*0x08*/ u8 playerGender; // MALE, FEMALE
    /*0x09*/ u8 specialSaveWarp;
    /*0x0A*/ u8 playerTrainerId[4];
    /*0x0E*/ u16 playTimeHours;
    /*0x10*/ u8 playTimeMinutes;
    /*0x11*/ u8 playTimeSeconds;
    /*0x12*/ u8 playTimeVBlanks;
    /*0x13*/ u8 optionsButtonMode;  // OPTIONS_BUTTON_MODE_[NORMAL/LR/L_EQUALS_A]
    /*0x14*/ u16 optionsTextSpeed:3; // OPTIONS_TEXT_SPEED_[SLOW/MID/FAST]
             u16 optionsWindowFrameType:5; // Specifies one of the 20 decorative borders for text boxes
             u16 optionsSound:1; // OPTIONS_SOUND_[MONO/STEREO]
             u16 optionsBattleStyle:1; // OPTIONS_BATTLE_STYLE_[SHIFT/SET]
             u16 optionsBattleSceneOff:1; // whether battle animations are disabled
             u16 regionMapZoom:1; // whether the map is zoomed in
    /*0x18*/ struct Pokedex pokedex;
    /*0x90*/ u8 filler_90[0x8];
    /*0x98*/ struct Time localTimeOffset;
    /*0xA0*/ struct Time lastBerryTreeUpdate;
	/*0xA8*/ u32 xor_key;
    /*0xAC*/ struct SaveBlock2_Sub filler_A8;
} SaveBlock2_E;

typedef union {
	// SaveBlock1_RS rs;
	// SaveBlock1_FRLG frlg;
	SaveBlock1_E e;
} SaveBlock1, *pSaveBlock1;

typedef union {
	// SaveBlock2_RS rs;
	// SaveBlock2_FRLG frlg;
	SaveBlock2_E e;
} SaveBlock2, *pSaveBlock2;

typedef struct PokemonStorage SaveBlock3, *pSaveBlock3;