/* osdep.h
 *
 * ZX Spectrum emulator
 * Copyright (c) 2016 MikeDX
 *
 * Cross platform ZX Spectrum emulator
 *
 * A new port must supply these functions in order to operate
 */

#ifndef _OSDEP_H_
#define _OSDEP_H_
#include "../include.h"
#include <stdbool.h>
#include "lib_drawtftzx.h"


#pragma GCC diagnostic ignored "-Wwrite-strings"

#define FIRST_SHOWED_BYTE 14336
#define LAST_SHOWED_BYTE 57246
#define ONE_LINE_TICKS 1792
#define ONE_MICROLINE_TICKS 224


enum reg {A=0,F,B,C,D,E,H,L,IXH,IXL,IYH,IYL,SPL,SPH,I,R,PC,AF,BC,DE,HL,IX,IY,SP,iff1,iff2};

typedef enum reg reg;

//extern unsigned short palette[16];
extern unsigned char palette[16];
extern unsigned int rowKey[8];
extern unsigned int border;
extern unsigned int beeper;
extern char currPath[1024];
extern char strMapFile[13];
extern int nMenuPos;
extern int nFirstShowed;
extern unsigned long long int  total;
extern unsigned long long int  lasttotal;


typedef enum specmodel {ZX48,ZX128,ZX_LAST} specmodel;
extern specmodel spectrum_model;
extern int page;
extern unsigned char *membank[5];
extern const unsigned char* rom;

//define positions of PicoPad keys in array mapKeys
#define KBD_UP		0
#define KBD_DOWN	1
#define KBD_LEFT	2
#define KBD_RIGHT	3
#define KBD_A	4
#define KBD_B	5
#define KBD_Y	6
extern unsigned char mapKeys[8];

//return code from loading file with Z80 extension
#define Z80SNAP_OK 0
#define Z80SNAP_BROKEN -1
#define Z80SNAP_BADHW -2
#define Z80SNAP_128 -3

//indicate that rom is patched for TAP loading
extern int nPatchedRom;

int main(int argc, char *argv[]);

void OSD_Input(void);

#endif
