#include "osdep.h"
#include "fileop.h"
#include "debug.h"


#define MAXFILES	1024

// display
int DispX, DispY; // X and Y text coordinate


typedef struct { // 10 bytes
	u8	attr;		// attributes ATTR_*
	u8	len;		// file name length
	char	name[13];	// file name (without extension and without terminating zero)
} sFileDesc;

// files
sFileDesc FileDesc[MAXFILES]; // file descriptors
int FileNum; // number of files
int FileTop; // index of first visible file
sFile FileF; // search structure
sFileInfo FileI; // search file info


void LoadFileList(char* strPath) {
	// clear file list
	FileNum = 0;

	DiskAutoMount();
	// set current directory
	if (!SetDir(strPath)) return;

	// open search
	if (!FindOpen(&FileF, "")) return;

	// load files
	int inx, i, len;
	sFileDesc* fd = FileDesc;
	char ch;
	char* name;
	Bool dir;
	for (inx = 0; inx < MAXFILES; inx++) {

		// find next file
		if (!FindNext(&FileF, &FileI, ATTR_DIR_MASK, "*.*")) break;

		// check directory
		dir = (FileI.attr & ATTR_DIR) != 0;

		// skip directory "."
		len = FileI.namelen;
		name = FileI.name;
		if (dir && (len == 1) && (name[0] == '.')) continue;

		// get attributes
		fd->attr = FileI.attr & ATTR_MASK;

		// copy directory ".."
		if (dir && (len == 2) && (name[0] == '.') && (name[1] == '.')) {
			fd->len = len;
			fd->name[0] = '.';
			fd->name[1] = '.';
			fd++;
			FileNum++;
		} else {
			// skip hidden entry
			if ((FileI.attr & ATTR_HID) != 0) continue;

			// directory
			if (dir) {
				// copy directory name (without extension)
				for (i = 0; (i < len) && (i < 13); i++) {
					ch = name[i];
					if (ch == '.') break;
					fd->name[i] = ch;
				}

				// directory is valid only if has no extension
				if (i == len) {
					fd->len = len;
					fd++;
					FileNum++;
				}

			}

			// file
			else {
				// check extension "SNA" or "Z80" or "TAP"
				if ((len > 4) && (((name[len-4] == '.') && (name[len-3] == 'T') &&
				                   (name[len-2] == 'A') && (name[len-1] == 'P'))||((name[len-4] == '.') && (name[len-3] == 'S') &&
				                   (name[len-2] == 'N') && (name[len-1] == 'A'))||((name[len-4] == '.') && (name[len-3] == 'Z') &&
				                       (name[len-2] == '8') && (name[len-1] == '0')))) {
					fd->len = len;
					memcpy(fd->name, name, len);
					fd++;
					FileNum++;
				}
			}
		}
	}


	// sort files (using bubble sort)
	fd = FileDesc;
	for (inx = 0; inx < FileNum-1;) {
		Bool ok = True;

		// directory '..' must be at first place
		if ((fd[1].len == 2) && (fd[1].name[0] == '.') && (fd[1].name[1] == '.')) ok = False;

		// directory must be before the files
		if (((fd[0].attr & ATTR_DIR) == 0) && ((fd[1].attr & ATTR_DIR) != 0)) ok = False;

		// entry of the same group
		if (((fd[0].attr ^ fd[1].attr) & ATTR_DIR) == 0) {
			// compare names
			len = fd[0].len;
			if (fd[1].len < fd[0].len) len = fd[1].len;
			for (i = 0; i < len; i++) {
				if (fd[0].name[i] != fd[1].name[i]) break;
			}

			if (i < len) { // names are different
				if (fd[0].name[i] > fd[1].name[i]) ok = False;
			} else { // names are equal, check name lengths
				if (fd[1].len < fd[0].len) ok = False;
			}
		}

		// exchange files
		if (!ok) {
			ch = fd[0].attr;
			fd[0].attr = fd[1].attr;
			fd[1].attr = ch;

			ch = fd[0].len;
			fd[0].len = fd[1].len;
			fd[1].len = ch;

			for (i = 0; i < 13; i++) {
				ch = fd[0].name[i];
				fd[0].name[i] = fd[1].name[i];
				fd[1].name[i] = ch;
			}

			// shift index down
			if (inx > 0) {
				inx -= 2;
				fd -= 2;
			}
		}

		// shift index up
		inx++;
		fd++;
	}

}

void getNameAtPos(char* strShortFile,int nPos) {
	sFileDesc* fd = &FileDesc[FileTop];
	memcpy(strShortFile,fd[nPos].name,fd[nPos].len);
	strShortFile[fd[nPos].len]=0;
}

int getDirPosition(char* strDirName) {
	int nRet=-1;
	sFileDesc* fd = &FileDesc[FileTop];
	for (int i = 0; i < FileNum; i++) {
		if(fd[i].len==strlen(strDirName)) {
			if ((fd[i].attr & ATTR_DIR) != 0) {
				if(memcmp(fd[i].name,strDirName,fd[i].len)==0) {
					nRet=i;
					break;
				}
			}
		}
	}
	return nRet;
}


int getFilesNum() {
	return FileNum;
}

bool isFileAtPos(int nPos) {
	sFileDesc* fd = &FileDesc[FileTop];
	if ((fd[nPos].attr & ATTR_DIR) != 0) {
		return false;
	} else {
		return true;
	}

}

void DispFileList(int nFrstShowed,int nMenuPosition) {
	// reset cursor
	DispY = 0;

	// display files
	int i, j;
	char ch;
	Bool dir;
	char strBuf[14];
	sFileDesc* fd = &FileDesc[FileTop];
	for (i = nFrstShowed; (i < nFrstShowed + FILEROWS)&&(i < FileNum); i++) {
		DispX = 32;
		int nColFrg=COL_WHITE_ZX;
		int nColBkg=COL_BLACK_ZX;
		if(i == nMenuPosition) {
			nColFrg=COL_BLACK_ZX;
			nColBkg=COL_WHITE_ZX;
		}
		memset(strBuf,32,13);
		strBuf[13]=0;
		if ((fd[i].attr & ATTR_DIR) != 0) {
			memcpy(strBuf,"/",1);
			memcpy(&strBuf[1],fd[i].name,fd[i].len);
			DrawTextBgZx(strBuf,DispX,48+DispY*FONTH, nColFrg, nColBkg);
		} else {
			memcpy(strBuf,fd[i].name,fd[i].len);
			DrawTextBgZx(strBuf,DispX,48+DispY*FONTH, nColFrg, nColBkg);
		}
		// increase line
		DispY++;
	}


}
