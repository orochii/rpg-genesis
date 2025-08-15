#pragma once
#include <resources.h>

void map_init(int _id) {
	map.id = _id;
    map.collisionW = DATA_MAPS[map.id].collisionW;
    map.collisionH = DATA_MAPS[map.id].collisionH;
    map.collisionData = DATA_MAPS[map.id].collisionData;
	map.worldmap = DATA_MAPS[map.id].worldmap;
	map.numEvents = DATA_MAPS[map.id].numEvents;
	map.events = malloc(map.numEvents * sizeof(RPG_StateCharacter));
	for (int i = 0; i < map.numEvents; i++) {
		RPG_DataEvent ev = DATA_MAPS[map.id].events[i];
		map_characterUpdate(i);
		map.events[i].posX = ev.x<<MAP_SCROLL_DEPTH;
		map.events[i].posY = ev.y<<MAP_SCROLL_DEPTH;
	}
	//
	player.id = map.worldmap ? DATA_HEROS[0].characterIdxWmap : DATA_HEROS[0].characterIdx;
	//
	interpreter_reset();
}

void map_characterUpdate(int i) {
	RPG_DataEvent ev = DATA_MAPS[map.id].events[i];
	map.events[i].currPageIdx = -1;
	for (int pi = ev.numPages-1; pi >= 0; pi--) {
		// Check page condition
		// char condition; u16 compare; int value;
		bool valid = false;
		RPG_DataEventPage* page = &ev.pages[pi];
		switch (page->condition) {
			case EVP_CMP_EQUAL:
				valid = stateFlags[page->compare] == page->value;
				break;
			default:
				valid = true;
				break;
		}
		if (valid) {
			map.events[i].currPageIdx = pi;
			pi = -1;
		}
	}
	if (map.events[i].currPageIdx < 0) {
		map.events[i].hidden = true;
		map.events[i].solid = false;
	} else {
		RPG_DataEventPage* currPage = &ev.pages[map.events[i].currPageIdx];
		//character_init(map.events[i], currPage->characterIdx);
		map.events[i].id = currPage->characterIdx;
		map.events[i].moveSpeed = DATA_CHARS[currPage->characterIdx].moveSpeed;
		map.events[i].boxSize = DATA_CHARS[currPage->characterIdx].boxSize;
		map.events[i].ox = DATA_CHARS[currPage->characterIdx].ox;
		map.events[i].oy = DATA_CHARS[currPage->characterIdx].oy;
		map.events[i].pal = currPage->pal;
		map.events[i].solid = currPage->solid;
		map.events[i].hidden = false;
	}
}

void map_update() {
    map.scrollX = player.posX;
    map.scrollY = player.posY;
    // Scroll update
    u32 mx = (map.scrollX - CENTER_X) >> MAP_SCROLL_DEPTH;
    u32 my = (map.scrollY - CENTER_Y) >> MAP_SCROLL_DEPTH;
    MAP_scrollTo(bga, mx, my);
	if (map.refreshEvents) {
		for (int i = 0; i < map.numEvents; i++) {
			map_characterUpdate(i);
		}
	}
}

u16 map_checkCollision(int x, int y) {
	int ptx = (x >> MAP_SCROLL_DEPTH) >> 3;
	int pty = (y >> MAP_SCROLL_DEPTH) >> 3;
	// Bounds
	while (ptx < 0) ptx += map.collisionW;
	while (ptx >= map.collisionW) ptx -= map.collisionW;
	while (pty < 0) pty += map.collisionH;
	while (pty >= map.collisionH) pty -= map.collisionH;
	// Return tile property.
	int cIdx = pty * map.collisionW + ptx;
	return map.collisionData[cIdx];
}
bool character_checkOverlap(RPG_StateCharacter* character, int x, int y, int s);
char map_checkActivation(int x, int y, int activation) {
	for (int i = 0; i < map.numEvents; i++) {
		RPG_StateCharacter* ev = &map.events[i];
		if (!ev->hidden) {
			RPG_DataEvent* data = &DATA_MAPS[map.id].events[i];
			RPG_DataEventPage* page = &data->pages[ev->currPageIdx];
			if (page->activation != activation) continue;
			if (character_checkOverlap(ev, x, y, 8)) return i;
		}
	}
	return -1;
}
