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
typedef enum {
	EActionEffect_NONE, 		//none()							-> does nothing
	EActionEffect_PHYSDMG,		//physDmg(base,mult) 				-> damage = (user.atk * 2 * mult + base) - target.def
	EActionEffect_PHYSDMGSTAT, 	//physDmgStat(base,mult,stat,val) 	-> same formula as ^. Set stat to val
	EActionEffect_SETSTAT, 		//setStat(s1,v1,s2,v2) 				-> set stat if id !=0
	EActionEffect_MAGICDMG,		//magicDmg(base,mult) 				-> damage = (user.mag * 2 * mult + base) - target.mag
	EActionEffect_MAGICDMGBUFF, //magicDmgBuff(base,mult,buff,v) 	-> same formula as ^. Add v to buff
	EActionEffect_MAGICDMGPIERCE,//magicDmgPierce(base,mult) 		-> damage = (user.mag * 2 * mult + base)
	EActionEffect_PERCDMG,		//percDmg(perc)						-> damage = target.hp * perc / 100
	EActionEffect_BUFF, 		//buff(buff,v) 						-> Add v to buff
	EActionEffect_REVIVE, 		//revive(perc)						-> sets HP to perc% to battler with 0HP
} EActionEffect;
typedef enum {
	EAttrib_ATK,
	EAttrib_DEF,
	EAttrib_MAG,
	EAttrib_SPD,
	EAttrib__MAX
}EAttrib;

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
	EActionEffect effectId;
	s16 effectParam1;
	s16 effectParam2;
	s16 effectParam3;
	s16 effectParam4;
	// visuals
	u16 visualsIdx;
} RPG_DataSkill;
const RPG_DataSkill DATA_SKILLS[28] = {
	// NAME			DESCRIPTION					BATT MENU
	// Basic
	{ "Attack", 	"Deals basic damage.",		true,false,0,0,		EActionScope_ENEMY,		EActionEffect_PHYSDMG,1,10,0,0,			0}, //1		physDmg(base,mult)
	{ "Guard", 		"Halves incoming damage.",	true,false,0,0,		EActionScope_USER,		EActionEffect_NONE,0,0,0,0,				0}, //2		none()
	// SKILLS
	{ "Critical", 	"Boosted basic attack.",	true,false,2,0,		EActionScope_ENEMY,		EActionEffect_PHYSDMG,1,15,0,0,			0}, //3		physDmg(base,mult)
	{ "Tackle", 	"Basic hit, adds STUN.",	true,false,2,0,		EActionScope_ENEMY,		EActionEffect_PHYSDMGSTAT,1,7,EBattlerState_STUN,1,		0}, //4		physDmgStat(base,mult,stat,val)
	{ "Provoke", 	"Gets foes attention.",		true,false,2,0,		EActionScope_USER,		EActionEffect_SETSTAT,EBattlerState_PROVOKE,0,-1,0,			0}, //5		setStat(s1,v1,s2,v2)
	{ "Laser", 		"Piercing magic damage.",	true,false,4,0,		EActionScope_ENEMY,		EActionEffect_MAGICDMGPIERCE,12,16,0,0,	0}, //6		magicDmgPierce(base,mult)
	// FIRE MAGIC - spends rubies
	{ "Fire", 		"Low fire damage. -ATK.",	true,false,4,0,		EActionScope_ENEMY,		EActionEffect_MAGICDMGBUFF,24,10,EAttrib_ATK,-1,	0}, //7		magicDmgBuff(base,mult,buff,v)
	{ "Flame", 		"Fire damage to all.",		true,false,8,0,		EActionScope_ENEMIES,	EActionEffect_MAGICDMG,20,10,0,0,		0}, //8		magicDmg(base,mult)
	{ "Inferno", 	"Strong fire damage.",		true,false,12,0,	EActionScope_ENEMY,		EActionEffect_MAGICDMG,48,10,0,0,		0}, //9		magicDmg(base,mult)
	{ "Burn", 		"Causes BURN.",				true,false,6,0,		EActionScope_ENEMY,		EActionEffect_SETSTAT,EBattlerState_BURN,-1,0,0,		0}, //10	setStat(s1,v1,s2,v2)
	// WATER MAGIC - spends sapphires
	{ "Ice", 		"Low water damage. -DEF",	true,false,4,0,		EActionScope_ENEMY,		EActionEffect_MAGICDMGBUFF,24,10,EAttrib_DEF,-1,	0}, //11	magicDmgBuff(base,mult,buff,v)
	{ "Winter", 	"Water damage to all.",		true,false,8,0,		EActionScope_ENEMIES,	EActionEffect_MAGICDMG,20,10,0,0,		0}, //12	magicDmg(base,mult)
	{ "Blizzard", 	"Strong water damage.",		true,false,12,0,	EActionScope_ENEMY,		EActionEffect_MAGICDMG,48,10,0,0,		0}, //13	magicDmg(base,mult)
	{ "Freeze", 	"Causes FROZEN.",			true,false,6,0,		EActionScope_ENEMY,		EActionEffect_SETSTAT,EBattlerState_FROZEN,3,-1,0,		0}, //14	setStat(s1,v1,s2,v2)
	// WIND MAGIC - spends emeralds
	{ "Windcut", 	"Low wind damage. -SPD.",	true,false,4,0,		EActionScope_ENEMY,		EActionEffect_MAGICDMGBUFF,24,10,EAttrib_SPD,-1,	0}, //15	magicDmgBuff(base,mult,buff,v)
	{ "Storm", 		"Wind damage to all.",		true,false,8,0,		EActionScope_ENEMIES,	EActionEffect_MAGICDMG,20,10,0,0,		0}, //16	magicDmg(base,mult)
	{ "Thunder", 	"Strong wind damage.",		true,false,12,0,	EActionScope_ENEMY,		EActionEffect_MAGICDMG,48,10,0,0,		0}, //17	magicDmg(base,mult)
	{ "Evasion", 	"Causes EVASION.",			true,false,6,0,		EActionScope_ALLY,		EActionEffect_SETSTAT,EBattlerState_EVASION,2,-1,0,		0}, //18	setStat(s1,v1,s2,v2)
	// HOLY MAGIC - spends citrines
	{ "Missile", 	"Magic damage to all.",		true,true,4,0,		EActionScope_ENEMIES,	EActionEffect_MAGICDMG,16,15,0,0,		0}, //19	magicDmg(base,mult)
	{ "Heal", 		"Heals some HP.",			true,true,4,0,		EActionScope_ALLY,		EActionEffect_MAGICDMGPIERCE,-12,20,0,0,0}, //20	magicDmgPierce(base,mult)
	{ "Regen", 		"Heals 1/16HP per turn.",	true,false,10,0,	EActionScope_ALLIES,	EActionEffect_SETSTAT,EBattlerState_REGEN,2,-1,0,		0}, //21	setStat(s1,v1,s2,v2)
	{ "Remedy", 	"Cures BURN and FROZEN.",	true,true,6,0,		EActionScope_ALLY,		EActionEffect_SETSTAT,EBattlerState_BURN,0,EBattlerState_FROZEN,0,		0}, //22	setStat(s1,v1,s2,v2)
	{ "Revive", 	"Cures KO.",				true,true,10,0,		EActionScope_ALLY,		EActionEffect_REVIVE,25,0,0,0,			0}, //23	revive(perc)
	{ "Nuke", 		"Massive magic damage.",	true,true,40,-10,	EActionScope_ENEMY,		EActionEffect_MAGICDMGBUFF,60,20,EAttrib_MAG,-1,			0}, //24	magicDmgBuff(perc)
	// STATUS MAGIC (?) - I guess also spends citrines? or maybe onyxes, and they're rare
	{ "Strong", 	"Powers up ATK.",			true,false,12,0,	EActionScope_ALLY,		EActionEffect_BUFF,EAttrib_ATK,1,-1,0,				0}, //25	buff(buff,v)
	{ "Durable", 	"Powers up DEF.",			true,false,12,0,	EActionScope_ALLY,		EActionEffect_BUFF,EAttrib_DEF,1,-1,0,				0}, //26	buff(buff,v)
	{ "Sage", 		"Powers up MAG.",			true,false,12,0,	EActionScope_ALLY,		EActionEffect_BUFF,EAttrib_MAG,1,-1,0,				0}, //27	buff(buff,v)
	{ "Swift", 		"Powers up SPD.",			true,false,12,0,	EActionScope_ALLY,		EActionEffect_BUFF,EAttrib_SPD,1,-1,0,				0}, //28	buff(buff,v)
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
	u16 effectParam3;
	u16 effectParam4;
	// visuals
	u16 visualsIdx;
} RPG_DataItem;
const RPG_DataItem DATA_ITEMS[10] = {
	{ "Potion","Recover 30% HP." 		,true,true,true,0,	EActionScope_ALLY,EActionEffect_PERCDMG,-30,0,0,0, 	0 },	//percDmg(perc)
	{ "HiPotion","Recover 60% HP." 		,true,true,true,0,	EActionScope_ALLY,EActionEffect_PERCDMG,-60,0,0,0, 	0 },	//percDmg(perc)
	{ "Elixir","Full recovery."			,true,true,true,0,	EActionScope_ALLY,EActionEffect_PERCDMG,-100,0,0,0, 0 },	//percDmg(perc)
	{ "Remedy","Cures BURN."			,true,true,true,0,	EActionScope_ALLY,EActionEffect_SETSTAT,EBattlerState_BURN,0,-1,0, 		0 },	//setStat(s1,v1,s2,v2)
	{ "Thaw","Cures FROZEN."			,true,true,true,0,	EActionScope_ALLY,EActionEffect_SETSTAT,EBattlerState_FROZEN,0,-1,0, 	0 },	//setStat(s1,v1,s2,v2)
	{ "Reviver","Cures KO." 			,true,true,true,0,	EActionScope_ALLY,EActionEffect_REVIVE,100,0,0,0, 	0 },	//revive(perc)
	{ "Ruby","Powers FIRE magic."		,false,false,true,0,EActionScope_USER,EActionEffect_NONE,0,0,0,0,		0 },	//none()
	{ "Sapphire","Powers WATER magic."	,false,false,true,0,EActionScope_USER,EActionEffect_NONE,0,0,0,0,		0 },	//none()
	{ "Emerald","Powers WIND magic."	,false,false,true,0,EActionScope_USER,EActionEffect_NONE,0,0,0,0,		0 },	//none()
	{ "Citrine","Powers HOLY magic."	,false,false,true,0,EActionScope_USER,EActionEffect_NONE,0,0,0,0,		0 },	//none()
	{ "Onyx","Powers NATURE magic."		,false,false,true,0,EActionScope_USER,EActionEffect_NONE,0,0,0,0,		0 },	//none()
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
u8 LEARNINGS_0[12] = {
	0,	20, //Heal
	5,	19, //Missile 
	10,	22, //Remedy
	20,	21, //Regen
	25,	23, //Revive
	35,	24, //Nuke
};
u8 LEARNINGS_1[24] = {
	0,	7,  //Fire
	0,	11, //Ice
	4,	15, //Windcut
	8,	10, //Burn
	12,	8,  //Flame
	16,	12, //Winter
	20,	16, //Storm
	24,	18, //Evasion
	28,	14, //Freeze
	32,	9,  //Inferno
	36,	13, //Blizzard
	40,	17, //Thunder
};
u8 LEARNINGS_2[8] = {
	0,	3, 
	0,	4, 
	5,	5, 
	10,	6,
};
const RPG_DataHero DATA_ACTORS[3] = {
	//c  w  b  name         hp mp  a d m s     	hp mp a d m s
	{ 0, 1, 0, "Lin",		20, 5, 5,6,4,8,		5,2, 2,1,1,2,	6,&LEARNINGS_0 },
	{ 2, 3, 1, "Rayne",	12, 8, 2,4,8,6,			4,3, 1,1,2,2,	12,&LEARNINGS_1 },
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
