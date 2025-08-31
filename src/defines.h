#pragma once

#define MAP_SCROLL_DEPTH 4
#define ANIM_FRAME_DELAY 8
#define CENTER_X (160<<MAP_SCROLL_DEPTH)
#define CENTER_Y (112<<MAP_SCROLL_DEPTH)
#define MAX_SCENE_SPRITES 32
#define MAX_FLAGS 256
#define MAX_ACTORS 3
#define MAX_PARTY 3
#define MAX_TROOP 6
#define MAX_GOLD 999999
#define MAX_INVENTORY 60
#define MAX_SKILLS 128
#define MAX_STATES 8
#define MAX_BUFF 5

#define INPUT_REPEAT_L 16
#define INPUT_REPEAT_S 8

#define EVP_CMP_NONE 0
#define EVP_CMP_EQUAL 1
#define EVP_CMP_LESS 2
#define EVP_CMP_MORE 3
#define EVP_CMP_LESSEQ 4
#define EVP_CMP_MOREEQ 5
#define EVP_CMP_NOTEQUAL 6

#define CMD_END         0 // --
#define CMD_MESSAGE     1 // 1:line1(str*), 2:line2(str*), 3:line3(str*), 4:face(SpriteDefinition*), 5:palette(u16?), 6:faceFlip(bool),
#define CMD_SETFLAG     2 // 1:id(u16) 2:value(u16)
#define CMD_LABEL       3 // 1:id(u16)
#define CMD_JUMP        4 // 1:id(u16) 2:oper(char) 3:value(i16) 4:labelId(u16)
#define CMD_OPTIONS     5 // 1:line1(str*), 2:line2(str*), 3:line3(str*), 4:face(SpriteDefinition*), 5:palette(u16?), 6:faceFlip(bool), 7:flagId(u16)
#define CMD_BATTLE      6 // 1:id(u16)
#define CMD_SHOP        7 // 1:canSell(bool) 2:shopId(u16)
#define CMD_NAMING      8 // 1:heroId(u16)

typedef void (*callback_t) (void);
