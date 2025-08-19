#pragma once
#include <resources.h>

/*
0: setup
1: action select
2: turn execution
4: battle end
*/
int scenebattle_phase;
int scenebattle_sx;
int scenebattle_sy;
RPG_StateBattler scenebattle_actors[MAX_PARTY];
RPG_StateBattler scenebattle_enemies[MAX_TROOP];
Image* scenebattle_battleback;

void scenebattle_recalcStats(RPG_StateBattler* battler) {
    // just copy for now
    battler->hp = battler->origHp;
    battler->mp = battler->origMp;
    battler->atk = battler->origAtk;
    battler->def = battler->origDef;
    battler->mag = battler->origMag;
    battler->spd = battler->origSpd;
}
void scenebattle_initBattle() {
    scenebattle_phase = 0;
    scenebattle_sx = 0;
    scenebattle_sy = 0;
}
void scenebattle_initEnemy() {
    u16* formation = DATA_FORMATIONS[0];
    u16 size = formation[0];
    for (int i = 0; i < MAX_TROOP; i++) {
        if (i < size) {
            u16 j = i*3;
            char id = formation[j+1];
            u16 x = formation[j+2];
            u16 y = formation[j+3];
            // Get display
            scenebattle_enemies[i].hidden = false;
            scenebattle_enemies[i].name = DATA_ENEMIES[id].name;
            scenebattle_enemies[i].battlerIdx = DATA_ENEMIES[id].battlerIdx;
            scenebattle_enemies[i].x = x;
            scenebattle_enemies[i].y = y;
            // Get original stats
	        scenebattle_enemies[i].origHp = DATA_ENEMIES[id].hp;
            scenebattle_enemies[i].origMp = DATA_ENEMIES[id].mp;
            scenebattle_enemies[i].origAtk = DATA_ENEMIES[id].atk;
            scenebattle_enemies[i].origDef = DATA_ENEMIES[id].def;
            scenebattle_enemies[i].origMag = DATA_ENEMIES[id].mag;
            scenebattle_enemies[i].origSpd = DATA_ENEMIES[id].spd;
            // Recalc stats
            scenebattle_recalcStats(&scenebattle_enemies[i]);
            // Get current state
            scenebattle_enemies[i].currHp = scenebattle_enemies[i].hp;
            scenebattle_enemies[i].currMp = scenebattle_enemies[i].mp;
        } else {
            scenebattle_enemies[i].hidden = true;
        }
        scenebattle_enemies[i].sprIdx = -1;
    }
}
void scenebattle_initParty() {
    for (int i = 0; i < MAX_PARTY; i++) {
        // Unset
        if (party.members[i] < 0) {
            scenebattle_actors[i].hidden = true;
        }
        // Set
        else {
            char id = party.members[i];
            u16 x = 224 + 24 * i;
            u16 y = 112 + 24 * i;
            // Get display
            scenebattle_actors[i].hidden = false;
            scenebattle_actors[i].name = heroes[id].name;
            scenebattle_actors[i].battlerIdx = DATA_HEROS[id].battlerIdx;
            scenebattle_actors[i].x = x;
            scenebattle_actors[i].y = y;
            // Get original stats
	        scenebattle_actors[i].origHp = heroes[id].hp;
            scenebattle_actors[i].origMp = heroes[id].mp;
            scenebattle_actors[i].origAtk = heroes[id].atk;
            scenebattle_actors[i].origDef = heroes[id].def;
            scenebattle_actors[i].origMag = heroes[id].mag;
            scenebattle_actors[i].origSpd = heroes[id].spd;
            // Get current state
            scenebattle_actors[i].currHp = heroes[id].currHp;
            scenebattle_actors[i].currMp = heroes[id].currMp;
            // Recalc stats
            scenebattle_recalcStats(&scenebattle_actors[i]);
        }
        scenebattle_actors[i].sprIdx = -1;
    }
}
void scenebattle_setBattleback(Image* b) {
    scenebattle_battleback = b;
    PAL_setPalette(PAL0, scenebattle_battleback->palette->data, DMA);
    VDP_drawImageEx(BG_B, scenebattle_battleback, TILE_ATTR_FULL(PAL0, 0, 0, 0, tileIdx), 0, 0, 0, DMA);
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
}
void scenebattle_createActorSprites() {
    for (int i = 0; i < MAX_PARTY; i++) {
        char sprIdx = scenebattle_actors[i].sprIdx;
        if (sprIdx > -1) {
            SPR_releaseSprite(mapSprites[sprIdx]);
        }
        if (scenebattle_actors[i].hidden == false) {
            u16 battlerIdx = scenebattle_actors[i].battlerIdx;
            if (scenebattle_actors[i].sprIdx == -1) {
                scenebattle_actors[i].sprIdx = mapSprites_c;
                mapSprites_c++;
            }
            if (battlerIdx >= 0) {
                SpriteDefinition* s = DATA_BATTLERS[battlerIdx].graphic;
                u16 sprIdx = scenebattle_actors[i].sprIdx;
                u16 x = scenebattle_actors[i].x;
                u16 y = scenebattle_actors[i].y;
                x -= s->w >> 1;
                y -= s->h;
                mapSprites[sprIdx] = SPR_addSprite(s, x, y, TILE_ATTR(PAL2, false, false, false));
            }
        }
    }
}
void scenebattle_createEnemySprites() {
    for (int i = 0; i < MAX_TROOP; i++) {
        char sprIdx = scenebattle_enemies[i].sprIdx;
        if (sprIdx > -1) {
            SPR_releaseSprite(mapSprites[sprIdx]);
        }
        if (scenebattle_enemies[i].hidden == false) {
            u16 battlerIdx = scenebattle_enemies[i].battlerIdx;
            if (scenebattle_enemies[i].sprIdx == -1) {
                scenebattle_enemies[i].sprIdx = mapSprites_c;
                mapSprites_c++;
            }
            if (battlerIdx >= 0) {
                SpriteDefinition* s = DATA_BATTLERS[battlerIdx].graphic;
                u16 sprIdx = scenebattle_enemies[i].sprIdx;
                u16 x = scenebattle_enemies[i].x;
                u16 y = scenebattle_enemies[i].y;
                x -= s->w >> 1;
                y -= s->h;
                mapSprites[sprIdx] = SPR_addSprite(s, x, y, TILE_ATTR(PAL2, false, false, false));
            }
        }
    }
}
void scenebattle_redrawHUD() {
    for (int idx = 0; idx < MAX_PARTY; idx++) {
        if (scenebattle_actors[idx].hidden == false) {
            char id = party.members[idx];
            u16 x = 11+(idx*10);
            sys_drawTextBack(scenebattle_actors[idx].name, x, 26, PAL2);
            char* str[10];
            sprintf(str, "%3d/%3d", scenebattle_actors[idx].currHp, scenebattle_actors[idx].hp);
            sys_drawTextBack(str, x, 27, PAL0);
            //scenebattle_debugDrawStats(&scenebattle_actors[idx], x, 20);
        }
    }
}

void scenebattle_create() {
    // Setup system
    sys_pal = PAL0;
    VDP_loadTileSet(&ts_system, tileIdx, DMA);
	tileIdx += ts_system.numTile;
    scenebattle_initBattle();
    scenebattle_initEnemy();
    scenebattle_initParty();
    scenebattle_setBattleback(&battleback1);
    // Create actors
    PAL_setPalette(PAL2, DATA_CHARS[player.id].graphic->palette->data, DMA);
    scenebattle_createActorSprites();
    scenebattle_createEnemySprites();
    scenebattle_redrawHUD();
}
void scenebattle_update() {
    //
    //scenebattle_debugScan(&scenebattle_enemies[0]);
    if (input_trigger(BUTTON_C)) {
        scene_goto(scenemap_create, scenemap_update, scenemap_destroy);
    }
    // Update sprites.
	SPR_update();
}
void scenebattle_destroy() {
    sys_fadeOut(20);
    // Clear screen
    VDP_fillTileMapRect(BG_B, 0, 0, 0, 40, 30);
    // Destroy sprites
    for (int i = 0; i < MAX_PARTY; i++) {
        char sprIdx = scenebattle_actors[i].sprIdx;
        if (sprIdx > -1) {
            SPR_releaseSprite(mapSprites[sprIdx]);
        }
    }
    for (int i = 0; i < MAX_TROOP; i++) {
        char sprIdx = scenebattle_enemies[i].sprIdx;
        if (sprIdx > -1) {
            SPR_releaseSprite(mapSprites[sprIdx]);
        }
    }
    mapSprites_c = 0;
}

void scenebattle_debugDrawStats(RPG_StateBattler* battler, u16 x, u16 y) {
    // DEBUG
    char* str[10];
    sys_drawTextBack("LEVEL UP!", x, y+0, PAL2);
    sprintf(str, "ATK %3d", battler->atk);
    sys_drawTextBack(str, x, y+1, PAL0);
    sprintf(str, "DEF %3d", battler->def);
    sys_drawTextBack(str, x, y+2, PAL0);
    sprintf(str, "MAG %3d", battler->mag);
    sys_drawTextBack(str, x, y+3, PAL0);
    sprintf(str, "SPD %3d", battler->spd);
    sys_drawTextBack(str, x, y+4, PAL0);
}

void scenebattle_debugScan(RPG_StateBattler* battler) {
    char* str[10];
    u16 x = 1;
    u16 y = 1;
    sys_drawTextBack(battler->name, x, y, PAL2);
    y++;
    sprintf(str, "HP %3d/%3d", battler->currHp, battler->hp);
    sys_drawTextBack(str, x, y, PAL0);
    y++;
    sprintf(str, "ATK %3d", battler->atk);
    sys_drawTextBack(str, x, y, PAL0);
    y++;
    sprintf(str, "DEF %3d", battler->def);
    sys_drawTextBack(str, x, y, PAL0);
    y++;
    sprintf(str, "MAG %3d", battler->mag);
    sys_drawTextBack(str, x, y, PAL0);
    y++;
    sprintf(str, "SPD %3d", battler->spd);
    sys_drawTextBack(str, x, y, PAL0);
    y++;
}