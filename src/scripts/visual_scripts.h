#pragma once
#include <resources.h>
#include <genesis.h>

//mapSprites
//mapSprites_c
//RPG_StateBattler* scenebattle_currBattler;
//RPG_StateBattler* scenebattle_currTargets[MAX_PARTY+MAX_TROOP];
//u8 scenebattle_currTargetsCount;
//u16 scenebattle_animFrame = 0;

void vs_singleSprite() {
    if (scenebattle_animFrame == 0) {
        for (int i=0; i < scenebattle_currTargetsCount; i++) {
            //spawn slash animation
            u16 x = scenebattle_currTargets[i]->x - (scenebattle_visualScriptSprite->w >> 1);
            u16 y = scenebattle_currTargets[i]->y - (scenebattle_visualScriptSprite->h);
            bool flip = scenebattle_currBattler->isEnemy;
            mapSprites[mapSprites_c] = SPR_addSprite(scenebattle_visualScriptSprite, x, y, TILE_ATTR(PAL2,true,false,flip));
            SPR_setDepth(mapSprites[mapSprites_c], SPR_MIN_DEPTH);
            mapSprites_c++;
        }
    }
    u16 totalFrames = 0;
    Animation* anim = scenebattle_visualScriptSprite->animations[0];
    for (u8 i = 0; i < anim->numFrame; i++) {
        totalFrames += anim->frames[i]->timer;
    }
    // On end remember to clean the script.
    if(scenebattle_animFrame >= totalFrames) {
        for (int i=0; i < scenebattle_currTargetsCount; i++) {
            //erase slash animation?
            mapSprites_c--;
            SPR_releaseSprite(mapSprites[mapSprites_c]);
            mapSprites[mapSprites_c] = NULL;
            scenebattle_visualScript = NULL;
        }
    }
    // Advance animation.
    scenebattle_animFrame++;
}

callback_t BATTLE_VISUAL_SCRIPTS[2] = {
    NULL,
    vs_singleSprite
};