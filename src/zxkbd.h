#ifndef _ZXKBD_H
#define _ZXKBD_H

#define INV(x) (~(x&0xFF))&0xFF;
#define KEY_BIT0  0xfe
#define KEY_BIT1  0xfd
#define KEY_BIT2  0xfb
#define KEY_BIT3  0xf7
#define KEY_BIT4  0xef
#define KEY_BIT7  0x7f
void fillKeyInfo();
u8 getRow(int nPosition);
u8 getRowAlt(int nPosition);
u8 getPressMask(int nPosition);
u8 getReleaseMask(int nPosition);
u8 getPressMaskAlt(int nPosition);
u8 getReleaseMaskAlt(int nPosition);
void getKeyName(char* strName,int nPosition);
u8 getKeyPosition(const char* strKey);

extern u8 usbkeys[256];
void ProcessUSBToZXKeyboard();

#endif // _ZXKBD_H
