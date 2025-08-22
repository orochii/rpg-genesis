#pragma once
#include <genesis.h>

u16 input_lastState = 0;
u16 input_currState = 0;
u8 input_repeatState[16];

/*static void joyEvent(u16 joy, u16 changed, u16 state) {
	// you didn't do aaaanything
}*/

void input_init() {
	for(int i=0; i<16; i++) {
		input_repeatState[i] = 0;
	}
	//JOY_setEventHandler(joyEvent);
}
void input_update() {
	input_lastState = input_currState;
	input_currState = JOY_readJoypad(JOY_1);
	for(int i=0; i<16; i++) {
		if (input_repeatState[i] > 0) input_repeatState[i]--;
	}
}
bool input_trigger(u16 button) {
	return !(input_lastState & button) && (input_currState & button);
}
u16 input_getRepeatIdx(u16 button) {
	switch (button) {
		case BUTTON_UP:
			return 0;
		case BUTTON_DOWN:
			return 1;
		case BUTTON_LEFT:
			return 2;
		case BUTTON_RIGHT:
			return 3;
		case BUTTON_A:
			return 4;
		case BUTTON_B:
			return 5;
		case BUTTON_C:
			return 6;
		case BUTTON_START:
			return 7;
		case BUTTON_X:
			return 8;
		case BUTTON_Y:
			return 9;
		case BUTTON_Z:
			return 10;
		case BUTTON_MODE:
			return 11;
		default:
			return 15;
	}
}

bool input_release(u16 button) {
	return (input_lastState & button) && !(input_currState & button);
}
bool input_press(u16 button) {
	return (input_currState & button);
}

bool input_repeat(u16 button) {
	u16 idx = input_getRepeatIdx(button);
	if (input_trigger(button)) {
		input_repeatState[idx] = INPUT_REPEAT_L;
		return true;
	}
	if (input_repeatState[idx] > 0) return false;
	if (input_press(button)) {
		input_repeatState[idx] = INPUT_REPEAT_S;
		return true;
	}
	return false;
}