#pragma once
#include <genesis.h>

u16 input_lastState = 0;
u16 input_currState = 0;

/*static void joyEvent(u16 joy, u16 changed, u16 state) {
	// you didn't do aaaanything
}*/

void input_init() {
	//JOY_setEventHandler(joyEvent);
}
void input_update() {
	input_lastState = input_currState;
	input_currState = JOY_readJoypad(JOY_1);
}
bool input_trigger(u16 button) {
	return !(input_lastState & button) && (input_currState & button);
}
bool input_release(u16 button) {
	return (input_lastState & button) && !(input_currState & button);
}
bool input_press(u16 button) {
	return (input_currState & button);
}
