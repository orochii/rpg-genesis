#pragma once
#include <resources.h>
#include "database.h"
#include "state.h"

char interpreter_evIdx;
char interpreter_pageIdx;
int interpreter_cmdIdx;
char interpreter_subIdx;
int interpreter_subCmdIdx;

bool interpreter_isRunning() {
    return (interpreter_evIdx >= 0) || (interpreter_subIdx >= 0);
}
RPG_StateCharacter* interpreter_getEvent() {
    return &map.events[interpreter_evIdx];
}

int* interpreter_update() {
    int* command = 0;
    if (interpreter_subIdx >= 0) {
        // TODO
        interpreter_subIdx = -1;
        return 0;
    } else {
        RPG_DataEvent* ev = &DATA_MAPS[map.id].events[interpreter_evIdx];
        RPG_DataEventPage* page = &ev->pages[interpreter_pageIdx];
        command = &page->script[interpreter_cmdIdx];
        interpreter_cmdIdx += interpreter_commandSize(*command);
        if (interpreter_cmdIdx >= page->scriptLen) {
            interpreter_evIdx = -1;
        }
    }
    return command;
}
int interpreter_commandSize(int code) {
    switch (code) {
        case CMD_MESSAGE:
            return 7;
        case CMD_SETFLAG:
            return 3;
        case CMD_LABEL:
            return 2;
        case CMD_JUMP:
            return 5;
        case CMD_OPTIONS:
            return 8;
        case CMD_BATTLE:
            return 2;
        case CMD_SHOP:
            return 3;
        case CMD_NAMING:
            return 2;
        default:
            return 1;
    }
}

void interpreter_resetSub() {
	interpreter_subIdx = -1;
}
void interpreter_setSubEvent(int idx) {
	interpreter_subIdx = idx;
	interpreter_subCmdIdx = 0;
}
void interpreter_reset() {
	interpreter_evIdx = -1;
	interpreter_evIdx = -1;
	interpreter_resetSub();
}
void interpreter_setEvent(char evIdx) {
	interpreter_evIdx = evIdx;
	interpreter_evIdx = map.events[evIdx].currPageIdx;
	interpreter_cmdIdx = 0;
	interpreter_resetSub();
}
int interpreter_findLabel(u16 label) {
    RPG_DataEvent* ev = &DATA_MAPS[map.id].events[interpreter_evIdx];
    RPG_DataEventPage* page = &ev->pages[interpreter_pageIdx];
    int i = 0;
    int* command = 0;
    while (i < page->scriptLen) {
        command = &page->script[i];
        if (command[0] == CMD_LABEL) {
            if (command[1] == label) return i;
        }
        i += interpreter_commandSize(command[0]);
    }
}