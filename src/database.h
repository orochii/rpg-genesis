#pragma once
#include <resources.h>

typedef struct {
	u16 characterIdx;
	u16 characterIdxWmap;
	u16 battlerIdx;
	const char* name;
	u16 hp,mp,atk,def,mag,spd;
	u16 hpPlus,mpPlus,atkPlus,defPlus,magPlus,spdPlus;
} RPG_DataHero;
typedef struct {
	u16 battlerIdx;
	const char* name;
	u16 hp,mp,atk,def,mag,spd;
} RPG_DataEnemy;
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
	RPG_DataEvent* events;
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
const RPG_DataHero DATA_HEROS[3] = {
	//c  w  b  name         hp mp  a d m s     	hp mp a d m s
	{ 0, 1, 0, "Lin",		20, 5, 5,6,4,8,		5,2, 2,1,1,2 },
	{ 2, 3, 1, "Rayne",	12, 8, 2,4,8,6,			4,3, 1,1,2,2 },
	{ 4, 5, 2, "Greese",	40, 2, 9,9,2,4,		6,1, 2,2,1,1 },
};
const RPG_DataEnemy DATA_ENEMIES[1] = {
	//b				hp mp  a d m s
	{ 3, "Worm", 	30, 5, 6,3,1,4 },
};
const u16 DATA_FORMATION1[4] = {
	1, //size
	0, 80, 160, //id, x, y
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
