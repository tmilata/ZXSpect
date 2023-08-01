
#ifndef _ZXTAP_H_
#define _ZXTAP_H_

#include <stdbool.h>

void unassignTapFile();
bool assignTapFile(char* strName);
void restartTapFile();
void setTapFilePos(long nPos);
long getTapFilePos();
void getTapFileName(char* strName);
void* getTapFileHandle();

extern long TapFilePos;
extern char strTapFileName[15];

#endif //_ZXTAP_H_
