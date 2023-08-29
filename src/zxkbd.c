#include "osdep.h"
#include "zxkbd.h"
#include "debug.h"

typedef struct {
	u8 rownum; //row position
	u8 bitmask;  //bit number
	u8 rownumalt; //row position alternate
	u8 bitmaskalt;  //bit number alternate
	char name[13];	//key name
} sKeyInfo;

sKeyInfo KeyInfo[44];

u8 getRow(int nPosition) {
	return KeyInfo[nPosition].rownum;
}

u8 getRowAlt(int nPosition) {
	return KeyInfo[nPosition].rownumalt;
}

u8 getPressMask(int nPosition) {
	return (KeyInfo[nPosition].bitmask) & 0xFF;
}

u8 getReleaseMask(int nPosition) {
	return (~((KeyInfo[nPosition].bitmask) & 0xFF)) & 0xFF;
}

u8 getPressMaskAlt(int nPosition) {
	return (KeyInfo[nPosition].bitmaskalt) & 0xFF;
}

u8 getReleaseMaskAlt(int nPosition) {
	return (~((KeyInfo[nPosition].bitmaskalt) & 0xFF)) & 0xFF;
}

u8 getKeyPosition(const char *strKey) {
	u8 nRet = 5; //Poslu kod Enter, kdyz nenajdu shodu
	for (int i = 0; i < 44; i++) {
		if (strcmp(KeyInfo[i].name, strKey) == 0) {
			nRet = i;
			break;
		}
	}
	return nRet;
}

void getKeyName(char *strName, int nPosition) {
	memcpy(strName, KeyInfo[nPosition].name, strlen(KeyInfo[nPosition].name) + 1);
}

void fillKeyInfo() {
// Row B - Break/Space
	KeyInfo[0].rownum = KeyInfo[1].rownum = KeyInfo[2].rownum = KeyInfo[3].rownum = KeyInfo[4].rownum = 7;
	KeyInfo[0].rownumalt = KeyInfo[1].rownumalt = KeyInfo[2].rownumalt = KeyInfo[3].rownumalt = KeyInfo[4].rownumalt = 255;
	KeyInfo[0].bitmask = KEY_BIT0;
	memcpy(KeyInfo[0].name, "Break Space", 12);

	KeyInfo[1].bitmask = KEY_BIT1;
	memcpy(KeyInfo[1].name, "Symbol Shift", 13);

	KeyInfo[2].bitmask = KEY_BIT2;
	memcpy(KeyInfo[2].name, "M", 2);

	KeyInfo[3].bitmask = KEY_BIT3;
	memcpy(KeyInfo[3].name, "N", 2);

	KeyInfo[4].bitmask = KEY_BIT4;
	memcpy(KeyInfo[4].name, "B", 2);

// Row ENTER - H
	KeyInfo[5].rownum = KeyInfo[6].rownum = KeyInfo[7].rownum = KeyInfo[8].rownum = KeyInfo[9].rownum = 6;
	KeyInfo[5].rownumalt = KeyInfo[6].rownumalt = KeyInfo[7].rownumalt = KeyInfo[8].rownumalt = KeyInfo[9].rownumalt = 255;
	KeyInfo[5].bitmask = KEY_BIT0;
	memcpy(KeyInfo[5].name, "Enter", 6);

	KeyInfo[6].bitmask = KEY_BIT1;
	memcpy(KeyInfo[6].name, "L", 2);

	KeyInfo[7].bitmask = KEY_BIT2;
	memcpy(KeyInfo[7].name, "K", 2);

	KeyInfo[8].bitmask = KEY_BIT3;
	memcpy(KeyInfo[8].name, "J", 2);

	KeyInfo[9].bitmask = KEY_BIT4;
	memcpy(KeyInfo[9].name, "H", 2);

// Row P - Y
	KeyInfo[10].rownum = KeyInfo[11].rownum = KeyInfo[12].rownum = KeyInfo[13].rownum = KeyInfo[14].rownum = 5;
	KeyInfo[10].rownumalt = KeyInfo[11].rownumalt = KeyInfo[12].rownumalt = KeyInfo[13].rownumalt = KeyInfo[14].rownumalt = 255;
	KeyInfo[10].bitmask = KEY_BIT0;
	memcpy(KeyInfo[10].name, "P", 2);

	KeyInfo[11].bitmask = KEY_BIT1;
	memcpy(KeyInfo[11].name, "O", 2);

	KeyInfo[12].bitmask = KEY_BIT2;
	memcpy(KeyInfo[12].name, "I", 2);

	KeyInfo[13].bitmask = KEY_BIT3;
	memcpy(KeyInfo[13].name, "U", 2);

	KeyInfo[14].bitmask = KEY_BIT4;
	memcpy(KeyInfo[14].name, "Y", 2);

// Row 0 - 6
	KeyInfo[15].rownum = KeyInfo[16].rownum = KeyInfo[17].rownum = KeyInfo[18].rownum = KeyInfo[19].rownum = 4;
	KeyInfo[15].rownumalt = KeyInfo[16].rownumalt = KeyInfo[17].rownumalt = KeyInfo[18].rownumalt = KeyInfo[19].rownumalt = 255;
	KeyInfo[15].bitmask = KEY_BIT0;
	memcpy(KeyInfo[15].name, "0", 2);

	KeyInfo[16].bitmask = KEY_BIT1;
	memcpy(KeyInfo[16].name, "9", 2);

	KeyInfo[17].bitmask = KEY_BIT2;
	memcpy(KeyInfo[17].name, "8", 2);

	KeyInfo[18].bitmask = KEY_BIT3;
	memcpy(KeyInfo[18].name, "7", 2);

	KeyInfo[19].bitmask = KEY_BIT4;
	memcpy(KeyInfo[19].name, "6", 2);

// Row 1 - 5
	KeyInfo[20].rownum = KeyInfo[21].rownum = KeyInfo[22].rownum = KeyInfo[23].rownum = KeyInfo[24].rownum = 3;
	KeyInfo[20].rownumalt = KeyInfo[21].rownumalt = KeyInfo[22].rownumalt = KeyInfo[23].rownumalt = KeyInfo[24].rownumalt = 255;
	KeyInfo[20].bitmask = KEY_BIT0;
	memcpy(KeyInfo[20].name, "1", 2);

	KeyInfo[21].bitmask = KEY_BIT1;
	memcpy(KeyInfo[21].name, "2", 2);

	KeyInfo[22].bitmask = KEY_BIT2;
	memcpy(KeyInfo[22].name, "3", 2);

	KeyInfo[23].bitmask = KEY_BIT3;
	memcpy(KeyInfo[23].name, "4", 2);

	KeyInfo[24].bitmask = KEY_BIT4;
	memcpy(KeyInfo[24].name, "5", 2);

// Row Q - T
	KeyInfo[25].rownum = KeyInfo[26].rownum = KeyInfo[27].rownum = KeyInfo[28].rownum = KeyInfo[29].rownum = 2;
	KeyInfo[25].rownumalt = KeyInfo[26].rownumalt = KeyInfo[27].rownumalt = KeyInfo[28].rownumalt = KeyInfo[29].rownumalt = 255;
	KeyInfo[25].bitmask = KEY_BIT0;
	memcpy(KeyInfo[25].name, "Q", 2);

	KeyInfo[26].bitmask = KEY_BIT1;
	memcpy(KeyInfo[26].name, "W", 2);

	KeyInfo[27].bitmask = KEY_BIT2;
	memcpy(KeyInfo[27].name, "E", 2);

	KeyInfo[28].bitmask = KEY_BIT3;
	memcpy(KeyInfo[28].name, "R", 2);

	KeyInfo[29].bitmask = KEY_BIT4;
	memcpy(KeyInfo[29].name, "T", 2);

// Row A - G
	KeyInfo[30].rownum = KeyInfo[31].rownum = KeyInfo[32].rownum = KeyInfo[33].rownum = KeyInfo[34].rownum = 1;
	KeyInfo[30].rownumalt = KeyInfo[31].rownumalt = KeyInfo[32].rownumalt = KeyInfo[33].rownumalt = KeyInfo[34].rownumalt = 255;
	KeyInfo[30].bitmask = KEY_BIT0;
	memcpy(KeyInfo[30].name, "A", 2);

	KeyInfo[31].bitmask = KEY_BIT1;
	memcpy(KeyInfo[31].name, "S", 2);

	KeyInfo[32].bitmask = KEY_BIT2;
	memcpy(KeyInfo[32].name, "D", 2);

	KeyInfo[33].bitmask = KEY_BIT3;
	memcpy(KeyInfo[33].name, "F", 2);

	KeyInfo[34].bitmask = KEY_BIT4;
	memcpy(KeyInfo[34].name, "G", 2);

// Row Caps Shift - V
	KeyInfo[35].rownum = KeyInfo[36].rownum = KeyInfo[37].rownum = KeyInfo[38].rownum = KeyInfo[39].rownum = 0;
	KeyInfo[35].rownumalt = KeyInfo[36].rownumalt = KeyInfo[37].rownumalt = KeyInfo[38].rownumalt = KeyInfo[39].rownumalt = 255;

	KeyInfo[35].bitmask = KEY_BIT0;
	memcpy(KeyInfo[35].name, "Caps Shift", 11);

	KeyInfo[36].bitmask = KEY_BIT1;
	memcpy(KeyInfo[36].name, "Z", 2);

	KeyInfo[37].bitmask = KEY_BIT2;
	memcpy(KeyInfo[37].name, "X", 2);

	KeyInfo[38].bitmask = KEY_BIT3;
	memcpy(KeyInfo[38].name, "C", 2);

	KeyInfo[39].bitmask = KEY_BIT4;
	memcpy(KeyInfo[39].name, "V", 2);

// 128K additional keys
	KeyInfo[40].rownum = KeyInfo[41].rownum = KeyInfo[43].rownum = 4;
	KeyInfo[40].rownumalt = KeyInfo[41].rownumalt = KeyInfo[42].rownumalt = KeyInfo[43].rownumalt = 0;

	KeyInfo[40].bitmask = KEY_BIT4;
	KeyInfo[40].bitmaskalt = KEY_BIT0;
	memcpy(KeyInfo[40].name, "Down-128k", 10);

	KeyInfo[41].bitmask = KEY_BIT3;
	KeyInfo[41].bitmaskalt = KEY_BIT0;
	memcpy(KeyInfo[41].name, "Up-128k", 8);

	KeyInfo[42].rownum = 3;
	KeyInfo[42].bitmask = KEY_BIT4;
	KeyInfo[42].bitmaskalt = KEY_BIT0;
	memcpy(KeyInfo[42].name, "Left-128k", 10);

	KeyInfo[43].bitmask = KEY_BIT2;
	KeyInfo[43].bitmaskalt = KEY_BIT0;
	memcpy(KeyInfo[43].name, "Right-128k", 11);

}

u8 usbkeys[256];
void ProcessUSBToZXKeyboard() {
	int keypos = 0;
	char keyname[2];
	keyname[1] = '\0';
	//A-Z
	for (int i = HID_KEY_A; i <= HID_KEY_Z; i++) {
		if (UsbKeyIsPressed(i)) {
			keyname[0] = 'A' - HID_KEY_A + i;
			keypos = getKeyPosition(keyname);
			rowKey[getRow(keypos)] &= getPressMask(keypos);
			usbkeys[i] = 1;
		} else {
			if (usbkeys[i] == 1) {
				keyname[0] = 'A' - HID_KEY_A + i;
				keypos = getKeyPosition(keyname);
				rowKey[getRow(keypos)] |= getReleaseMask(keypos);
				usbkeys[i] = 0;
			}
		}
	}
	//1-9
	for (int i = HID_KEY_1; i <= HID_KEY_9; i++) {
		if (UsbKeyIsPressed(i)) {
			keyname[0] = '1' - HID_KEY_1 + i;
			keypos = getKeyPosition(keyname);
			rowKey[getRow(keypos)] &= getPressMask(keypos);
			usbkeys[i] = 1;
		} else {
			if (usbkeys[i] == 1) {
				keyname[0] = '1' - HID_KEY_1 + i;
				keypos = getKeyPosition(keyname);
				rowKey[getRow(keypos)] |= getReleaseMask(keypos);
				usbkeys[i] = 0;
			}
		}
	}
	//0
	if (UsbKeyIsPressed(HID_KEY_0)) {
		keypos = getKeyPosition("0");
		rowKey[getRow(keypos)] &= getPressMask(keypos);
		usbkeys[HID_KEY_0] = 1;
	} else {
		if (usbkeys[HID_KEY_0] == 1) {
			keypos = getKeyPosition("0");
			rowKey[getRow(keypos)] |= getReleaseMask(keypos);
			usbkeys[HID_KEY_0] = 0;
		}
	}
	//SPACE
	if (UsbKeyIsPressed(HID_KEY_SPACE)) {
		keypos = getKeyPosition("Break Space");
		rowKey[getRow(keypos)] &= getPressMask(keypos);
		usbkeys[HID_KEY_SPACE] = 1;
	} else {
		if (usbkeys[HID_KEY_SPACE] == 1) {
			keypos = getKeyPosition("Break Space");
			rowKey[getRow(keypos)] |= getReleaseMask(keypos);
			usbkeys[HID_KEY_SPACE] = 0;
		}
	}
	//SYMB.SHIFT
	if (UsbKeyIsPressed(HID_KEY_CONTROL_RIGHT) || UsbKeyIsPressed(HID_KEY_CONTROL_LEFT)) {
		keypos = getKeyPosition("Symbol Shift");
		rowKey[getRow(keypos)] &= getPressMask(keypos);
		usbkeys[HID_KEY_CONTROL_RIGHT] = 1;
	} else {
		if (usbkeys[HID_KEY_CONTROL_RIGHT] == 1) {
			keypos = getKeyPosition("Symbol Shift");
			rowKey[getRow(keypos)] |= getReleaseMask(keypos);
			usbkeys[HID_KEY_CONTROL_RIGHT] = 0;
		}
	}
	//CAPS.SHIFT
	if (UsbKeyIsPressed(HID_KEY_SHIFT_RIGHT) || UsbKeyIsPressed(HID_KEY_SHIFT_LEFT)) {
		keypos = getKeyPosition("Caps Shift");
		rowKey[getRow(keypos)] &= getPressMask(keypos);
		usbkeys[HID_KEY_SHIFT_RIGHT] = 1;
	} else {
		if (usbkeys[HID_KEY_SHIFT_RIGHT] == 1) {
			keypos = getKeyPosition("Caps Shift");
			rowKey[getRow(keypos)] |= getReleaseMask(keypos);
			usbkeys[HID_KEY_SHIFT_RIGHT] = 0;
		}
	}
	//ENTER
	if (UsbKeyIsPressed(HID_KEY_ENTER)) {
		keypos = getKeyPosition("Enter");
		rowKey[getRow(keypos)] &= getPressMask(keypos);
		usbkeys[HID_KEY_ENTER] = 1;
	} else {
		if (usbkeys[HID_KEY_ENTER] == 1) {
			keypos = getKeyPosition("Enter");
			rowKey[getRow(keypos)] |= getReleaseMask(keypos);
			usbkeys[HID_KEY_ENTER] = 0;
		}
	}
	//picopad keys mapped to USB keyboard
	if (UsbKeyIsPressed(HID_KEY_ARROW_UP)) {
		rowKey[getRow(mapKeys[KBD_UP])] &= getPressMask(mapKeys[KBD_UP]);
		if (getRowAlt(mapKeys[KBD_UP]) != 255) {
			rowKey[getRowAlt(mapKeys[KBD_UP])] &= getPressMaskAlt(mapKeys[KBD_UP]);
		}
		usbkeys[HID_KEY_ARROW_UP] = 1;
	} else {
		if (usbkeys[HID_KEY_ARROW_UP] == 1) {
			rowKey[getRow(mapKeys[KBD_UP])] |= getReleaseMask(mapKeys[KBD_UP]);
			if (getRowAlt(mapKeys[KBD_UP]) != 255) {
				rowKey[getRowAlt(mapKeys[KBD_UP])] |= getReleaseMaskAlt(mapKeys[KBD_UP]);
			}
			usbkeys[HID_KEY_ARROW_UP] = 0;
		}
	}
	if (UsbKeyIsPressed(HID_KEY_ARROW_DOWN)) {
		rowKey[getRow(mapKeys[KBD_DOWN])] &= getPressMask(mapKeys[KBD_DOWN]);
		if (getRowAlt(mapKeys[KBD_DOWN]) != 255) {
			rowKey[getRowAlt(mapKeys[KBD_DOWN])] &= getPressMaskAlt(mapKeys[KBD_DOWN]);
		}
		usbkeys[HID_KEY_ARROW_DOWN] = 1;
	} else {
		if (usbkeys[HID_KEY_ARROW_DOWN] == 1) {
			rowKey[getRow(mapKeys[KBD_DOWN])] |= getReleaseMask(mapKeys[KBD_DOWN]);
			if (getRowAlt(mapKeys[KBD_DOWN]) != 255) {
				rowKey[getRowAlt(mapKeys[KBD_DOWN])] |= getReleaseMaskAlt(mapKeys[KBD_DOWN]);
			}
			usbkeys[HID_KEY_ARROW_DOWN] = 0;
		}
	}
	if (UsbKeyIsPressed(HID_KEY_ARROW_LEFT)) {
		rowKey[getRow(mapKeys[KBD_LEFT])] &= getPressMask(mapKeys[KBD_LEFT]);
		if (getRowAlt(mapKeys[KBD_LEFT]) != 255) {
			rowKey[getRowAlt(mapKeys[KBD_LEFT])] &= getPressMaskAlt(mapKeys[KBD_LEFT]);
		}
		usbkeys[HID_KEY_ARROW_LEFT] = 1;
	} else {
		if (usbkeys[HID_KEY_ARROW_LEFT] == 1) {
			rowKey[getRow(mapKeys[KBD_LEFT])] |= getReleaseMask(mapKeys[KBD_LEFT]);
			if (getRowAlt(mapKeys[KBD_LEFT]) != 255) {
				rowKey[getRowAlt(mapKeys[KBD_LEFT])] |= getReleaseMaskAlt(mapKeys[KBD_LEFT]);
			}
			usbkeys[HID_KEY_ARROW_LEFT] = 0;
		}
	}
	if (UsbKeyIsPressed(HID_KEY_ARROW_RIGHT)) {
		rowKey[getRow(mapKeys[KBD_RIGHT])] &= getPressMask(mapKeys[KBD_RIGHT]);
		if (getRowAlt(mapKeys[KBD_RIGHT]) != 255) {
			rowKey[getRowAlt(mapKeys[KBD_RIGHT])] &= getPressMaskAlt(mapKeys[KBD_RIGHT]);
		}
		usbkeys[HID_KEY_ARROW_RIGHT] = 1;
	} else {
		if (usbkeys[HID_KEY_ARROW_RIGHT] == 1) {
			rowKey[getRow(mapKeys[KBD_RIGHT])] |= getReleaseMask(mapKeys[KBD_RIGHT]);
			if (getRowAlt(mapKeys[KBD_RIGHT]) != 255) {
				rowKey[getRowAlt(mapKeys[KBD_RIGHT])] |= getReleaseMaskAlt(mapKeys[KBD_RIGHT]);
			}
			usbkeys[HID_KEY_ARROW_RIGHT] = 0;
		}
	}

	if (UsbKeyIsPressed(HID_KEY_PAGE_DOWN)) {
		rowKey[getRow(mapKeys[KBD_A])] &= getPressMask(mapKeys[KBD_A]);
		usbkeys[HID_KEY_PAGE_DOWN] = 1;
	} else {
		if (usbkeys[HID_KEY_PAGE_DOWN] == 1) {
			rowKey[getRow(mapKeys[KBD_A])] |= getReleaseMask(mapKeys[KBD_A]);
			usbkeys[HID_KEY_PAGE_DOWN] = 0;
		}
	}
	if (UsbKeyIsPressed(HID_KEY_END)) {
		rowKey[getRow(mapKeys[KBD_B])] &= getPressMask(mapKeys[KBD_B]);
		usbkeys[HID_KEY_END] = 1;
	} else {
		if (usbkeys[HID_KEY_END] == 1) {
			rowKey[getRow(mapKeys[KBD_B])] |= getReleaseMask(mapKeys[KBD_B]);
			usbkeys[HID_KEY_END] = 0;
		}
	}
	if (UsbKeyIsPressed(HID_KEY_DELETE)) {
		rowKey[getRow(mapKeys[KBD_Y])] &= getPressMask(mapKeys[KBD_Y]);
		usbkeys[HID_KEY_DELETE] = 1;
	} else {
		if (usbkeys[HID_KEY_DELETE] == 1) {
			rowKey[getRow(mapKeys[KBD_Y])] |= getReleaseMask(mapKeys[KBD_Y]);
			usbkeys[HID_KEY_DELETE] = 0;
		}
	}
//Some of ZX Spectrum + keyboard keys
	if (UsbKeyIsPressed(HID_KEY_BACKSPACE)) {
			keypos = getKeyPosition("Caps Shift");
			rowKey[getRow(keypos)] &= getPressMask(keypos);
			keypos = getKeyPosition("0");
			rowKey[getRow(keypos)] &= getPressMask(keypos);
			usbkeys[HID_KEY_BACKSPACE] = 1;
		} else {
			if (usbkeys[HID_KEY_BACKSPACE] == 1) {
				keypos = getKeyPosition("Caps Shift");
				rowKey[getRow(keypos)] |= getReleaseMask(keypos);
				keypos = getKeyPosition("0");
				rowKey[getRow(keypos)] |= getReleaseMask(keypos);
				usbkeys[HID_KEY_BACKSPACE] = 0;
			}
		}

	if (UsbKeyIsPressed(HID_KEY_PERIOD)) {
		keypos = getKeyPosition("Symbol Shift");
		rowKey[getRow(keypos)] &= getPressMask(keypos);
		keypos = getKeyPosition("M");
		rowKey[getRow(keypos)] &= getPressMask(keypos);
		usbkeys[HID_KEY_PERIOD] = 1;
	} else {
		if (usbkeys[HID_KEY_PERIOD] == 1) {
			keypos = getKeyPosition("Symbol Shift");
			rowKey[getRow(keypos)] |= getReleaseMask(keypos);
			keypos = getKeyPosition("M");
			rowKey[getRow(keypos)] |= getReleaseMask(keypos);
			usbkeys[HID_KEY_PERIOD] = 0;
		}
	}

	if (UsbKeyIsPressed(HID_KEY_COMMA)) {
		keypos = getKeyPosition("Symbol Shift");
		rowKey[getRow(keypos)] &= getPressMask(keypos);
		keypos = getKeyPosition("N");
		rowKey[getRow(keypos)] &= getPressMask(keypos);
		usbkeys[HID_KEY_COMMA] = 1;
	} else {
		if (usbkeys[HID_KEY_COMMA] == 1) {
			keypos = getKeyPosition("Symbol Shift");
			rowKey[getRow(keypos)] |= getReleaseMask(keypos);
			keypos = getKeyPosition("N");
			rowKey[getRow(keypos)] |= getReleaseMask(keypos);
			usbkeys[HID_KEY_COMMA] = 0;
		}
	}

	if (UsbKeyIsPressed(HID_KEY_APOSTROPHE)) {
		keypos = getKeyPosition("Symbol Shift");
		rowKey[getRow(keypos)] &= getPressMask(keypos);
		keypos = getKeyPosition("P");
		rowKey[getRow(keypos)] &= getPressMask(keypos);
		usbkeys[HID_KEY_APOSTROPHE] = 1;
	} else {
		if (usbkeys[HID_KEY_APOSTROPHE] == 1) {
			keypos = getKeyPosition("Symbol Shift");
			rowKey[getRow(keypos)] |= getReleaseMask(keypos);
			keypos = getKeyPosition("P");
			rowKey[getRow(keypos)] |= getReleaseMask(keypos);
			usbkeys[HID_KEY_APOSTROPHE] = 0;
		}
	}

	if (UsbKeyIsPressed(HID_KEY_SEMICOLON)) {
		keypos = getKeyPosition("Symbol Shift");
		rowKey[getRow(keypos)] &= getPressMask(keypos);
		keypos = getKeyPosition("O");
		rowKey[getRow(keypos)] &= getPressMask(keypos);
		usbkeys[HID_KEY_SEMICOLON] = 1;
	} else {
		if (usbkeys[HID_KEY_SEMICOLON] == 1) {
			keypos = getKeyPosition("Symbol Shift");
			rowKey[getRow(keypos)] |= getReleaseMask(keypos);
			keypos = getKeyPosition("O");
			rowKey[getRow(keypos)] |= getReleaseMask(keypos);
			usbkeys[HID_KEY_SEMICOLON] = 0;
		}
	}

	UsbFlushKey();

}
