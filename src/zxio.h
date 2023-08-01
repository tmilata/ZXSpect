/* zxio.h

*/

#ifndef _ZXIO_H_
#define _ZXIO_H_

void SndInit();
void BufferInit();
void setVolume(int value);
void FinishFrame(u64 uTotalCycles);
GLOBAL uint8_t *zxmem;
GLOBAL uint8_t *cached;

// memory handling
uint8_t readbyte(uint16_t addr);
uint16_t readword(uint16_t addr);
void writebyte(uint16_t addr, uint8_t data);
void writeword(uint16_t addr, uint16_t data);

// IO
uint8_t input(uint16_t port);
void output(uint16_t port, uint8_t data);

// Keyboard handling
void ZX_Input(void);

#endif

