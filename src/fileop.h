#ifndef _FILEOP_H
#define _FILEOP_H

/*
 *
 * loading a list of games from the SD card
 *
 */

#define FILEROWS	10
#define PATHMAX 1024
void LoadFileList(char* strPath);
void DispFileList(int nFrstSeen,int nMenuPosition);
int getFilesNum();
bool isFileAtPos(int nPos);
void getNameAtPos(char* strShortFile,int nPos);
int getDirPosition(char* strDirName);

#endif // _FILEOP_H
