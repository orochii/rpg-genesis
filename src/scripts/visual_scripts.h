#pragma once
#include <resources.h>
#include <genesis.h>

//mapSprites
//mapSprites_c
//RPG_StateBattler* scenebattle_currBattler;
//RPG_StateBattler* scenebattle_currTargets[MAX_PARTY+MAX_TROOP];
//u8 scenebattle_currTargetsCount;
//u16 scenebattle_animFrame = 0;

void vs_slash() {
    if (scenebattle_animFrame == 0) {
        for (int i=0; i < scenebattle_currTargetsCount; i++) {
            //spawn slash animation
            u16 x = scenebattle_currTargets[i]->x - (vfx_slash.w >> 1);
            u16 y = scenebattle_currTargets[i]->y - (vfx_slash.h);
            bool flip = scenebattle_currBattler->isEnemy;
            mapSprites[mapSprites_c] = SPR_addSprite(&vfx_slash, x, y, TILE_ATTR(PAL2,true,false,flip));
            SPR_setDepth(mapSprites[mapSprites_c], SPR_MIN_DEPTH);
            mapSprites_c++;
        }
    }
    // On end remember to clean the script.
    if(scenebattle_animFrame >= 24) {
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
    vs_slash
};