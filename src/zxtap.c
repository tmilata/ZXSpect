#include "zxtap.h"
#include "osdep.h"

sFile* tapfile=NULL;
long TapFilePos = 0;
char strTapFileName[15];
sFile tapfile_static;

void* getTapFileHandle(){
  return tapfile;
}

void unassignTapFile() {
	if(tapfile!=NULL) {
		FileClose(tapfile);
		tapfile=NULL;
		TapFilePos=0;
	}
}

bool assignTapFile(char* strName) {
	bool bRet=false;
	unassignTapFile();
	if(FileOpen(&tapfile_static, strName)) {
		bRet=true;
		char* pcharSlash=strrchr(strName,'/');
		if(pcharSlash!=NULL){
		 strcpy(strTapFileName,&pcharSlash[1]);
		}else{
		 strcpy(strTapFileName,strName);
		}
		tapfile=&tapfile_static;
	}
	return bRet;
}

void restartTapFile() {
	if(tapfile!=NULL) {
		FileSeek(tapfile, 0);
		TapFilePos=0;
	}
}

void setTapFilePos(long nPos){
	if(tapfile!=NULL) {
		FileSeek(tapfile, nPos);
		TapFilePos=nPos;
	}
}

long getTapFilePos(){
	if(tapfile!=NULL) {	
		return TapFilePos;
	}
	return 0;
}

void getTapFileName(char* strName){
 strName[0]='\0';
 if(tapfile!=NULL) {
   strcpy(strName,strTapFileName);
 }
}
