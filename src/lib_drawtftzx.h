
// ****************************************************************************
//
//                 Drawing to TFT or VGA display 8-bit 332 buffer
//
// ****************************************************************************
// PicoLibSDK - Alternative SDK library for Raspberry Pico and RP2040
// Copyright (c) 2023 Miroslav Nemecek, Panda38@seznam.cz, hardyplotter2@gmail.com
// 	https://github.com/Panda381/PicoLibSDK
//	https://www.breatharian.eu/hw/picolibsdk/index_en.html
//	https://github.com/pajenicko/picopad
//	https://picopad.eu/en/
// License:
//	This source code is freely available for any purpose, including commercial.
//	It is possible to take and modify the code or parts of it, without restriction.

//Modification of the original library to halve the framebuffer size by using 8-bit RGB332 colors.
//Necessary to free up enough memory for the ZX Spectrum 128K.
//This is not a complete rewrite of the original library, only the functions used in the emulator have been rewritten

#ifndef _LIB_DRAWTFTZX_H
#define _LIB_DRAWTFTZX_H


#ifdef __cplusplus
extern "C" {
#endif

#define RGBTO8(r,g,b)	( ((r)&0xE0) | (((g)&0xE0)>>3) | (((b)&0xC0)>>6) )
//#define RGB8TO16(x)	( (((x)&0xE0)<<8) | (((x)&0x1C)<<6) | (((x)&0x03)<<3) )
#define RGB8TO16(x)	( (((x)&0xE0)<<8) | (((x)&0x1C)<<6) | (((x)&0x03)<<3) )

// 8-bit color definitions
// - base colors
#define COL_BLACK_ZX	RGBTO8(0,0,0)		// 0x0000
#define COL_BLUE_ZX	RGBTO8(0,0,255)	// 0x001F
#define COL_GREEN_ZX	RGBTO8(0,255,0)	// 0x07E0
#define COL_CYAN_ZX	RGBTO8(0,255,255)	// 0x07FF
#define COL_RED_ZX		RGBTO8(255,0,0)	// 0xF800
#define COL_MAGENTA_ZX	RGBTO8(255,0,255)	// 0xF81F
#define COL_YELLOW_ZX	RGBTO8(255,255,0)	// 0xFFE0
#define COL_WHITE_ZX	RGBTO8(255,255,255)	// 0xFFFF
#define COL_GRAY_ZX	RGBTO8(127,127,127)	// 0x7BEF
// - dark colors
#define COL_DKBLUE_ZX	RGBTO8(0,0,127)	// 0x000F
#define COL_DKGREEN_ZX	RGBTO8(0,127,0)	// 0x03E0
#define COL_DKCYAN_ZX	RGBTO8(0,127,127)	// 0x03EF
#define COL_DKRED_ZX	RGBTO8(127,0,0)	// 0x7800
#define COL_DKMAGENTA_ZX	RGBTO8(127,0,127)	// 0x780F
#define COL_DKYELLOW_ZX	RGBTO8(127,127,0)	// 0x7BE0
#define COL_DKWHITE_ZX	RGBTO8(127,127,127)	// 0x7BEF
#define COL_DKGRAY_ZX	RGBTO8(63, 63, 63)	// 0x39E7
// - light colors
#define COL_LTBLUE_ZX	RGBTO8(127,127,255)	// 0x001F
#define COL_LTGREEN_ZX	RGBTO8(127,255,127)	// 0x07E0
#define COL_LTCYAN_ZX	RGBTO8(127,255,255)	// 0x07FF
#define COL_LTRED_ZX	RGBTO8(255,127,127)	// 0xF800
#define COL_LTMAGENTA_ZX	RGBTO8(255,127,255)	// 0xF81F
#define COL_LTYELLOW_ZX	RGBTO8(255,255,127)	// 0xFFE0
#define COL_LTGRAY_ZX	RGBTO8(191,191,191)	// 0xBDF7

#define COL_AZURE_ZX	RGBTO8(0, 127, 255)	// 0x03FF
#define COL_ORANGE_ZX	RGBTO8(255, 127, 0)	// 0xFCE0

#define COL_PRINT_DEF_ZX	COL_GRAY_ZX		// default console print color

extern const u8* pDrawFontZX; // font
extern int DrawFontHeightZx; // font height
extern int DrawFontWidthZx; // font width (5 to 8)
extern u8 DrawPrintPosNumZx; // number of text positions per row (= WIDTH/DrawFontWidth)
extern u8 DrawPrintRowNumZx; // number of text rows (= HEIGHT/DrawFontHeight)
extern u8 DrawPrintPosZx;  // console print character position
extern u8 DrawPrintRowZx;  // console print character row
extern u8 DrawPrintInvZx; // offset added to character (128 = print inverted characters, 0 = normal character)
extern u8 DrawPrintSizeZx; // font size: 0=normal, 1=double-height, 2=double-width, 3=double-size
extern u16 DrawPrintColZx; // console print color

extern u8* FrameBufZx;

void DispWindow(u16 x1, u16 x2, u16 y1, u16 y2);

void DispWriteData(const void* data, int len);

// draw rectangle
void DrawRectZx(int x, int y, int w, int h, u8 col);

// Draw frame
void DrawFrameZx(int x, int y, int w, int h, u8 col);


// clear canvas with black color
void DrawClearZx();

void DrawClearColZx(u8 col);

// Draw point
void DrawPointZx(int x, int y, u8 col);


// Draw text (transparent background)
void DrawTextZx(const char* text, int x, int y, u8 col);
void DrawText2Zx(const char* text, int x, int y, u8 col);

void DrawTextBgZx(const char* text, int x, int y, u8 col, u8 bgcol);

void DispUpdateZx();

#ifdef __cplusplus
}
#endif

#endif // _LIB_DRAWTFT_H


