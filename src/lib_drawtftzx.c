
// ****************************************************************************
//
//                   Drawing to TFT or VGA display 8-bit 332 buffer
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

#include "osdep.h"	// globals
#include "../../../_display/st7789/st7789.h"

ALIGNED u16 FrameBuf[FRAMESIZE/2];

u8* FrameBufZx=(u8*) FrameBuf;
ALIGNED u16 LineBuf[WIDTH];



// update - send dirty window to display
void DispUpdateZx()
{
	if ((DispDirtyX1 < DispDirtyX2) && (DispDirtyY1 < DispDirtyY2))
	{
		// set draw window
		DispWindow((u16)DispDirtyX1, (u16)DispDirtyX2, (u16)DispDirtyY1, (u16)DispDirtyY2);

		// send data from frame buffer
		u8* s0 = &FrameBufZx[DispDirtyX1 + DispDirtyY1*WIDTH];
		int i;
		for (i = DispDirtyY2 - DispDirtyY1; i > 0; i--)
		{
			for(int nCnt=0;nCnt<DispDirtyX2-DispDirtyX1;nCnt++){
				LineBuf[nCnt]=RGB8TO16(s0[nCnt]);
			}
			DispWriteData(LineBuf, (DispDirtyX2 - DispDirtyX1)*2);
			s0 += WIDTH;
		}

		// set dirty none
		DispDirtyNone();
	}
}



// Draw character (transparent background)
void DrawCharZx(char ch, int x, int y, u8 col)
{
	int x0 = x;
	int i, j;
	u8* d;

	// prepare pointer to font sample
	const u8* s = &pDrawFont[ch];

	// check if drawing is safe to use fast drawing
	if ((x >= 0) && (y >= DispMinY) && (x + DrawFontWidth <= WIDTH) && (y + DrawFontHeight <= DispMaxY))
	{
		// update dirty rectangle
		DispDirtyRect(x, y, DrawFontWidth, DrawFontHeight);

		// destination address
		d = &FrameBufZx[(y-DispMinY)*WIDTH + x];

		// loop through lines of one character
		for (i = DrawFontHeight; i > 0; i--)
		{
			// get one font sample
			ch = *s;
			s += 256;

			// loop through pixels of one character line
			for (j = DrawFontWidth; j > 0; j--)
			{
				// draw pixel
				if ((ch & 0x80) != 0) *d = col;
				d++;
				ch <<= 1;
			}
			d += WIDTH - DrawFontWidth;
		}
	}

	// use slow safe drawing
	else
	{
		// loop through lines of one character
		for (i = DrawFontHeight; i > 0; i--)
		{
			// get one font sample
			ch = *s;
			s += 256;

			// loop through pixels of one character line
			x = x0;
			for (j = DrawFontWidth; j > 0; j--)
			{
				// pixel is set
				if ((ch & 0x80) != 0) DrawPointZx(x, y, col);
				x++;
				ch <<= 1;
			}
			y++;
		}
	}
}


// Draw character with background
void DrawCharBg(char ch, int x, int y, u8 col, u8 bgcol)
{
	int x0 = x;
	int i, j;
	u8 c;
	u8* d;

	// prepare pointer to font sample
	const u8* s = &pDrawFont[ch];

	// check if drawing is safe to use fast drawing
	if ((x >= 0) && (y >= DispMinY) && (x + DrawFontWidth <= WIDTH) && (y + DrawFontHeight <= DispMaxY))
	{
		// update dirty rectangle
		DispDirtyRect(x, y, DrawFontWidth, DrawFontHeight);

		// destination address
		d = &FrameBufZx[(y-DispMinY)*WIDTH + x];

		// loop through lines of one character
		for (i = DrawFontHeight; i > 0; i--)
		{
			// get one font sample
			ch = *s;
			s += 256;

			// loop through pixels of one character line
			for (j = DrawFontWidth; j > 0; j--)
			{
				// draw pixel
				*d++ = ((ch & 0x80) != 0) ? col : bgcol;
				ch <<= 1;
			}
			d += WIDTH - DrawFontWidth;
		}
	}

	// use slow safe drawing
	else
	{
		// loop through lines of one character
		for (i = DrawFontHeight; i > 0; i--)
		{
			// get one font sample
			ch = *s;
			s += 256;

			// loop through pixels of one character line
			x = x0;
			for (j = DrawFontWidth; j > 0; j--)
			{
				// pixel is set
				c = ((ch & 0x80) != 0) ? col : bgcol;

				// draw pixel
				DrawPointZx(x, y, c);
				x++;
				ch <<= 1;
			}
			y++;
		}
	}
}

// Draw text (transparent background)
void DrawTextZx(const char* text, int x, int y, u8 col)
{
	u8 ch;

	// loop through characters of text
	for (;;)
	{
		// get next character of the text
		ch = (u8)*text++;
		if (ch == 0) break;

		// draw character
		DrawCharZx(ch, x, y, col);

		// shift to next character position
		x += DrawFontWidth;
	}
}

// Draw text with background
void DrawTextBgZx(const char* text, int x, int y, u8 col, u8 bgcol)
{
	u8 ch;

	// loop through characters of text
	for (;;)
	{
		// get next character of the text
		ch = (u8)*text++;
		if (ch == 0) break;

		// draw character
		DrawCharBg(ch, x, y, col, bgcol);

		// shift to next character position
		x += DrawFontWidth;
	}
}



// Draw character double sized (transparent background)
void DrawChar2Zx(char ch, int x, int y, u8 col)
{
	int x0 = x;
	int i, j;
	u8* d;
	int w = WIDTH;

	// prepare pointer to font sample
	const u8* s = &pDrawFont[ch];

	// check if drawing is safe to use fast drawing
	if ((x >= 0) && (y >= DispMinY) && (x + 2*DrawFontWidth <= w) && (y + 2*DrawFontHeight <= DispMaxY))
	{
		// update dirty rectangle
		DispDirtyRect(x, y, 2*DrawFontWidth, 2*DrawFontHeight);

		// destination address
		d = &FrameBufZx[(y-DispMinY)*w + x];

		// loop through lines of one character
		for (i = DrawFontHeight; i > 0; i--)
		{
			// get one font sample
			ch = *s;
			s += 256;

			// loop through pixels of one character line
			for (j = DrawFontWidth; j > 0; j--)
			{
				// draw pixel
				if ((ch & 0x80) != 0)
				{
					*d = col;
					d[1] = col;
					d[w] = col;
					d[w+1] = col;
				}
				d += 2;
				ch <<= 1;
			}
			d += 2*w - 2*DrawFontWidth;
		}
	}

	// use slow safe drawing
	else
	{
		// loop through lines of one character
		for (i = DrawFontHeight; i > 0; i--)
		{
			// get one font sample
			ch = *s;
			s += 256;

			// loop through pixels of one character line
			x = x0;
			for (j = DrawFontWidth; j > 0; j--)
			{
				// pixel is set
				if ((ch & 0x80) != 0)
				{
					DrawPointZx(x, y, col);
					DrawPointZx(x+1, y, col);
					DrawPointZx(x, y+1, col);
					DrawPointZx(x+1, y+1, col);
				}
				x += 2;
				ch <<= 1;
			}
			y += 2;
		}
	}
}


// Draw text double sized (transparent background)
void DrawText2Zx(const char* text, int x, int y, u8 col)
{
	u8 ch;

	// loop through characters of text
	for (;;)
	{
		// get next character of the text
		ch = (u8)*text++;
		if (ch == 0) break;

		// draw character
		DrawChar2Zx(ch, x, y, col);

		// shift to next character position
		x += DrawFontWidth*2;
	}
}


// Draw point
void DrawPointZx(int x, int y, u8 col)
{
	// check coordinates
	if (((u32)x >= (u32)WIDTH) || (y < DispMinY) || (y >= DispMaxY)) return;

	// draw pixel
	FrameBufZx[x + (y-DispMinY)*WIDTH] = col;

	// update dirty area by rectangle (must be in valid limits)
	DispDirtyPoint(x, y);
}



// draw rectangle
void DrawRectZx(int x, int y, int w, int h, u8 col)
{
	// limit x
	if (x < 0)
	{
		w += x;
		x = 0;
	}

	// limit w
	if (x + w > WIDTH) w = WIDTH - x;
	if (w <= 0) return;

	// limit y
	if (y < DispMinY)
	{
		h -= DispMinY - y;
		y = DispMinY;
	}

	// limit h
	if (y + h > DispMaxY) h = DispMaxY - y;
	if (h <= 0) return;

	// update dirty rectangle
	DispDirtyRect(x, y, w, h);

	// draw
	u8* d = &FrameBufZx[x + (y-DispMinY)*WIDTH];
	int wb = WIDTH - w;
	int i;
	for (; h > 0; h--)
	{
		for (i = w; i > 0; i--) *d++ = col;
		d += wb;
	}
}

// Draw frame
void DrawFrameZx(int x, int y, int w, int h, u8 col)
{
	if ((w <= 0) || (h <= 0)) return;
	DrawRectZx(x, y, w-1, 1, col);
	DrawRectZx(x+w-1, y, 1, h-1, col);
	DrawRectZx(x+1, y+h-1, w-1, 1, col);
	DrawRectZx(x, y+1, 1, h-1, col);
}

// clear canvas with color
void DrawClearColZx(u8 col)
{
	DrawRectZx(0, 0, WIDTH, HEIGHT, col);
}

// clear canvas with black color
void DrawClearZx()
{
	DrawClearColZx(COL_BLACK_ZX);
}

