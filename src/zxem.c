/* zxem.c
 *
 * ZX Spectrum emulator
 * Copyright (c) 2016 MikeDX
 *
 * Cross platform ZX Spectrum emulator
 *
 */

#define GLOBALS
#include "zxem.h"
#include "zxini.h"
#include "debug.h"
#include "zxz80.h"
#include "zxsna.h"
#include "zxtap.h"
#include "zxsnd.h"

unsigned long long int total;
unsigned long long int  lasttotal=0;
int stopped;


uint8_t zxmem_static[49152];
uint8_t cached_static[6912];
unsigned char mapKeys[8];
char currPath[1024];
char strMapFile[13];
int nMenuPos = 0;
int nFirstShowed = 0;
unsigned int border;
unsigned int beeper;
int nPatchedRom;

int bAutoLoad = 0;
bool bLoaded = true;
int nLoadKeys = 5;


uint8_t cachedread(uint16_t addr) {
	if (cached[addr] != zxmem[addr]) {
		return 1;
	} else {
		return 0;
	}
}

void cachedwrite(uint16_t addr, uint8_t value) {
	cached[addr] = value;
}


/* Handle Interrupt - called before each screen render */
void ZX_Int(void) {
	CPU_Interrupt(CPU_Handle);
}

u64 next_total = CYCLES_PER_STEP;
u64 real_ticks;

void MemDump() {

	sFile f1;
	char strTmp[1024];
	char strLogFile[] = "/ram.txt";
	if (DiskValidate()) {
		if (FileCreate(&f1, strLogFile)) {
			FileWrite(&f1, (const void*) zxmem, 49152);
			FileFlush(&f1);
			FileClose(&f1);
		}
	}
	DiskFlush();
}


void mainloop(void) {
	lasttotal=total;
	u64 ftm = Time64();
	if (!debug) {
		//no-debug code
		total += CPU_Emulate(CPU_Handle, FIRST_SHOWED_BYTE);
		for (int nLine = 0; nLine < 24; nLine++) {
			ZX_Draw(nLine);
			total += CPU_Emulate(CPU_Handle, ONE_LINE_TICKS);
		}
		total += CPU_Emulate(CPU_Handle, CYCLES_PER_STEP - (FIRST_SHOWED_BYTE + 24 * ONE_LINE_TICKS));
	} else {
		//only if debug is switched on
		d_fast_fprintf(
				"PC: %4.4X AF: %4.4X BC: %4.4X DE: %4.4X HL: %4.4X IX: %4.4X IY: %4.4X SP: %4.4X IR: %4.4X %f\n",
				CPU_GetReg16(CPU_Handle, PC), CPU_GetReg16(CPU_Handle, AF),
				CPU_GetReg16(CPU_Handle, BC), CPU_GetReg16(CPU_Handle, DE),
				CPU_GetReg16(CPU_Handle, HL), CPU_GetReg16(CPU_Handle, IX),
				CPU_GetReg16(CPU_Handle, IY), CPU_GetReg16(CPU_Handle, SP),
				256 * CPU_GetReg8(CPU_Handle, I) + CPU_GetReg8(CPU_Handle, R),
				total);
		total += CPU_Emulate(CPU_Handle, 1);
		for (int nLine = 0; nLine < 24; nLine++) {
			ZX_Draw(nLine);
		}
		if (KeyPressed(KEY_Y)) {
			d_fast_fclose();
			DiskFlush();
			WaitMs(1000);
			DiskUnmount();
			ResetToBootLoader();
		}

	}

	if (total >= next_total) {
		next_total += CYCLES_PER_STEP;
		DispUpdate();
		ZX_Input();
		ZX_Int();
		//ZX_Draw();
		//uprava rychlosti emulace
		u64 ltm = Time64();
		signed long long int nWait = 20000 - (ltm - ftm);

		if (nWait > 0) {
			WaitUs(nWait);
		}
		//sound frame synchronization
		sndFinishFrame(total);

		//autoload TAP file, ie. LOAD "" typing
		if (bAutoLoad > 0) {
			bAutoLoad--;
		}
		if ((bAutoLoad == 0) && (!bLoaded)) {
			bLoaded = doAutoLoadTape();
		}

	}

}




int LoadSnapshot(char *strSnaFile) {
	int nRet = Z80SNAP_BROKEN;
	int nLenSn = strlen(strSnaFile);
	if (nLenSn > 4) {
		if ((strSnaFile[nLenSn - 3] == 'S') && (strSnaFile[nLenSn - 2] == 'N')
				&& (strSnaFile[nLenSn - 1] = 'A')) {
			nRet = LoadSna(strSnaFile);
		} else {
			if ((strSnaFile[nLenSn - 3] == 'Z')
					&& (strSnaFile[nLenSn - 2] == '8')
					&& (strSnaFile[nLenSn - 1] = '0')) {
				nRet = LoadZ80(strSnaFile);
			}
		}

		strcpy(strMapFile, strSnaFile);

		int nLenNm = strlen(strMapFile);
		strMapFile[nLenNm - 3] = 'M';
		strMapFile[nLenNm - 2] = 'A';
		strMapFile[nLenNm - 1] = 'P';
		readMapFile(strMapFile, mapKeys);
		if (!FileExist(strMapFile)) {
			saveMapFile(strMapFile, mapKeys);
		}

	}
	return nRet;
}



void setAutoLoadOn() {
	nPatchedRom = 1;
	bAutoLoad = 100;
	nLoadKeys = 5;
	bLoaded = false;
}

void setAutoLoadOff() {
	nPatchedRom = 0;
	bAutoLoad = 0;
	nLoadKeys = 5;
	bLoaded = true;
}

bool doAutoLoadTape() {
	bool bRet = true;
	int LAST_K = 23560 - 16384;
	int FLAGS = 23611 - 16384;
	nLoadKeys--;
	if (nLoadKeys > 0) {
		switch (nLoadKeys) {
		case 4: {
			zxmem[LAST_K] = 0xEF; // LOAD keyword
			zxmem[FLAGS] = 0x20;  // signal that a key was pressed
			bRet = false;
		}
			break;
		case 3: {
			zxmem[LAST_K] = 0x22; // "
			zxmem[FLAGS] = 0x20;  // signal that a key was pressed
			bRet = false;
		}
			break;
		case 2: {
			zxmem[LAST_K] = 0x22; // "
			zxmem[FLAGS] = 0x20;  // signal that a key was pressed
			bRet = false;
		}
			break;
		case 1: {
			zxmem[LAST_K] = 0x0D; // ENTER key
			zxmem[FLAGS] = 0x20;  // signal that a key was pressed
			bRet = true;
		}
			break;
		}
	}
	return bRet;
}

int ZX_main(int argc, char *argv[]) {
	int i = 0;

	//clear display
	for (int y = 0; y < 240; y++) {
		for (int x = 0; x < 320; x++) {
			DrawPoint(x, y, palette[7]);
		}
	}
	//set border to white
	border = 7;

	zxmem = (uint8_t*) &zxmem_static;
	cached = (uint8_t*) &cached_static;
	// Create a z80 cpu instance
	CPU_Handle = CPU_Create();

	// Clear main memory
	memset(zxmem, 0, 49152);

	// Invalidate screen via cached memory
	memset(cached, 255, 6912);

	setVolume(5);
	readIniFile("/ZXSpec.ini");

	setAutoLoadOff();
	CPU_Reset(CPU_Handle);

	SndInit();


	total = 0;
	running = 1;
	debug = 0;

	while (running) {
		mainloop();
	}

	return 0;
}

