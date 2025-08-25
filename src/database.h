#pragma once
#include <resources.h>

typedef enum {
	EBattlerState_STUN,
    EBattlerState_BURN,
    EBattlerState_FROZEN,
    EBattlerState_PROVOKE,
    EBattlerState_EVASION,
    EBattlerState_REGEN,
	EBattlerState_CONFUSE,
	EBattlerState_BERSERK
} EBattlerState;
typedef enum {
	EActionScope_USER,
	EActionScope_ALLY,
	EActionScope_ALLIES,
	EActionScope_ENEMY,
	EActionScope_ENEMIES,
	EActionScope_EVERYONE
} EActionScope;

typedef struct {
	u16 characterIdx;
	u16 characterIdxWmap;
	u16 battlerIdx;
	const char* name;
	u16 hp,mp,atk,def,mag,spd;
	u16 hpPlus,mpPlus,atkPlus,defPlus,magPlus,spdPlus;
	//
	u8 numLearnings;
	u8* learnings; // lv,id,lv,id ...
} RPG_DataHero;
typedef struct {
	u16 battlerIdx;
	const char* name;
	u16 hp,mp,atk,def,mag,spd;
} RPG_DataEnemy;
typedef struct {
	const char* name;
	const char* desc;
	// use conditions
	bool canUseInBattle;
	bool canUseInMenu;
	u16 mpCost;
	u8 priority;
	// effect
	u8 scope;
	u16 effectId; //0:physDmg(base,mult) 1:magicDmg(base,mult) 2:percDmg(perc,useMax)
	u16 effectParam1;
	u16 effectParam2;
	// visuals
	u16 visualsIdx;
} RPG_DataSkill;
const RPG_DataSkill DATA_SKILLS[26] = {
	// NAME			DESCRIPTION					BATT MENU  
	// Basic
	{ "Attack", 	"Deals basic damage.",		true,false,0,0,	EActionScope_ENEMY,0,1,10,		0}, //1
	{ "Guard", 		"Halves incoming damage.",	true,false,0,0,	EActionScope_USER,0,0,0,		0}, //2
	// SKILLS
	{ "Critical", 	"Boosted basic attack.",	true,false,1,0,	EActionScope_ENEMY,0,1,12,		0}, //3
	{ "Tackle", 	"Basic hit, adds STUN.",	true,false,1,0,	EActionScope_ENEMY,0,1,7,		0}, //4
	{ "Provoke", 	"Gets foes attention.",		true,false,1,0,	EActionScope_USER,0,0,0,		0}, //5
	{ "Laser", 		"Piercing magic damage.",	true,false,2,0,	EActionScope_ENEMY,1,12,10,		0}, //6
	// FIRE MAGIC
	{ "Fire", 		"Basic fire damage.",		true,false,2,0,	EActionScope_ENEMY,1,24,10,		0}, //7
	{ "Flame", 		"Fire damage to all.",		true,false,4,0,	EActionScope_ENEMIES,1,24,10,	0}, //8
	{ "Inferno", 	"Strong fire damage.",		true,false,6,0,	EActionScope_ENEMY,1,24,10,		0}, //9
	{ "Burn", 		"Causes BURN.",				true,false,3,0,	EActionScope_ENEMY,1,24,10,		0}, //10
	// WATER MAGIC
	{ "Ice", 		"Basic water damage.",		true,false,2,0,	EActionScope_ENEMY,1,24,10,		0}, //11
	{ "Winter", 	"Water damage to all.",		true,false,4,0,	EActionScope_ENEMIES,1,24,10,	0}, //12
	{ "Blizzard", 	"Strong water damage.",		true,false,6,0,	EActionScope_ENEMY,1,24,10,		0}, //13
	{ "Freeze", 	"Causes FROZEN.",			true,false,3,0,	EActionScope_ENEMY,1,24,10,		0}, //14
	// WIND MAGIC
	{ "Windcut", 	"Basic wind damage.",		true,false,2,0,	EActionScope_ENEMY,1,24,10,		0}, //15
	{ "Storm", 		"Wind damage to all.",		true,false,4,0,	EActionScope_ENEMIES,1,24,10,	0}, //16
	{ "Thunder", 	"Strong wind damage.",		true,false,6,0,	EActionScope_ENEMY,1,24,10,		0}, //17
	{ "Evasion", 	"Causes EVASION.",			true,false,3,0,	EActionScope_ALLY,1,24,10,		0}, //18
	// HOLY MAGIC
	{ "Heal", 		"Heals some HP.",			true,true,2,0,	EActionScope_ALLY,1,24,10,		0}, //19
	{ "Regen", 		"Heals 1/16HP per turn.",	true,false,5,0,	EActionScope_ALLIES,1,24,10,	0}, //20
	{ "Remedy", 	"Cures BURN and FROZEN.",	true,true,3,0,	EActionScope_ALLY,1,24,10,		0}, //21
	{ "Revive", 	"Cures KO.",				true,true,5,0,	EActionScope_ALLY,1,24,10,		0}, //22
	// STATUS MAGIC (?)
	{ "Strong", 	"Powers up ATK.",			true,false,6,0,	EActionScope_ALLY,1,24,10,		0}, //23
	{ "Durable", 	"Powers up DEF.",			true,false,6,0,	EActionScope_ALLY,1,24,10,		0}, //24
	{ "Sage", 		"Powers up MAG.",			true,false,6,0,	EActionScope_ALLY,1,24,10,		0}, //25
	{ "Swift", 		"Powers up SPD.",			true,false,6,0,	EActionScope_ALLY,1,24,10,		0}, //26
};
typedef struct {
	const char* name;
	const char* desc;
	// use condition
	bool canUseInBattle;
	bool canUseInMenu;
	bool consumeWhenUsed;
	u8 priority;
	// effect
	u8 scope;
	u16 effectId; //0:physDmg(base,mult) 1:magicDmg(base,mult) 2:percDmg(perc,useMax)
	u16 effectParam1;
	u16 effectParam2;
	// visuals
	u16 visualsIdx;
} RPG_DataItem;
const RPG_DataItem DATA_ITEMS[4] = {
	{ "Potion","Recover 30% HP." 	,true,true,true,0,	EActionScope_ALLY,2,30,1, 	0 },
	{ "HiPotion","Recover 60% HP." 	,true,true,true,0,	EActionScope_ALLY,2,60,1, 	0 },
	{ "Elixir","Full recovery."		,true,true,true,0,	EActionScope_ALLY,2,100,1, 	0 },
	{ "Reviver","Cures KO." 		,true,true,true,0,	EActionScope_ALLY,2,0,1, 	0 },
};
typedef struct {
	char condition;
	u16 compare;
	int value;
	/**/
	int characterIdx;
	bool solid;
	u16 pal;
	/**/
	char activation; // 0:trigger 1:touch 2:auto
	int* script;
	int scriptLen;
} RPG_DataEventPage;
typedef struct {
	u16 x;
	u16 y;
	RPG_DataEventPage* pages;
	u16 numPages;
} RPG_DataEvent;

typedef struct {
	TileSet* tileset;
	MapDefinition* bottomLayer;
	Palette* paletteA;
	Palette* paletteB;
	u16* collisionData;
	u16 collisionW;
	u16 collisionH;
	bool worldmap;
	const RPG_DataEvent* events;
	u16 numEvents;
} RPG_DataMap;

typedef struct {
	SpriteDefinition* graphic;
	int moveSpeed;
	int boxSize;
	int ox;
	int oy;
} RPG_DataCharacter;
typedef struct {
	SpriteDefinition* graphic;
} RPG_DataBattler;

const RPG_DataCharacter DATA_CHARS[6] = {
	{ &char1, 16, 4, 0, 4 },
	{ &char1_wmap, 16, 4, 0, 4 },
	{ &char1, 16, 4, 0, 4 },
	{ &char1_wmap, 16, 4, 0, 4 },
	{ &char3, 16, 4, 0, 4 },
	{ &char3_wmap, 16, 4, 0, 4 },
};
const RPG_DataBattler DATA_BATTLERS[4] = {
	{ &char1_battler },
	{ &char2_battler },
	{ &char3_battler },
	{ &worm_battler },
};
u8 LEARNINGS_0[8] = {0,15, 5,16, 10,17, 15,18};
u8 LEARNINGS_1[8] = {0,7, 5,8, 10,9, 15,10};
u8 LEARNINGS_2[8] = {0,3, 5,4, 10,5, 15,6};
const RPG_DataHero DATA_HEROS[3] = {
	//c  w  b  name         hp mp  a d m s     	hp mp a d m s
	{ 0, 1, 0, "Lin",		20, 5, 5,6,4,8,		5,2, 2,1,1,2,	4,&LEARNINGS_0 },
	{ 2, 3, 1, "Rayne",	12, 8, 2,4,8,6,			4,3, 1,1,2,2,	4,&LEARNINGS_1 },
	{ 4, 5, 2, "Greese",	40, 2, 9,9,2,4,		6,1, 2,2,1,1,	4,&LEARNINGS_2 },
};
const RPG_DataEnemy DATA_ENEMIES[1] = {
	//b				hp mp  a d m s
	{ 3, "Worm", 	30, 5, 6,3,1,4 },
};
const u16 DATA_FORMATION1[7] = {
	2, //size
	0, 80, 120, //id, x, y
	0, 80, 192, //id, x, y
};
const u16* DATA_FORMATIONS[1] = {
	&DATA_FORMATION1,
};

const u16* DATA_SCRIPT_EMPTY[1] = { 0 };
const u16* DATA_MAPS0_EVENT0_PAGE0[25] = { 
	CMD_MESSAGE, &DIALOGS[5], 	&DIALOGS[0],	0, 				&char1_face, PAL2, 0,
	CMD_MESSAGE, &DIALOGS[6], 	&DIALOGS[1], 	0, 				&char1_face, PAL2, 1,
	CMD_MESSAGE, &DIALOGS[2], 	&DIALOGS[3], 	&DIALOGS[4], 	&char1_face, PAL2, 0,
	CMD_SETFLAG, 0, 1,
	CMD_END,
};

const RPG_DataEventPage DATA_MAPS0_EVENT0_PAGE[2] = {
	{EVP_CMP_NONE,  0, 0, /**/ 4, true, PAL2, /**/ 0, DATA_MAPS0_EVENT0_PAGE0, 22},
	{EVP_CMP_EQUAL, 0, 1, /**/ 4, true, PAL3, /**/ 0, DATA_SCRIPT_EMPTY, 1},
};
const RPG_DataEventPage DATA_MAPS0_EVENT1_PAGE[2] = {
	{EVP_CMP_EQUAL, 0, 1, /**/ 5, true, PAL2, /**/ 0, DATA_SCRIPT_EMPTY, 1},
	{EVP_CMP_EQUAL, 0, 1, /**/ 5, true, PAL2, /**/ 0, DATA_SCRIPT_EMPTY, 1},
};
const RPG_DataEvent DATA_MAPS0_EVENT[2] = {
	{ 180, 120, DATA_MAPS0_EVENT0_PAGE, 2},
	{ 150, 172, DATA_MAPS0_EVENT1_PAGE, 2},
};
const RPG_DataMap DATA_MAPS[1] = {
	{ &ts_world, &tm_world, &pa_world, &pa_world, WORLD_COLL, WORLD_COLL_W, WORLD_COLL_H, true, DATA_MAPS0_EVENT, 2},
};
