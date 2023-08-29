#include "debug.h"
#include "../include.h"
#include <stdbool.h>
#include "lib_drawtftzx.h"
#define LIMIT_BF 1024
#define FLUSH_CYCLE 200
#ifndef FONTW
#define FONTW		8		// width of system font
#endif
#ifndef FONTH
#define FONTH		16		// height of system font
#endif

debugstate dbgstate = { .DispX = 0, .DispY = 0, .nColor = 0, .nOpened = 0,
		.nNeedFlush = FLUSH_CYCLE };

void d_DispText(const char *text) {
	SelFont8x16();
	int nCol = COL_WHITE_ZX;
	if (dbgstate.nColor == 1) nCol = COL_YELLOW;
	DrawTextBgZx(text, dbgstate.DispX * FONTW, dbgstate.DispY * FONTH, 0, nCol);
	dbgstate.DispX += StrLen(text);
	DispUpdateZx();
}

//print debug message to display
void d_debug(char *text, ...) {
	va_list list;
	char strTmp[LIMIT_BF];
	va_start(list, text);
	vsnprintf(strTmp, LIMIT_BF - 1, text, list);
	va_end(list);
	d_DispText(strTmp);
	dbgstate.DispX = 0;
	dbgstate.DispY++;
	if (dbgstate.DispY * FONTH > 240) {
		dbgstate.DispY = 0;
		dbgstate.nColor++;
		if(dbgstate.nColor>1){
			dbgstate.nColor=0;
		}
	}
}

//fast fprintf to file, file stays opened, so d_fast_fclose must be called at finish
void d_fast_fprintf(char *text, ...) {
	sFile filcnv;
	memcpy(&filcnv, &dbgstate.fst1, sizeof(sFile));
	va_list list;
	bool bOpenedLoc = false;
	char strTmp[LIMIT_BF];
	char strLogFile[] = "/flog.txt";
	if (dbgstate.nOpened == 0) {
		if (DiskAutoMount()) {
			if (FileExist(strLogFile)) {
				if (true == (bOpenedLoc = FileOpen(&filcnv, strLogFile))) {
					FileSeek(&filcnv, FileSize(&filcnv));
				}
			} else {
				bOpenedLoc = FileCreate(&filcnv, strLogFile);
			}
			if (bOpenedLoc) {
				dbgstate.nOpened = 1;
				va_start(list, text);
				vsnprintf(strTmp, LIMIT_BF - 1, text, list);
				va_end(list);
				FileWrite(&filcnv, (const void*) strTmp, strlen(strTmp));
				dbgstate.nNeedFlush--;
				if (dbgstate.nNeedFlush == 0) {
					dbgstate.nNeedFlush = FLUSH_CYCLE;
					DiskFlush();
				}
				memcpy(&dbgstate.fst1, &filcnv, sizeof(sFile));
			}
		}
	} else {
		va_start(list, text);
		vsnprintf(strTmp, LIMIT_BF - 1, text, list);
		va_end(list);
		FileWrite(&filcnv, (const void*) strTmp, strlen(strTmp));
		dbgstate.nNeedFlush--;
		if (dbgstate.nNeedFlush == 0) {
			dbgstate.nNeedFlush = FLUSH_CYCLE;
			DiskFlush();
		}
		memcpy(&dbgstate.fst1, &filcnv, sizeof(sFile));
	}
}

void d_fast_fclose() {
	if (dbgstate.nOpened == 1) {
		sFile filcnv;
		memcpy(&filcnv, &dbgstate.fst1, sizeof(_sFile));
		FileFlush(&filcnv);
		FileClose(&filcnv);
		dbgstate.nOpened = 0;
	}
}

//slow fprintf to file, file is closed on every call
void d_fprintf(char *text, ...) {
	bool bOpened = false;
	sFile f1;
	va_list list;
	char strTmp[LIMIT_BF];
	char strLogFile[] = "/log.txt";
	if (DiskAutoMount()) {
		if (FileExist(strLogFile)) {
			if (true == (bOpened = FileOpen(&f1, strLogFile))) {
				FileSeek(&f1, FileSize(&f1));
			}
		} else {
			bOpened = FileCreate(&f1, strLogFile);
		}
		if (bOpened) {
			va_start(list, text);
			vsnprintf(strTmp, LIMIT_BF - 1, text, list);
			va_end(list);
			FileWrite(&f1, (const void*) strTmp, strlen(strTmp));
			FileFlush(&f1);
			FileClose(&f1);
		}
	}
}
