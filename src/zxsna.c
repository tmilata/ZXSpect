#include "zxsna.h"
#include "cpuintf.h"
#include "zxini.h"
#include "zxtap.h"
#include "ay8912.h"

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
		//nastavim currPath na cestu z tapinfo
		char *pcharSlash = strrchr(strTmpBuff, '/');
		//kontrola na prvni lomitko - nesmi byt smazano
		if (pcharSlash == strTmpBuff) {
			pcharSlash[1] = 0;
		} else {
			pcharSlash[0] = 0;
		}
		strcpy(currPath, strTmpBuff);
		FileClose(snap);
	}
}

int LoadSna(char *strSnaFile) {
	int nRet = Z80SNAP_BROKEN;
	int nType = 0;
	sFile snap;
	int k = 0, iff = 0;
	if (DiskAutoMount()) {
		if (FileExist(strSnaFile)) {
			if (FileOpen(&snap, strSnaFile)) {
				int nSize = FileSize(&snap);
				if ((nSize >= 49179) && (nSize <= 49179 + 1024)) {
					nType = 0;
				} else if ((nSize >= 131103) && (nSize <= 131103 + 1024)) {
					nType = 1;
				} else if ((nSize >= 147487) && (nSize <= 147487 + 1024)) {
					nType = 1;
				} else {
					return nRet;
				}

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

				if (nType == 0) {
					//48k
					setModel(ZX48);
					for (int i = 16384; i < 65536; i++) {
						writebyte(i, fgetc(&snap));
					}
					unsigned short usPC = 0;
					if (CPU_GetReg16(CPU_Handle, SP) >= 0x4000) {
						usPC = readbyte(CPU_GetReg16(CPU_Handle, SP))
								+ 256
										* readbyte(
												CPU_GetReg16(CPU_Handle, SP)
														+ 1);
						CPU_SetPC(CPU_Handle, usPC);
						/*
						 if (FileRead(&snap, (void*) &k, 2) > 0) {
						 writebyte(CPU_GetReg16(CPU_Handle, SP) + 1, k >> 8);
						 writebyte(CPU_GetReg16(CPU_Handle, SP), k & 0xFF);
						 }
						 */
					} // SNA fix - preserve stack
					usPC = CPU_GetReg16(CPU_Handle, SP) + 2;
					CPU_PutReg8(CPU_Handle, SPL, usPC & 0xFF);
					CPU_PutReg8(CPU_Handle, SPH, usPC >> 8);
					CPU_SetIff(CPU_Handle, iff1, iff);
				} else {
					//128K
					setModel(ZX128);
					ay_reset(&ay0);
					ay0_enable=true;
					FileSeek(&snap, 49179);
					CPU_SetPC(CPU_Handle, fgetc(&snap) | (fgetc(&snap) << 8));
					int locPage = fgetc(&snap);
					page_set(locPage);
					locPage &= 7;
					fgetc(&snap);
					//TR-DOS byte

					FileSeek(&snap, 27);
					FileRead(&snap, (void*) (zxmem + 5 * 16384), 16384);
					FileRead(&snap, (void*) (zxmem + 2 * 16384), 16384);
					FileRead(&snap, (void*) (zxmem + locPage * 16384), 16384);
					FileSeek(&snap, 49183);
					for (int i = 0; i < 8; i++) {
						if ((i != 2) && (i != 5) && (i != (page & 0x07))) {
							FileRead(&snap, (void*) (zxmem + i * 16384), 16384);
						}
					}

				}
				if (strcmp(strSnaFile, "/ZXSLOT.SNA") == 0) {
					getTAPInfo(&snap);
				}

				FileClose(&snap);

				//invalidate attributes. force to redraw
				for (int i = 6144; i < 6144 + 768; i++) {
					cached[i] = membank[4][i] ^ 0xFF;
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
	if (DiskAutoMount()) {
		if (FileExist(strFileName)) {
			if (0 != (bOpened = FileOpen(&snap, strFileName))) {
				//truncate
				SetFileSize(&snap, 0);
			}
		} else {
			bOpened = FileCreate(&snap, strFileName);
		}
		if (bOpened) {
			unsigned short usTmp;
			if (spectrum_model == ZX48) {
				if (CPU_GetReg16(CPU_Handle, SP) >= 0x4002) { // SNA fix - preserve stack
					k = ((unsigned short) readbyte(
							CPU_GetReg16(CPU_Handle, SP) - 1) << 8)
							+ readbyte(CPU_GetReg16(CPU_Handle, SP) - 2);
					writebyte(CPU_GetReg16(CPU_Handle, SP) - 1,
							CPU_GetReg16(CPU_Handle, PC) / 256);
					writebyte(CPU_GetReg16(CPU_Handle, SP) - 2,
							CPU_GetReg16(CPU_Handle, PC) % 256);
				}
				usTmp = CPU_GetReg16(CPU_Handle, SP) - 2;
				CPU_PutReg8(CPU_Handle, SPL, usTmp & 0xFF);
				CPU_PutReg8(CPU_Handle, SPH, usTmp >> 8);
			}
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

			if (spectrum_model == ZX48) {
				//ZX48
				//int i = FileWrite(&snap, (const void*) zxmem, 49152);
				for (int i = 16384; i < 65536; i++) {
					u8 v = readbyte(i);
					FileWrite(&snap, (const void*) &v, 1);
				}
				//FileWrite(&snap, (const void*) &k, 2);
				if (CPU_GetReg16(CPU_Handle, SP) >= 0x4000) {
					CPU_SetPC(CPU_Handle,
							256 * readbyte(CPU_GetReg16(CPU_Handle, SP) + 1)
									+ readbyte(CPU_GetReg16(CPU_Handle, SP)));
					writebyte(CPU_GetReg16(CPU_Handle, SP) + 1, k >> 8);
					writebyte(CPU_GetReg16(CPU_Handle, SP), k & 0xFF);
				}

				usTmp = CPU_GetReg16(CPU_Handle, SP) + 2;
				CPU_PutReg8(CPU_Handle, SPL, usTmp & 0xFF);
				CPU_PutReg8(CPU_Handle, SPH, usTmp >> 8);
			} else {
				//ZX128
				unsigned char locPage = page & 7;
				FileWrite(&snap, (const void*) (zxmem + 5 * 16384), 16384);
				FileWrite(&snap, (const void*) (zxmem + 2 * 16384), 16384);
				FileWrite(&snap, (const void*) (zxmem + locPage * 16384),
						16384);
				unsigned short usPC = CPU_GetReg16(CPU_Handle, PC);
				fputc(usPC % 256, &snap);
				fputc(usPC / 256, &snap);
				fputc(page, &snap);
				fputc(0, &snap);
				for (int i = 0; i < 8; i++) {
					if ((i != 2) && (i != 5) && (i != locPage)) {
						FileWrite(&snap, (const void*) (zxmem + i * 16384),
								16384);
					}
				}

			}

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
