#pragma once
#include <genesis.h>
#include <resources.h>

u16 scenemap_winX,scenemap_winY,scenemap_winW,scenemap_winH;
const char* scenemap_line1[40];
const char* scenemap_line2[40];
const char* scenemap_line3[40];
u16 scenemap_lineIdx, scenemap_lineTotal, scenemap_charIdx, scenemap_drawX;
bool scenemap_highlight;
char* scenemap_drawingChar[2];
Sprite* scenemap_portait;

void scenemap_createSprite(int id, int x, int y, u16 pal) {
	if (id >= 0) {
		SpriteDefinition* s = DATA_CHARS[id].graphic;
		mapSprites[mapSprites_c] = SPR_addSprite(s, x, y, TILE_ATTR(pal, FALSE, FALSE, FALSE));
		//SPR_setDelayedFrameUpdate(mapSprites[mapSprites_c], false);
	}
	mapSprites_c += 1;
}

void scenemap_create() {
	sys_pal = PAL3;
	// Load window
	VDP_loadTileSet(&ts_system, tileIdx, DMA);
	tileIdx += ts_system.numTile;
	// Setup background
	VDP_loadTileSet(DATA_MAPS[map.id].tileset, tileIdx, DMA);
	PAL_setPalette(PAL0, DATA_MAPS[map.id].paletteA->data, DMA);
	PAL_setPalette(PAL1, DATA_MAPS[map.id].paletteB->data, DMA);
	bgaIdx = tileIdx;
	tileIdx += DATA_MAPS[map.id].tileset->numTile;
	bga = MAP_create(DATA_MAPS[map.id].bottomLayer, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, bgaIdx));
	//Set player sprite
	PAL_setPalette(PAL2, DATA_CHARS[player.id].graphic->palette->data, DMA);
	scenemap_createSprite(player.id, player.posX, player.posY, player.pal);
	player.updateFrame = true;
	for (int i = 0; i < map.numEvents; i++) {
		RPG_StateCharacter ev = map.events[i];
		scenemap_createSprite(ev.id, ev.posX, ev.posY, ev.pal);
		ev.updateFrame = true;
	}
	// a
	//sys_fadeIn(20);
	// Avoid re-creating sprites on reentering the scene.
	map.refreshEvents = false;
}

void scenemap_cleanMessage() {
	// Cleanup previous window
	VDP_clearTileMapRect(BG_A, scenemap_winX,scenemap_winY,scenemap_winW,scenemap_winH);
	scenemap_winX = 0;
	scenemap_winY = 0;
	scenemap_winW = 0;
	scenemap_winH = 0;
	scenemap_lineIdx = 0;
	scenemap_charIdx = 0;
	scenemap_drawX = 0;
	if (scenemap_portait != 0) {
		SPR_releaseSprite(scenemap_portait);
		scenemap_portait = 0;
	}
}
void scenemap_drawMessage(u16 x, u16 y, int* l1, int* l2, int* l3, SpriteDefinition* portrait, int pal, int portraitPos) {
	// Calculate size
	scenemap_lineTotal = 0;
	scenemap_highlight = false;
	u16 max = 0;
	if (l1 != 0) {
		sys_processText(scenemap_line1, 40, *l1);
		scenemap_lineTotal += 1;
		u16 l = strlen(scenemap_line1);
		l -= sys_countCharacters(scenemap_line1);
		if (l > max) max = l;
	}
	if (l2 != 0) {
		sys_processText(scenemap_line2, 40, *l2);
		scenemap_lineTotal += 1;
		u16 l = strlen(scenemap_line2);
		l -= sys_countCharacters(scenemap_line2);
		if (l > max) max = l;
	}
	if (l3 != 0) {
		sys_processText(scenemap_line3, 40, *l3);
		scenemap_lineTotal += 1;
		u16 l = strlen(scenemap_line3);
		l -= sys_countCharacters(scenemap_line3);
		if (l > max) max = l;
	}
	u16 w = 2 + max;
	u16 h = 2 + scenemap_lineTotal;
	// Position
	x -= w/2;
	y -= h;
	// Draw window
	sys_drawWindow(x, y, w, h, TILE_USER_INDEX);
	scenemap_lineIdx = 0;
	scenemap_charIdx = 0;
	scenemap_drawX = 0;
	// Set portrait
	if (portrait != 0) {
		bool flip = portraitPos & 1;
		s16 xx = flip ? (x*8) + (w*8) - 8 - portrait->w : (x*8) + 8;
		s16 yy = (y*8) - portrait->h;
		scenemap_portait = SPR_addSprite(portrait, xx, yy, TILE_ATTR(pal, TRUE, FALSE, flip));
		SPR_setAnim(scenemap_portait, 1);
	}
	// Cache information for erasure
	scenemap_winX = x;
	scenemap_winY = y;
	scenemap_winW = w;
	scenemap_winH = h;
}
char* scenemap_getCurrentLine() {
	switch (scenemap_lineIdx) {
		case 0:
			return scenemap_line1;
		case 1:
			return scenemap_line2;
		case 2:
			return scenemap_line3;
		default:
			return 0;
	}
}
void scenemap_updateDrawMessage() {
	char* currLine = scenemap_getCurrentLine();
	strncpy(scenemap_drawingChar, currLine+scenemap_charIdx, 1);
	if (strcmp(scenemap_drawingChar, "&")==0) {
		scenemap_highlight = !scenemap_highlight;
	}
	else {
		u16 x = scenemap_winX + 1 + scenemap_drawX;
		u16 y = scenemap_winY + 1 + scenemap_lineIdx;
		sys_drawText(scenemap_drawingChar, x, y, scenemap_highlight);
		scenemap_drawX += 1;
	}
	// Advance
	scenemap_charIdx += 1;
	if (scenemap_charIdx >= strlen(currLine)) {
		scenemap_lineIdx += 1;
		scenemap_charIdx = 0;
		scenemap_drawX = 0;
	}
}

void scenemap_updateInput() {
	player.moveX = 0;
	player.moveY = 0;
	if (input_press(BUTTON_RIGHT)) {
		player.moveX = 1;
		if (player.direction != 3) {
			player.updateFrame = true;
			player.direction = 3;
		}
	}
	else if (input_press(BUTTON_LEFT)) {
		player.moveX = -1;
		if (player.direction != 1) {
			player.updateFrame = true;
			player.direction = 1;
		}
	}
	if (input_press(BUTTON_DOWN)) {
		player.moveY = 1;
		if (player.direction != 0) {
			player.updateFrame = true;
			player.direction = 0;
		}
	}
	else if (input_press(BUTTON_UP)) {
		player.moveY = -1;
		if (player.direction != 2) {
			player.updateFrame = true;
			player.direction = 2;
		}
	}
	// Interact
	if (input_trigger(BUTTON_A)) {
		// find event on front, ignore step activation (would be done differently)
		int cX = player.posX;
		int cY = player.posY;
		int cOff = (player.boxSize + 4) << MAP_SCROLL_DEPTH;
		switch(player.direction) {
			case 0: //down
				cY += cOff;
				break;
			case 1: //left
				cX -= cOff;
				break;
			case 2: //up
				cY -= cOff;
				break;
			case 3: //right
				cX += cOff;
				break;
		}
		char evIdx = map_checkActivation(cX, cY, 0);
		// if an event is found, interact
		if (evIdx >= 0) {
			player.moveX = 0;
			player.moveY = 0;
			interpreter_setEvent(evIdx);
		}
	}
}

void scenemap_refreshEvents() {
	for (int i = 0; i < map.numEvents; i++) {
		RPG_StateCharacter ev = map.events[i];
		// Remove old sprite
		SPR_releaseSprite(mapSprites[i+1]);
		// Create new sprite
		if (ev.id >= 0) {
			SpriteDefinition* s = DATA_CHARS[ev.id].graphic;
			mapSprites[i+1] = SPR_addSprite(s, ev.posX, ev.posY, TILE_ATTR(ev.pal, FALSE, FALSE, FALSE));
			//SPR_setDelayedFrameUpdate(mapSprites[i+1], false);
		}
	}
	map.refreshEvents = false;
}

void scenemap_updateEvents() {
	for (int i = 0; i < map.numEvents; i++) {
		if (map.events[i].id >= 0) {
			int dX = (map.events[i].posX - map.scrollX) >> MAP_SCROLL_DEPTH;
			int dY = (map.events[i].posY - map.scrollY) >> MAP_SCROLL_DEPTH;
			bool _visible = (dX > -320 && dX < 320) && (dY > -224 && dY < 224);
			_visible = _visible && !map.events[i].hidden;
			if (_visible) {
				SPR_setVisibility(mapSprites[i+1], VISIBLE);
				character_updateMoving(&map.events[i]);
				character_updateFrame(&map.events[i], mapSprites[i+1]);
				character_updateSprite(&map.events[i], mapSprites[i+1]);
			} else {
				SPR_setVisibility(mapSprites[i+1], HIDDEN);
			}
		}
	}
}
void scenebattle_create();
void scenebattle_update();
void scenebattle_destroy();

void scenemap_update() {
	/// Player update
	if (scenemap_winW != 0) {
		//
		if (scenemap_lineIdx < scenemap_lineTotal) {
			scenemap_updateDrawMessage();
			if (input_press(BUTTON_A)) {
				if (scenemap_lineIdx < scenemap_lineTotal) scenemap_updateDrawMessage();
				if (scenemap_lineIdx < scenemap_lineTotal) scenemap_updateDrawMessage();
				if (scenemap_lineIdx < scenemap_lineTotal) scenemap_updateDrawMessage();
			}
		} else {
			if (scenemap_portait != 0) {
				SPR_setAnim(scenemap_portait, 0);
			}
			//
			if (input_trigger(BUTTON_A)) {
				scenemap_cleanMessage();
			}
		}
	}
	else if (interpreter_isRunning()) {
		int* command = interpreter_update();
		int code = command[0];
		RPG_StateCharacter* evChar = interpreter_getEvent();
		switch (code) {
			case CMD_MESSAGE:
				// Show message
				scenemap_drawMessage(20,27,command[1],command[2],command[3], command[4], command[5], command[6]);
				break;
			case CMD_SETFLAG:
				// Set progress flag
				set_flag(command[1], command[2]);
				break;
			case CMD_LABEL:
				// This actually does nothing.
				break;
			case CMD_JUMP: // 1:id(u16) 2:oper(char) 3:value(i16) 4:labelId(u16)
				bool jumpCheck = sys_compare(command[1], command[2], command[3]);
				if (jumpCheck) {
					interpreter_cmdIdx = interpreter_findLabel(command[4]);
				}
				break;
			case CMD_OPTIONS:
				break;
			case CMD_BATTLE:
				break;
			case CMD_SHOP:
				break;
			case CMD_NAMING:
				return 2;
		}
	}
	else {
		scenemap_updateInput();
		if (input_trigger(BUTTON_C)) {
			scene_goto(scenebattle_create, scenebattle_update, scenebattle_destroy);
		}
	}
	// If moving
	character_updateMoving(&player);
	character_updateFrame(&player, mapSprites[0]);
	character_updateSprite(&player, mapSprites[0]);
	// Map update
	map_update();
	// Update events
	if (map.refreshEvents) {
		scenemap_refreshEvents();
	};
	scenemap_updateEvents();
	// Update sprites.
	SPR_update();
}

void scenemap_destroy() {
	// 
    sys_fadeOut(20);
	// cleanup map
	MEM_free(bga);
	VDP_fillTileMapRect(BG_B, 0, 0, 0, 40, 30);
	VDP_setHorizontalScroll(BG_B, 0);
	VDP_setVerticalScroll(BG_B, 0);
	// cleanup sprites
	SPR_releaseSprite(mapSprites[0]);
	for (int i = 0; i < map.numEvents; i++) {
		RPG_StateCharacter ev = map.events[i];
		SPR_releaseSprite(mapSprites[i+1]);
	}
	mapSprites_c = 0;
}
