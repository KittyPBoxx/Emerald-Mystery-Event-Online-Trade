/*
 * Example Gen3-multiboot payload by slipstream/RoL 2017.
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 *
 * saveblocks.h: describes structures used by saveblocks for all of Gen 3
 */

// Most of the structures come from pokeruby, FR/LG changes come from my own research / the firered IDB on pokecommunity

#define POKEMON_NAME_LENGTH 10
#define OT_NAME_LENGTH 7
#define TILE_SIZE_4BPP 32

struct Coords16
{
    s16 x;
    s16 y;
};

struct UCoords16
{
    u16 x;
    u16 y;
};

struct SecretBaseRecord
{
    u8 sbr_field_0; // ID?
    u8 sbr_field_1_0:4;
    u8 gender:1;
    u8 sbr_field_1_5:1;
    u8 sbr_field_2[7]; // 0xFF bytes?
    u8 trainerId[4]; // byte 0 is used for determining trainer class
    u16 sbr_field_e;
    u8 sbr_field_10;
    u8 sbr_field_11;
    u8 decorations[16];
    u8 sbr_field_22[16];
    u32 partyPersonality[6];
    u16 partyMoves[6 * 4];
    u16 partySpecies[6];
    u16 partyHeldItems[6];
    u8 partyLevels[6];
    u8 partyEVs[6];
};

typedef void (*TilesetCB)(void);

struct Tileset
{
    u8 isCompressed;
    u8 isSecondary;
    void *tiles;
    void *palettes;
    void *metatiles;
    void *metatileAttributes;
    TilesetCB callback;
};

struct MapData
{
    s32 width;
    s32 height;
    u16 *border;
    u16 *map;
    struct Tileset *primaryTileset;
    struct Tileset *secondaryTileset;
};

struct ObjectEventTemplate
{
    /*0x00*/ u8 localId;
    /*0x01*/ u8 graphicsId;
    /*0x02*/ u8 kind; // Always OBJ_KIND_NORMAL in Emerald.
    /*0x03*/ //u8 padding1;
    /*0x04*/ s16 x;
    /*0x06*/ s16 y;
    /*0x08*/ u8 elevation;
    /*0x09*/ u8 movementType;
    /*0x0A*/ u16 movementRangeX:4;
             u16 movementRangeY:4;
             //u16 padding2:8;
    /*0x0C*/ u16 trainerType;
    /*0x0E*/ u16 trainerRange_berryTreeId;
    /*0x10*/ const u8 *script;
    /*0x14*/ u16 flagId;
    /*0x16*/ //u8 padding3[2];
    /*size = 0x18*/
}; 

struct WarpEvent
{
    s16 x, y;
    s8 warpId;
    u8 mapGroup;
    u8 mapNum;
    u8 unk7;
};

struct CoordEvent
{
    s16 x, y;
    u8 unk4;
    u8 filler_5;
    u16 trigger;
    u16 index;
    u8 filler_A[0x2];
    u8 *script;
};

struct BgEvent
{
    s16 x, y;
    u8 unk4;
    u8 kind;
    s16 filler_6;
    u8 *script;
};

struct MapEvents
{
    u8 mapObjectCount;
    u8 warpCount;
    u8 coordEventCount;
    u8 bgEventCount;

    struct MapObjectTemplate *mapObjects;
    struct WarpEvent *warps;
    struct CoordEvent *coordEvents;
    struct BgEvent *bgEvents;
};

struct MapConnection
{
    u8 direction;
    u32 offset;
    u8 mapGroup;
    u8 mapNum;
};

struct MapConnections
{
    s32 count;
    struct MapConnection *connections;
};

struct MapHeader
{
    struct MapData *mapData;
    struct MapEvents *events;
    u8 *mapScripts;
    struct MapConnections *connections;
    u16 music;
    u16 mapDataId;
    u8 name;
    u8 cave;
    u8 weather;
    /* 0x17 */ u8 mapType;
    u8 filler_18;
    u8 escapeRope;
    u8 flags;
    u8 battleType;
};

struct ObjectEvent
{
    /*0x00*/ u32 active:1;
             u32 singleMovementActive:1;
             u32 triggerGroundEffectsOnMove:1;
             u32 triggerGroundEffectsOnStop:1;
             u32 disableCoveringGroundEffects:1;
             u32 landingJump:1;
             u32 heldMovementActive:1;
             u32 heldMovementFinished:1;
    /*0x01*/ u32 frozen:1;
             u32 facingDirectionLocked:1;
             u32 disableAnim:1;
             u32 enableAnim:1;
             u32 inanimate:1;
             u32 invisible:1;
             u32 offScreen:1;
             u32 trackedByCamera:1;
    /*0x02*/ u32 isPlayer:1;
             u32 hasReflection:1;
             u32 inShortGrass:1;
             u32 inShallowFlowingWater:1;
             u32 inSandPile:1;
             u32 inHotSprings:1;
             u32 hasShadow:1;
             u32 spriteAnimPausedBackup:1;
    /*0x03*/ u32 spriteAffineAnimPausedBackup:1;
             u32 disableJumpLandingGroundEffect:1;
             u32 fixedPriority:1;
             u32 hideReflection:1;
             //u32 padding:4;
    /*0x04*/ u8 spriteId;
    /*0x05*/ u8 graphicsId;
    /*0x06*/ u8 movementType;
    /*0x07*/ u8 trainerType;
    /*0x08*/ u8 localId;
    /*0x09*/ u8 mapNum;
    /*0x0A*/ u8 mapGroup;
    /*0x0B*/ u8 currentElevation:4;
             u8 previousElevation:4;
    /*0x0C*/ struct Coords16 initialCoords;
    /*0x10*/ struct Coords16 currentCoords;
    /*0x14*/ struct Coords16 previousCoords;
    /*0x18*/ u16 facingDirection:4; // current direction?
             u16 movementDirection:4;
             u16 rangeX:4;
             u16 rangeY:4;
    /*0x1A*/ u8 fieldEffectSpriteId;
    /*0x1B*/ u8 warpArrowSpriteId;
    /*0x1C*/ u8 movementActionId;
    /*0x1D*/ u8 trainerRange_berryTreeId;
    /*0x1E*/ u8 currentMetatileBehavior;
    /*0x1F*/ u8 previousMetatileBehavior;
    /*0x20*/ u8 previousMovementDirection;
    /*0x21*/ u8 directionSequenceIndex;
    /*0x22*/ u8 playerCopyableMovement; // COPY_MOVE_*
    /*0x23*/ //u8 padding2;
    /*size = 0x24*/
};

struct Berry
{
    const u8 name[7];
    u8 firmness;
    u16 size;
    u8 maxYield;
    u8 minYield;
    const u8 *description1;
    const u8 *description2;
    u8 stageDuration;
    u8 spicy;
    u8 dry;
    u8 sweet;
    u8 bitter;
    u8 sour;
    u8 smoothness;
};

struct EnigmaBerry
{
    struct Berry berry;
    u8 pic[(6 * 6) * TILE_SIZE_4BPP];
    u16 palette[16];
    u8 description1[45];
    u8 description2[45];
    u8 itemEffect[18];
    u8 holdEffect;
    u8 holdEffectParam;
    u32 checksum;
};

struct BattleEnigmaBerry
{
    u8 name[7];
    u8 holdEffect;
    u8 itemEffect[18];
    u8 holdEffectParam;
};

struct EnigmaBerryFRLGE {
	struct Berry berry; // 0x00
	u8 itemEffect[18]; // 0x1C
	u8 holdEffect; // 0x2E
	u8 holdEffectParam; // 0x2F
	u32 checksum; // 0x30
};

struct __attribute__((aligned(4))) BerryTree
{
    u8 berry;
    u8 stage:7;
    u8 growthSparkle:1;
    u16 secondsUntilNextStage;
    u8 berryYield;
    u8 regrowthCount:4;
    u8 watered1:1;
    u8 watered2:1;
    u8 watered3:1;
    u8 watered4:1;
};

struct PokemonSubstruct0
{
    u16 species;
    u16 heldItem;
    u32 experience;
    u8 ppBonuses;
    u8 friendship;
};

struct PokemonSubstruct1
{
    u16 moves[4];
    u8 pp[4];
};

struct PokemonSubstruct2
{
    u8 hpEV;
    u8 attackEV;
    u8 defenseEV;
    u8 speedEV;
    u8 spAttackEV;
    u8 spDefenseEV;
    u8 cool;
    u8 beauty;
    u8 cute;
    u8 smart;
    u8 tough;
    u8 sheen;
};

struct PokemonSubstruct3
{
 /* 0x00 */ u8 pokerus;
 /* 0x01 */ u8 metLocation;

 /* 0x02 */ u16 metLevel:7;
 /* 0x02 */ u16 metGame:4;
 /* 0x03 */ u16 pokeball:4;
 /* 0x03 */ u16 otGender:1;

 /* 0x04 */ u32 hpIV:5;
 /* 0x04 */ u32 attackIV:5;
 /* 0x05 */ u32 defenseIV:5;
 /* 0x05 */ u32 speedIV:5;
 /* 0x05 */ u32 spAttackIV:5;
 /* 0x06 */ u32 spDefenseIV:5;
 /* 0x07 */ u32 isEgg:1;
 /* 0x07 */ u32 altAbility:1;

 /* 0x08 */ u32 coolRibbon:3;
 /* 0x08 */ u32 beautyRibbon:3;
 /* 0x08 */ u32 cuteRibbon:3;
 /* 0x09 */ u32 smartRibbon:3;
 /* 0x09 */ u32 toughRibbon:3;
 /* 0x09 */ u32 championRibbon:1;
 /* 0x0A */ u32 winningRibbon:1;
 /* 0x0A */ u32 victoryRibbon:1;
 /* 0x0A */ u32 artistRibbon:1;
 /* 0x0A */ u32 effortRibbon:1;
 /* 0x0A */ u32 giftRibbon1:1;
 /* 0x0A */ u32 giftRibbon2:1;
 /* 0x0A */ u32 giftRibbon3:1;
 /* 0x0A */ u32 giftRibbon4:1;
 /* 0x0B */ u32 giftRibbon5:1;
 /* 0x0B */ u32 giftRibbon6:1;
 /* 0x0B */ u32 giftRibbon7:1;
 /* 0x0B */ u32 fatefulEncounter:5; // unused in Ruby/Sapphire, but the high bit must be set for Mew/Deoxys to obey in FR/LG/Emerald
};

union PokemonSubstruct
{
    struct PokemonSubstruct0 type0;
    struct PokemonSubstruct1 type1;
    struct PokemonSubstruct2 type2;
    struct PokemonSubstruct3 type3;
    u16 raw[6];
};

struct BoxPokemon
{
    u32 personality;
    u32 otId;
    u8 nickname[POKEMON_NAME_LENGTH];
    u8 language;
    u8 isBadEgg:1;
    u8 hasSpecies:1;
    u8 isEgg:1;
    u8 unused:5;
    u8 otName[OT_NAME_LENGTH];
    u8 markings;
    u16 checksum;
    u16 unknown;

    union
    {
        u32 raw[12];
        union PokemonSubstruct substructs[4];
    } secure;
};

struct Pokemon
{
    struct BoxPokemon box;
    u32 status;
    u8 level;
    u8 pokerus;
    u16 hp;
    u16 maxHP;
    u16 attack;
    u16 defense;
    u16 speed;
    u16 spAttack;
    u16 spDefense;
};

struct UnknownPokemonStruct
{
    u16 species;
    u16 heldItem;
    u16 moves[4];
    u8 level;
    u8 ppBonuses;
    u8 hpEV;
    u8 attackEV;
    u8 defenseEV;
    u8 speedEV;
    u8 spAttackEV;
    u8 spDefenseEV;
    u32 otId;
    u32 hpIV:5;
    u32 attackIV:5;
    u32 defenseIV:5;
    u32 speedIV:5;
    u32 spAttackIV:5;
    u32 spDefenseIV:5;
    u32 gap:1;
    u32 altAbility:1;
    u32 personality;
    u8 nickname[POKEMON_NAME_LENGTH + 1];
    u8 friendship;
};

struct BattlePokemon
{
 /* 0x00 */ u16 species;
 /* 0x02 */ u16 attack;
 /* 0x04 */ u16 defense;
 /* 0x06 */ u16 speed;
 /* 0x08 */ u16 spAttack;
 /* 0x0A */ u16 spDefense;
 /* 0x0C */ u16 moves[4];
 /* 0x14 */ u32 hpIV:5;
 /* 0x14 */ u32 attackIV:5;
 /* 0x15 */ u32 defenseIV:5;
 /* 0x15 */ u32 speedIV:5;
 /* 0x16 */ u32 spAttackIV:5;
 /* 0x17 */ u32 spDefenseIV:5;
 /* 0x17 */ u32 isEgg:1;
 /* 0x17 */ u32 altAbility:1;
 /* 0x18 */ s8 statStages[8];
 /* 0x20 */ u8 ability;
 /* 0x21 */ u8 type1;
 /* 0x22 */ u8 type2;
 /* 0x23 */ u8 unknown;
 /* 0x24 */ u8 pp[4];
 /* 0x28 */ u16 hp;
 /* 0x2A */ u8 level;
 /* 0x2B */ u8 friendship;
 /* 0x2C */ u16 maxHP;
 /* 0x2E */ u16 item;
 /* 0x30 */ u8 nickname[POKEMON_NAME_LENGTH + 1];
 /* 0x3B */ u8 ppBonuses;
 /* 0x3C */ u8 otName[8];
 /* 0x44 */ u32 experience;
 /* 0x48 */ u32 personality;
 /* 0x4C */ u32 status1;
 /* 0x50 */ u32 status2;
 /* 0x54 */ u32 otId;
};

struct BaseStats
{
 /* 0x00 */ u8 baseHP;
 /* 0x01 */ u8 baseAttack;
 /* 0x02 */ u8 baseDefense;
 /* 0x03 */ u8 baseSpeed;
 /* 0x04 */ u8 baseSpAttack;
 /* 0x05 */ u8 baseSpDefense;
 /* 0x06 */ u8 type1;
 /* 0x07 */ u8 type2;
 /* 0x08 */ u8 catchRate;
 /* 0x09 */ u8 expYield;
 /* 0x0A */ u16 evYield_HP:2;
 /* 0x0A */ u16 evYield_Attack:2;
 /* 0x0A */ u16 evYield_Defense:2;
 /* 0x0A */ u16 evYield_Speed:2;
 /* 0x0B */ u16 evYield_SpAttack:2;
 /* 0x0B */ u16 evYield_SpDefense:2;
 /* 0x0C */ u16 item1;
 /* 0x0E */ u16 item2;
 /* 0x10 */ u8 genderRatio;
 /* 0x11 */ u8 eggCycles;
 /* 0x12 */ u8 friendship;
 /* 0x13 */ u8 growthRate;
 /* 0x14 */ u8 eggGroup1;
 /* 0x15 */ u8 eggGroup2;
 /* 0x16 */ u8 ability1;
 /* 0x17 */ u8 ability2;
 /* 0x18 */ u8 safariZoneFleeRate;
 /* 0x19 */ u8 bodyColor;
};

struct BattleMove
{
    u8 effect;
    u8 power;
    u8 type;
    u8 accuracy;
    u8 pp;
    u8 secondaryEffectChance;
    u8 target;
    u8 priority;
    u32 flags;
};

struct PokemonStorage
{
 /* 0x00 */ u8 currentBox;
 /* 0x01 */ struct BoxPokemon boxes[14][30];
    u8 boxNames[14][9];
    u8 boxBackground[14];
};

struct WarpData
{
    s8 mapGroup;
    s8 mapNum;
    s8 warpId;
    s16 x, y;
};

struct ItemSlot
{
    u16 itemId;
    u16 quantity;
};

struct __attribute__((aligned(2))) Pokeblock
{
    u8 color;
    u8 spicy;
    u8 dry;
    u8 sweet;
    u8 bitter;
    u8 sour;
    u8 feel;
};

struct Roamer
{
    /*0x00*/ u32 ivs;
    /*0x04*/ u32 personality;
    /*0x08*/ u16 species;
    /*0x0A*/ u16 hp;
    /*0x0C*/ u8 level;
    /*0x0D*/ u8 status;
    /*0x0E*/ u8 cool;
    /*0x0F*/ u8 beauty;
    /*0x10*/ u8 cute;
    /*0x11*/ u8 smart;
    /*0x12*/ u8 tough;
    /*0x13*/ u8 active;
};

struct RamScriptData
{
    u8 magic;
    u8 mapGroup;
    u8 mapNum;
    u8 objectId;
    u8 script[995];
} __attribute__((aligned(1),packed));

struct RamScript
{
    u32 checksum;
    struct RamScriptData data;
} __attribute__((aligned(1),packed));

struct SB1_2EFC_Struct
{
    u8 unknown[0x20];
};

struct EasyChatPair
{
    u16 unk0_0:7;
    u16 unk0_7:7;
    u16 unk1_6:1;
    u16 unk2;
    u16 words[2];
}; /*size = 0x8*/

struct TVShowCommon {
    /*0x00*/ u8 var00;
    /*0x01*/ u8 var01;
};

struct TVShowFanClubLetter {
    /*0x00*/ u8 var00;
    /*0x01*/ u8 var01;
    /*0x02*/ u16 species;
    u8 pad04[12];
    /*0x10*/ u8 playerName[8];
    /*0x18*/ u8 var18;
};

struct TVShowRecentHappenings {
    /*0x00*/ u8 var00;
    /*0x01*/ u8 var01;
    /*0x02*/ u16 var02;
    u8 pad04[12];
    /*0x10*/ u8 var10[8];
    /*0x18*/ u8 var18;
    u8 pad19[10];
};

struct TVShowFanclubOpinions {
    /*0x00*/ u8 var00;
    /*0x01*/ u8 var01;
    /*0x02*/ u16 var02;
    /*0x04*/ u8 var04A:4;
    u8 var04B:4;
    /*0x04*/ u8 var05[8];
    /*0x0D*/ u8 var0D;
    /*0x0E*/ u8 var0E;
    /*0x0F*/ u8 var0F;
    /*0x10*/ u8 var10[8];
};

struct TVShowNameRaterShow {
    /*0x00*/ u8 var00;
    /*0x01*/ u8 var01;
    /*0x02*/ u16 species;
    /*0x04*/ u8 pokemonName[11];
    /*0x0F*/ u8 trainerName[11];
    /*0x1A*/ u8 random;
    /*0x1B*/ u8 random2;
    /*0x1C*/ u16 var1C;
    /*0x1E*/ u8 language;
    /*0x1F*/ u8 var1F;
};

struct TVShowMassOutbreak {
    /*0x00*/ u8 var00;
    /*0x01*/ u8 var01;
    /*0x02*/ u8 var02;
    /*0x03*/ u8 var03;
    /*0x04*/ u16 moves[4];
    /*0x0C*/ u16 species;
    /*0x0E*/ u16 var0E;
    /*0x10*/ u8 locationMapNum;
    /*0x11*/ u8 locationMapGroup;
    /*0x12*/ u8 var12;
    /*0x13*/ u8 probability;
    /*0x14*/ u8 level;
    /*0x15*/ u8 var15;
    /*0x16*/ u16 var16;
    /*0x18*/ u8 var18;
    u8 pad19[11];
};

typedef union TVShow {
    struct TVShowCommon common;
    struct TVShowFanClubLetter fanclubLetter;
    struct TVShowRecentHappenings recentHappenings;
    struct TVShowFanclubOpinions fanclubOpinions;
    struct TVShowNameRaterShow nameRaterShow;
    struct TVShowMassOutbreak massOutbreak;
} TVShow;

struct __attribute__((aligned(4))) MailStruct
{
    /*0x00*/ u16 words[9];
    /*0x12*/ u8 playerName[8];
    /*0x1A*/ u8 trainerId[4];
    /*0x1E*/ u16 species;
    /*0x20*/ u16 itemId;
};

struct UnkMauvilleOldManStruct
{
    u8 unk_2D94;
    u8 unk_2D95;
    /*0x2D96*/ u16 mauvilleOldMan_ecArray[6];
    /*0x2DA2*/ u16 mauvilleOldMan_ecArray2[6];
    /*0x2DAE*/ u8 playerName[8];
    /*0x2DB6*/ u8 filler_2DB6[0x3];
    /*0x2DB9*/ u8 playerTrainerId[4];
    u8 unk_2DBD;
	/* size = 0x2C */
};

struct UnkMauvilleOldManStruct2
{
	u8 filler0;
	u8 unk1;
	u8 unk2;
	u16 mauvilleOldMan_ecArray[10];
	u16 mauvilleOldMan_ecArray2[6];
    u8 fillerF[0x2];
	/* size = 0x2C */
};

typedef union OldMan {
	struct UnkMauvilleOldManStruct oldMan1;
	struct UnkMauvilleOldManStruct2 oldMan2;
} OldMan;

struct QuestStoryNPC {
	u16 bitfield;
	u8 direction;
	u8 height;
	u8 type_id;
	u8 running_behaviour_or_picture_id;
	u8 is_trainer;
	u8 local_id;
	u8 local_mapnumber;
	u8 local_mapbank;
	u16 x;
	u16 y;
	u8 sight_distance;
	u8 role_from;
	u8 unknown_decrement_on_step;
	u8 unk_11;
	u16 padding_12;
};

struct QuestStory {
	u8 active;
	u8 bank;
	u8 map;
	u8 warpId;
	u16 x;
	u16 y;
	struct QuestStoryNPC npc[0x10];
	u8 unk_148[0x51f];
};

struct NPCState {
	u8 bitfield;
	u8 obj_anim_and_vis_control;
	u8 unk_2;
	u8 unk_3;
	u8 oamid;
	u8 type_id;
	u8 running_behaviour_or_picture_id;
	u8 is_trainer;
	u8 local_id;
	u8 local_mapnumber;
	u8 local_mapbank;
	u8 height;
	struct Coords16 stay_around;
	struct Coords16 to;
	struct Coords16 from;
	u8 direction;
	u8 movement_area;
	u8 objid_surfing;
	u8 objid_1B;
	u8 idx_movement_behaviour;
	u8 sight_distance;
	u8 role_to;
	u8 role_from;
	u8 unk_20;
	u8 unknown_decrement_on_step;
	u8 unk_22;
	u8 unk_23;
};

struct DaycarePokemon {
	struct BoxPokemon pokemon;
	u8 unk_50[56];
	u32 steps;
};


struct Time
{
    /*0x00*/ s16 days;
    /*0x02*/ s8 hours;
    /*0x03*/ s8 minutes;
    /*0x04*/ s8 seconds;
};

struct Pokedex
{
    /*0x00*/ u8 order;
    /*0x01*/ u8 unknown1;
    /*0x02*/ u8 nationalMagic; // must equal 0xDA in order to have National mode
    /*0x03*/ u8 unknown2;
    /*0x04*/ u32 unownPersonality; // set when you first see Unown
    /*0x08*/ u32 spindaPersonality; // set when you first see Spinda
    /*0x0C*/ u32 unknown3;
    /*0x10*/ u8 owned[52];
    /*0x44*/ u8 seen[52];
};