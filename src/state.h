#pragma once
#include <resources.h>
#include "database.h"

#pragma region State

typedef struct {
	char* name[9];
	char level; // 127 max but probably not even!
	u16 exp; // 1000exp per level
	u16 currHp,currMp;
	u16 hp,mp,atk,def,mag,spd;
	// states
	// skills
	// equip
} RPG_StateHero;
typedef struct {
	char* name;
	char battlerIdx;
	bool hidden;
	u16 x, y;
	char sprIdx;
	// 
	u16 currHp,currMp;
	u16 hp,mp,atk,def,mag,spd; // Calculated value (only when needed)
	u16 origHp,origMp,origAtk,origDef,origMag,origSpd; // original value. enemy=database, hero=state value
	// states
} RPG_StateBattler;

typedef struct {
	char* members[MAX_PARTY]; // 3
	int gold; // 4
	u8* inventoryItemId[MAX_INVENTORY]; // 60
	u8* inventoryItemQty[MAX_INVENTORY];// 60
} RPG_StateParty; //3 + 4 + 120

typedef struct {
	int id;
	u16 pal;
	int moveSpeed; //32
	int boxSize; //96
	int ox; //0
	int oy; //-96
	int posX; // 180<<MAP_SCROLL_DEPTH
	int posY; // 150<<MAP_SCROLL_DEPTH
	int moveX;
	int moveY;
	int direction;
	int frame;
	int animCount;
	bool updateFrame;
	bool solid;
	bool hidden;
	char currPageIdx;
} RPG_StateCharacter;

typedef struct {
	int id;
	int scrollX;
	int scrollY;
	const u16* collisionData;
	u16 collisionW;
	u16 collisionH;
	bool worldmap;
	RPG_StateCharacter* events; //54bytes*32
	u16 numEvents;
	bool refreshEvents;
} RPG_StateMap;

u16 tileIdx = TILE_USER_INDEX;
// Display
Map* bga;
u16 bgaIdx = 0;
Sprite* mapSprites[MAX_SCENE_SPRITES];
u16 mapSprites_c = 0;
// State
RPG_StateHero heroes[MAX_HEROS]; 	// 84
RPG_StateParty party;				// 127
RPG_StateMap map;				 	// 1750
RPG_StateCharacter player;			// 54
u16 stateFlags[MAX_FLAGS];			// 512

void state_levelUpCharacter(u16 id) {
	u16 a = random();
	// 0123456789ABCDEF
	// HHHHMMMMaaddmmss
	u16 hpRand = (a & 0b1111000000000000) >> 12;
	u16 mpRand = (a & 0b0000111100000000) >> 8;
	u16 atkRand= (a & 0b0000000011000000) >> 6;
	u16 defRand= (a & 0b0000000000110000) >> 4;
	u16 magRand= (a & 0b0000000000001100) >> 2;
	u16 spdRand= (a & 0b0000000000000011);
	heroes[id].hp += DATA_HEROS[id].hpPlus + hpRand;
	heroes[id].mp += DATA_HEROS[id].mpPlus + mpRand;
	heroes[id].atk += DATA_HEROS[id].atkPlus + atkRand;
	heroes[id].def += DATA_HEROS[id].defPlus + defRand;
	heroes[id].mag += DATA_HEROS[id].magPlus + magRand;
	heroes[id].spd += DATA_HEROS[id].spdPlus + spdRand;
}
void state_setupCharacter(u16 id, char avgLevel) {
	u16 l = strlen(DATA_HEROS[id].name);
	strncpy(heroes[id].name, DATA_HEROS[id].name, l);
	heroes[id].level = 1;
	heroes[id].exp = 1000 * (avgLevel - 1);
	heroes[id].hp = DATA_HEROS[id].hp;
	heroes[id].mp = DATA_HEROS[id].mp;
	heroes[id].atk = DATA_HEROS[id].atk;
	heroes[id].def = DATA_HEROS[id].def;
	heroes[id].mag = DATA_HEROS[id].mag;
	heroes[id].spd = DATA_HEROS[id].spd;
	while (heroes[id].level < avgLevel) {
		heroes[id].level += 1;
		state_levelUpCharacter(id);
	}
	heroes[id].currHp = heroes[id].hp;
	heroes[id].currMp = heroes[id].mp;
}
void state_initCharacters() {
	for (int i = 0; i < MAX_HEROS; i++) {
		state_setupCharacter(i, 50);
	}
}
void state_initParty() {
	party.members[0] = 0;
	party.members[1] = 1;
	party.members[2] = 2;
	party.gold = 100;
	for (int i = 0; i < MAX_INVENTORY; i++) {
		party.inventoryItemId[i] = 0;
		party.inventoryItemQty[i] = 0;
	}
}

void state_init() {
	for (int i = 0; i < MAX_FLAGS; i++) {
		stateFlags[i] = 0;
	}
	state_initParty();
	state_initCharacters();
	map_init(0);
	character_init(&player, player.id);
	character_teleport(&player, 180, 150);
	player.pal = PAL2;
}

void set_flag(u16 id, u16 value) {
	stateFlags[id] = value;
	map.refreshEvents = true;
}

#pragma endregion
