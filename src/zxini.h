#ifndef _ZXINI_H
#define _ZXINI_H
bool readMapFile(char* strFileName,unsigned char* mapKeys);
void saveMapFile(char* strFileName,unsigned char* mapKeys);
void saveIniFile(char* strFileName);
bool readIniFile(char* strFileName);
#endif // _ZXINI_H