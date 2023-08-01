#ifndef _ZXKBD_H
#define _ZXKBD_H
void fillKeyInfo();
u8 getRow(int nPosition);
u8 getPressMask(int nPosition);
u8 getReleaseMask(int nPosition);
void getKeyName(char* strName,int nPosition);
u8 getKeyPosition(const char* strKey);

#endif // _ZXKBD_H
