#include "osdep.h"
#include "zxz80.h"
#include "zxem.h"
#include "zxio.h"
#include "debug.h"
#include <stdint.h>
#include "ay8912.h"

typedef struct z80snapshot {
	uint8_t A, F, B, C, D, E, H, L, IXH, IXL, IYH, IYL, SPH, SPL, AA, FF, BB,
			CC, DD, EE, HH, LL, R, I, IFF1, IFF2, Imode;
	bool issue_3;
	uint16_t PC;
	uint8_t type;        // bit 0/1: 48K/128K/+3
	uint8_t border;      // border color
	uint8_t pager;       // content of pagination register in 128K mode
	unsigned int found_pages; // bit=1: pageloc exists. bit=0: pageloc don't exists.
	unsigned char ay_regs[16];
	unsigned char ay_latch;
	unsigned char joystick;
} z80snapshot;

void load_cpu(z80snapshot *snap) {
	//d_fprintf("AA:%d FF:%d BB:%d CC:%d DD:%d EE:%d HH:%d LL:%d\n", snap->AA, snap->FF, snap->BB, snap->CC, snap->DD, snap->EE, snap->HH, snap->LL);

	CPU_PutReg8Alt(CPU_Handle, A, snap->AA);
	CPU_PutReg8Alt(CPU_Handle, F, snap->FF);
	CPU_PutReg8Alt(CPU_Handle, B, snap->BB);
	CPU_PutReg8Alt(CPU_Handle, C, snap->CC);
	CPU_PutReg8Alt(CPU_Handle, D, snap->DD);
	CPU_PutReg8Alt(CPU_Handle, E, snap->EE);
	CPU_PutReg8Alt(CPU_Handle, H, snap->HH);
	CPU_PutReg8Alt(CPU_Handle, L, snap->LL);

	//d_fprintf("A:%d F:%d B:%d C:%d D:%d E:%d H:%d L:%d\n", snap->A, snap->F, snap->B, snap->C, snap->D, snap->E, snap->H, snap->L);

	CPU_PutReg8(CPU_Handle, A, snap->A);
	CPU_PutReg8(CPU_Handle, F, snap->F);
	CPU_PutReg8(CPU_Handle, B, snap->B);
	CPU_PutReg8(CPU_Handle, C, snap->C);
	CPU_PutReg8(CPU_Handle, D, snap->D);
	CPU_PutReg8(CPU_Handle, E, snap->E);
	CPU_PutReg8(CPU_Handle, H, snap->H);
	CPU_PutReg8(CPU_Handle, L, snap->L);

	CPU_PutReg8(CPU_Handle, IXH, snap->IXH);
	CPU_PutReg8(CPU_Handle, IXL, snap->IXL);
	CPU_PutReg8(CPU_Handle, IYH, snap->IYH);
	CPU_PutReg8(CPU_Handle, IYL, snap->IYL);
	CPU_PutReg8(CPU_Handle, SPH, snap->SPH);
	CPU_PutReg8(CPU_Handle, SPL, snap->SPL);

	//R, I, IFF1, IFF2, Imode;
	//d_fprintf("PC:%d\n", snap->PC);

	CPU_SetPC(CPU_Handle, snap->PC);

	//d_fprintf("I:%d R:%d\n", snap->I, snap->R);
	CPU_PutReg8(CPU_Handle, I, snap->I);
	CPU_PutReg8(CPU_Handle, R, snap->R);

	//d_fprintf("IFF1:%d IFF2:%d Imode:%d Border:%d\n", snap->IFF1, snap->IFF2,snap->Imode, snap->border);
	CPU_SetIff(CPU_Handle, iff1, snap->IFF1);
	CPU_SetIff(CPU_Handle, iff2, snap->IFF2);

	CPU_SetIntMode(CPU_Handle, snap->Imode);

	border = snap->border;
	if (spectrum_model == ZX128) {
		page_set(snap->pager);
		for (int i = 0; i < 16; i++) {
			ay_reg_select(&ay0, i);
			ay_reg_write(&ay0, snap->ay_regs[i]);
		}
		ay_reg_select(&ay0, snap->ay_latch);
		ay0_enable=true;
	}

}

void load_uncompressed_z80(sFile *file, int length, uint16_t memStart) {
	unsigned char b;
	for (int i = 0; i < length; i++) {
		FileRead(file, &b, 1);
		writebyte(memStart + i, b);
	}
}

void load_compressed_z80(sFile *file, int length, uint16_t memStart) {
	unsigned char byte_loaded, EDfound, counter;
	int position;

	counter = 0; //the x y uncompression counter
	byte_loaded = 0; //last byte loaded
	EDfound = 0; //if two EDs are found
	position = memStart;

	do {
		if (counter) {
			writebyte(position++, byte_loaded);
			counter--;
			continue;
		} else {
			FileRead(file, &byte_loaded, 1);
		}

		if (EDfound == 2) { // we have two EDs
			counter = byte_loaded;
			FileRead(file, &byte_loaded, 1);
			EDfound = 0;
			continue;
		}

		if (byte_loaded == 0xED) {
			EDfound++;
		} else {
			if (EDfound == 1) { // we found single ED xx. We write ED and xx
				writebyte(position++, 0xED);
				EDfound = 0;
			}
			if (position >= length + memStart) {
				break;
			}
			writebyte(position++, byte_loaded);
		}
	} while (position < memStart + length);
}

//https://worldofspectrum.org/faq/reference/z80format.htm
int load_z80_file(sFile *file) {
	struct z80snapshot *snap;
	unsigned char header1[30], header2[56], type, compressed, pageloc,
			pageHeader[3];
	int size = 0, length, bucle, nMaxPos = 0;

	snap = (struct z80snapshot*) malloc(sizeof(struct z80snapshot));

	size = FileSize(file);
	nMaxPos = size;

	//d_fprintf("Read Z80 file\n");

	for (int i = 0; i < 16; i++) {
		snap->ay_regs[i] = 0;
	}

	//d_fprintf("Read header (first 30 bytes)\n");
	FileRead(file, header1, 30);

	if ((header1[6] == 0) && (header1[7] == 0)) { // extended Z80
		//d_fprintf("It's an extended Z80 file\n");
		type = 1;                     // new type

		FileRead(file, header2, 2);	// read the length of the extension header

		size = ((int) header2[0]) + ((int) header2[1] << 8);
		if (size > 54) {
			//d_fprintf("Not suported Z80 file\n");
			free(snap);
			return Z80SNAP_BROKEN; // not a supported Z80 file
		}
		//d_fprintf("Header2 Length: %d\n", size);
		FileRead(file, header2 + 2, size); //load the rest of header2

		if (size == 23) { // z80 ver 2.01
			switch (header2[4]) {
			case 0:
			case 1:
				snap->type = 0; // 48K
				break;

			case 3:
			case 4:
				snap->type = 1; // 128K
				break;

			default:
				//d_fprintf("Hardware Mode not suported Z80 file\n");
				free(snap);
				return Z80SNAP_BADHW; // not a supported Z80 file
				break;
			}
		} else {
			// z80 ver 3.0x
			switch (header2[4]) {
			case 0:
			case 1:
			case 3:
				snap->type = 0; // 48K
				break;

			case 4:
			case 5:
			case 6:
				snap->type = 1; // 128K
				break;

			default:
				free(snap);
				return Z80SNAP_BADHW; // not a supported Z80 file
				break;
			}
		}
	} else {
		//d_fprintf("Old type z80\n");
		type = 0; // old type
		snap->type = 0; // 48k
	}

	if (header1[29] & 0x04) {
		//d_fprintf("Issue 2\n");
		snap->issue_3 = false; // issue2
	} else {
		//d_fprintf("Issue 3\n");
		snap->issue_3 = true; // issue3
	}

	snap->A = header1[0];
	snap->F = header1[1];
	snap->C = header1[2];
	snap->B = header1[3];
	snap->L = header1[4];
	snap->H = header1[5];
	if (type) {
		snap->PC = ((uint16_t) header2[2]) + ((uint16_t) header2[3] << 8);
		for (int i = 0; i < 16; i++) {
			snap->ay_regs[i] = header2[9 + i];
		}
		snap->ay_latch = header2[8];
	} else {
		snap->PC = ((uint16_t) header1[6]) + ((uint16_t) header1[7] << 8);
	}

	snap->SPL = header1[8];
	snap->SPH = header1[9];
	snap->I = header1[10];
	snap->R = (header1[11] & 0x7F);

	if (header1[12] == 255) {
		//d_fprintf("Header1 Byte 12 is 255! set this to 1\n");
		header1[12] = 1;
	}

	if (header1[12] & 0x01) {
		snap->R |= 0x80;
	}

	snap->border = (header1[12] >> 1) & 0x07;

	compressed = ((header1[12] & 32) || (type)) ? 1 : 0;

	snap->E = header1[13];
	snap->D = header1[14];
	snap->CC = header1[15];
	snap->BB = header1[16];
	snap->EE = header1[17];
	snap->DD = header1[18];
	snap->LL = header1[19];
	snap->HH = header1[20];
	snap->AA = header1[21];
	snap->FF = header1[22];
	snap->IYL = header1[23];
	snap->IYH = header1[24];
	snap->IXL = header1[25];
	snap->IXH = header1[26];

	snap->IFF1 = (header1[27] == 0) ? 0 : 1;
	snap->IFF2 = (header1[28] == 0) ? 0 : 1;

	snap->Imode = (header1[29] & 0x03);

	snap->joystick = ((header1[29] >> 6) & 0x03);

	if (type) {
		snap->pager = header2[5];
	}

	if (type) { // extended z80
		if (snap->type == 1) { // 128K snapshot
			setModel(ZX128);
			unsigned char pagebkp = page;
			while (FilePos(file) < nMaxPos) {
				FileRead(file, pageHeader, 3);
				if (FilePos(file) >= nMaxPos) {
					break;
				}

				length = ((int) pageHeader[0]) + ((int) pageHeader[1] << 8);

				uint16_t memStart = 0xC000; //this is meant as a flag to skip loading this pageloc
				page_set(pageHeader[2] - 3); //switching the memory pageloc

				if (length == 0xFFFF) { // uncompressed raw data
					load_uncompressed_z80(file, 16384, memStart);
				} else {
					load_compressed_z80(file, 16384, memStart);
				}

			}
			page_set(pagebkp);

		} else {  //48K
			setModel(ZX48);
			while (FilePos(file) < nMaxPos) {
				FileRead(file, pageHeader, 3);
				if (FilePos(file) >= nMaxPos) {
					break;
				}
				length = ((int) pageHeader[0]) + ((int) pageHeader[1] << 8);
				pageloc = pageHeader[2];

				//d_fprintf("Page header: pageloc=%u length=%u\n",pageloc,length);

				uint16_t memStart = 0xFFFF; //this is meant as a flag to skip loading this pageloc
				switch (pageloc) {
				case 4:
					memStart = 0x8000;
					break;

				case 5:
					memStart = 0xC000;
					break;

				case 8:
					memStart = 0x4000;
					break;

				default:
					//pageloc = 11;
					break;
				}
				if (memStart != 0xFFFF) {
					if (length == 0xFFFF) { // uncompressed raw data
						//d_fprintf("Load uncompressed Block at pageloc %u\n",pageloc );
						load_uncompressed_z80(file, 16384, memStart);
					} else {
						//d_fprintf("Load compressed Block at pageloc %u\n",pageloc );
						load_compressed_z80(file, 16384, memStart);
					}
				} else
					//skip this block
					FileSeek(file, FilePos(file) + length);
			}
		}
	} else {
		setModel(ZX48);
		if (compressed) {
			//d_fprintf("48k compressed z80 loader\n");

			//single 48K block compressed
			load_compressed_z80(file, 49152, 0x4000);
		} else {
			//d_fprintf("48k uncompressed z80 loader\n");
			//d_fprintf("Load uncompressed blocks\n");
			load_uncompressed_z80(file, 16384, 0x4000);
			load_uncompressed_z80(file, 16384, 0x8000);
			load_uncompressed_z80(file, 16384, 0xC000);
		}
	}

	load_cpu(snap);
	free(snap);
	return Z80SNAP_OK; // all right
}

int LoadZ80(char *strSnaFile) {
	int nRet = Z80SNAP_BROKEN;
	sFile snapFile;
	if (DiskAutoMount()) {
		if (FileExist(strSnaFile)) {
			if (FileOpen(&snapFile, strSnaFile)) {
				if (Z80SNAP_OK == (nRet = load_z80_file(&snapFile))) {
					FileClose(&snapFile);
					//invalidate attributes. focrce to redraw
					for (int i = 6144; i < 6144 + 768; i++) {
						cached[i] = membank[4][i] ^ 0xFF;
					}
				}
			}
		}

	}
	return nRet;
}
