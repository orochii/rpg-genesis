#pragma once
#include <resources.h>
#include <genesis.h>

u16 sys_pal = PAL3;

void sys_drawWindow(u16 x, u16 y, u16 w, u16 h, u16 indexStart) {
	w-=1;
	h-=1;
	u16 xw = x+w;
	u16 yh = y+h;
	u16 t_tl = TILE_ATTR_FULL(sys_pal, TRUE, 0, 0, indexStart);
	u16 t_tr = TILE_ATTR_FULL(sys_pal, TRUE, 0, 1, indexStart);
	u16 t_bl = TILE_ATTR_FULL(sys_pal, TRUE, 1, 0, indexStart);
	u16 t_br = TILE_ATTR_FULL(sys_pal, TRUE, 1, 1, indexStart);
	u16 t_tm = TILE_ATTR_FULL(sys_pal, TRUE, 0, 0, indexStart+1);
	u16 t_bm = TILE_ATTR_FULL(sys_pal, TRUE, 1, 0, indexStart+1);
	u16 t_ml = TILE_ATTR_FULL(sys_pal, TRUE, 0, 0, indexStart+2);
	u16 t_mr = TILE_ATTR_FULL(sys_pal, TRUE, 0, 1, indexStart+2);
	u16 t_mm = TILE_ATTR_FULL(sys_pal, TRUE, 0, 0, indexStart+3);
	//
	for (int yy = y; yy <= yh; yy++) {
		for (int xx = x; xx <= xw; xx++) {
			u16 t = t_mm;
			if (xx==x) { // Left side
				if (yy==y) t = t_tl;
				else if (yy==yh) t = t_bl;
				else t = t_ml;
			}
			else if (xx==xw) { // right side
				if (yy==y) t = t_tr;
				else if (yy==yh) t = t_br;
				else t = t_mr;
			}
			else {	// middle
				if (yy==y) t = t_tm;
				else if (yy==yh) t = t_bm;
				else t = t_mm;
			}
			VDP_setTileMapXY(BG_A, t, xx, yy);
		}
	}
}

void sys_setFont(bool transparent) {
	if (transparent) VDP_loadTileSet(&ts_font0, 1440, DMA);
	else VDP_loadTileSet(&ts_font1, 1440, DMA);
}

void sys_drawItemDesc(u16 x, u16 y, u16 w, u16 idx) {
	// Clear
	u16 t_mm = TILE_ATTR_FULL(sys_pal, TRUE, 0, 0, TILE_USER_INDEX+3);
    VDP_fillTileMapRect(BG_A,t_mm,x,y,w,1);
	// Draw
	u8 itemId = party.inventoryItemId[idx];
	if (itemId==0) return;
    char* desc = DATA_ITEMS[itemId-1].desc;
    sys_drawText(desc,x,y,false);
}
void sys_drawItemList(u16 x, u16 y, u16 w, u16 h, u16 topRow, u16 selectedIdx) {
	char* str[w];
	u16 t_mm = TILE_ATTR_FULL(sys_pal, TRUE, 0, 0, TILE_USER_INDEX+3);
	VDP_fillTileMapRect(BG_A, t_mm, x,y, w,h);
	for (int i=0; i<h; i++) {
		int idx = topRow + i;
		if (idx >= 60) break;
		u8 itemId = party.inventoryItemId[idx];
		u8 itemQty = party.inventoryItemQty[idx];
		if (itemId==0 || itemQty==0) break;
		char* item = DATA_ITEMS[itemId-1].name;
		bool selected = idx == selectedIdx;
		if (selected) {
			sprintf(str, "`%s", item);
		} else {
			sprintf(str, " %s", item);
		}
		sys_drawText(str, x, y+i, selected);
		sprintf(str, ":%2d", itemQty);
		sys_drawText(str, x+w-3, y+i, selected);
	}
}
void sys_drawSkillDesc(u16 x, u16 y, u16 w, u16 idx, u8* list) {
	// Clear
	u16 t_mm = TILE_ATTR_FULL(sys_pal, TRUE, 0, 0, TILE_USER_INDEX+3);
    VDP_fillTileMapRect(BG_A,t_mm,x,y,w,1);
	// Draw
	u8 skillId = list[idx];
	if (skillId==0) return;
    char* desc = DATA_SKILLS[skillId-1].desc;
    sys_drawText(desc,x,y,false);
}
void sys_drawSkillList(u16 x, u16 y, u16 w, u16 h, u16 topRow, u16 selectedIdx, u8* list) {
	char* str[w];
	u16 t_mm = TILE_ATTR_FULL(sys_pal, TRUE, 0, 0, TILE_USER_INDEX+3);
	VDP_fillTileMapRect(BG_A, t_mm, x,y, w,h);
	u16 i = 0;
	while(list[i+topRow] != 0) {
		int idx = topRow + i;
		u8 skillId = list[idx];
		RPG_DataSkill* item = &DATA_SKILLS[skillId-1];
		bool selected = idx == selectedIdx;
		if (selected) {
			sprintf(str, "`%s", item->name);
		} else {
			sprintf(str, " %s", item->name);
		}
		sys_drawText(str, x, y+i, selected);
		sprintf(str, " %2d", item->mpCost);
		sys_drawText(str, x+w-3, y+i, selected);
		i++;
	}
}

void sys_drawOptions(const char** list, u16 size, u16 x, u16 y, u16 selectedIdx) {
	char* str[16];
	for (int idx = 0; idx < size; idx++) {
		char* item = list[idx];
		if (idx == selectedIdx) {
			sprintf(str, "`%s", item);
			sys_drawText(str, x, y+idx, true);
		} else {
			sprintf(str, " %s", item);
			sys_drawText(str, x, y+idx, false);
		}
	}
}

void sys_drawText(const char* str, u16 x, u16 y, bool system) {
	u16 basetile = system ? TILE_ATTR_FULL(PAL2, TRUE, FALSE, FALSE, 0) : TILE_ATTR_FULL(sys_pal, TRUE, FALSE, FALSE, 0);
	VDP_drawTextEx(BG_A, str, basetile, x, y, DMA);
}
void sys_drawTextBack(const char* str, u16 x, u16 y, u16 pal) {
	u16 basetile = TILE_ATTR_FULL(pal, TRUE, FALSE, FALSE, 0);
	VDP_drawTextEx(BG_B, str, basetile, x, y, DMA);
}

void sys_processText(char* to, u16 len, const char* from) {
	char *src = from;
    char *dst = to;
	u16 dstLen = len;
	bool extracting = false;
	char code = ' ';
	char* param = malloc(4);
	char* paramIdx = param;
	int exIdx = -1;
	// 
	while (dstLen && *src) {
		if (*src == '%') {
			extracting = !extracting;
			if (extracting) {
				// Start extracting
				param[0] = 0; param[2] = 0;
				param[1] = 0; param[3] = 0;
			} else {
				// Stop extracting
				u16 a = 0;
				int i = 0;
				for (char* p = param; *p /*i < exIdx*/; p++) {
					u16 d = (*p - '0');
					u16 s = (exIdx - i) - 1;
					u16 pp = s==0 ? 1 : (s==1 ? 10 : 100);
					a += d * pp;
					i++;
				}
				switch(code) {
					case 'h':
						exIdx = strlen(heroes[a].name);
						strncpy(dst, heroes[a].name, exIdx);
						dst += exIdx;
						break;
					case 'f':
						char* buffer[6];
						//sprintf(buffer, "%d", stateFlags[a]);
						intToStr(stateFlags[a], buffer, 1);
						exIdx = strlen(buffer);
						strncpy(dst, buffer, exIdx);
						dst += exIdx;
						break;
				}
				// Reset param
				paramIdx = param;
				exIdx = -1;
			}
		} else {
			if (extracting) {
				if (exIdx < 0) {
					code = *src;
					exIdx = 0;
				}
				else if(exIdx < 3) {
					*paramIdx++ = *src;
					exIdx += 1;
				}
			} else {
				*dst = *src;
				dst++;
				dstLen--;
			}
		}
		// Continue
		src++;
	}
	// Erase anything else
	for (; *dst; dst++) {
		*dst = 0;
	}
}
u16 sys_countCharacters(char* str) {
	int i, count;
	for (i=0, count=0; str[i]; i++) {
		count += (str[i] == '&');
	}
	return count;
}

bool sys_compare(u16 value, u16 compare, u16 check) {
	switch (compare) {
		case EVP_CMP_EQUAL:
			return value == check;
		case EVP_CMP_LESS:
			return value < check;
		case EVP_CMP_MORE:
			return value > check;
		case EVP_CMP_LESSEQ:
			return value <= check;
		case EVP_CMP_MOREEQ:
			return value >= check;
		case EVP_CMP_NOTEQUAL:
			return value != check;
	}
}

void sys_fadeOut(u16 frames) {
	u16 frameCount = frames;
    PAL_fadeOutAll(frames, true);
    while (frameCount > 0) {
        frameCount--;
        SYS_doVBlankProcess();
    }
}

void sys_fadeIn(u16 frames) {
	u16 frameCount = frames;
	u16* pal[64];
	PAL_getPalette(PAL0, pal[0]);
	PAL_getPalette(PAL1, pal[16]);
	PAL_getPalette(PAL2, pal[32]);
	PAL_getPalette(PAL3, pal[48]);
	PAL_fadeInAll(pal, frames, false);
	/*while (frameCount > 0) {
        frameCount--;
        SYS_doVBlankProcess();
    }*/
}
