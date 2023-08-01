/* cpuintf.h
 * 
 * CPU Interface
 *
 */

#include "zxem.h"

uint8_t CPU_Create(void);
void CPU_Reset(uint8_t cpuid);
void CPU_Destroy(uint8_t cpuid);
void CPU_Interrupt(uint8_t cpuid);
void CPU_NMI(uint8_t cpuid);
int CPU_Emulate(uint8_t cpuid, int ticks);

uint8_t CPU_GetReg8(uint8_t cpuid, reg rWhich);
uint16_t CPU_GetReg16(uint8_t cpuid, reg rWhich);
uint16_t CPU_GetReg16Alt(uint8_t cpuid, reg rWhich);
void CPU_PutReg8(uint8_t cpuid, reg rWhich, unsigned char value);
void CPU_PutReg8Alt(uint8_t cpuid, reg rWhich, unsigned char value);
void CPU_SetPC(uint8_t cpuid, unsigned short value);
void CPU_SetIff(uint8_t cpuid, reg rWhich, int value);
uint8_t CPU_GetIff(uint8_t cpuid, reg rWhich);
void CPU_SetIntMode(uint8_t cpuid, int value);
uint8_t CPU_GetIntMode(uint8_t cpuid);
bool CPU_getHaltFlag(uint8_t cpuid);
void CPU_resetHaltFlag(uint8_t cpuid);
int CPU_getCycles(uint8_t cpuid);

