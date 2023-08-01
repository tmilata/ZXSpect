/* cpuintf.c
 *
 * CPU Interface (mz80)
 *
 * Allows ZXEM to be used with different cpu cores
 *
 */

#include "zxem.h"
#include "z80emu.h"
#include "debug.h"

#define MAX_CPU 10

Z80_STATE *zxcpu[MAX_CPU];
uint8_t num_cpus = -1;

// create CPU context and return a handle
uint8_t CPU_Create(void) {
	num_cpus++;
	if(num_cpus<MAX_CPU) {
		zxcpu[num_cpus]=(Z80_STATE*)malloc(sizeof(Z80_STATE));
		zxcpu[num_cpus]->memory = zxmem;
		zxcpu[num_cpus]->readbyte = readbyte;
		zxcpu[num_cpus]->readword = readword;
		zxcpu[num_cpus]->writeword = writeword;
		zxcpu[num_cpus]->writebyte = writebyte;
		zxcpu[num_cpus]->input = input;
		zxcpu[num_cpus]->output = output;
		return num_cpus;
	}
	return 255;
}

// reset a CPU context
void CPU_Reset(uint8_t cpuid) {

	Z80Reset(zxcpu[cpuid]);
	zxcpu[cpuid]->im = Z80_INTERRUPT_MODE_0;
}


// Assign a block of memory as read/write
void CPU_MEMRW(int addr, int length) {

}

// Assign a block of memory as read only
void CPU_MEMR(int addr, int length) {

}

// Destroy a CPU context
void CPU_Destroy(uint8_t cpuid) {
	free(zxcpu[cpuid]);
	zxcpu[cpuid]=NULL;
}

// Emulatoe virtual CPU
int CPU_Emulate(uint8_t cpuid, int ticks) {
	return Z80Emulate(zxcpu[cpuid], ticks);
}

int oim;

// Call interrupt on a CPU
void CPU_Interrupt(uint8_t cpuid) {
	if(oim!=zxcpu[cpuid]->im) {
		oim = zxcpu[cpuid]->im;
		//printf("New interrupt: MODE: %d ",zxcpu[cpuid]->im);
		switch(oim) {
		case Z80_INTERRUPT_MODE_0:
			//printf("mode 0\n");
			break;
		case Z80_INTERRUPT_MODE_1:
			//printf("mode 1\n");
			break;
		case Z80_INTERRUPT_MODE_2:
			//printf("mode 2\n");
			break;
			//default:
			//printf("????\n");
		}

	}
	Z80Interrupt(zxcpu[cpuid],0xff);
}

// Call an NMI on a CPU
void CPU_NMI(uint8_t cpuid) {
	// TODO
}

bool CPU_getHaltFlag(uint8_t cpuid) {
	return zxcpu[cpuid]->bHalt;
}

void CPU_resetHaltFlag(uint8_t cpuid) {
	zxcpu[cpuid]->bHalt=false;
}

uint8_t CPU_GetReg8(uint8_t cpuid, reg rWhich) {

	switch(rWhich) {
	case I:
		return zxcpu[cpuid]->i;
		break;
	case R:
		return zxcpu[cpuid]->r;
		break;
	case A:
		return zxcpu[cpuid]->registers.byte[Z80_A];
		break;
	case F:
		return zxcpu[cpuid]->registers.byte[Z80_F];
		break;
	case B:
		return zxcpu[cpuid]->registers.byte[Z80_B];
		break;
	case C:
		return zxcpu[cpuid]->registers.byte[Z80_C];
		break;
	case D:
		return zxcpu[cpuid]->registers.byte[Z80_D];
		break;
	case E:
		return zxcpu[cpuid]->registers.byte[Z80_E];
		break;
	case H:
		return zxcpu[cpuid]->registers.byte[Z80_H];
		break;
	case L:
		return zxcpu[cpuid]->registers.byte[Z80_L];
		break;
	case IXH:
		return zxcpu[cpuid]->registers.byte[Z80_IXH];
		break;
	case IXL:
		return zxcpu[cpuid]->registers.byte[Z80_IXL];
		break;
	case IYH:
		return zxcpu[cpuid]->registers.byte[Z80_IYH];
		break;
	case IYL:
		return zxcpu[cpuid]->registers.byte[Z80_IYL];
		break;
	case SPH:
		return zxcpu[cpuid]->registers.byte[Z80_SPH];
		break;
	case SPL:
		return zxcpu[cpuid]->registers.byte[Z80_SPL];
		break;
	default:
		return 0;
		break;
	}
}

uint16_t CPU_GetReg16(uint8_t cpuid, reg rWhich) {

	switch(rWhich) {
	case PC:
		return zxcpu[cpuid]->pc;
		break;
	case AF:
		return zxcpu[cpuid]->registers.word[Z80_AF];
		break;
	case BC:
		return zxcpu[cpuid]->registers.word[Z80_BC];
		break;
	case DE:
		return zxcpu[cpuid]->registers.word[Z80_DE];
		break;
	case HL:
		return zxcpu[cpuid]->registers.word[Z80_HL];
		break;
	case IX:
		return zxcpu[cpuid]->registers.word[Z80_IX];
		break;
	case IY:
		return zxcpu[cpuid]->registers.word[Z80_IY];
		break;
	case SP:
		return zxcpu[cpuid]->registers.word[Z80_SP];
		break;
	default:
		return 0;
		break;
	}
}

uint16_t CPU_GetReg16Alt(uint8_t cpuid, reg rWhich) {

	switch(rWhich) {
	case AF:
		return zxcpu[cpuid]->alternates.word[Z80_AF];
		break;
	case BC:
		return zxcpu[cpuid]->alternates.word[Z80_BC];
		break;
	case DE:
		return zxcpu[cpuid]->alternates.word[Z80_DE];
		break;
	case HL:
		return zxcpu[cpuid]->alternates.word[Z80_HL];
		break;
	default:
		return 0;
		break;
	}
}

void CPU_SetPC(uint8_t cpuid, unsigned short value) {
	zxcpu[cpuid]->pc=value;
}

void CPU_PutReg8(uint8_t cpuid, reg rWhich, unsigned char value) {

	switch(rWhich) {
	case I:
		zxcpu[cpuid]->i=value;
		break;
	case R:
		zxcpu[cpuid]->r=value;
		break;
	case A:
		zxcpu[cpuid]->registers.byte[Z80_A]=value;
		break;
	case F:
		zxcpu[cpuid]->registers.byte[Z80_F]=value;
		break;
	case B:
		zxcpu[cpuid]->registers.byte[Z80_B]=value;
		break;
	case C:
		zxcpu[cpuid]->registers.byte[Z80_C]=value;
		break;
	case D:
		zxcpu[cpuid]->registers.byte[Z80_D]=value;
		break;
	case E:
		zxcpu[cpuid]->registers.byte[Z80_E]=value;
		break;
	case H:
		zxcpu[cpuid]->registers.byte[Z80_H]=value;
		break;
	case L:
		zxcpu[cpuid]->registers.byte[Z80_L]=value;
		break;
	case IXH:
		zxcpu[cpuid]->registers.byte[Z80_IXH]=value;
		break;
	case IXL:
		zxcpu[cpuid]->registers.byte[Z80_IXL]=value;
		break;
	case IYH:
		zxcpu[cpuid]->registers.byte[Z80_IYH]=value;
		break;
	case IYL:
		zxcpu[cpuid]->registers.byte[Z80_IYL]=value;
		break;
	case SPH:
		zxcpu[cpuid]->registers.byte[Z80_SPH]=value;
		break;
	case SPL:
		zxcpu[cpuid]->registers.byte[Z80_SPL]=value;
		break;
	default:
		break;
		//printf("Unimplemented reg return %s\n",reg);
	}
}

void CPU_PutReg8Alt(uint8_t cpuid, reg rWhich, unsigned char value) {

	switch(rWhich) {
	case A:
		zxcpu[cpuid]->alternates.byte[Z80_A]=value;
		break;
	case F:
		zxcpu[cpuid]->alternates.byte[Z80_F]=value;
		break;
	case B:
		zxcpu[cpuid]->alternates.byte[Z80_B]=value;
		break;
	case C:
		zxcpu[cpuid]->alternates.byte[Z80_C]=value;
		break;
	case D:
		zxcpu[cpuid]->alternates.byte[Z80_D]=value;
		break;
	case E:
		zxcpu[cpuid]->alternates.byte[Z80_E]=value;
		break;
	case H:
		zxcpu[cpuid]->alternates.byte[Z80_H]=value;
		break;
	case L:
		zxcpu[cpuid]->alternates.byte[Z80_L]=value;
		break;
	default:
		break;
		//printf("Unimplemented reg return %s\n",reg);
	}
}

void CPU_SetIff(uint8_t cpuid, reg rWhich, int value) {
	switch(rWhich) {
	case iff1:
		zxcpu[cpuid]->iff1=value;
		break;
	case iff2:
		zxcpu[cpuid]->iff2=value;
		break;
	default:
		break;
		//printf("Unimplemented reg return %s\n",reg);
	}
}

uint8_t CPU_GetIff(uint8_t cpuid, reg rWhich){
 switch(rWhich) {
	case iff1:
		return zxcpu[cpuid]->iff1;
		break;
	case iff2:
		return zxcpu[cpuid]->iff2;
		break;
	default:
		break;
		//printf("Unimplemented reg return %s\n",reg);
	}
 return 0;
}

void CPU_SetIntMode(uint8_t cpuid, int value) {
	zxcpu[cpuid]->im=value;
}

uint8_t CPU_GetIntMode(uint8_t cpuid) {
   return zxcpu[cpuid]->im;
}

int CPU_getCycles(uint8_t cpuid) {
	return zxcpu[cpuid]->elapsed_cycles;
}

