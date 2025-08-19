#pragma once
#include <resources.h>

typedef struct {
	bool initialized;
	bool destroyed;
	callback_t create;
	callback_t update;
	callback_t destroy;
} Scene;
Scene scene;

void scene_reinit() {
	// Reset ind
	tileIdx = TILE_USER_INDEX;
}

void scene_update() {
	// Destroy first
	if (scene.destroyed) {
		scene.update();
		scene.destroyed = false;
	}
	// Create
	if (!scene.initialized) {
		scene_reinit();
		scene.create();
		scene.initialized = true;
	}
	// Update
	scene.update();
}

void scene_goto(void* create, void* update, void* destroy) {
	if (scene.initialized) {
		scene.initialized = false;
		scene.destroy();
		scene.destroyed = true;
		// Vsync
		SYS_doVBlankProcess();
	}
	scene.create = create;
	scene.update = update;
	scene.destroy = destroy;
}
