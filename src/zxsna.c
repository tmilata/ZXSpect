#include "zxsna.h"
#include "cpuintf.h"
#include "zxini.h"
#include "zxtap.h"

char strTmpBuff[2048];

unsigned char fgetc(sFile *fil) {
	unsigned char chRet;
	FileRead(fil, &chRet, 1);
	return chRet;
}

void fputc(unsigned char value, sFile *fil) {
	FileWrite(fil, (const void*) &value, 1);
}

//pripoji informaci o TAP souboru (pouziva se u rychleho SAVE state)
void AppendTAPInfo(sFile *snap) {
	fputc(nPatchedRom, snap);
	if (nPatchedRom > 0) {
		unsigned long nPos = getTapFilePos();
		FileWrite(snap, (void*) &nPos, 4);
		strncpy(strTmpBuff, currPath, 2048);
		if (strTmpBuff[strlen(strTmpBuff) - 1] != '/') {
			strcpy(&strTmpBuff[strlen(strTmpBuff)], "/");
		}
		getTapFileName(&strTmpBuff[strlen(strTmpBuff)]);
		FileWrite(snap, (const void*) strTmpBuff, strlen(strTmpBuff));
	}
	FileClose(snap);

}

//nacte informaci o TAP souboru (pouziva se u rychleho LOAD state)
void getTAPInfo(sFile *snap) {
	nPatchedRom = fgetc(snap);
	if (nPatchedRom > 0) {
		unsigned long nPos = 0;
		FileRead(snap, (void*) &nPos, 4);
		long nReaded = FileRead(snap, (void*) strTmpBuff, 2047);
		strTmpBuff[2047] = '\0';
		assignTapFile(strTmpBuff);
		setTapFilePos(nPos);
		FileClose(snap);
	}
}


int LoadSna(char *strSnaFile) {
	int nRet = Z80SNAP_BROKEN;
	sFile snap;
	int k = 0, iff = 0;
	if (DiskValidate()) {
		if (FileExist(strSnaFile)) {
			if (FileOpen(&snap, strSnaFile)) {

				CPU_PutReg8(CPU_Handle, I, fgetc(&snap));
				CPU_PutReg8Alt(CPU_Handle, L, fgetc(&snap));
				CPU_PutReg8Alt(CPU_Handle, H, fgetc(&snap));
				CPU_PutReg8Alt(CPU_Handle, E, fgetc(&snap));
				CPU_PutReg8Alt(CPU_Handle, D, fgetc(&snap));
				CPU_PutReg8Alt(CPU_Handle, C, fgetc(&snap));
				CPU_PutReg8Alt(CPU_Handle, B, fgetc(&snap));
				CPU_PutReg8Alt(CPU_Handle, F, fgetc(&snap));
				CPU_PutReg8Alt(CPU_Handle, A, fgetc(&snap));

				CPU_PutReg8(CPU_Handle, L, fgetc(&snap));
				CPU_PutReg8(CPU_Handle, H, fgetc(&snap));
				CPU_PutReg8(CPU_Handle, E, fgetc(&snap));
				CPU_PutReg8(CPU_Handle, D, fgetc(&snap));
				CPU_PutReg8(CPU_Handle, C, fgetc(&snap));
				CPU_PutReg8(CPU_Handle, B, fgetc(&snap));
				CPU_PutReg8(CPU_Handle, IYL, fgetc(&snap));
				CPU_PutReg8(CPU_Handle, IYH, fgetc(&snap));
				CPU_PutReg8(CPU_Handle, IXL, fgetc(&snap));
				CPU_PutReg8(CPU_Handle, IXH, fgetc(&snap));
				if (fgetc(&snap) & 4) {
					CPU_SetIff(CPU_Handle, iff2, 1);
					iff = 1;
				} else {
					CPU_SetIff(CPU_Handle, iff2, 0);
					iff = 0;
				}
				CPU_PutReg8(CPU_Handle, R, fgetc(&snap));
				CPU_PutReg8(CPU_Handle, F, fgetc(&snap));
				CPU_PutReg8(CPU_Handle, A, fgetc(&snap));
				CPU_PutReg8(CPU_Handle, SPL, fgetc(&snap));
				CPU_PutReg8(CPU_Handle, SPH, fgetc(&snap));
				CPU_SetIntMode(CPU_Handle, fgetc(&snap));
				output(254, fgetc(&snap) & 7);
				FileRead(&snap, (void*) zxmem, 49152);
				unsigned short usPC = 0;
				if (CPU_GetReg16(CPU_Handle, SP) >= 0x4000) {
					usPC = zxmem[CPU_GetReg16(CPU_Handle, SP) - 0x4000]
							+ 256
									* zxmem[CPU_GetReg16(CPU_Handle, SP)
											- 0x3FFF];
					CPU_SetPC(CPU_Handle, usPC);
					if (FileRead(&snap, (void*) &k, 2) > 0) {
						zxmem[CPU_GetReg16(CPU_Handle, SP) - 0x3FFF] = k >> 8;
						zxmem[CPU_GetReg16(CPU_Handle, SP) - 0x4000] = k & 0xFF;
					}
				} // SNA fix - preserve stack
				usPC = CPU_GetReg16(CPU_Handle, SP) + 2;
				CPU_PutReg8(CPU_Handle, SPL, usPC & 0xFF);
				CPU_PutReg8(CPU_Handle, SPH, usPC >> 8);
				CPU_SetIff(CPU_Handle, iff1, iff);
				if (strcmp(strSnaFile, "/ZXSLOT.SNA") == 0) {
					getTAPInfo(&snap);
				}

				FileClose(&snap);

				//		D_fprintf("AF: %X BC: %X DE: %X HL: %X IX: %X IY: %X SP: %X PC: %X IR: %X\r\n",CPU_GetReg16(CPU_Handle,AF),CPU_GetReg16(CPU_Handle,BC),CPU_GetReg16(CPU_Handle,DE),CPU_GetReg16(CPU_Handle,HL),CPU_GetReg16(CPU_Handle,IX),CPU_GetReg16(CPU_Handle,IY),CPU_GetReg16(CPU_Handle,SP),CPU_GetReg16(CPU_Handle,PC),256*CPU_GetReg8(CPU_Handle,I)+CPU_GetReg8(CPU_Handle,R));
				//		D_fprintf("AF': %X BC': %X DE': %X HL': %X\r\n",CPU_GetReg16Alt(CPU_Handle,AF),CPU_GetReg16Alt(CPU_Handle,BC),CPU_GetReg16Alt(CPU_Handle,DE),CPU_GetReg16Alt(CPU_Handle,HL));

				//invalidate attributes. force to redraw
				for (int i = 6144; i < 6144 + 768; i++) {
					cached[i] = zxmem[i] + 1;
				}
				nRet = Z80SNAP_OK;
			}
		}
	}
	return nRet;
}


void SaveSna(char *strFileName) {
	int k = 0, iff = 0;
	bool bOpened = false;
	sFile snap;
	if (DiskValidate()) {
		if (FileExist(strFileName)) {
			if (0 != (bOpened = FileOpen(&snap, strFileName))) {
				//truncate
				SetFileSize(&snap, 0);
			}
		} else {
			bOpened = FileCreate(&snap, strFileName);
		}
		if (bOpened) {
			if (CPU_GetReg16(CPU_Handle, SP) >= 0x4002) { // SNA fix - preserve stack
				k = ((unsigned short) zxmem[CPU_GetReg16(CPU_Handle, SP)
						- 0x4001] << 8)
						+ zxmem[CPU_GetReg16(CPU_Handle, SP) - 0x4002];
				zxmem[CPU_GetReg16(CPU_Handle, SP) - 0x4001] = CPU_GetReg16(
						CPU_Handle, PC) / 256;
				zxmem[CPU_GetReg16(CPU_Handle, SP) - 0x4002] = CPU_GetReg16(
						CPU_Handle, PC) % 256;
			}
			unsigned short usTmp = CPU_GetReg16(CPU_Handle, SP) - 2;
			CPU_PutReg8(CPU_Handle, SPL, usTmp & 0xFF);
			CPU_PutReg8(CPU_Handle, SPH, usTmp >> 8);
			fputc(CPU_GetReg8(CPU_Handle, I), &snap);
			usTmp = CPU_GetReg16Alt(CPU_Handle, HL);
			fputc(usTmp & 0xFF, &snap);
			fputc(usTmp >> 8, &snap);
			usTmp = CPU_GetReg16Alt(CPU_Handle, DE);
			fputc(usTmp & 0xFF, &snap);
			fputc(usTmp >> 8, &snap);
			usTmp = CPU_GetReg16Alt(CPU_Handle, BC);
			fputc(usTmp & 0xFF, &snap);
			fputc(usTmp >> 8, &snap);
			usTmp = CPU_GetReg16Alt(CPU_Handle, AF);
			fputc(usTmp & 0xFF, &snap);
			fputc(usTmp >> 8, &snap);

			fputc(CPU_GetReg8(CPU_Handle, L), &snap);
			fputc(CPU_GetReg8(CPU_Handle, H), &snap);
			fputc(CPU_GetReg8(CPU_Handle, E), &snap);
			fputc(CPU_GetReg8(CPU_Handle, D), &snap);
			fputc(CPU_GetReg8(CPU_Handle, C), &snap);
			fputc(CPU_GetReg8(CPU_Handle, B), &snap);
			fputc(CPU_GetReg8(CPU_Handle, IYL), &snap);
			fputc(CPU_GetReg8(CPU_Handle, IYH), &snap);
			fputc(CPU_GetReg8(CPU_Handle, IXL), &snap);
			fputc(CPU_GetReg8(CPU_Handle, IXH), &snap);
			if (CPU_GetIff(CPU_Handle, iff1) != 0) {
				fputc(0xFF, &snap);
			} else {
				fputc(0, &snap);
			}
			fputc(CPU_GetReg8(CPU_Handle, R), &snap);
			fputc(CPU_GetReg8(CPU_Handle, F), &snap);
			fputc(CPU_GetReg8(CPU_Handle, A), &snap);
			fputc(CPU_GetReg8(CPU_Handle, SPL), &snap);
			fputc(CPU_GetReg8(CPU_Handle, SPH), &snap);
			fputc(CPU_GetIntMode(CPU_Handle), &snap);
			fputc(border & 7, &snap);
			int i = FileWrite(&snap, (const void*) zxmem, 49152);
			FileWrite(&snap, (const void*) &k, 2);
			if (CPU_GetReg16(CPU_Handle, SP) >= 0x4000) {
				CPU_SetPC(CPU_Handle,
						256 * zxmem[CPU_GetReg16(CPU_Handle, SP) - 0x3FFF]
								+ zxmem[CPU_GetReg16(CPU_Handle, SP) - 0x4000]);
				zxmem[CPU_GetReg16(CPU_Handle, SP) - 0x3FFF] = k >> 8;
				zxmem[CPU_GetReg16(CPU_Handle, SP) - 0x4000] = k & 0xFF;
			}
			usTmp = CPU_GetReg16(CPU_Handle, SP) + 2;
			CPU_PutReg8(CPU_Handle, SPL, usTmp & 0xFF);
			CPU_PutReg8(CPU_Handle, SPH, usTmp >> 8);

			AppendTAPInfo(&snap);

			FileFlush(&snap);
			FileClose(&snap);

			strcpy(strMapFile, strFileName);

			int nLenNm = strlen(strMapFile);
			strMapFile[nLenNm - 3] = 'M';
			strMapFile[nLenNm - 2] = 'A';
			strMapFile[nLenNm - 1] = 'P';
			saveMapFile(strMapFile, mapKeys);
		}
	}
}


