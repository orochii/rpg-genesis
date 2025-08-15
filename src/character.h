#pragma once
#include <resources.h>

void character_init(RPG_StateCharacter* character, int _id) {
	character->id = _id;
	character->moveSpeed = DATA_CHARS[character->id].moveSpeed;
	character->boxSize = DATA_CHARS[character->id].boxSize;
	character->ox = DATA_CHARS[character->id].ox;
	character->oy = DATA_CHARS[character->id].oy;
	character->solid = true;
}

void character_teleport(RPG_StateCharacter* character, int _tx, int _ty) {
	character->posX = _tx<<MAP_SCROLL_DEPTH;
	character->posY = _ty<<MAP_SCROLL_DEPTH;
}

void character_updateMoving(RPG_StateCharacter* character) {
	bool moving = (character->moveX != 0 || character->moveY != 0);
	if (moving) {
		/// Check collision
		int boxSize = character->boxSize << MAP_SCROLL_DEPTH;
		// Horizontal check
		int px = character->posX;
		int py = character->posY;
		int mx = (character->moveX * character->moveSpeed);
		int cx = px + mx + (character->moveX * boxSize);
		u16 check0 = map_checkCollision(cx, py);
		u16 check1 = map_checkCollision(cx, py - boxSize);
		u16 check2 = map_checkCollision(cx, py + boxSize);
		if (check0 == 0 && check1 == 0 && check2 == 0) {
			character->posX += mx;
		}
		// Vertical check
		int my = (character->moveY * character->moveSpeed);
		int cy = py + my + (character->moveY * boxSize);
		check0 = map_checkCollision(px, cy);
		check1 = map_checkCollision(px - boxSize, cy);
		check2 = map_checkCollision(px + boxSize, cy);
		if (check0 == 0 && check1 == 0 && check2 == 0) {
			character->posY += my;
		}
		// Sprite update
		character->animCount += 1;
		if (character->animCount >= ANIM_FRAME_DELAY) {
			character->animCount = 0;
			character->frame += 1;
			if (character->frame > 3) {
				character->frame = 0;
			}
			character->updateFrame = true;
		}
	} else {
		character->animCount = 0;
		if (character->frame != 0) {
			character->frame = 0;
			character->updateFrame = true;
		}
	}
}

void character_updateFrame(RPG_StateCharacter* character, Sprite* sprite) {
	if (character->updateFrame) {
		// Update player's sprite
		int anim = character->direction;
		if (anim == 3) {
			anim = 1;
			SPR_setHFlip(sprite, true);
		} else {
			SPR_setHFlip(sprite, false);
		}
		int frame = 0;
		switch (character->frame) {
		case 1:
			frame = 1;
			break;
		case 2:
			frame = 0;
			break;
		case 3:
			frame = 2;
			break;
		}
		SPR_setAnimAndFrame(sprite, anim, frame);
		character->updateFrame = false;
	}
}

void character_updateSprite(RPG_StateCharacter* character, Sprite* sprite) {
	int ox = sprite->definition->w >> 1;
	int oy = sprite->definition->h;
	int px = ((character->posX + CENTER_X - map.scrollX) >> MAP_SCROLL_DEPTH) - ox;
	int py = ((character->posY + CENTER_Y - map.scrollY) >> MAP_SCROLL_DEPTH) - oy;
	SPR_setPosition(sprite, px+character->ox, py+character->oy);
	int depth = (-py - oy) >> 3;
	SPR_setDepth(sprite, depth);
}

bool character_checkOverlap(RPG_StateCharacter* character, int x, int y, int s) {
	int size = (character->boxSize + s) << MAP_SCROLL_DEPTH;
	int sX = character->posX - size;
	int eX = character->posX + size;
	int sY = character->posY - size;
	int eY = character->posY + size;
	return (x >= sX && x < eX) && (y >= sY && y < eY);
}