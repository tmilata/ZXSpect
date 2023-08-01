#include "osdep.h"
#include <stdio.h>
#include "zxem.h"
#include "debug.h"
#include "zx48rom.h"
#include "palette.h"
#include "zxtap.h"
#include "fileop.h"
#include "zxkbd.h"
#include "zxini.h"
#include "zxsna.h"
#include "zxsnd.h"

void defaultKeyMap() {
//default keys mapping - QAOPM
	mapKeys[KBD_UP] = getKeyPosition("Q");
	mapKeys[KBD_DOWN] = getKeyPosition("A");
	mapKeys[KBD_LEFT] = getKeyPosition("O");
	mapKeys[KBD_RIGHT] = getKeyPosition("P");
	mapKeys[KBD_A] = getKeyPosition("M");
	mapKeys[KBD_Y] = getKeyPosition("Enter");
	mapKeys[KBD_B] = getKeyPosition("Break Space");
}

int main(int argc, char *argv[]) {
	//overclocking picopad
	ClockPllSysFreq(270000);
	currPath[0] = '/';
	currPath[1] = 0;
	strMapFile[0] = 0;
	fillKeyInfo();
	for (int i = 0; i < 8; i++)
		rowKey[i] = 191;
	defaultKeyMap();
	beeper = 0;
	ZX_main(argc, argv);
	return 0;
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
	DispUpdate();
	while (bExit == false) {
		ch = KeyGet();
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
					if ((strFileName[nLenTp - 3] == 'T')
					    && (strFileName[nLenTp - 2] == 'A')
					    && (strFileName[nLenTp - 1] = 'P')) {
						assignTapFile(strFileName);
						setAutoLoadOn();
						strMapFile[0] = 0;
						SndInit();
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
						int nSnpRet = LoadSnapshot(strFileName);
						switch (nSnpRet) {
						case Z80SNAP_BROKEN:
							DrawClear(COL_BLACK);
							DrawText("File is broken", 48,
							         (240 - FONTH) / 2 - FONTH, COL_RED);
							DispUpdate();
							WaitMs(2000);
							bRefresh = true;
							break;
						case Z80SNAP_BADHW:
							DrawClear(COL_BLACK);
							DrawText("Bad HW specification in file", 48,
							         (240 - FONTH) / 2 - FONTH, COL_RED);
							DispUpdate();
							WaitMs(2000);
							bRefresh = true;
							break;
						case Z80SNAP_128:
							DrawClear(COL_BLACK);
							DrawText("Unsupported ZX128 file", 48,
							         (240 - FONTH) / 2 - FONTH, COL_RED);
							DispUpdate();
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
					memcpy(&currPath[nLen], strFileName,
					       strlen(strFileName) + 1);
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
			DrawClear(COL_BLACK);
			DrawText2("Select file to load", (WIDTH - 19 * 16) / 2, 24,
			          COL_YELLOW);
			DispFileList(nFirstShowed, nMenuPos);
			DispUpdate();
			bRefresh = false;
		}
	}
	return bRet;
}

//select mapping key menu
void MenuKeySelect(int nWhatKey) {
	//alphabetic ordered keys
	int orderKi[] = { 15, 20, 21, 22, 23, 24, 19, 18, 17, 16, 30, 4, 0, 38, 35,
	                  32, 27, 5, 33, 34, 9, 12, 8, 7, 6, 2, 3, 11, 10, 25, 28, 31, 1, 29,
	                  13, 39, 26, 37, 14, 36
	                };
	bool bRet = true;
	bool bExit = false;
	bool bRefresh = true;
	char strBuffer[20];
	int nFirstShowedKey = 0;
	int nMenuPosKey = 0;
	int DispX, DispY;
	SelFont8x8();
	unsigned char ch;
	DispUpdate();
	while (bExit == false) {
		ch = KeyGet();
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
			if (nMenuPosKey >= 40)
				nMenuPosKey = 40 - 1;
			if (nMenuPosKey >= nFirstShowedKey + FILEROWS) {
				nFirstShowedKey++;
				if (nFirstShowedKey + FILEROWS > 40) {
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
			if (nFirstShowedKey + FILEROWS >= 40)
				nFirstShowedKey = 40 - FILEROWS;
			if (nMenuPosKey >= 40)
				nMenuPosKey = 40 - 1;
			if (nMenuPosKey >= nFirstShowedKey + FILEROWS) {
				nFirstShowedKey++;
				if (nFirstShowedKey + FILEROWS > 40) {
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
			DrawClear(COL_BLACK);
			DrawText2("Select key", (WIDTH - 10 * 16) / 2, 24, COL_YELLOW);
			DispY = 0;
			for (int i = nFirstShowedKey;
			     (i < nFirstShowedKey + FILEROWS) && (i < 40); i++) {
				DispX = 32;
				int nColFrg = COL_WHITE;
				int nColBkg = COL_BLACK;
				if (i == nMenuPosKey) {
					nColFrg = COL_BLACK;
					nColBkg = COL_WHITE;
				}
				strBuffer[0] = ' ';
				getKeyName(&strBuffer[1], orderKi[i]);
				int nLen = strlen(strBuffer);
				strBuffer[nLen] = ' ';
				strBuffer[nLen + 1] = '\0';
				DrawTextBg(strBuffer, DispX, 48 + DispY * FONTH, nColFrg,
				           nColBkg);
				DispY++;
			}
			DispUpdate();
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
		char ch = KeyGet();
		switch (ch) {
		case KEY_LEFT:
			KeyFlush();
			nNewVolume--;
			if (nNewVolume < 0) {
				nNewVolume = 0;
			}
			bSave = true;
			bRefresh = true;
			break;
		case KEY_RIGHT:
			KeyFlush();
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
			KeyFlush();
			break;
		}
		if (bRefresh) {
			setVolume(nNewVolume);
			DrawClear(COL_BLACK);
			DrawText2("Sound volume", (WIDTH - 12 * 16) / 2, 24, COL_YELLOW);
#define PROGRESS_X 32
#define PROGRESS_Y 82
#define PROGRESS_W 256
#define PROGRESS_H 16
			if (nNewVolume == 0) {
				sprintf(strBuffer, "Off");
			} else {
				sprintf(strBuffer, "%i  ", nNewVolume);
			}
			DrawText2(strBuffer, (WIDTH - 16) / 2 - 8, PROGRESS_Y - FONTW - 10,
			          COL_WHITE);
			DrawFrame(PROGRESS_X - 2, PROGRESS_Y - 2, PROGRESS_W + 4,
			          PROGRESS_H + 4, COL_WHITE);
			DrawRect(PROGRESS_X, PROGRESS_Y, PROGRESS_W, PROGRESS_H, COL_GRAY);
			int col = COL_GREEN;
			if (nNewVolume > 7) {
				col = COL_RED;
			}
			if (nNewVolume < 3) {
				col = COL_YELLOW;
			}
			DrawRect(PROGRESS_X, PROGRESS_Y, nNewVolume * PROGRESS_W / 9,
			         PROGRESS_H, col);
			DispUpdate();
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
	char *keyMenu[] = {
		"KEY UP    = ", "KEY DOWN  = ", "KEY LEFT  = ", "KEY RIGHT = ",
		"KEY A     = ", "KEY B     = ", "KEY Y     = ", "\0"
	};
	int nMenuPos = 0;
	bool bRefresh = true;
	char strBuffer[30];
	while (bExit == false) {
		char ch = KeyGet();
		switch (ch) {
		case KEY_X:
			KeyFlush();
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
			KeyFlush();
			break;
		}
		if (bRefresh) {
			DrawClear(COL_BLACK);
			DrawText2("Keyboard mapping", (WIDTH - 16 * 16) / 2, 24,
			          COL_YELLOW);
			int i = 0;
			int nPosY = 48;
			while (keyMenu[i][0] != 0) {
				int nColFrg = COL_WHITE;
				int nColBkg = COL_BLACK;
				if (i == nMenuPos) {
					nColFrg = COL_BLACK;
					nColBkg = COL_WHITE;
				}
				memcpy(strBuffer, &keyMenu[i][0], 12);
				getKeyName(&strBuffer[12], mapKeys[i]);
				DrawTextBg(strBuffer, 32, nPosY, nColFrg, nColBkg);
				nPosY += FONTH;
				i++;
			}
			DispUpdate();
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
		bUpPressed = true;
	} else {
		if (bUpPressed) {
			rowKey[getRow(mapKeys[KBD_UP])] |= getReleaseMask(mapKeys[KBD_UP]);
			bUpPressed = false;
		}
	}
	if (KeyPressed(KEY_DOWN)) {
		rowKey[getRow(mapKeys[KBD_DOWN])] &= getPressMask(mapKeys[KBD_DOWN]);
		bDownPressed = true;
	} else {
		if (bDownPressed) {
			rowKey[getRow(mapKeys[KBD_DOWN])] |= getReleaseMask(
			                                       mapKeys[KBD_DOWN]);
			bDownPressed = false;
		}
	}
	if (KeyPressed(KEY_LEFT)) {
		rowKey[getRow(mapKeys[KBD_LEFT])] &= getPressMask(mapKeys[KBD_LEFT]);
		bLeftPressed = true;
	} else {
		if (bLeftPressed) {
			rowKey[getRow(mapKeys[KBD_LEFT])] |= getReleaseMask(
			                                       mapKeys[KBD_LEFT]);
			bLeftPressed = false;
		}
	}
	if (KeyPressed(KEY_RIGHT)) {
		rowKey[getRow(mapKeys[KBD_RIGHT])] &= getPressMask(mapKeys[KBD_RIGHT]);
		bRighPressed = true;
	} else {
		if (bRighPressed) {
			rowKey[getRow(mapKeys[KBD_RIGHT])] |= getReleaseMask(
			                                        mapKeys[KBD_RIGHT]);
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

	//main menu
	char *mainMenu[] = { "Load Snapshot/Tape  ", "Map Keys            ",
	                     "Sound settings      ", "Reset emulator      ",
	                     "Quit emulator       ", "\0"
	                   };
	char ch = KeyGet();
	bool bSave = false;
	switch (ch) {
	case KEY_X: {
		KeyFlush();
		//sound off
		sndOff();
		int nMenuPos = 0;
		bool bRefresh = true;
		DrawClear(COL_BLACK);
		SelFont8x8();
		bool bExit = false;
		while (bExit == false) {
			ch = KeyGet();
			switch (ch) {
			case KEY_X:
				KeyFlush();
				bExit = true;
				break;
			case KEY_Y:
				KeyFlush();
				//load fast state
				if (FileExist("/ZXSLOT.SNA")) {
					LoadSna("/ZXSLOT.SNA");
					readMapFile("/ZXSLOT.MAP", mapKeys);
				}
				bExit = true;
				break;
			case KEY_B:
				KeyFlush();
				//save fast state
				SaveSna("/ZXSLOT.SNA");
				bExit = true;
				break;
			case KEY_UP:
				nMenuPos--;
				if (nMenuPos < 0)
					nMenuPos = 4;
				bRefresh = true;
				break;
			case KEY_DOWN:
				nMenuPos++;
				if (nMenuPos > 4)
					nMenuPos = 0;
				bRefresh = true;
				break;
			case KEY_A:
				KeyFlush();
				if (nMenuPos == 0) {
					if (MenuSNA()) {
						bSave = true;
						SndInit();
						bExit = true;
					} else {
						DrawClear(COL_BLACK);
						bRefresh = true;
					}
				}
				if (nMenuPos == 1) {
					MenuMap();
					DrawClear(COL_BLACK);
					bRefresh = true;
				}
				if (nMenuPos == 2) {
					bSave = MenuSound();
					DrawClear(COL_BLACK);
					bRefresh = true;
				}
				if (nMenuPos == 3) {
					strMapFile[0] = 0;
					SndInit();
					defaultKeyMap();
					unassignTapFile();
					setAutoLoadOff();
					CPU_Reset(CPU_Handle);
					bExit = true;
				}
				if (nMenuPos == 4) {
					if (bSave) {
						saveIniFile("/ZXSpec.ini");
					}
					DiskFlush();
					WaitMs(1000);
					DiskUnmount();
					ResetToBootLoader();
				}
				break;
			}
			if (bRefresh) {
				DrawText2("Emulator menu", (WIDTH - 13 * 16) / 2, 24,
				          COL_YELLOW);
				int i = 0;
				int nPosY = 48;
				while (mainMenu[i][0] != 0) {
					int nColFrg = COL_WHITE;
					int nColBkg = COL_BLACK;
					if (i == nMenuPos) {
						nColFrg = COL_BLACK;
						nColBkg = COL_WHITE;
					}
					DrawTextBg(mainMenu[i], 32, nPosY, nColFrg, nColBkg);
					nPosY += FONTH;
					i++;
				}
				DrawTextBg("Y-load", 320 - 6 * 8, 0, RGBTO16(0x77, 0xDD, 0x77),
				           COL_BLACK); //green
				DrawTextBg("B-save", 320 - 6 * 8, 240 - 8,
				           RGBTO16(0xFF, 0x69, 0x61), COL_BLACK); //red
				DispUpdate();
				bRefresh = false;
			}

		}
		if (bSave) {
			saveIniFile("/ZXSpec.ini");
		}
		sndOn();
		//invalidate attributes. focrce to redraw
		for (int i = 6144; i < 6144 + 768; i++) {
			cached[i] = zxmem[i] + 1;
		}
		//draw correct border
		DrawRect(0, 0, 320, 24, palette[border]);
		DrawRect(0, 216, 320, 24, palette[border]);
		DrawRect(0, 24, 32, 192, palette[border]);
		DrawRect(288, 24, 32, 192, palette[border]);

	}
	break;
	// unknown key
	default:
		KeyFlush();
		break;
	}

}
