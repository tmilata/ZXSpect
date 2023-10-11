#include "zxem.h"
#include "zxsnd.h"
#include "debug.h"
#include "osdep.h"
#include "ay8912.h"

//memory access routines
uint8_t readbyte(uint16_t addr) {
	if (nPatchedRom != 0) {
		if (addr == 0x0556) {
			return 0xED;
		}
		if (addr == 0x0557) {
			return 0xFB;
		}
		if (addr == 0x056c) {
			return 0xC3;
		}
		if (addr == 0x056D) {
			return 0x9F;
		}
		if (addr == 0x056E) {
			return 0x05;
		}
		if (addr == 0x059E) {
			return 0x00;
		}
		if (addr == 0x05c8) {
			return 0xED;
		}
		if (addr == 0x05c9) {
			return 0xFB;
		}
	}
	return membank[addr >> 14][addr & 0x3fff];
}

uint16_t readword(uint16_t addr) {
	return (uint16_t) readbyte(addr) + (((uint16_t) readbyte(addr + 1)) << 8);
}

void writebyte(uint16_t addr, uint8_t data) {
	// Don't allow writing to ROM
	if (addr >= 16384) {
		membank[addr >> 14][addr & 0x3fff] = data;
	}

}

void writeword(uint16_t addr, uint16_t data) {
	writebyte(addr, data & 0xff);
	writebyte(addr + 1, data >> 8);
}

//IO ports access routines

uint8_t zx_data = 0;
unsigned int rowKey[8];

uint8_t input(uint16_t port) {
	//keyboard
	if ((port & 1) == 0) {
		unsigned char res = (port >> 8) & 0xFF;
		unsigned char rescut;
		unsigned char c = 191;
		rescut = (res & 128);
		if (rescut == 0) {
			c = c & rowKey[7];
		}
		rescut = (res & 64);
		if (rescut == 0) {
			c = c & rowKey[6];
		}
		rescut = (res & 32);
		if (rescut == 0) {
			c = c & rowKey[5];
		}
		rescut = (res & 16);
		if (rescut == 0) {
			c = c & rowKey[4];
		}
		rescut = (res & 8);
		if (rescut == 0) {
			c = c & rowKey[3];
		}
		rescut = (res & 4);
		if (rescut == 0) {
			c = c & rowKey[2];
		}
		rescut = (res & 2);
		if (rescut == 0) {
			c = c & rowKey[1];
		}
		rescut = (res & 1);
		if (rescut == 0) {
			c = c & rowKey[0];
		}
		return c;
	}

	//joystick
	if ((port & 224) == 0) {
		return 0;
	}

	//read actually showed attrib byte
	if ((port & 0xFF) == 0xFF) {
		//up/down border
		int cyc = total - lasttotal + CPU_getCycles(CPU_Handle);
		if ((total - lasttotal + CPU_getCycles(CPU_Handle)) < FIRST_SHOWED_BYTE
				|| (total - lasttotal + CPU_getCycles(CPU_Handle))
						> LAST_SHOWED_BYTE) {
			return 0xff;
		}
		//left/right border
		int col = ((total - lasttotal + CPU_getCycles(CPU_Handle))
				% ONE_MICROLINE_TICKS) - 3;
		if (col > 124) {
			return 0xff;
		}
		int row = (total - lasttotal + CPU_getCycles(CPU_Handle))
				/ ONE_MICROLINE_TICKS - 64;
		return membank[4][6144 + 32 * row / 8 + col / 4];
	}

	return 0xFF;
}

uint8_t ay_idx = 0;

void output(uint16_t port, uint8_t data) {

	if (!(port & 0x0001)) {
		if ((port & 0x00FF) == 254) {
			if ((border & 7) != (data & 7)) {
				//border color changed
				border = data & 7;
				DrawRectZx(0, 0, 320, 24, palette[border]);
				DrawRectZx(0, 216, 320, 24, palette[border]);
				DrawRectZx(0, 24, 32, 192, palette[border]);
				DrawRectZx(288, 24, 32, 192, palette[border]);
			}
			if ((beeper & 16) != (data & 16)) {
				//sound bit changed
				beeper = data & 16;
				u64 nPos = (u64) ((441 * (total + CPU_getCycles(CPU_Handle)))
						/ (u64) CYCLES_PER_STEP);
				writeSndByte(nPos, beeper);
				//d_fprintf("pos=%llu,rdabs=%llu>=wrtabs=%llu,wrrel=%i\n",nPos,bs.nReadAbsolutePos,bs.nWriteAbsolutePos,bs.nWritePos);
			}

		}
	} else {

		if ((port | 0x3FFD) == 0xFFFD) {
			//AY sound - write latch

			u64 nPos = (u64) ((441 * (total + CPU_getCycles(CPU_Handle)))
									/ (u64) CYCLES_PER_STEP);
			if(ay0_enable) ay_reg_select(&ay0, data);
		}

		if ((port | 0x3FFD) == 0xBFFD) {
			//AY sound - write data
			u64 nPos = (u64) ((441 * (total + CPU_getCycles(CPU_Handle)))
												/ (u64) CYCLES_PER_STEP);
			if(ay0_enable) ay_reg_write(&ay0, data);
		}

		if (((port & 0x8002) == 0)&&(spectrum_model!=ZX48)) {
			page_set(data);
		}

	}

}

void ZX_Input(void) {
	OSD_Input();
}

