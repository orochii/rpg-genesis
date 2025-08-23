#pragma once
#include <resources.h>

/*
0: action select
1: turn execution
2: round end
4: battle end
*/
int scenebattle_phase = 0;
int scenebattle_step = 0;
int scenebattle_idx = 0;
int scenebattle_substep = 0;
int scenebattle_actionSelIdx = 0;
int scenebattle_objectSelIdx = 0;
int scenebattle_targetSelIdx = 0;
int scenebattle_waitCount = 0;
int scenebattle_sx = 0;
int scenebattle_sy = 0;
bool scenebattle_itemMode = 0;
int scenebattle_itemIdx = 0;
int scenebattle_itemTopRow = 0;
int scenebattle_skillIdx = 0;
int scenebattle_skillTopRow = 0;
RPG_StateBattler scenebattle_actors[MAX_PARTY];
RPG_StateBattler scenebattle_enemies[MAX_TROOP];
Image* scenebattle_battleback;
Sprite* scenebattle_selectorSprite;
Sprite* scenebattle_targetSelectorSprite;
u8* scenebattle_cachedTargets[MAX_TROOP];
u8 scenebattle_numCachedTargets;
u8 scenebattle_cachedTargetIdx;

bool battler_canMove(RPG_StateBattler* battler) {
    if (battler->hp <= 0) return false;
    if (battler->states[EBattlerState_STUN] != 0) return false;
    if (battler->states[EBattlerState_FROZEN] != 0) return false;
    return true;
}
bool battler_canInput(RPG_StateBattler* battler) {
    if (battler->states[EBattlerState_CONFUSE] != 0) return false;
    if (battler->states[EBattlerState_BERSERK] != 0) return false;
    return battler_canMove(battler);
}
void battler_setAnimation(RPG_StateBattler* battler, EBattlerAnim animation, bool loop) {
    Sprite* spr = mapSprites[battler->sprIdx];
    u16 max = spr->definition->numAnimation;
    if (animation >= max) animation = EBattlerAnim_IDLE;
    SPR_setAnimationLoop(spr, loop);
    SPR_setAnimAndFrame(spr, (s16)animation, 0);
}
void battler_resetAnimation(RPG_StateBattler* battler) {
    EBattlerAnim anim = EBattlerAnim_IDLE;
    if (battler->currHp == 0) anim = EBattlerAnim_DOWN;
    // todo: states
    else {
        bool status = battler->states[EBattlerState_STUN] != 0 || battler->states[EBattlerState_BURN] != 0 || battler->states[EBattlerState_FROZEN] != 0;
        bool weak = battler->currHp < (battler->hp>>2);
        if (status || weak) anim = EBattlerAnim_WEAK;
    }
    //
    battler_setAnimation(battler, anim, true);
}
void battler_setDepth(RPG_StateBattler* battler) {
    Sprite* spr = mapSprites[battler->sprIdx];
    s16 depth = -(battler->y >> 3);
    //SPR_setDepth(spr, depth);
}

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
    scenebattle_step = 0;
    scenebattle_idx = 0;
    scenebattle_substep = 0;
    scenebattle_actionSelIdx = 0;
    scenebattle_objectSelIdx = 0;
    scenebattle_targetSelIdx = 0;
    scenebattle_waitCount = 0;
    scenebattle_sx = 0;
    scenebattle_sy = 0;
    scenebattle_itemIdx = 0;
    scenebattle_itemTopRow = 0;
    scenebattle_skillTopRow = 0;
    scenebattle_skillIdx = 0;
}
void scenebattle_initEnemy() {
    u16* formation = DATA_FORMATIONS[0];
    u16 size = formation[0];
    for (int i = 0; i < MAX_TROOP; i++) {
        if (i < size) {
            u16 j = i*3;
            unsigned char id = formation[j+1];
            u16 x = formation[j+2];
            u16 y = formation[j+3];
            // Get display
            scenebattle_enemies[i].hidden = false;
            scenebattle_enemies[i].name = DATA_ENEMIES[id].name;
            scenebattle_enemies[i].battlerIdx = DATA_ENEMIES[id].battlerIdx;
            scenebattle_enemies[i].x = x;
            scenebattle_enemies[i].y = y;
            scenebattle_enemies[i].baseX = x;
            scenebattle_enemies[i].baseY = y;
            scenebattle_enemies[i].targetX = x;
            scenebattle_enemies[i].targetY = y;
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
            unsigned char id = party.members[i];
            u16 x = 224 + 24 * i;
            u16 y = 112 + 24 * i;
            // Get display
            scenebattle_actors[i].hidden = false;
            scenebattle_actors[i].name = heroes[id].name;
            scenebattle_actors[i].battlerIdx = DATA_HEROS[id].battlerIdx;
            scenebattle_actors[i].x = x;
            scenebattle_actors[i].y = y;
            scenebattle_actors[i].baseX = x;
            scenebattle_actors[i].baseY = y;
            scenebattle_actors[i].targetX = x;
            scenebattle_actors[i].targetY = y;
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
u8 scenebattle_getEnemyTargets(bool includeHP0) {
    // Get valid targets
    bool provokeActive = false;
    u8 numTargets = 0;
    for(u8 i = 0; i < MAX_TROOP; i++) {
        if(scenebattle_enemies[i].hidden) continue;
        if(!includeHP0 && scenebattle_enemies[i].currHp <= 0) continue;
        bool provoking = scenebattle_enemies[i].states[EBattlerState_PROVOKE] != 0;
        if(provoking) {
            if (!provokeActive) {
                numTargets = 0;
                provokeActive = true;
            }
        }
        if (provokeActive && !provoking) continue;
        scenebattle_cachedTargets[numTargets] = i;
        numTargets++;
    }
    return numTargets;
}
u8 scenebattle_getRandomEnemyTarget(bool includeHP0) {
    u8 numTargets = scenebattle_getEnemyTargets(includeHP0);
    // Get random target from valid
    u8 _idx = 0;
    u16 r = random();
    if (numTargets != 0) _idx = scenebattle_cachedTargets[r % numTargets];
    return _idx;
}
u8 scenebattle_getHeroTargets(bool includeHP0) {
    // Get valid targets
    bool provokeActive = false;
    u8 numTargets = 0;
    for(u8 i = 0; i < MAX_PARTY; i++) {
        if(scenebattle_actors[i].hidden) continue;
        if(!includeHP0 && scenebattle_actors[i].currHp <= 0) continue;
        bool provoking = scenebattle_actors[i].states[EBattlerState_PROVOKE] != 0;
        if(provoking) {
            if (!provokeActive) {
                numTargets = 0;
                provokeActive = true;
            }
        }
        if (provokeActive && !provoking) continue;
        scenebattle_cachedTargets[numTargets] = i;
        numTargets++;
    }
    return numTargets;
}
u8 scenebattle_getRandomHeroTarget(bool includeHP0) {
    u8 numTargets = scenebattle_getHeroTargets(includeHP0);
    // Get random target from valid
    u8 _idx = 0;
    u16 r = random();
    if (numTargets != 0) _idx = scenebattle_cachedTargets[r % numTargets];
    return _idx;
}

void scenebattle_create() {
    //
    scenebattle_selectorSprite = 0;
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
void scenebattle_createActorSelector(int idx) {
    RPG_StateBattler* actor = &scenebattle_actors[idx];
    u16 x = actor->x - (battle_selector.w / 2);
    u16 y = actor->y - (battle_selector.h / 2);
    scenebattle_selectorSprite = SPR_addSprite(&battle_selector, x, y, TILE_ATTR(PAL0, 0,0,0));
}
void scenebattle_moveActorSelector(int idx) {
    if (scenebattle_selectorSprite==0) return;
    RPG_StateBattler* actor = &scenebattle_actors[idx];
    u16 x = actor->x - (scenebattle_selectorSprite->definition->w / 2);
    u16 y = actor->y - (scenebattle_selectorSprite->definition->h / 2);
    SPR_setPosition(scenebattle_selectorSprite, x, y);
}
void scenebattle_drawActionWindow() {
    VDP_clearPlane(BG_A, true);
    // Draw top window
    sys_drawWindow(25,1,14,3,TILE_USER_INDEX);
    sys_drawText("MP", 27,2, true);
    RPG_StateBattler* actor = &scenebattle_actors[scenebattle_idx];
    char* str[10];
    sprintf(str, "%3d/%3d", actor->currMp, actor->mp);
    sys_drawText(str, 30,2, false);
    // Draw action window
    sys_drawWindow(25,4,10,6,TILE_USER_INDEX);
    sys_drawOptions(VOCAB,4,26,5,scenebattle_actionSelIdx);
}
void scenebattle_drawSkillWindow() {
    char id = party.members[scenebattle_idx];
    sys_drawWindow(1,1,24,3,TILE_USER_INDEX);
    sys_drawWindow(11,4,14,15,TILE_USER_INDEX);
    sys_drawSkillList(12,5, 12,13, scenebattle_skillTopRow, scenebattle_skillIdx, &heroes[id].skills);
    sys_drawSkillDesc(2, 2, 22, scenebattle_itemIdx, &heroes[id].skills);
}
void scenebattle_drawItemWindow() {
    sys_drawWindow(1,1,24,3,TILE_USER_INDEX);
    sys_drawWindow(11,4,14,15,TILE_USER_INDEX);
    sys_drawItemList(12,5, 12,13, scenebattle_itemTopRow, scenebattle_itemIdx);
    sys_drawItemDesc(2, 2, 22, scenebattle_itemIdx);
}
void scenebattle_clearAuxWindow() {
    VDP_clearTileMapRect(BG_A, 0,0, 25,25);
}
void scenebattle_createTargetSelector(u8 scope) {
    bool v = (scenebattle_numCachedTargets > 0);
    u8 idx = v ? scenebattle_cachedTargets[scenebattle_cachedTargetIdx] : 0;
    u16 x = 0; u16 y = 0;
    bool flipH = false;
    switch(scope) {
        case EActionScope_USER:
        case EActionScope_ALLY:
            x = scenebattle_actors[idx].x - 16;
            y = scenebattle_actors[idx].y - 16;
            flipH = true;
            break;
        case EActionScope_ENEMY:
            x = scenebattle_enemies[idx].x;
            y = scenebattle_enemies[idx].y - 16;
            break;
    }
    //y = y - target_selector.h;
    scenebattle_targetSelectorSprite = SPR_addSprite(&target_selector, x,y, TILE_ATTR(sys_pal,1,0,flipH));
    SPR_setDepth(scenebattle_targetSelectorSprite, SPR_MIN_DEPTH);
}
void scenebattle_moveTargetSelector() {
    bool v = (scenebattle_numCachedTargets > 0);
    u8 idx = v ? scenebattle_cachedTargets[scenebattle_cachedTargetIdx] : 0;
    u16 x = 0; u16 y = 0;
    u8 scope = scenebattle_actors[scenebattle_idx].actionTargetScope;
    switch(scope) {
        case EActionScope_USER:
        case EActionScope_ALLY:
            x = scenebattle_actors[idx].x - 16;
            y = scenebattle_actors[idx].y - 16;
            break;
        case EActionScope_ENEMY:
            x = scenebattle_enemies[idx].x;
            y = scenebattle_enemies[idx].y - 16;
            break;
    }
    SPR_setPosition(scenebattle_targetSelectorSprite, x, y);
}
void scenebattle_destroyTargetSelector() {
    SPR_releaseSprite(scenebattle_targetSelectorSprite);
    scenebattle_targetSelectorSprite = NULL;
}
void scenebattle_commitSkillIdx() {
    unsigned char id = party.members[scenebattle_idx];
    heroes[id].lastSkillIdx = scenebattle_skillIdx;
    //
    unsigned char heroId = party.members[scenebattle_idx];
    unsigned char skillId= heroes[id].skills[scenebattle_skillIdx];
    u8 scope = DATA_SKILLS[skillId-1].scope;
    scenebattle_actors[scenebattle_idx].actionId = skillId;
    scenebattle_actors[scenebattle_idx].actionTargetScope = scope;
    scenebattle_actors[scenebattle_idx].actionTargetIdx = 0;
    scenebattle_cachedTargetIdx = 0;
    //
    switch(scope) {
        case EActionScope_USER:
            scenebattle_cachedTargets[0] = scenebattle_idx;
            scenebattle_numCachedTargets = 1;
            break;
        case EActionScope_ALLY:
            scenebattle_numCachedTargets = scenebattle_getHeroTargets(false);
            break;
        case EActionScope_ENEMY:
            scenebattle_numCachedTargets = scenebattle_getEnemyTargets(false);
            break;
        default:
            scenebattle_numCachedTargets = 0;
            break;
    }
    scenebattle_createTargetSelector(scope);
}
void scenebattle_commitItemIdx() {
    unsigned char id = party.members[scenebattle_idx];
    heroes[id].lastItemIdx = scenebattle_itemIdx;
    //
    unsigned char itemId = party.inventoryItemId[scenebattle_skillIdx];
    u8 scope = DATA_ITEMS[itemId-1].scope;
    scenebattle_actors[scenebattle_idx].actionId = itemId;
    scenebattle_actors[scenebattle_idx].actionTargetScope = scope;
    scenebattle_actors[scenebattle_idx].actionTargetIdx = 0;
    scenebattle_cachedTargetIdx = 0;
    //
    switch(scope) {
        case EActionScope_USER:
            scenebattle_cachedTargets[0] = scenebattle_idx;
            scenebattle_numCachedTargets = 1;
            break;
        case EActionScope_ALLY:
            scenebattle_numCachedTargets = scenebattle_getHeroTargets(false);
            break;
        case EActionScope_ENEMY:
            scenebattle_numCachedTargets = scenebattle_getEnemyTargets(false);
            break;
        default:
            scenebattle_numCachedTargets = 0;
            break;
    }
    scenebattle_createTargetSelector(scope);
}
void scenebattle_resetIdxAndRow() {
    unsigned char id = party.members[scenebattle_idx];
    scenebattle_skillIdx = heroes[id].lastSkillIdx;
    scenebattle_skillTopRow = 0;
    if (scenebattle_skillTopRow > scenebattle_skillIdx) scenebattle_skillTopRow = scenebattle_skillIdx;
    if (scenebattle_skillTopRow < scenebattle_skillIdx-12) scenebattle_skillTopRow = scenebattle_skillIdx-12;
    scenebattle_itemIdx = heroes[id].lastItemIdx;
    scenebattle_itemTopRow = 0;
    if (scenebattle_itemTopRow > scenebattle_itemIdx) scenebattle_itemTopRow = scenebattle_itemIdx;
    if (scenebattle_itemTopRow < scenebattle_itemIdx-12) scenebattle_itemTopRow = scenebattle_itemIdx-12;
}
void scenebattle_gotoPreviousActor() {
    bool foundNext = false;
    while (foundNext==false && scenebattle_idx > 0) {
        scenebattle_idx--;
        RPG_StateBattler* actor = &scenebattle_actors[scenebattle_idx];
        if (actor->hidden == false && battler_canInput(actor)) {
            scenebattle_drawActionWindow();
            scenebattle_moveActorSelector(scenebattle_idx);
            foundNext = true;
        }
    }
}
void scenebattle_gotoNextActor() {
    bool foundNext = false;
    while (foundNext==false && scenebattle_idx < MAX_PARTY) {
        RPG_StateBattler* actor = &scenebattle_actors[scenebattle_idx];
        if (actor->hidden == false) {
            if (battler_canInput(actor)) {
                if (scenebattle_selectorSprite != 0) {
                    scenebattle_drawActionWindow();
                    scenebattle_moveActorSelector(scenebattle_idx);
                }
                foundNext = true;
            } else if (battler_canMove(actor)) {
                //todo: confuse/berserk
                actor->actionBasic = 0;
                actor->actionId = 0;
                actor->actionTargetIdx = 0;
                actor->actionTargetScope = EActionScope_USER;
                if (actor->states[EBattlerState_BERSERK] != 0) {
                    //get random enemy target
                    scenebattle_getRandomEnemyTarget(false);
                    //set scope
                    actor->actionTargetScope = EActionScope_ENEMY;
                }
                if (actor->states[EBattlerState_CONFUSE] != 0) {
                    actor->actionTargetScope = EActionScope_ALLY;
                }
            } else {
                actor->actionBasic = 4; //wait
                actor->actionId = 0;
                actor->actionTargetScope = 0;
                actor->actionTargetIdx = 0;
            }
        }
        if (!foundNext) scenebattle_idx++;
    }
    if (foundNext==false) {
        if (scenebattle_selectorSprite != 0) {
            SPR_releaseSprite(scenebattle_selectorSprite);
            scenebattle_selectorSprite = 0;
        }
        VDP_clearPlane(BG_A, true);
        scenebattle_phase = 1;
        scenebattle_step = 0;
    }
}

void scenebattle_updatePhase0ActionSelect() { //action select
    RPG_StateBattler* actor = &scenebattle_actors[scenebattle_idx];
    switch(scenebattle_step) {
        case 0: // Setup
            switch (scenebattle_substep) {
                case 0:
                    // On start, select actions for enemy
                    // Setup player
                    scenebattle_gotoNextActor();
                    scenebattle_waitCount = 8;
                    break;
                case 1:
                    scenebattle_createActorSelector(scenebattle_idx);
                    scenebattle_drawActionWindow();
                    break;
                default:
                    scenebattle_resetIdxAndRow();
                    scenebattle_step = 1;
                    //scenebattle_idx = 0;
                    scenebattle_substep = 0;
                    break;
            }
            scenebattle_substep += 1;
            break;
        case 1: // Action select
            // Cycle for action selections.
            if (input_repeat(BUTTON_UP)) {
                scenebattle_actionSelIdx -= 1;
                if (scenebattle_actionSelIdx < 0) scenebattle_actionSelIdx = 3;
                sys_drawOptions(VOCAB,4,26,5,scenebattle_actionSelIdx);
            } else if (input_repeat(BUTTON_DOWN)) {
                scenebattle_actionSelIdx += 1;
                if (scenebattle_actionSelIdx > 3) scenebattle_actionSelIdx = 0;
                sys_drawOptions(VOCAB,4,26,5,scenebattle_actionSelIdx);
            }
            if (input_trigger(BUTTON_A)) {
                switch (scenebattle_actionSelIdx) {
                    case 0: //attack
                        actor->actionBasic = 0;
                        actor->actionId = 0;
                        actor->actionTargetScope = EActionScope_ENEMY;
                        actor->actionTargetIdx = 0;
                        scenebattle_createTargetSelector(actor->actionTargetScope);
                        scenebattle_step = 3;
                        break;
                    case 1: //skill
                        actor->actionBasic = 1;
                        scenebattle_step = 2;
                        scenebattle_itemMode = 0;
                        scenebattle_drawSkillWindow();
                        break;
                    case 2: //item
                        actor->actionBasic = 2;
                        scenebattle_step = 2;
                        scenebattle_itemMode = 1;
                        scenebattle_drawItemWindow();
                        break;
                    case 3: //guard
                        actor->actionBasic = 3;
                        actor->actionId = 0;
                        actor->actionTargetScope = 0;
                        actor->actionTargetIdx = 0;
                        scenebattle_idx++;
                        battler_setAnimation(actor, EBattlerAnim_READY, false);
                        scenebattle_gotoNextActor();
                        break;
                }
            } else if (input_trigger(BUTTON_B)) {
                scenebattle_gotoPreviousActor();
                scenebattle_resetIdxAndRow();
                RPG_StateBattler* prevActor = &scenebattle_actors[scenebattle_idx];
                battler_resetAnimation(prevActor);
            }
            break;
        case 2: // Object select
            //
            if (scenebattle_itemMode==0) { //skill
                if (input_repeat(BUTTON_UP)) {
                    scenebattle_skillIdx -= 1;
                    if (scenebattle_skillIdx < 0) scenebattle_skillIdx = 0;
                    if (scenebattle_skillTopRow > scenebattle_skillIdx) scenebattle_skillTopRow = scenebattle_skillIdx;
                    char id = party.members[scenebattle_idx];
                    sys_drawSkillList(12,5, 12,13, scenebattle_skillTopRow, scenebattle_skillIdx, &heroes[id].skills);
                    sys_drawSkillDesc(2, 2, 22, scenebattle_itemIdx, &heroes[id].skills);
                }
                else if (input_repeat(BUTTON_DOWN)) {
                    scenebattle_skillIdx += 1;
                    if (scenebattle_skillIdx >= MAX_INVENTORY) scenebattle_skillIdx = MAX_INVENTORY-1;
                    u8 _id = party.inventoryItemId[scenebattle_skillIdx];
                    u8 _qty= party.inventoryItemQty[scenebattle_skillIdx];
                    if (_id==0 || _qty==0) scenebattle_skillIdx--;
                    if (scenebattle_skillTopRow < scenebattle_skillIdx-12) scenebattle_skillTopRow = scenebattle_skillIdx-12;
                    char id = party.members[scenebattle_idx];
                    sys_drawSkillList(12,5, 12,13, scenebattle_skillTopRow, scenebattle_skillIdx, &heroes[id].skills);
                    sys_drawSkillDesc(2, 2, 22, scenebattle_itemIdx, &heroes[id].skills);
                }
                if (input_trigger(BUTTON_A)) {
                    // check cost
                    scenebattle_clearAuxWindow();
                    scenebattle_commitSkillIdx();
                    scenebattle_step = 3;
                }
            } else { //item
                if (input_repeat(BUTTON_UP)) {
                    scenebattle_itemIdx -= 1;
                    if (scenebattle_itemIdx < 0) scenebattle_itemIdx = 0;
                    if (scenebattle_itemTopRow > scenebattle_itemIdx) scenebattle_itemTopRow = scenebattle_itemIdx;
                    sys_drawItemList(12,5, 12,13, scenebattle_itemTopRow, scenebattle_itemIdx);
                    sys_drawItemDesc(2, 2, 22, scenebattle_itemIdx);
                }
                else if (input_repeat(BUTTON_DOWN)) {
                    scenebattle_itemIdx += 1;
                    if (scenebattle_itemIdx >= MAX_INVENTORY) scenebattle_itemIdx = MAX_INVENTORY-1;
                    u8 _id = party.inventoryItemId[scenebattle_itemIdx];
                    u8 _qty= party.inventoryItemQty[scenebattle_itemIdx];
                    if (_id==0 || _qty==0) scenebattle_itemIdx--;
                    if (scenebattle_itemTopRow < scenebattle_itemIdx-12) scenebattle_itemTopRow = scenebattle_itemIdx-12;
                    sys_drawItemList(12,5, 12,13, scenebattle_itemTopRow, scenebattle_itemIdx);
                    sys_drawItemDesc(2, 2, 22, scenebattle_itemIdx);
                }
                if (input_trigger(BUTTON_A)) {
                    // check availability
                    scenebattle_clearAuxWindow();
                    scenebattle_commitItemIdx();
                    scenebattle_step = 3;
                }
            }
            //
            if (input_trigger(BUTTON_B)) {
                scenebattle_clearAuxWindow();
                scenebattle_step = 1;
            }
            break;
        case 3: // Target select
            // Cycle through targets
            //
            bool v = (scenebattle_numCachedTargets > 0);
            u8 idx = v ? scenebattle_cachedTargets[scenebattle_cachedTargetIdx] : 0;
            switch (actor->actionTargetScope) {
                case EActionScope_USER:
                    //scenebattle_actors[idx].x, scenebattle_actors[idx].y
                    if (input_trigger(BUTTON_A)) {
                        // Submit
                        actor->actionTargetIdx = idx;
                    }
                    break;
                case EActionScope_ALLY:
                    //scenebattle_actors[idx].x, scenebattle_actors[idx].y
                    if (input_repeat(BUTTON_UP)) {
                        scenebattle_cachedTargetIdx--;
                        if (scenebattle_cachedTargetIdx>200) scenebattle_cachedTargetIdx = scenebattle_numCachedTargets-1;
                        scenebattle_moveTargetSelector();
                    } else if (input_repeat(BUTTON_DOWN)) {
                        scenebattle_cachedTargetIdx++;
                        if (scenebattle_cachedTargetIdx>=scenebattle_numCachedTargets) scenebattle_cachedTargetIdx = 0;
                        scenebattle_moveTargetSelector();
                    }
                    if (input_trigger(BUTTON_A)) {
                        // Submit
                        actor->actionTargetIdx = idx;
                    }
                    break;
                case EActionScope_ENEMY:
                    //scenebattle_enemies[idx].x, scenebattle_enemies[idx].y
                    if (input_repeat(BUTTON_UP)) {
                        scenebattle_cachedTargetIdx--;
                        if (scenebattle_cachedTargetIdx>200) scenebattle_cachedTargetIdx = scenebattle_numCachedTargets-1;
                        scenebattle_moveTargetSelector();
                    } else if (input_repeat(BUTTON_DOWN)) {
                        scenebattle_cachedTargetIdx++;
                        if (scenebattle_cachedTargetIdx>=scenebattle_numCachedTargets) scenebattle_cachedTargetIdx = 0;
                        scenebattle_moveTargetSelector();
                    }
                    if (input_trigger(BUTTON_A)) {
                        // Submit
                        actor->actionTargetIdx = idx;
                    }
                    break;
                default:
                    break;
            }
            // 
            if (input_trigger(BUTTON_B)) {
                scenebattle_destroyTargetSelector();
                if (actor->actionBasic == 0) scenebattle_step = 1;
                else {
                    if (scenebattle_itemMode == 0) scenebattle_drawSkillWindow();
                    else scenebattle_drawItemWindow();
                    scenebattle_step = 2;
                }
            }
            break;
    }
}
void scenebattle_updatePhase1TurnExecution() { //turn execution
    switch(scenebattle_step) {
        case 0:
            // Action start
            break;
        case 1: // Move to
            break;
        case 2: // Action display
            break;
        case 3: // Action execution (dmg, etc)
            break;
        case 4: // Move back
            break;
        case 5: // Judge
            break;
    }
}
void scenebattle_updatePhase2RoundEnd() { //round end
    //Reset stuff, pass turns in states, etc.
}
void scenebattle_updatePhase3BattleEnd() { //battle end
    // Wait for end of animation for battle end display
    // Go to next scene
    if (input_trigger(BUTTON_C)) {
        scene_goto(scenemap_create, scenemap_update, scenemap_destroy);
    }
}
void scenebattle_updateObjects() {
    for (int i=0; i<MAX_PARTY;i++) {
        if (scenebattle_actors[i].hidden) continue;
        battler_setDepth(&scenebattle_actors[i]);
    }
    for(int i=0; i<MAX_TROOP;i++) {
        if (scenebattle_enemies[i].hidden) continue;
        battler_setDepth(&scenebattle_enemies[i]);
    }
}
void scenebattle_updatePhase() {
    switch (scenebattle_phase) {
    case 0: //action select
        scenebattle_updatePhase0ActionSelect();
        break;
    case 1: //turn execution
        scenebattle_updatePhase1TurnExecution();
        break;
    case 2: //round end
        scenebattle_updatePhase2RoundEnd();
        break;
    case 3: //battle end
        scenebattle_updatePhase3BattleEnd();
        break;
    }
}
void scenebattle_update() {
    scenebattle_updateObjects();
    if (scenebattle_waitCount > 0) {
        scenebattle_waitCount -= 1;
    } else {
        // Phase Update
        scenebattle_updatePhase();
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
