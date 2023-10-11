/* zxem.h
 * 
 * ZX Spectrum emulator 
 * Copyright (c) 2016 MikeDX
 *
 * Cross platform ZX Spectrum emulator
 * 
 */

#ifndef _ZXEM_H_
#define _ZXEM_H_
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// macro for global vars
#ifndef GLOBALS
#define GLOBAL extern
#else
#define GLOBAL
#endif

// Platform specific
#include "osdep.h"

// CPU interface
#include "cpuintf.h"

// ZX IO interface
#include "zxio.h"

// ZX Video Hardware
#include "zxvid.h"

#define Z80_CPU_SPEED           3500000   /* In Hz. */
#define CYCLES_PER_STEP         69888
 //(Z80_CPU_SPEED / 50)

GLOBAL uint8_t *screenbuf;

GLOBAL uint8_t running;
GLOBAL uint8_t debug;
GLOBAL uint8_t CPU_Handle;

int LoadSnapshot(char* strSnaFile);
uint8_t cachedread(uint16_t addr);
void cachedwrite(uint16_t addr,uint8_t value);
//autowrite LOAD ""
bool doAutoLoadTape();
void setAutoLoadOn();
void setAutoLoadOff();
// Main entry point
int ZX_main(int argc, char *argv[]);
void setModel(specmodel nModel);
void page_set(int data);

extern uint8_t zxmem[131072];
extern uint8_t cached[6912];

void MemDump1();
void MemDump2();


#endif // include once
