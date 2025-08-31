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
	char states[MAX_STATES];
	// skills
	u8 skills[MAX_SKILLS];
	// equip
	// last picked option
	u16 lastItemIdx;
	u16 lastSkillIdx;
} RPG_StateHero;
typedef enum {
	EBattlerAnim_IDLE,
	EBattlerAnim_ATTACK,
	EBattlerAnim_DOWN,
	EBattlerAnim_MOVE,
	EBattlerAnim_WEAK,
	EBattlerAnim_MAGIC,
	EBattlerAnim_ITEM,
	EBattlerAnim_VICTORY,
	EBattlerAnim_READY,
} EBattlerAnim;

typedef struct {
	// visuals
	char* name;
	char battlerIdx;
	bool hidden;
	bool isEnemy;
	u8 idx;
	u16 x, y;
	u16 baseX,baseY;
	u16 targetX,targetY,moveDuration;
	char sprIdx;
	// attributes
	u16 currHp,currMp;
	u16 hp,mp,atk,def,mag,spd; // Calculated value (only when needed)
	u16 origHp,origMp,origAtk,origDef,origMag,origSpd; // original value. enemy=database, hero=state value
	// states turns
	char states[MAX_STATES];
	char buffs[EAttrib__MAX];
	// action
	unsigned char actionBasic; // 0:attack 1:skill 2:item 3:guard
	unsigned char actionId; //b0:-- b1:skillId b2:itemId b3:--
	unsigned char actionTargetScope; // 0:user 1:ally 2:allies 3:enemy 4:enemies
	unsigned char actionTargetIdx;
	s16 actionSpeed;
	s16 actionSpeedTotal;
	u8 boosts;
	//
	char damageString[8];
	bool damageDisplay;
} RPG_StateBattler;

typedef struct {
	char members[MAX_PARTY]; // 3
	int gold; // 4
	u8 inventoryItemId[MAX_INVENTORY]; // 60
	u8 inventoryItemQty[MAX_INVENTORY];// 60
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
RPG_StateHero actors[MAX_ACTORS]; 	// 84
RPG_StateParty party;				// 127
RPG_StateMap map;				 	// 1750
RPG_StateCharacter player;			// 54
u16 stateFlags[MAX_FLAGS];			// 512

void state_levelUpCharacter(u16 id) {
	u16 a = random();
	// 0123456789ABCDEF
	// HHHHMMMMaaddmmss
	u16 hpRand = ((a & 0b1111000000000000) >> 12) >> 2;
	u16 mpRand = ((a & 0b0000111100000000) >> 8) != 0;
	u16 atkRand= ((a & 0b0000000011000000) >> 6) >> 1;
	u16 defRand= ((a & 0b0000000000110000) >> 4) >> 1;
	u16 magRand= ((a & 0b0000000000001100) >> 2) >> 1;
	u16 spdRand= ((a & 0b0000000000000011)) >> 1;
	actors[id].hp += DATA_ACTORS[id].hpPlus + hpRand;
	actors[id].mp += DATA_ACTORS[id].mpPlus + mpRand;
	actors[id].atk += DATA_ACTORS[id].atkPlus + atkRand;
	actors[id].def += DATA_ACTORS[id].defPlus + defRand;
	actors[id].mag += DATA_ACTORS[id].magPlus + magRand;
	actors[id].spd += DATA_ACTORS[id].spdPlus + spdRand;
	// Learnings
	for (int i = 0; i < DATA_ACTORS[id].numLearnings; i++) {
		u8 pos = i << 1;
		u8 level = DATA_ACTORS[id].learnings[pos];
		if (level <= actors[id].level) {
			u8 skillId = DATA_ACTORS[id].learnings[pos+1];
			u8 _idx = 0;
			while (_idx < MAX_SKILLS) {
				// Already learned
				if (actors[id].skills[_idx] == skillId) {
					_idx = 255;
				} else {
					// Slot is empty
					if (actors[id].skills[_idx] == 0) {
						actors[id].skills[_idx] = skillId;
						_idx = 255;
					} else {
						_idx++;
					}
				}
			}
		}
	}
}
void state_setupCharacter(u16 id, char avgLevel) {
	u16 l = strlen(DATA_ACTORS[id].name);
	strncpy(actors[id].name, DATA_ACTORS[id].name, l);
	actors[id].level = 1;
	actors[id].exp = 1000 * (avgLevel - 1);
	actors[id].hp = DATA_ACTORS[id].hp;
	actors[id].mp = DATA_ACTORS[id].mp;
	actors[id].atk = DATA_ACTORS[id].atk;
	actors[id].def = DATA_ACTORS[id].def;
	actors[id].mag = DATA_ACTORS[id].mag;
	actors[id].spd = DATA_ACTORS[id].spd;
	u8 _idx = 0;
	while (_idx < MAX_STATES) {
		actors[id].states[_idx] = 0;
		_idx++;
	}
	_idx = 0;
	while (_idx < MAX_SKILLS) {
		actors[id].skills[_idx] = 0;
		_idx++;
	}
	state_levelUpCharacter(id);
	while (actors[id].level < avgLevel) {
		actors[id].level += 1;
		state_levelUpCharacter(id);
	}
	actors[id].currHp = actors[id].hp;
	actors[id].currMp = actors[id].mp;
	actors[id].lastItemIdx = 0;
	actors[id].lastSkillIdx= 0;
}
void state_initCharacters() {
	for (int i = 0; i < MAX_ACTORS; i++) {
		state_setupCharacter(i, 1);
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
	//
	party.inventoryItemId[0] = 1;
	party.inventoryItemQty[0] = 4;
	party.inventoryItemId[1] = 2;
	party.inventoryItemQty[1] = 2;
	party.inventoryItemId[2] = 3;
	party.inventoryItemQty[2] = 69;
	party.inventoryItemId[3] = 4;
	party.inventoryItemQty[3] = 18;
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
