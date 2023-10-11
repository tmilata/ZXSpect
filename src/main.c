#include "osdep.h"
#include "zxem.h"
#include "debug.h"
#include "palette.h"
#include "zxtap.h"
#include "fileop.h"
#include "zxkbd.h"
#include "zxini.h"
#include "zxsna.h"
#include "zxsnd.h"
#include "ay8912.h"

unsigned char *membank[5];

void defaultKeyMap() {
//default keys mapping - QAOPM
	mapKeys[KBD_UP] = getKeyPosition("Q");
	mapKeys[KBD_DOWN] = getKeyPosition("A");
	mapKeys[KBD_LEFT] = getKeyPosition("O");
	mapKeys[KBD_RIGHT] = getKeyPosition("P");
	mapKeys[KBD_A] = getKeyPosition("M");
	if (spectrum_model == ZX128) {
		mapKeys[KBD_UP] = getKeyPosition("Up-128k");
		mapKeys[KBD_DOWN] = getKeyPosition("Down-128k");
		mapKeys[KBD_LEFT] = getKeyPosition("Left-128k");
		mapKeys[KBD_RIGHT] = getKeyPosition("Right-128k");
		mapKeys[KBD_A] = getKeyPosition("Enter");
	}
	mapKeys[KBD_Y] = getKeyPosition("Enter");
	mapKeys[KBD_B] = getKeyPosition("Break Space");
}

int main(int argc, char *argv[]) {
//overclocking picopad
	ClockPllSysFreq(270000);
	ClockSetup(CLK_SYS, CLK_PLL_SYS, 0, 0);
	InitDirtyTiles();
	currPath[0] = '/';
	currPath[1] = 0;
	strMapFile[0] = 0;
	fillKeyInfo();
	for (int i = 0; i < 8; i++)
		rowKey[i] = 191;
	beeper = 0;
#define ZX_SOUND_TICKS_SMP 159
	ay_init(&ay0, ZX_SOUND_TICKS_SMP);
	ay_reset(&ay0);
	setVolume(5);
	setModel(ZX48);
	readIniFile("/ZXSpec.ini");
	defaultKeyMap();
	UsbHostInit();
	ZX_main(argc, argv);
	return 0;
}

//maps usb keyboard keys to picopad keys in menus
char USBMenuMap(char ch) {
	if ((UsbKeyIsMounted() && (ch == NOKEY))) {
		if (UsbKeyIsPressed(HID_KEY_ARROW_UP)) {
			ch = KEY_UP;
		}
		if (UsbKeyIsPressed(HID_KEY_ARROW_DOWN)) {
			ch = KEY_DOWN;
		}
		if (UsbKeyIsPressed(HID_KEY_ARROW_LEFT)) {
			ch = KEY_LEFT;
		}
		if (UsbKeyIsPressed(HID_KEY_ARROW_RIGHT)) {
			ch = KEY_RIGHT;
		}
		if (UsbKeyIsPressed(HID_KEY_ENTER)) {
			ch = KEY_A;
			WaitMs(200);
		}
		if (UsbKeyIsPressed(HID_KEY_ESCAPE) || UsbKeyIsPressed(HID_KEY_F3)) {
			ch = KEY_X;
			WaitMs(200);
		}
		if (UsbKeyIsPressed(HID_KEY_F5) || UsbKeyIsPressed(HID_KEY_B) || UsbKeyIsPressed(HID_KEY_S)) {
			ch = KEY_B;
		}
		if (UsbKeyIsPressed(HID_KEY_F8) || UsbKeyIsPressed(HID_KEY_Y) || UsbKeyIsPressed(HID_KEY_L)) {
			ch = KEY_Y;
		}
		UsbFlushKey();
		if (ch != NOKEY)
			WaitMs(70);
	}
	return ch;
}

bool MenuModel() {
	bool bExit = false;
	bool bSave = false;
	SelFont8x8();
	char *keyModel[] = { "Model     = ", "AY Sound  = ", "\0" };
	int nBkpModel = spectrum_model;
	bool ay0_enable_bkp = ay0_enable;
	int nMenuPos = 0;
	bool bRefresh = true;
	char strBuffer[40];
	while (bExit == false) {
		char ch = USBMenuMap(KeyGet());
		switch (ch) {
		case KEY_X:
			if (nBkpModel != spectrum_model)
				bSave = true;
			if ((bSave) || (ay0_enable_bkp != ay0_enable)) {
				saveIniFile("/ZXSpec.ini");
			}
			bExit = true;
			break;
		case KEY_LEFT:
		case KEY_RIGHT:
		case KEY_A:
			KeyFlush();
			switch (nMenuPos) {
			case 0: {
				int nLocModel = spectrum_model;
				nLocModel++;
				if (nLocModel >= ZX_LAST) {
					nLocModel = ZX48;
				}
				setModel((specmodel) nLocModel);
				if (nLocModel == ZX128) {
					ay0_enable = true;
				}
			}
				break;
			case 1:
				ay0_enable = !ay0_enable;
				break;
			}
			bRefresh = true;
			break;
		case KEY_UP:
			nMenuPos--;
			if (nMenuPos < 0)
				nMenuPos = 1;
			bRefresh = true;
			break;
		case KEY_DOWN:
			nMenuPos++;
			if (nMenuPos > 1)
				nMenuPos = 0;
			bRefresh = true;
			break;
		default:
			break;
		}
		if (bRefresh) {
			DrawClearZx();
			DrawText2Zx("Select model", (WIDTH - 16 * 16) / 2, 24,
			COL_YELLOW_ZX);
			int i = 0;
			int nPosY = 48;
			while (keyModel[i][0] != 0) {
				int nColFrg = COL_WHITE_ZX;
				int nColBkg = COL_BLACK_ZX;
				if (i == nMenuPos) {
					nColFrg = COL_BLACK_ZX;
					nColBkg = COL_WHITE_ZX;
				}
				memcpy(strBuffer, &keyModel[i][0], 12);
				//getKeyName(&strBuffer[12], mapKeys[i]);
				switch (i) {
				case 0:
					if (spectrum_model == ZX48) {
						strcpy(&strBuffer[12], "48K");
					}
					if (spectrum_model == ZX128) {
						strcpy(&strBuffer[12], "128K");
					}
					break;
				case 1:
					if (ay0_enable) {
						strcpy(&strBuffer[12], "Enabled");
					} else {
						strcpy(&strBuffer[12], "Disabled");
					}
					break;
				}

				DrawTextBgZx(strBuffer, 32, nPosY, nColFrg, nColBkg);
				nPosY += FONTH;
				i++;
			}
			DispUpdateZx();
			bRefresh = false;
		}
	}
	return bSave;
}

//Select snapshot menu
bool MenuSNA() {
	bool bRet = true;
	SelFont8x8();
	LoadFileList(currPath);
	unsigned char ch;
	bool bExit = false;
	int nMaxFiles = getFilesNum();
	bool bRefresh = true;
	DispFileList(nFirstShowed, nMenuPos);
	DispUpdateZx();
	while (bExit == false) {
		ch = USBMenuMap(KeyGet());
		switch (ch) {
		case KEY_UP:
			nMenuPos--;
			if (nMenuPos < 0)
				nMenuPos = 0;
			if (nMenuPos < nFirstShowed) {
				nFirstShowed--;
				if (nFirstShowed < 0) {
					nFirstShowed = 0;
				}
			}
			bRefresh = true;
			break;
		case KEY_DOWN:
			nMenuPos++;
			if (nMenuPos >= nMaxFiles)
				nMenuPos = nMaxFiles - 1;
			if (nMenuPos >= nFirstShowed + FILEROWS) {
				nFirstShowed++;
				if (nFirstShowed + FILEROWS > nMaxFiles) {
					nFirstShowed--;
				}
			}
			bRefresh = true;
			break;

		case KEY_LEFT:
			nMenuPos -= FILEROWS;
			nFirstShowed -= FILEROWS;
			if (nFirstShowed < 0)
				nFirstShowed = 0;
			if (nMenuPos < 0)
				nMenuPos = 0;
			if (nMenuPos < nFirstShowed) {
				nFirstShowed--;
				if (nFirstShowed < 0) {
					nFirstShowed = 0;
				}
			}
			bRefresh = true;
			break;
		case KEY_RIGHT:
			nMenuPos += FILEROWS;
			if (FILEROWS < nMaxFiles) {
				nFirstShowed += FILEROWS;
				if (nFirstShowed + FILEROWS >= nMaxFiles)
					nFirstShowed = nMaxFiles - FILEROWS;
			}
			if (nMenuPos >= nMaxFiles)
				nMenuPos = nMaxFiles - 1;
			if (nMenuPos >= nFirstShowed + FILEROWS) {
				nFirstShowed++;
				if (nFirstShowed + FILEROWS > nMaxFiles) {
					nFirstShowed--;
				}
			}
			bRefresh = true;
			break;
		case KEY_A:
			KeyFlush();
			char strFileName[PATHMAX];
			if (isFileAtPos(nMenuPos)) {
				//spoustim soubor
				getNameAtPos(strFileName, nMenuPos);
				//jedna se o tap?
				int nLenTp = strlen(strFileName);
				if (nLenTp > 4) {
					if ((strFileName[nLenTp - 3] == 'T') && (strFileName[nLenTp - 2] == 'A') && (strFileName[nLenTp - 1] = 'P')) {
						assignTapFile(strFileName);
						setAutoLoadOn();
						strMapFile[0] = 0;
						SndInit();
						ay_reset(&ay0);
						defaultKeyMap();
						CPU_Reset(CPU_Handle);
						strcpy(strMapFile, strFileName);
						int nLenNm = strlen(strMapFile);
						strMapFile[nLenNm - 3] = 'M';
						strMapFile[nLenNm - 2] = 'A';
						strMapFile[nLenNm - 1] = 'P';
						readMapFile(strMapFile, mapKeys);
						if (!FileExist(strMapFile)) {
							saveMapFile(strMapFile, mapKeys);
						}
						bExit = true;
					} else {
						unassignTapFile();
						setAutoLoadOff();
						setModel(spectrum_model);
						CPU_Reset(CPU_Handle);
						int nSnpRet = LoadSnapshot(strFileName);
						switch (nSnpRet) {
						case Z80SNAP_BROKEN:
							DrawClearZx();
							DrawTextZx("File is broken", 48, (240 - FONTH) / 2 - FONTH, COL_RED_ZX);
							DispUpdateZx();
							WaitMs(2000);
							bRefresh = true;
							break;
						case Z80SNAP_BADHW:
							DrawClearZx();
							DrawTextZx("Bad HW specification in file", 48, (240 - FONTH) / 2 - FONTH, COL_RED_ZX);
							DispUpdateZx();
							WaitMs(2000);
							bRefresh = true;
							break;
						case Z80SNAP_128:
							DrawClearZx();
							DrawTextZx("Unsupported ZX128 file", 48, (240 - FONTH) / 2 - FONTH, COL_RED_ZX);
							DispUpdateZx();
							WaitMs(2000);
							bRefresh = true;
							break;
						default:
							bExit = true;
							break;
						}
					}
				}

			} else {
				getNameAtPos(strFileName, nMenuPos);
				if ((strFileName[0] == '.') && (strFileName[1] == '.')) {
					//o uroven vys
					char *pcharSlash = strrchr(currPath, '/');
					char *strBeforeDir = &pcharSlash[1];
					//kontrola na prvni lomitko - nesmi byt smazano
					if (pcharSlash == currPath) {
						pcharSlash[1] = 0;
					} else {
						pcharSlash[0] = 0;
					}
					LoadFileList(currPath);
					int nDirPos = getDirPosition(&pcharSlash[1]);
					//zkusim najit adresar ze ktereho jsem vyskocil, kvuli pozici kurzoru
					if (nDirPos >= 0) {
						nMenuPos = nDirPos;
					} else {
						nMenuPos = 0;
					}
					nFirstShowed = 0;
					nMaxFiles = getFilesNum();
					if (nFirstShowed + nMenuPos >= FILEROWS) {
						nFirstShowed = nMenuPos - (FILEROWS / 2);
						if (nFirstShowed + FILEROWS > nMaxFiles) {
							nFirstShowed = nMaxFiles - FILEROWS;
						}
					}

					bRefresh = true;
				} else {
					//do podadresare
					int nLen = strlen(currPath);
					if (currPath[nLen - 1] != '/') {
						currPath[nLen] = '/';
						currPath[nLen + 1] = 0;
						nLen++;
					}
					memcpy(&currPath[nLen], strFileName, strlen(strFileName) + 1);
					LoadFileList(currPath);
					nMenuPos = 0;
					nFirstShowed = 0;
					nMaxFiles = getFilesNum();
					bRefresh = true;
				}
			}
			break;
		case KEY_X:
			KeyFlush();
			bRet = false;
			bExit = true;
			break;
		}
		if (bRefresh) {
			DrawClearZx();
			DrawText2Zx("Select file to load", (WIDTH - 19 * 16) / 2, 24,
			COL_YELLOW_ZX);
			DispFileList(nFirstShowed, nMenuPos);
			DispUpdateZx();
			bRefresh = false;
		}
	}
	return bRet;
}

//select mapping key menu
void MenuKeySelect(int nWhatKey) {
//alphabetic ordered keys
	int orderKi[] = { 15, 20, 21, 22, 23, 24, 19, 18, 17, 16, 30, 4, 0, 38, 35, 32, 40, 27, 5, 33, 34, 9, 12, 8, 7, 6, 42, 2, 3, 11, 10, 25, 28, 43, 31, 1, 29, 13, 41, 39, 26, 37,
			14, 36 };
	int nCntKeys = 44;
	bool bRet = true;
	bool bExit = false;
	bool bRefresh = true;
	char strBuffer[20];
	int nFirstShowedKey = 0;
	int nMenuPosKey = 0;
	int DispX, DispY;
	SelFont8x8();
	unsigned char ch;
	DispUpdateZx();
	while (bExit == false) {
		ch = USBMenuMap(KeyGet());
		switch (ch) {
		case KEY_X:
			KeyFlush();
			bExit = true;
			break;
		case KEY_UP:
			nMenuPosKey--;
			if (nMenuPosKey < 0)
				nMenuPosKey = 0;
			if (nMenuPosKey < nFirstShowedKey) {
				nFirstShowedKey--;
				if (nFirstShowedKey < 0) {
					nFirstShowedKey = 0;
				}
			}
			bRefresh = true;
			break;
		case KEY_DOWN:
			nMenuPosKey++;
			if (nMenuPosKey >= nCntKeys)
				nMenuPosKey = nCntKeys - 1;
			if (nMenuPosKey >= nFirstShowedKey + FILEROWS) {
				nFirstShowedKey++;
				if (nFirstShowedKey + FILEROWS > nCntKeys) {
					nFirstShowedKey--;
				}
			}
			bRefresh = true;
			break;
		case KEY_LEFT:
			nMenuPosKey -= FILEROWS;
			nFirstShowedKey -= FILEROWS;
			if (nFirstShowedKey < 0)
				nFirstShowedKey = 0;
			if (nMenuPosKey < 0)
				nMenuPosKey = 0;
			if (nMenuPosKey < nFirstShowedKey) {
				nFirstShowedKey--;
				if (nFirstShowedKey < 0) {
					nFirstShowedKey = 0;
				}
			}
			bRefresh = true;
			break;
		case KEY_RIGHT:
			nMenuPosKey += FILEROWS;
			nFirstShowedKey += FILEROWS;
			if (nFirstShowedKey + FILEROWS >= nCntKeys)
				nFirstShowedKey = nCntKeys - FILEROWS;
			if (nMenuPosKey >= nCntKeys)
				nMenuPosKey = nCntKeys - 1;
			if (nMenuPosKey >= nFirstShowedKey + FILEROWS) {
				nFirstShowedKey++;
				if (nFirstShowedKey + FILEROWS > nCntKeys) {
					nFirstShowedKey--;
				}
			}
			bRefresh = true;
			break;
		case KEY_A:
			KeyFlush();
			mapKeys[nWhatKey] = orderKi[nMenuPosKey];
			bExit = true;
			break;
		}
		if (bRefresh) {
			DrawClearZx();
			DrawText2Zx("Select key", (WIDTH - 10 * 16) / 2, 24, COL_YELLOW_ZX);
			DispY = 0;
			for (int i = nFirstShowedKey; (i < nFirstShowedKey + FILEROWS) && (i < nCntKeys); i++) {
				DispX = 32;
				int nColFrg = COL_WHITE_ZX;
				int nColBkg = COL_BLACK_ZX;
				if (i == nMenuPosKey) {
					nColFrg = COL_BLACK_ZX;
					nColBkg = COL_WHITE_ZX;
				}
				strBuffer[0] = ' ';
				//getKeyName(&strBuffer[1], orderKi[i]);
				getKeyName(&strBuffer[1], orderKi[i]);
				int nLen = strlen(strBuffer);
				strBuffer[nLen] = ' ';
				strBuffer[nLen + 1] = '\0';
				DrawTextBgZx(strBuffer, DispX, 48 + DispY * FONTH, nColFrg, nColBkg);
				DispY++;
			}
			DispUpdateZx();
			bRefresh = false;
		}
	}
}

//sound volume select menu
bool MenuSound() {
	bool bExit = false;
	bool bRefresh = true;
	bool bSave = false;
	char strBuffer[30];
	int nNewVolume = getVolume();
	while (bExit == false) {
		char ch = USBMenuMap(KeyGet());
		switch (ch) {
		case KEY_LEFT:
			nNewVolume--;
			if (nNewVolume < 0) {
				nNewVolume = 0;
			}
			bSave = true;
			bRefresh = true;
			break;
		case KEY_RIGHT:
			nNewVolume++;
			if (nNewVolume > 9) {
				nNewVolume = 9;
			}
			bSave = true;
			bRefresh = true;
			break;
		case KEY_X:
			KeyFlush();
			bExit = true;
			break;
		default:
			break;
		}
		if (bRefresh) {
			setVolume(nNewVolume);
			DrawClearZx();
			DrawText2Zx("Sound volume", (WIDTH - 12 * 16) / 2, 24,
			COL_YELLOW_ZX);
#define PROGRESS_X 32
#define PROGRESS_Y 82
#define PROGRESS_W 256
#define PROGRESS_H 16
			if (nNewVolume == 0) {
				sprintf(strBuffer, "Off");
			} else {
				sprintf(strBuffer, "%i  ", nNewVolume);
			}
			DrawText2Zx(strBuffer, (WIDTH - 16) / 2 - 8,
			PROGRESS_Y - FONTW - 10,
			COL_WHITE_ZX);
			DrawFrameZx(PROGRESS_X - 2, PROGRESS_Y - 2, PROGRESS_W + 4,
			PROGRESS_H + 4, COL_WHITE_ZX);
			DrawRectZx(PROGRESS_X, PROGRESS_Y, PROGRESS_W, PROGRESS_H,
			COL_GRAY_ZX);
			int col = COL_GREEN_ZX;
			if (nNewVolume > 7) {
				col = COL_RED_ZX;
			}
			if (nNewVolume < 3) {
				col = COL_YELLOW_ZX;
			}
			DrawRectZx(PROGRESS_X, PROGRESS_Y, nNewVolume * PROGRESS_W / 9,
			PROGRESS_H, col);
			DispUpdateZx();
			bRefresh = false;
		}
	}
	return bSave;
}

//mapping keyboard menu
void MenuMap() {
	bool bExit = false;
	bool bSave = false;
	SelFont8x8();
	char *keyMenu[] = { "KEY UP    = ", "KEY DOWN  = ", "KEY LEFT  = ", "KEY RIGHT = ", "KEY A     = ", "KEY B     = ", "KEY Y     = ", "\0" };
	int nMenuPos = 0;
	bool bRefresh = true;
	char strBuffer[30];
	while (bExit == false) {
		char ch = USBMenuMap(KeyGet());
		switch (ch) {
		case KEY_X:
			if ((bSave) && (strMapFile[0] != 0)) {
				saveMapFile(strMapFile, mapKeys);
			}
			bExit = true;
			break;
		case KEY_A:
			KeyFlush();
			MenuKeySelect(nMenuPos);
			bSave = true;
			bRefresh = true;
			break;
		case KEY_UP:
			nMenuPos--;
			if (nMenuPos < 0)
				nMenuPos = 6;
			bRefresh = true;
			break;
		case KEY_DOWN:
			nMenuPos++;
			if (nMenuPos > 6)
				nMenuPos = 0;
			bRefresh = true;
			break;
		default:
			break;
		}
		if (bRefresh) {
			DrawClearZx();
			DrawText2Zx("Keyboard mapping", (WIDTH - 16 * 16) / 2, 24,
			COL_YELLOW_ZX);
			int i = 0;
			int nPosY = 48;
			while (keyMenu[i][0] != 0) {
				int nColFrg = COL_WHITE_ZX;
				int nColBkg = COL_BLACK_ZX;
				if (i == nMenuPos) {
					nColFrg = COL_BLACK_ZX;
					nColBkg = COL_WHITE_ZX;
				}
				memcpy(strBuffer, &keyMenu[i][0], 12);
				getKeyName(&strBuffer[12], mapKeys[i]);
				DrawTextBgZx(strBuffer, 32, nPosY, nColFrg, nColBkg);
				nPosY += FONTH;
				i++;
			}
			DispUpdateZx();
			bRefresh = false;
		}
	}
}

bool bUpPressed = false;
bool bDownPressed = false;
bool bLeftPressed = false;
bool bRighPressed = false;
bool bAPressed = false;
bool bBPressed = false;
bool bYPressed = false;

void OSD_Input(void) {
//response keys according mapping
	if (KeyPressed(KEY_UP)) {
		rowKey[getRow(mapKeys[KBD_UP])] &= getPressMask(mapKeys[KBD_UP]);
		if (getRowAlt(mapKeys[KBD_UP]) != 255) {
			rowKey[getRowAlt(mapKeys[KBD_UP])] &= getPressMaskAlt(mapKeys[KBD_UP]);
		}
		bUpPressed = true;
	} else {
		if (bUpPressed) {
			rowKey[getRow(mapKeys[KBD_UP])] |= getReleaseMask(mapKeys[KBD_UP]);
			if (getRowAlt(mapKeys[KBD_UP]) != 255) {
				rowKey[getRowAlt(mapKeys[KBD_UP])] |= getReleaseMaskAlt(mapKeys[KBD_UP]);
			}
			bUpPressed = false;
		}
	}
	if (KeyPressed(KEY_DOWN)) {
		rowKey[getRow(mapKeys[KBD_DOWN])] &= getPressMask(mapKeys[KBD_DOWN]);
		if (getRowAlt(mapKeys[KBD_DOWN]) != 255) {
			rowKey[getRowAlt(mapKeys[KBD_DOWN])] &= getPressMaskAlt(mapKeys[KBD_DOWN]);
		}
		bDownPressed = true;
	} else {
		if (bDownPressed) {
			rowKey[getRow(mapKeys[KBD_DOWN])] |= getReleaseMask(mapKeys[KBD_DOWN]);
			if (getRowAlt(mapKeys[KBD_DOWN]) != 255) {
				rowKey[getRowAlt(mapKeys[KBD_DOWN])] |= getReleaseMaskAlt(mapKeys[KBD_DOWN]);
			}
			bDownPressed = false;
		}
	}
	if (KeyPressed(KEY_LEFT)) {
		rowKey[getRow(mapKeys[KBD_LEFT])] &= getPressMask(mapKeys[KBD_LEFT]);
		if (getRowAlt(mapKeys[KBD_LEFT]) != 255) {
			rowKey[getRowAlt(mapKeys[KBD_LEFT])] &= getPressMaskAlt(mapKeys[KBD_LEFT]);
		}
		bLeftPressed = true;
	} else {
		if (bLeftPressed) {
			rowKey[getRow(mapKeys[KBD_LEFT])] |= getReleaseMask(mapKeys[KBD_LEFT]);
			if (getRowAlt(mapKeys[KBD_LEFT]) != 255) {
				rowKey[getRowAlt(mapKeys[KBD_LEFT])] |= getReleaseMaskAlt(mapKeys[KBD_LEFT]);
			}
			bLeftPressed = false;
		}
	}
	if (KeyPressed(KEY_RIGHT)) {
		rowKey[getRow(mapKeys[KBD_RIGHT])] &= getPressMask(mapKeys[KBD_RIGHT]);
		if (getRowAlt(mapKeys[KBD_RIGHT]) != 255) {
			rowKey[getRowAlt(mapKeys[KBD_RIGHT])] &= getPressMaskAlt(mapKeys[KBD_RIGHT]);
		}
		bRighPressed = true;
	} else {
		if (bRighPressed) {
			rowKey[getRow(mapKeys[KBD_RIGHT])] |= getReleaseMask(mapKeys[KBD_RIGHT]);
			if (getRowAlt(mapKeys[KBD_RIGHT]) != 255) {
				rowKey[getRowAlt(mapKeys[KBD_RIGHT])] |= getReleaseMaskAlt(mapKeys[KBD_RIGHT]);
			}
			bRighPressed = false;
		}
	}
	if (KeyPressed(KEY_A)) {
		rowKey[getRow(mapKeys[KBD_A])] &= getPressMask(mapKeys[KBD_A]);
		bAPressed = true;
	} else {
		if (bAPressed) {
			rowKey[getRow(mapKeys[KBD_A])] |= getReleaseMask(mapKeys[KBD_A]);
			bAPressed = false;
		}
	}
	if (KeyPressed(KEY_B)) {
		rowKey[getRow(mapKeys[KBD_B])] &= getPressMask(mapKeys[KBD_B]);
		bBPressed = true;
	} else {
		if (bBPressed) {
			rowKey[getRow(mapKeys[KBD_B])] |= getReleaseMask(mapKeys[KBD_B]);
			bBPressed = false;
		}
	}
	if (KeyPressed(KEY_Y)) {
		rowKey[getRow(mapKeys[KBD_Y])] &= getPressMask(mapKeys[KBD_Y]);
		bYPressed = true;
	} else {
		if (bYPressed) {
			rowKey[getRow(mapKeys[KBD_Y])] |= getReleaseMask(mapKeys[KBD_Y]);
			bYPressed = false;
		}
	}
	if (UsbKeyIsMounted()) {
		ProcessUSBToZXKeyboard();
	}
//main menu
	char *mainMenu[] = { "Load Snapshot/Tape  ", "Map Keys            ","Set Spectrum Model  ", "Sound volume        ", "Reset emulator      ", "Quit emulator       ",
			"\0" };
	char ch = KeyGet();

	if ((UsbKeyIsMounted() && (ch == NOKEY))) {
		if (UsbKeyIsPressed(HID_KEY_ESCAPE) || UsbKeyIsPressed(HID_KEY_F3)) {
			ch = KEY_X;
		}
		if (ch != NOKEY)
			WaitMs(250);
	}

	bool bSave = false;
	switch (ch) {
	case KEY_X: {
		KeyFlush();
		//sound off
		sndOff();
		int nMenuPos = 0;
		bool bRefresh = true;
		DrawClearZx();
		SelFont8x8();
		bool bExit = false;
		while (bExit == false) {
			ch = USBMenuMap(KeyGet());
			//End of USB mappings in this menu
			switch (ch) {
			case KEY_X:
				KeyFlush();
				bExit = true;
				break;
			case KEY_Y:
				KeyFlush();
				setModel(spectrum_model);
				CPU_Reset(CPU_Handle);
				//load fast state
				if (FileExist("/ZXSLOT.SNA")) {
					LoadSna("/ZXSLOT.SNA",true,true);
					readMapFile("/ZXSLOT.MAP", mapKeys);
				}
				//MemDump2();
				SndInit();
				bExit = true;
				break;
			case KEY_B:
				KeyFlush();
				//MemDump1();
				//save fast state
				SaveSna("/ZXSLOT.SNA",true,true);
				bExit = true;
				break;
			case KEY_UP:
				nMenuPos--;
				if (nMenuPos < 0)
					nMenuPos = 5;
				bRefresh = true;
				break;
			case KEY_DOWN:
				nMenuPos++;
				if (nMenuPos > 5)
					nMenuPos = 0;
				bRefresh = true;
				break;
			case KEY_A:
				KeyFlush();
				if (nMenuPos == 0) {
					if (MenuSNA()) {
						bSave = true;
						SndInit();
						ay_reset(&ay0);
						bExit = true;
					} else {
						DrawClearZx();
						bRefresh = true;
					}
				}
				if (nMenuPos == 1) {
					MenuMap();
					DrawClearZx();
					bRefresh = true;
				}
				if (nMenuPos == 2) {
					if (MenuModel()) {
						SndInit();
						ay_reset(&ay0);
						defaultKeyMap();
						unassignTapFile();
						setAutoLoadOff();
						CPU_Reset(CPU_Handle);
					}
					DrawClearZx();
					bRefresh = true;
				}
				if (nMenuPos == 3) {
					bSave = MenuSound();
					DrawClearZx();
					bRefresh = true;
				}
				if (nMenuPos == 4) {
					strMapFile[0] = 0;
					setModel(spectrum_model);
					SndInit();
					ay_reset(&ay0);
					defaultKeyMap();
					unassignTapFile();
					setAutoLoadOff();
					CPU_Reset(CPU_Handle);
					bExit = true;
				}
				if (nMenuPos == 5) {
					if (bSave) {
						saveIniFile("/ZXSpec.ini");
					}
					d_fast_fclose();
					DiskFlush();
					WaitMs(1000);
					DiskUnmount();
					ResetToBootLoader();
				}
				break;
			}
			if (bRefresh) {
				DrawText2Zx("Emulator menu", (WIDTH - 13 * 16) / 2, 24,
				COL_YELLOW_ZX);
				int i = 0;
				int nPosY = 48;
				while (mainMenu[i][0] != 0) {
					int nColFrg = COL_WHITE_ZX;
					int nColBkg = COL_BLACK_ZX;
					if (i == nMenuPos) {
						nColFrg = COL_BLACK_ZX;
						nColBkg = COL_WHITE_ZX;
					}
					DrawTextBgZx(mainMenu[i], 32, nPosY, nColFrg, nColBkg);
					nPosY += FONTH;
					i++;
				}
				DrawTextBgZx("Y-load", 320 - 6 * 8, 0, RGBTO8(0x77, 0xDD, 0x77),
				COL_BLACK_ZX); //green
				DrawTextBgZx("B-save", 320 - 6 * 8, 240 - 8, RGBTO8(0xFF, 0x69, 0x61), COL_BLACK_ZX); //red
				DispUpdateZx();
				bRefresh = false;
			}

		}
		if (bSave) {
			saveIniFile("/ZXSpec.ini");
		}
		sndOn();
		//invalidate attributes. focrce to redraw
		for (int i = 6144; i < 6144 + 768; i++) {
			cached[i] = membank[4][i] ^ 0xFF;
		}
		//draw correct border
		DrawRectZx(0, 0, 320, 24, palette[border]);
		DrawRectZx(0, 216, 320, 24, palette[border]);
		DrawRectZx(0, 24, 32, 192, palette[border]);
		DrawRectZx(288, 24, 32, 192, palette[border]);

	}
		break;
		// unknown key
	default:
		KeyFlush();
		break;
	}

}
