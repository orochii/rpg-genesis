#include "main.h"

int main(bool hard)
{
	// Initialize sprite system
	SPR_init();
	PAL_setPalette(PAL3, pa_system.data, DMA);
	sys_setFont(false);
	// Initialize game
	state_init();
    /// SCENE CREATION
	scene_goto(scenemap_create, scenemap_update, scenemap_destroy);
	// Main loop
	while(1)
	{
		input_update();
		scene_update();
        // Vsync
		SYS_doVBlankProcess();
	}
	return (0);
}
