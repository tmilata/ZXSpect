#include "osdep.h"
#include <stdio.h>
#include "zxini.h"
#include "zxkbd.h"
#include "zxsnd.h"
#include "debug.h"

char strRBuff[2048];

char *keywrd[]= {"UP=","DOWN=","LEFT=","RIGHT=","A=","B=","Y=","\0"};
bool readMapFile(char* strFileName,unsigned char* mapKeys) {
	bool bRet=false;
	bool bOpened=false;
	sFile f1;
	if (DiskValidate()) {
		if(FileExist(strFileName)) {
			if(FileOpen(&f1, strFileName)) {
				int nMax=0;
				if((nMax=FileRead(&f1, (void*) strRBuff, 511))>0) {
					strRBuff[nMax]='\0';
					int nKwrdPos=0;
					while(keywrd[nKwrdPos][0]!=0) {
						bool bVrat=false;
						char* pstrFoundBegin=0;
						char* pstrFoundEnd=0;
						if(0!=(pstrFoundBegin=strstr(strRBuff,keywrd[nKwrdPos]))) {
							if(0!=(pstrFoundEnd=strstr(&pstrFoundBegin[0],"\n"))) {
								pstrFoundEnd[0]='\0';
								bVrat=true;
							}
							mapKeys[nKwrdPos]=getKeyPosition(&pstrFoundBegin[strlen(keywrd[nKwrdPos])]);
							if(bVrat) {
								pstrFoundEnd[0]='\n';
								bVrat=false;
							}
						}
						nKwrdPos++;
					}


				}
				FileClose(&f1);
				bRet=true;
			}
		}
	}
	return bRet;
}

void saveMapFile(char* strFileName,unsigned char* mapKeys) {
	bool bOpened=false;
	sFile f1;
	if (DiskValidate()) {
		if(FileExist(strFileName)) {
			if(true==(bOpened=FileOpen(&f1, strFileName))) {
				//truncate
				SetFileSize(&f1, 0);
			}
		} else {
			bOpened=FileCreate(&f1, strFileName);
		}
		if(bOpened) {
			int nBuffPos=0;
			char strName[30];
			int nKwrdPos=0;
			while(keywrd[nKwrdPos][0]!=0) {
				getKeyName(strName,mapKeys[nKwrdPos]);
				sprintf(&strRBuff[nBuffPos],"%s%s\n",keywrd[nKwrdPos],strName);
				nBuffPos+=strlen(&strRBuff[nBuffPos]);
				nKwrdPos++;
			}
			FileWrite(&f1, (const void*) strRBuff, strlen(strRBuff));
			FileFlush(&f1);
			FileClose(&f1);
		}
	}
}

char *keywrdsini[]= {"PATH=","VOLUME=","\0"};
bool readIniFile(char* strFileName) {
	bool bRet=false;
	bool bOpened=false;
	sFile f1;
	if (DiskValidate()) {
		if(FileExist(strFileName)) {
			if(FileOpen(&f1, strFileName)) {
				int nMax=0;
				if((nMax=FileRead(&f1, (void*) strRBuff, 2047))>0) {
					strRBuff[nMax]='\0';
					int nKwrdPos=0;
					while(keywrdsini[nKwrdPos][0]!=0) {
						bool bVrat=false;
						char* pstrFoundBegin=0;
						char* pstrFoundEnd=0;
						if(0!=(pstrFoundBegin=strstr(strRBuff,keywrdsini[nKwrdPos]))) {
							if(0!=(pstrFoundEnd=strstr(&pstrFoundBegin[0],"\n"))) {
								pstrFoundEnd[0]='\0';
								bVrat=true;
							}
							switch (nKwrdPos) {
							case 0:
								strncpy(currPath,&pstrFoundBegin[strlen(keywrdsini[nKwrdPos])],1024);
								break;
							case 1:
								char strVol[2];
								strncpy(strVol,&pstrFoundBegin[strlen(keywrdsini[nKwrdPos])],2);
								strVol[1]='\0';
								int nVolumeTmp=atoi(strVol);
								if(nVolumeTmp>9) {nVolumeTmp=9;}
								if(nVolumeTmp<0) {nVolumeTmp=0;}
								setVolume(nVolumeTmp);
								break;
							}

							if(bVrat) {
								pstrFoundEnd[0]='\n';
								bVrat=false;
							}
						}
						nKwrdPos++;
					}
				}
				FileClose(&f1);
				bRet=true;
			}
		}
	}
	return bRet;
}

void saveIniFile(char* strFileName) {
	bool bOpened=false;
	sFile f1;
	if (DiskValidate()) {
		if(FileExist(strFileName)) {
			if(true==(bOpened=FileOpen(&f1, strFileName))) {
				//truncate
				SetFileSize(&f1, 0);
			}
		} else {
			bOpened=FileCreate(&f1, strFileName);
		}
		if(bOpened) {
			int nBuffPos=0;			
			int nKwrdPos=0;
			while(keywrdsini[nKwrdPos][0]!=0) {
				switch (nKwrdPos) {
				case 0:
					sprintf(&strRBuff[nBuffPos],"%s%s\n",keywrdsini[nKwrdPos],currPath);
					nBuffPos+=strlen(&strRBuff[nBuffPos]);					
					break;
				case 1:
					sprintf(&strRBuff[nBuffPos],"%s%i\n",keywrdsini[nKwrdPos],getVolume());
					nBuffPos+=strlen(&strRBuff[nBuffPos]);										
					break;
				}				
				nKwrdPos++;
			}
			FileWrite(&f1, (const void*) strRBuff, strlen(strRBuff));
			FileFlush(&f1);
			FileClose(&f1);
		}
	}
}
