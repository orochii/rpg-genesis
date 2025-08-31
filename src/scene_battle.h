#pragma once
#include <resources.h>

/*
0: action select
1: turn execution
2: round end
4: battle end
*/
#define MAX_BOOSTS 3
#define BOOST_COST 3
#define ORB_RADIUS_X 16
#define ORB_RADIUS_Y 8
#define ORB_OX 4
#define ORB_OY 20
#define BATTLER_DEPTHADJUST -320
#define BATTLER_ACTIONMOVING_START 7
#define BATTLER_MOVETOTARGET_FRAMES 24
#define BATTLER_MOVETOHOME_FRAMES 24
#define ACTIONSPEED_RANDMASK 0b0000000000001111

int scenebattle_result = 0;
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
bool scenebattle_scheduleRedrawWindow = false;
int scenebattle_itemIdx = 0;
int scenebattle_itemTopRow = 0;
int scenebattle_skillIdx = 0;
int scenebattle_skillTopRow = 0;
RPG_StateBattler scenebattle_actors[MAX_PARTY];
RPG_StateBattler scenebattle_enemies[MAX_TROOP];
Image* scenebattle_battleback;
Sprite* scenebattle_selectorSprite;
Sprite* scenebattle_targetSelectorSprite;
Sprite* scenebattle_orbSprites[MAX_PARTY*MAX_BOOSTS];
Sprite* scenebattle_actionOverlay;
u16 scenebattle_actionOverlayState;
u16 scenebattle_orbSpritesState[MAX_PARTY];
u16 scenebattle_orbState;
u8* scenebattle_cachedTargets[MAX_PARTY+MAX_TROOP];
u8 scenebattle_numCachedTargets;
u8 scenebattle_cachedTargetIdx;
RPG_StateBattler* scenebattle_turnOrder[MAX_PARTY+MAX_TROOP];
u8 scenebattle_turnOrderCount;
RPG_StateBattler* scenebattle_currBattler;
RPG_StateBattler* scenebattle_currTargets[MAX_PARTY+MAX_TROOP];
u8 scenebattle_currTargetsCount;
u16 scenebattle_animFrame = 0;
callback_t scenebattle_visualScript = NULL;

#include "scripts/visual_scripts.h"
#include "scripts/effect_scripts.h"

u16 battler_calcBuff(u16 base, char buff) {
    if (buff==0) return base;
    s16 plus = (base * buff) / 10;
    return base + plus + buff;
}
void battler_recalcStats(RPG_StateBattler* battler) {
    battler->hp = battler->origHp;
    battler->mp = battler->origMp;
    battler->atk = battler_calcBuff(battler->origAtk, battler->buffs[EAttrib_ATK]);
    battler->def = battler_calcBuff(battler->origDef, battler->buffs[EAttrib_DEF]);
    battler->mag = battler_calcBuff(battler->origMag, battler->buffs[EAttrib_MAG]);
    battler->spd = battler_calcBuff(battler->origSpd, battler->buffs[EAttrib_SPD]);
}
void battler_healHp(RPG_StateBattler* battler, s16 amount, bool display) {
    // apply change (clamped)
    u16 a = battler->hp - battler->currHp;
    if (a < amount) battler->currHp = battler->hp;
    else battler->currHp += amount;
    // display
    if (display) {
        sprintf(battler->damageString, "+%d", amount);
        battler->damageDisplay = true;
    }
}
void battler_dmgHp(RPG_StateBattler* battler, s16 amount, bool display) {
    // apply change (clamped)
    if (amount > battler->currHp) battler->currHp = 0;
    else battler->currHp -= amount;
    // display
    if (display) {
        sprintf(battler->damageString, "-%d", amount);
        battler->damageDisplay = true;
    }
}
void battler_applyStat(RPG_StateBattler* battler, u8 stat, char val) {
    battler->states[stat] = val;
}
void battler_applyBuff(RPG_StateBattler* battler, EAttrib buff, s16 value) {
    battler->buffs[buff] += value;
    if (value < -MAX_BUFF) value = -MAX_BUFF;
    if (value > MAX_BUFF) value = MAX_BUFF;
    battler_recalcStats(battler);
}
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
void battler_setFlip(RPG_StateBattler* battler, bool flip) {
    Sprite* spr = mapSprites[battler->sprIdx];
    SPR_setHFlip(spr, flip);
}
void battler_setAnimation(RPG_StateBattler* battler, EBattlerAnim animation, bool loop, EBattlerAnim fallback) {
    Sprite* spr = mapSprites[battler->sprIdx];
    u16 max = spr->definition->numAnimation;
    if (animation >= max) animation = fallback;
    if (animation >= max) animation = EBattlerAnim_IDLE;
    SPR_setAnimationLoop(spr, loop);
    SPR_setAnimAndFrame(spr, (s16)animation, 0);
}
void battler_resetAnimation(RPG_StateBattler* battler) {
    EBattlerAnim anim = EBattlerAnim_IDLE;
    if (battler->currHp == 0) anim = EBattlerAnim_DOWN;
    else {
        bool status = battler->states[EBattlerState_STUN] != 0 || battler->states[EBattlerState_BURN] != 0 || battler->states[EBattlerState_FROZEN] != 0;
        bool weak = battler->currHp < (battler->hp>>2);
        if (status || weak) anim = EBattlerAnim_WEAK;
    }
    //
    battler_setAnimation(battler, anim, true, EBattlerAnim_IDLE);
}
void battler_setActionAnimation(RPG_StateBattler* battler) {
    EBattlerAnim targetAnimation = EBattlerAnim_IDLE;
    switch(battler->actionBasic) {
        case 0:
            targetAnimation = EBattlerAnim_ATTACK;
            break;
        case 1:
            if (battler->actionId < BATTLER_ACTIONMOVING_START)
                targetAnimation = EBattlerAnim_ATTACK;
            else
                targetAnimation = EBattlerAnim_MAGIC;
            break;
        case 2:
            targetAnimation = EBattlerAnim_ITEM;
            break;
        case 3:
            targetAnimation = EBattlerAnim_READY;
            break;
    }
    battler_setAnimation(battler, targetAnimation, false, EBattlerAnim_ATTACK);
}
void battler_moveTo(RPG_StateBattler* battler, u16 x, u16 y, u16 d) {
    battler->targetX = x;
    battler->targetY = y;
    battler->moveDuration = d;
}
bool battler_shouldMoveToTarget(RPG_StateBattler* battler) {
    switch (battler->actionBasic) {
        case 0: //attack
            return true;
        case 1: //skill
            return (battler->actionId < BATTLER_ACTIONMOVING_START);
        case 2: //item
            return true;
        case 3: //guard
        case 4: //wait
        default:
            return false;
    }
}
void battler_moveToTarget(RPG_StateBattler* battler) {
    switch (battler->actionTargetScope) {
        case EActionScope_ALLY:
            battler->targetX = scenebattle_currTargets[0]->x - 32;
            battler->targetY = scenebattle_currTargets[0]->y + 4;
            battler->moveDuration = BATTLER_MOVETOTARGET_FRAMES;
            break;
        case EActionScope_ENEMY:
            battler->targetX = scenebattle_currTargets[0]->x + 32;
            battler->targetY = scenebattle_currTargets[0]->y + 4;
            battler->moveDuration = BATTLER_MOVETOTARGET_FRAMES;
            break;
    }
}
void battler_moveToHome(RPG_StateBattler* battler) {
    battler->targetX = battler->baseX;
    battler->targetY = battler->baseY;
    battler->moveDuration = BATTLER_MOVETOHOME_FRAMES;
}
void battler_updateMove(RPG_StateBattler* battler) {
    if (battler->x != battler->targetX || battler->y != battler->targetY) {
        if (battler->moveDuration < 1) return;
        battler->x = (battler->x * (battler->moveDuration - 1) + battler->targetX) / battler->moveDuration;
        battler->y = (battler->y * (battler->moveDuration - 1) + battler->targetY) / battler->moveDuration;
        battler->moveDuration = battler->moveDuration - 1;
        Sprite* spr = mapSprites[battler->sprIdx];
        u16 x = battler->x - (spr->definition->w >>1);
        u16 y = battler->y - spr->definition->h;
        SPR_setPosition(spr, x, y);
    }
}
void battler_setDepth(RPG_StateBattler* battler) {
    Sprite* spr = mapSprites[battler->sprIdx];
    s16 depth = BATTLER_DEPTHADJUST - (battler->y >> 2);
    SPR_setDepth(spr, depth);
}
void battler_setBoostCount(u8 idx, u8 c) {
    u16 x = scenebattle_actors[idx].x - ORB_OX;
    u16 y = scenebattle_actors[idx].y - ORB_OY;
    u8 si = idx*MAX_BOOSTS;
    for (u8 i=0; i < MAX_BOOSTS; i++) {
        if (i < c) {
            u16 xx = x + (i * 8);
            u16 yy = y;
            // Create
            if (scenebattle_orbSprites[si+i] == NULL) {
                scenebattle_orbSprites[si+i] = SPR_addSprite(&boost_orb, xx, yy, TILE_ATTR(PAL2,0,0,0));
                SPR_setDepth(scenebattle_orbSprites[si+i], -idx-0x1000);
            }
        } else {
            // Clean
            if (scenebattle_orbSprites[si+i] != NULL) {
                SPR_releaseSprite(scenebattle_orbSprites[si+i]);
                scenebattle_orbSprites[si+i] = NULL;
            }
        }
    }
}
void battler_setBoostSpend(int idx) {
    scenebattle_orbSpritesState[idx] = 24;
    u8 si = idx*MAX_BOOSTS;
    for (u8 i=0; i < MAX_BOOSTS; i++) {
        Sprite* orb = scenebattle_orbSprites[si+i];
        if (orb != NULL) {
            SPR_setAnimAndFrame(orb, 1, 0);
            SPR_setAnimationLoop(orb, false);
        }
    }
}
void battler_updateBoostSprites(int idx) {
    u16 x = scenebattle_actors[idx].x - ORB_OX;
    u16 y = scenebattle_actors[idx].y - ORB_OY;
    u8 si = idx*MAX_BOOSTS;
    for (u8 i=0; i < MAX_BOOSTS; i++) {
        if (scenebattle_orbSprites[si+i] != NULL) {
            Sprite* orb = scenebattle_orbSprites[si+i];
            if (orb->animInd == 0) {
                s16 angle = scenebattle_orbState - (i * 80);
                while (angle < 0) angle += 360;
                fix16 a = FIX16(angle);
                fix16 c = F16_mul(F16_cos(a), FIX16(ORB_RADIUS_X));
                fix16 s = F16_mul(F16_sin(a), FIX16(ORB_RADIUS_Y));
                u16 xx = x + F16_toInt(c);
                u16 yy = y + F16_toInt(s);
                u16 cx = SPR_getPositionX(orb);
                u16 cy = SPR_getPositionY(orb);
                u16 px = (xx+cx)>>1;
                u16 py = (yy+cy)>>1;
                SPR_setPosition(orb, px, py);
                s16 depth = -320 - ((scenebattle_actors[idx].y+F16_toInt(s)) >> 2);
                SPR_setDepth(orb, depth);
            } else if (orb->animInd == 1) {
                u16 d = scenebattle_orbSpritesState[idx];
                if (d > 0) {
                    u16 cx = SPR_getPositionX(orb);
                    u16 cy = SPR_getPositionY(orb);
                    u16 tx = x;
                    u16 ty = y - 32;
                    // x = (x * (d - 1) + tx) / d
                    cx = (cx * (d - 1) + tx) / d;
                    cy = (cy * (d - 1) + ty) / d;
                    SPR_setPosition(orb, cx, cy);
                    scenebattle_orbSpritesState[idx] -= 1;
                } else {
                    SPR_releaseSprite(orb);
                    scenebattle_orbSprites[si+i] = NULL;
                }
            }
        }
    }
}
bool battler_determineIf0HP(RPG_StateBattler* battler) {
    switch(battler->actionBasic) {
        case 1://skill
            if (DATA_SKILLS[battler->actionId].effectId == EActionEffect_REVIVE) return true;
            break;
        case 2://item
            if (DATA_ITEMS[battler->actionId].effectId == EActionEffect_REVIVE) return true;
            break;
    }
    return false;
}
void battler_displayDamage(RPG_StateBattler* battler) {
    if (battler->damageDisplay) {
        u16 x = battler->x >> 3;
        u16 y = battler->y >> 3;
        sys_drawText(battler->damageString, x, y, false);
        battler->damageDisplay = false;
        scenebattle_waitCount = 30;
    }
}

void scenebattle_initBattle() {
    scenebattle_result = 0;
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
    scenebattle_scheduleRedrawWindow = false;
    scenebattle_actionOverlay = NULL;
    for(int i = 0; i < MAX_PARTY*MAX_BOOSTS; i++) {
        scenebattle_orbSprites[i] = NULL;
    }
    scenebattle_visualScript = NULL;
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
            scenebattle_enemies[i].isEnemy = true;
            scenebattle_enemies[i].idx = i;
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
            battler_recalcStats(&scenebattle_enemies[i]);
            // Get current state
            scenebattle_enemies[i].currHp = scenebattle_enemies[i].hp;
            scenebattle_enemies[i].currMp = scenebattle_enemies[i].mp;
            scenebattle_enemies[i].boosts = 0;
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
            scenebattle_actors[i].isEnemy = false;
            scenebattle_actors[i].idx = i;
            scenebattle_actors[i].name = actors[id].name;
            scenebattle_actors[i].battlerIdx = DATA_ACTORS[id].battlerIdx;
            scenebattle_actors[i].x = x;
            scenebattle_actors[i].y = y;
            scenebattle_actors[i].baseX = x;
            scenebattle_actors[i].baseY = y;
            scenebattle_actors[i].targetX = x;
            scenebattle_actors[i].targetY = y;
            // Get original stats
            scenebattle_actors[i].origHp = actors[id].hp;
            scenebattle_actors[i].origMp = actors[id].mp;
            scenebattle_actors[i].origAtk = actors[id].atk;
            scenebattle_actors[i].origDef = actors[id].def;
            scenebattle_actors[i].origMag = actors[id].mag;
            scenebattle_actors[i].origSpd = actors[id].spd;
            // todo: equipment stats
            // Get current state
            scenebattle_actors[i].currHp = actors[id].currHp;
            scenebattle_actors[i].currMp = actors[id].currMp;
            scenebattle_actors[i].boosts = 0;
            // Recalc stats
            battler_recalcStats(&scenebattle_actors[i]);
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
void scenebattle_determineTargets() {
    //hits 0hp?
    bool hit0HP = battler_determineIf0HP(scenebattle_currBattler);
    u8 i = 0;
    RPG_StateBattler* target = NULL;
    //
    scenebattle_currTargetsCount = 0;
    switch (scenebattle_currBattler->actionTargetScope)
    {
    case EActionScope_USER:
        scenebattle_currTargets[0] = scenebattle_currBattler;
        scenebattle_currTargetsCount = 1;
        break;
    case EActionScope_ALLY:
        i = scenebattle_currBattler->actionTargetIdx;
        target = &scenebattle_actors[i];
        if (target->hidden == false) {
            bool dead = target->currHp <= 0;
            if (dead == hit0HP) {
                scenebattle_currTargets[0] = target;
                scenebattle_currTargetsCount = 1;
            }
        }
        if (scenebattle_currTargetsCount==0) {
            for(i = 0; i < MAX_PARTY; i++) {
                target = &scenebattle_actors[i];
                if(target->hidden==false) {
                    bool dead = target->currHp <= 0;
                    if (dead == hit0HP) {
                        scenebattle_currTargets[0] = target;
                        scenebattle_currTargetsCount = 1;
                        i = MAX_PARTY;
                    }
                }
            }
        }
        break;
    case EActionScope_ALLIES:
        for(i = 0; i < MAX_PARTY; i++){
            target = &scenebattle_actors[i];
            if(target->hidden==false) {
                bool dead = target->currHp <= 0;
                if (dead == hit0HP) {
                    scenebattle_currTargets[scenebattle_currTargetsCount] = target;
                    scenebattle_currTargetsCount++;
                }
            }
        }
        break;
    case EActionScope_ENEMY:
        i = scenebattle_currBattler->actionTargetIdx;
        target = &scenebattle_enemies[i];
        if (target->hidden == false) {
            bool dead = target->currHp <= 0;
            if (dead == hit0HP) {
                scenebattle_currTargets[0] = target;
                scenebattle_currTargetsCount = 1;
            }
        }
        if (scenebattle_currTargetsCount==0) {
            for(i = 0; i < MAX_TROOP; i++) {
                target = &scenebattle_enemies[i];
                if(target->hidden==false) {
                    bool dead = target->currHp <= 0;
                    if (dead == hit0HP) {
                        scenebattle_currTargets[0] = target;
                        scenebattle_currTargetsCount = 1;
                        i = MAX_TROOP;
                    }
                }
            }
        }
        break;
    case EActionScope_ENEMIES:
        for(i = 0; i < MAX_TROOP; i++){
            target = &scenebattle_enemies[i];
            if(target->hidden==false) {
                bool dead = target->currHp <= 0;
                if (dead == hit0HP) {
                    scenebattle_currTargets[scenebattle_currTargetsCount] = target;
                    scenebattle_currTargetsCount++;
                }
            }
        }
        break;
    case EActionScope_EVERYONE:
        for(i = 0; i < MAX_PARTY; i++){
            target = &scenebattle_actors[i];
            if(target->hidden==false) {
                bool dead = target->currHp <= 0;
                if (dead == hit0HP) {
                    scenebattle_currTargets[scenebattle_currTargetsCount] = target;
                    scenebattle_currTargetsCount++;
                }
            }
        }
        for(i = 0; i < MAX_TROOP; i++){
            target = &scenebattle_enemies[i];
            if(target->hidden==false) {
                bool dead = target->currHp <= 0;
                if (dead == hit0HP) {
                    scenebattle_currTargets[scenebattle_currTargetsCount] = target;
                    scenebattle_currTargetsCount++;
                }
            }
        }
        break;
    }
}
void scenebattle_executeVisualScript(RPG_StateBattler* battler, u16 scriptIdx) {
    scenebattle_animFrame = 0;
    //remember to cache the right targets.
    scenebattle_visualScript = BATTLE_VISUAL_SCRIPTS[scriptIdx];
}
void scenebattle_executeActionVisualScript(RPG_StateBattler* battler) {
    u16 visualScriptIdx = 0;
    switch (battler->actionBasic)
    {
    case 0: //attack
        visualScriptIdx = 1;//will do later
        break;
    case 1: //skill
        visualScriptIdx = DATA_SKILLS[battler->actionId].visualsIdx;
        break;
    case 2: //item
        visualScriptIdx = DATA_ITEMS[battler->actionId].visualsIdx;
        break;
    }
    scenebattle_executeVisualScript(battler, visualScriptIdx);
}
void scenebattle_executeEffectScript(RPG_StateBattler* target, EActionEffect id, s16 param1, s16 param2, s16 param3, s16 param4) {
    switch (id) {
        case EActionEffect_NONE:
            actionEffect_none();
            break;
        case EActionEffect_PHYSDMG:
            actionEffect_physDmg(scenebattle_currBattler, target, param1, param2);
            break;
        case EActionEffect_PHYSDMGSTAT:
            actionEffect_physDmgStat(scenebattle_currBattler, target, param1, param2, param3, param4);
            break;
        case EActionEffect_SETSTAT:
            actionEffect_setStat(scenebattle_currBattler, target, param1, param2, param3, param4);
            break;
        case EActionEffect_MAGICDMG:
            actionEffect_magicDmg(scenebattle_currBattler, target, param1, param2);
            break;
        case EActionEffect_MAGICDMGBUFF:
            actionEffect_magicDmgBuff(scenebattle_currBattler, target, param1, param2, param3, param4);
            break;
        case EActionEffect_MAGICDMGPIERCE:
            actionEffect_magicDmgPierce(scenebattle_currBattler, target, param1, param2);
            break;
        case EActionEffect_PERCDMG:
            actionEffect_percDmg(scenebattle_currBattler, target, param1);
            break;
        case EActionEffect_BUFF:
            actionEffect_buff(scenebattle_currBattler, target, param1, param2);
            break;
        case EActionEffect_REVIVE:
            actionEffect_revive(scenebattle_currBattler, target, param1);
            break;
        default:
            break;
    }
}
void scenebattle_executeActionEffectScript(RPG_StateBattler* target) {
    EActionEffect id = 0;
    s16 param1, param2, param3, param4;
    RPG_DataSkill* skill = NULL;
    switch(scenebattle_currBattler->actionBasic) {
        case 0: //attack
            skill = &DATA_SKILLS[0];
            break;
        case 1: //skill
            skill = &DATA_SKILLS[scenebattle_currBattler->actionId-1];
            break;
        case 2: //item
            id = DATA_ITEMS[scenebattle_currBattler->actionId-1].effectId;
            param1 = DATA_ITEMS[scenebattle_currBattler->actionId-1].effectParam1;
            param2 = DATA_ITEMS[scenebattle_currBattler->actionId-1].effectParam2;
            param3 = DATA_ITEMS[scenebattle_currBattler->actionId-1].effectParam3;
            param4 = DATA_ITEMS[scenebattle_currBattler->actionId-1].effectParam4;
            break;
        case 3: //guard
            skill = &DATA_SKILLS[1];
            break;
        default:
            break;
    }
    if (skill != NULL) {
        id = skill->effectId;
        param1 = skill->effectParam1;
        param2 = skill->effectParam2;
        param3 = skill->effectParam3;
        param4 = skill->effectParam4;
    }
    scenebattle_executeEffectScript(target, id, param1, param2, param3, param4);
}

void scenebattle_create() {
    //
    scenebattle_selectorSprite = NULL;
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
void scenebattle_drawMPWindow() {
    sys_drawWindow(25,1,14,3,TILE_USER_INDEX);
    sys_drawText("{", 26,2, false);
    sys_drawText("MP", 27,2, true);
    RPG_StateBattler* actor = &scenebattle_actors[scenebattle_idx];
    char* str[10];
    sprintf(str, "%3d/%3d}", actor->currMp, actor->mp);
    sys_drawText(str, 30,2, false);
}
void scenebattle_drawSelectedActionWindow(char* name) {
    VDP_clearTileMapRect(BG_A, 25,4, 10,6);
    sys_drawWindow(25,4,10,4,TILE_USER_INDEX);
    sys_drawText(VOCAB[scenebattle_actionSelIdx], 26,5, true);
    sys_drawText(name, 26, 6, false);
}
void scenebattle_drawActionSelWindow() {
    sys_drawWindow(25,4,10,6,TILE_USER_INDEX);
    sys_drawOptions(VOCAB,4,26,5,scenebattle_actionSelIdx);
}
void scenebattle_drawActionWindow() {
    VDP_clearPlane(BG_A, true);
    // Draw top window
    scenebattle_drawMPWindow();
    // Draw action window
    scenebattle_drawActionSelWindow();
}
void scenebattle_drawSkillWindow() {
    char id = party.members[scenebattle_idx];
    sys_drawWindow(1,1,24,3,TILE_USER_INDEX);
    sys_drawWindow(11,4,14,15,TILE_USER_INDEX);
    sys_drawSkillList(12,5, 12,13, scenebattle_skillTopRow, scenebattle_skillIdx, &actors[id].skills);
    sys_drawSkillDesc(2, 2, 22, scenebattle_itemIdx, &actors[id].skills);
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
            scenebattle_clearAuxWindow();
            sys_drawText(scenebattle_enemies[idx].name, x>>3, (y>>3)+2, false);
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
            scenebattle_clearAuxWindow();
            sys_drawText(scenebattle_enemies[idx].name, x>>3, (y>>3)+2, false);
            break;
    }
    SPR_setPosition(scenebattle_targetSelectorSprite, x, y);
}
void scenebattle_destroyTargetSelector() {
    scenebattle_clearAuxWindow();
    SPR_releaseSprite(scenebattle_targetSelectorSprite);
    scenebattle_targetSelectorSprite = NULL;
}
void scenebattle_cacheTargets(u8 scope, bool invert) {
    if (invert) {
        switch(scope) {
            case EActionScope_ALLY: scope=EActionScope_ENEMY;break;
            case EActionScope_ENEMY: scope=EActionScope_ALLY;break;
        }
    }
    switch(scope) {
        case EActionScope_USER:
            scenebattle_cachedTargets[0] = scenebattle_idx;
            scenebattle_numCachedTargets = 1;
            break;
        case EActionScope_ALLY:
        case EActionScope_ALLIES:
            scenebattle_numCachedTargets = scenebattle_getHeroTargets(false);
            break;
        case EActionScope_ENEMY:
        case EActionScope_ENEMIES:
            scenebattle_numCachedTargets = scenebattle_getEnemyTargets(false);
            break;
        default:
            scenebattle_numCachedTargets = 0;
            break;
    }
}
void scenebattle_drawActionName(RPG_StateBattler* battler) {
    char* actionName = VOCAB[battler->actionBasic];
    switch (battler->actionBasic) {
        case 1: //skill
            actionName = DATA_SKILLS[battler->actionId-1].name;
            break;
        case 2: //item
            actionName = DATA_ITEMS[battler->actionId-1].name;
            break;
    }
    //VDP_clearPlane(BG_A, true);
    u16 x = battler->isEnemy ? 1 : 25;
    sys_drawWindow(x,1,10,3,TILE_USER_INDEX);
    sys_drawText(actionName, x+1, 2, false);
}
void scenebattle_eraseActionName(RPG_StateBattler* battler) {
    u16 x = battler->isEnemy ? 1 : 25;
    VDP_clearTileMapRect(BG_A, x, 1, 10,3);
}
void scenebattle_sortActionOrder() {
    for (int curr = 1; curr < scenebattle_turnOrderCount; curr++) {
        s16 i = curr;
        s16 j = i-1;
        while(i>0 && scenebattle_turnOrder[i]->actionSpeedTotal < scenebattle_turnOrder[j]->actionSpeedTotal) {
            RPG_StateBattler* a = scenebattle_turnOrder[j];
            scenebattle_turnOrder[j] = scenebattle_turnOrder[i];
            scenebattle_turnOrder[i] = a;
            i--;
            j--;
        }
    }
}
void scenebattle_makeActionOrder() {
    scenebattle_turnOrderCount = 0;
    // Add all existing actors
    for (int i = 0; i < MAX_PARTY; i++) {
        RPG_StateBattler* actor = &scenebattle_actors[i];
        if (actor->hidden) continue;
        u16 r = random() & ACTIONSPEED_RANDMASK;
        //setRandomSeed(r);
        actor->actionSpeedTotal = actor->actionSpeed + r;
        scenebattle_turnOrder[scenebattle_turnOrderCount] = actor;
        scenebattle_turnOrderCount += 1;
    }
    // Add all existing enemies
    for (int i = 0; i < MAX_TROOP; i++) {
        RPG_StateBattler* enemy = &scenebattle_enemies[i];
        if (enemy->hidden) continue;
        u16 r = random() & ACTIONSPEED_RANDMASK;
        //setRandomSeed(r);
        enemy->actionSpeedTotal = enemy->actionSpeed + r;
        scenebattle_turnOrder[scenebattle_turnOrderCount] = enemy;
        scenebattle_turnOrderCount += 1;
    }
    // Order by actionSpeed
    scenebattle_sortActionOrder();
}
void scenebattle_commitSkillIdx() {
    unsigned char id = party.members[scenebattle_idx];
    actors[id].lastSkillIdx = scenebattle_skillIdx;
    //
    unsigned char heroId = party.members[scenebattle_idx];
    unsigned char skillId= actors[id].skills[scenebattle_skillIdx];
    u8 scope = DATA_SKILLS[skillId-1].scope;
    scenebattle_actors[scenebattle_idx].actionId = skillId;
    scenebattle_actors[scenebattle_idx].actionTargetScope = scope;
    scenebattle_actors[scenebattle_idx].actionTargetIdx = 0;
    scenebattle_actors[scenebattle_idx].actionSpeed = scenebattle_actors[scenebattle_idx].spd + DATA_SKILLS[skillId-1].priority;
    scenebattle_cachedTargetIdx = 0;
    //
    scenebattle_cacheTargets(scope,false);
    scenebattle_createTargetSelector(scope);
    scenebattle_drawSelectedActionWindow(DATA_SKILLS[skillId-1].name);
}
void scenebattle_commitItemIdx() {
    unsigned char id = party.members[scenebattle_idx];
    actors[id].lastItemIdx = scenebattle_itemIdx;
    //
    unsigned char itemId = party.inventoryItemId[scenebattle_skillIdx];
    u8 scope = DATA_ITEMS[itemId-1].scope;
    scenebattle_actors[scenebattle_idx].actionId = itemId;
    scenebattle_actors[scenebattle_idx].actionTargetScope = scope;
    scenebattle_actors[scenebattle_idx].actionTargetIdx = 0;
    scenebattle_actors[scenebattle_idx].actionSpeed = scenebattle_actors[scenebattle_idx].spd + DATA_ITEMS[itemId-1].priority;
    scenebattle_cachedTargetIdx = 0;
    //
    scenebattle_cacheTargets(scope,false);
    scenebattle_createTargetSelector(scope);
    scenebattle_drawSelectedActionWindow(DATA_ITEMS[itemId-1].name);
}
void scenebattle_resetIdxAndRow() {
    unsigned char id = party.members[scenebattle_idx];
    scenebattle_skillIdx = actors[id].lastSkillIdx;
    scenebattle_skillTopRow = 0;
    if (scenebattle_skillTopRow > scenebattle_skillIdx) scenebattle_skillTopRow = scenebattle_skillIdx;
    if (scenebattle_skillTopRow < scenebattle_skillIdx-12) scenebattle_skillTopRow = scenebattle_skillIdx-12;
    scenebattle_itemIdx = actors[id].lastItemIdx;
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
            VDP_clearPlane(BG_A, true);
            scenebattle_scheduleRedrawWindow = true;
            scenebattle_waitCount = 16;
            scenebattle_moveActorSelector(scenebattle_idx);
            foundNext = true;
        }
    }
    scenebattle_cachedTargetIdx = 0;
}
void scenebattle_gotoNextActor() {
    bool foundNext = false;
    while (foundNext==false && scenebattle_idx < MAX_PARTY) {
        RPG_StateBattler* actor = &scenebattle_actors[scenebattle_idx];
        if (actor->hidden == false) {
            if (battler_canInput(actor)) {
                if (scenebattle_selectorSprite != 0) {
                    VDP_clearPlane(BG_A, true);
                    scenebattle_scheduleRedrawWindow = true;
                    scenebattle_waitCount = 16;
                    scenebattle_moveActorSelector(scenebattle_idx);
                }
                foundNext = true;
                scenebattle_resetIdxAndRow();
            } else if (battler_canMove(actor)) {
                actor->actionBasic = 0;
                actor->actionId = 0;
                actor->actionTargetIdx = 0;
                actor->actionTargetScope = EActionScope_USER;
                if (actor->states[EBattlerState_BERSERK] != 0) {
                    actor->actionTargetScope = EActionScope_ENEMY;
                    actor->actionTargetIdx = scenebattle_getRandomEnemyTarget(false);
                }
                if (actor->states[EBattlerState_CONFUSE] != 0) {
                    actor->actionTargetScope = EActionScope_ALLY;
                    actor->actionTargetIdx = scenebattle_getRandomHeroTarget(false);
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
        scenebattle_makeActionOrder();
    }
    scenebattle_cachedTargetIdx = 0;
}
void scenebattle_makeEnemyActions() {
    //
    for (int i=0; i<MAX_TROOP; i++) {
        RPG_StateBattler* enemy = &scenebattle_enemies[i];
        if (enemy->hidden) continue;
        if (battler_canMove(enemy) == false) {
            enemy->actionBasic = 4; //wait
            enemy->actionId = 0;
            enemy->actionTargetScope = 0;
            enemy->actionTargetIdx = 0;
            enemy->actionSpeed = enemy->spd - 100;
        } else if (battler_canInput(enemy) == false) {
            enemy->actionBasic = 0;
            enemy->actionId = 0;
            enemy->actionTargetIdx = 0;
            enemy->actionTargetScope = EActionScope_USER;
            enemy->actionSpeed = enemy->spd;
            if (enemy->states[EBattlerState_BERSERK] != 0) {
                enemy->actionTargetScope = EActionScope_ALLY;
                enemy->actionTargetIdx = scenebattle_getRandomHeroTarget(false);
            }
            if (enemy->states[EBattlerState_CONFUSE] != 0) {
                enemy->actionTargetScope = EActionScope_ENEMY;
                enemy->actionTargetIdx = scenebattle_getRandomEnemyTarget(false);
            }
        }else {
            // TODO: Follow script... for now jusk... vicious mockery.
            enemy->actionBasic = 0;
            enemy->actionId = 0;
            enemy->actionTargetScope = EActionScope_ALLY;
            enemy->actionTargetIdx = scenebattle_getRandomHeroTarget(false);
            enemy->actionSpeed = enemy->spd;
        }
    }
}
void scenebattle_showSkillOverlay(RPG_StateBattler* battler) {
    u16 x = battler->x;
    u16 y = battler->y;
    s16 depth = BATTLER_DEPTHADJUST - ((y+4) >> 2);
    switch (battler->actionBasic) {
        case 1: //skill
            x -= vfx_magic_overlay.w >> 1;
            y -= vfx_magic_overlay.h;
            scenebattle_actionOverlay = SPR_addSprite(&vfx_magic_overlay, x, y, TILE_ATTR(PAL2,true,false,false));
            SPR_setDepth(scenebattle_actionOverlay, depth);
            scenebattle_actionOverlayState = 32;
            break;
        case 2: // might add
            break;
    }
}
void scenebattle_eraseDamages() {
    VDP_clearTileMapRect(BG_A, 0, 8, 40,20);
}
void scenebattle_judge() {
    bool partyAlive = false;
    for (int i = 0; i < MAX_PARTY; i++) {
        RPG_StateBattler* b = &scenebattle_actors[i];
        if (b->hidden==false && b->currHp > 0) {
            partyAlive = true;
        }
    }
    bool troopAlive = false;
    for (int i = 0; i < MAX_TROOP; i++) {
        RPG_StateBattler* b = &scenebattle_enemies[i];
        if (b->hidden==false && b->currHp > 0) {
            troopAlive = true;
        }
    }
    if (!partyAlive || !troopAlive) {
        if (partyAlive) scenebattle_result = 1;
        else scenebattle_result = -1;
        //
        scenebattle_turnOrderCount = 0;
        scenebattle_phase = 3;
    }
}

void scenebattle_updatePhase0ActionSelect() { //action select
    if (scenebattle_scheduleRedrawWindow) {
        scenebattle_drawActionWindow();
        scenebattle_scheduleRedrawWindow = false;
    }
    RPG_StateBattler* actor = &scenebattle_actors[scenebattle_idx];
    switch(scenebattle_step) {
        case 0: // Setup
            switch (scenebattle_substep) {
                case 0:
                    // On start, select actions for enemy
                    scenebattle_makeEnemyActions();
                    // Setup player
                    scenebattle_idx = 0;
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
            //
            if (input_trigger(BUTTON_LEFT)) {
                if (actor->boosts > 0) {
                    actor->boosts = actor->boosts - 1;
                    actor->currMp = actor->currMp + BOOST_COST;
                    battler_setBoostCount(scenebattle_idx, actor->boosts);
                    scenebattle_drawMPWindow();
                }
            } else if (input_trigger(BUTTON_RIGHT)) {
                if (actor->boosts < MAX_BOOSTS && actor->currMp >= BOOST_COST) {
                    actor->boosts = actor->boosts + 1;
                    actor->currMp = actor->currMp - BOOST_COST;
                    battler_setBoostCount(scenebattle_idx, actor->boosts);
                    scenebattle_drawMPWindow();
                }
            }
            //
            if (input_trigger(BUTTON_A)) {
                switch (scenebattle_actionSelIdx) {
                    case 0: //attack
                        actor->actionBasic = 0;
                        actor->actionId = 0;
                        actor->actionTargetScope = EActionScope_ENEMY;
                        actor->actionTargetIdx = 0;
                        actor->actionSpeed = actor->spd;
                        scenebattle_cacheTargets(actor->actionTargetScope, false);
                        scenebattle_createTargetSelector(actor->actionTargetScope);
                        scenebattle_drawSelectedActionWindow("");
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
                        actor->actionSpeed = actor->spd + 1000;
                        scenebattle_idx++;
                        battler_setAnimation(actor, EBattlerAnim_READY, false, EBattlerAnim_IDLE);
                        scenebattle_step = 1;
                        scenebattle_gotoNextActor();
                        scenebattle_substep = 0;
                        break;
                }
            } else if (input_trigger(BUTTON_B)) {
                scenebattle_gotoPreviousActor();
                scenebattle_resetIdxAndRow();
                RPG_StateBattler* prevActor = &scenebattle_actors[scenebattle_idx];
                battler_resetAnimation(prevActor);
                scenebattle_step = 1;
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
                    sys_drawSkillList(12,5, 12,13, scenebattle_skillTopRow, scenebattle_skillIdx, &actors[id].skills);
                    sys_drawSkillDesc(2, 2, 22, scenebattle_itemIdx, &actors[id].skills);
                }
                else if (input_repeat(BUTTON_DOWN)) {
                    scenebattle_skillIdx += 1;
                    if (scenebattle_skillIdx >= MAX_INVENTORY) scenebattle_skillIdx = MAX_INVENTORY-1;
                    char id = party.members[scenebattle_idx];
                    u8 _id = actors[id].skills[scenebattle_skillIdx];
                    if (_id==0) scenebattle_skillIdx--;
                    if (scenebattle_skillTopRow < scenebattle_skillIdx-12) scenebattle_skillTopRow = scenebattle_skillIdx-12;
                    sys_drawSkillList(12,5, 12,13, scenebattle_skillTopRow, scenebattle_skillIdx, &actors[id].skills);
                    sys_drawSkillDesc(2, 2, 22, scenebattle_itemIdx, &actors[id].skills);
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
                        scenebattle_destroyTargetSelector();
                        // Submit
                        actor->actionTargetIdx = idx;
                        scenebattle_idx++;
                        battler_setAnimation(actor, EBattlerAnim_READY, false, EBattlerAnim_IDLE);
                        scenebattle_step = 1;
                        scenebattle_gotoNextActor();
                        scenebattle_substep = 0;
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
                        scenebattle_destroyTargetSelector();
                        // Submit
                        actor->actionTargetIdx = idx;
                        scenebattle_idx++;
                        battler_setAnimation(actor, EBattlerAnim_READY, false, EBattlerAnim_IDLE);
                        scenebattle_step = 1;
                        scenebattle_gotoNextActor();
                        scenebattle_substep = 0;
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
                        scenebattle_destroyTargetSelector();
                        // Submit
                        actor->actionTargetIdx = idx;
                        scenebattle_idx++;
                        battler_setAnimation(actor, EBattlerAnim_READY, false, EBattlerAnim_IDLE);
                        scenebattle_step = 1;
                        scenebattle_gotoNextActor();
                        scenebattle_substep = 0;
                    }
                    break;
                default:
                    if (input_trigger(BUTTON_A)) {
                        scenebattle_destroyTargetSelector();
                        // Submit
                        actor->actionTargetIdx = 0;
                        scenebattle_idx++;
                        battler_setAnimation(actor, EBattlerAnim_READY, false, EBattlerAnim_IDLE);
                        scenebattle_step = 1;
                        scenebattle_gotoNextActor();
                        scenebattle_substep = 0;
                    }
                    break;
            }
            // 
            if (input_trigger(BUTTON_B)) {
                scenebattle_destroyTargetSelector();
                scenebattle_drawActionSelWindow();
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
    // When all battlers acted, go to round end
    if (scenebattle_turnOrderCount < 1) {
        scenebattle_phase = 2;
        scenebattle_step = 0;
        return;
    }
    //
    RPG_StateBattler* battler = scenebattle_turnOrder[scenebattle_turnOrderCount-1];
    switch(scenebattle_step) {
        case 0:
            // Make sure can act
            if (battler_canMove(battler) == false) {
                scenebattle_step = 0;
                scenebattle_turnOrderCount--;
                return;
            }
            // Action start
            scenebattle_currBattler = battler;
            scenebattle_determineTargets();
            scenebattle_drawActionName(battler);
            scenebattle_waitCount = 16;
            scenebattle_step++;
            break;
        case 1: // Move to
            if (battler_shouldMoveToTarget(battler)) {
                battler_moveToTarget(battler);
                battler_setAnimation(battler, EBattlerAnim_MOVE, true, EBattlerAnim_IDLE);
                scenebattle_waitCount = BATTLER_MOVETOTARGET_FRAMES;
            }
            scenebattle_step++;
            break;
        case 2: // Action display (user)
            scenebattle_waitCount = 24;
            if (battler->isEnemy==false) {
                if (battler->boosts > 0) {
                    battler_setBoostSpend(battler->idx);
                }
            }
            battler_setActionAnimation(battler);
            scenebattle_showSkillOverlay(battler);
            scenebattle_step++;
            break;
        case 3: // Action display (target)
            scenebattle_waitCount = 12;
            scenebattle_executeActionVisualScript(battler);
            scenebattle_step++;
            break;
        case 4: // Action execution (dmg, etc)
            for (int i = 0; i < scenebattle_currTargetsCount; i++) {
                scenebattle_executeActionEffectScript(scenebattle_currTargets[i]);
            }
            scenebattle_step++;
            break;
        case 5: // damage display
            //display damage calculated in previous step, if any, refresh target
            for (int i = 0; i < scenebattle_currTargetsCount; i++) {
                //todo: I want jumping numbers
                battler_displayDamage(scenebattle_currTargets[i]);
            }
            scenebattle_step++;
            break;
        case 6: // Move back
            scenebattle_eraseDamages();
            scenebattle_redrawHUD();
            if (battler->isEnemy==false) {
                battler_setBoostCount(battler->idx, 0);
                battler->boosts = 0;
            }
            if (battler_shouldMoveToTarget(battler)) {
                battler_setAnimation(battler, EBattlerAnim_MOVE, true, EBattlerAnim_IDLE);
                battler_setFlip(battler, true);
                battler_moveToHome(battler);
                scenebattle_waitCount = BATTLER_MOVETOHOME_FRAMES;
            }
            scenebattle_step++;
            break;
        case 7: // Judge
            battler_resetAnimation(battler);
            battler_setFlip(battler, false);
            scenebattle_eraseActionName(battler);
            scenebattle_waitCount = 30;
            scenebattle_step = 0;
            scenebattle_turnOrderCount--;
            // JUDGE
            scenebattle_judge();
            // Check who is alive etc
            break;
    }
}
void scenebattle_updatePhase2RoundEnd() { //round end
    //todo: pass turns in states, etc.
    scenebattle_phase = 0;
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
        battler_updateMove(&scenebattle_actors[i]);
        battler_setDepth(&scenebattle_actors[i]);
        battler_updateBoostSprites(i);
    }
    for(int i=0; i<MAX_TROOP;i++) {
        if (scenebattle_enemies[i].hidden) continue;
        battler_updateMove(&scenebattle_enemies[i]);
        battler_setDepth(&scenebattle_enemies[i]);
    }
    scenebattle_orbState += 4;
    if (scenebattle_orbState > 360) scenebattle_orbState -= 360;
    if (scenebattle_actionOverlay != NULL) {
        if (scenebattle_actionOverlayState > 0) {
            scenebattle_actionOverlayState -= 1;
            if (scenebattle_actionOverlayState == 0) {
                SPR_releaseSprite(scenebattle_actionOverlay);
                scenebattle_actionOverlay = NULL;
            }
        }
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
    if (scenebattle_visualScript != NULL) {
        scenebattle_visualScript();
    } else {
        scenebattle_updateObjects();
        if (scenebattle_waitCount > 0) {
            scenebattle_waitCount -= 1;
        } else {
            // Phase Update
            scenebattle_updatePhase();
        }
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
