#ifndef _ZXSND_H_
#define _ZXSND_H_

#include <stdbool.h>

void SndInit();
bool writeSndByte(unsigned long long int nAbsolutePos, unsigned char nByte);
void setVolume(int value);
int getVolume();
void sndFinishFrame(unsigned long long int uTotalCycles);
void sndOff();
void sndOn();

#endif //_ZXSND_H_
