/*
smallHex - hexadecimal editor for Windows and PS Vita
Copyright (C) 2015  Luciano Ciccariello (Xeeynamo)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include "system.h"
#include "graphics.h"
#include "draw.h"
#include "font8.h"

#define NCHARS_ROW 16
#define NCHARS_COL 16
#define CHAR_SIZE 8
#define CHAR_SIZE2x2 (CHAR_SIZE * CHAR_SIZE)

typedef struct
{
	FontType type;
	unsigned int width;
	unsigned int height;
	unsigned int startch;
	unsigned int count;
	unsigned int foreColor;
	unsigned int backColor;
	unsigned int size;
	unsigned int *data;
} _FontStructure;

Font DefaultFont = NULL;
unsigned int font8_un32[NCHARS_ROW * NCHARS_COL * CHAR_SIZE2x2];
_FontStructure DefaultFontStruture = { Font_Msx, 8, 8, 0, 0x100, RGB8(0xFF, 0xFF, 0xFF), RGB8(0x00, 0x00, 0x00), 8 * 8 * sizeof(int), font8_un32 };

void _UnpackFont8_1bpp(unsigned int *fontDst, const unsigned char *fontSrc, unsigned int foreColor, unsigned int backColor)
{
	// unpack font8 to be faster on drawing
	int i, j;
	for (j = 0; j < CHAR_SIZE * NCHARS_COL * NCHARS_ROW; j++)
	{
		unsigned int data = *fontSrc++;
		for (i = 0; i < 8; i++)
			*fontDst++ = data & 1 << (7 - i) ? foreColor : backColor;
	}
}
int DrawInit()
{
	if (CALL_FAILED(GraphicsInit()))
		return -1;
	// unpack font
	_UnpackFont8_1bpp(font8_un32, fontmsx, RGB8(0xFF, 0xFF, 0xFF), RGB8(0x00, 0x00, 0x00));
	return 0;
}
void DrawDestroy()
{
	GraphicsDestroy();
}

//////////////////////////////////////////////////////////////////////////
// SHAPES DRAWING STUFF
void DrawRectangle(Surface *surface, int x, int y, int width, int height, Color32 color)
{
	unsigned int *pDstMain = (unsigned int*)surface->data + x + y * surface->width;

	if (x < 0 || y < 0)
		return;
	if (x + width > surface->width)
		width = surface->width - x;
	if (y + height > surface->height)
		height = surface->height - y;
	if (width < 0 || height < 0)
		return;
	
	unsigned int w = width;
	unsigned int *pDst = pDstMain;
	while (w--)
		*pDst++ = color;
	pDstMain += surface->width;
	height--;
	while (--height)
	{
		pDstMain[0] = color;
		pDstMain[width - 1] = color;
		pDstMain += surface->width;
	}
	w = width;
	pDst = pDstMain;
	while (w--)
		*pDst++ = color;
}
void FillRectangle(Surface *surface, int x, int y, int width, int height, Color32 color)
{
	unsigned int *pDstMain = (unsigned int*)surface->data + x + y * surface->width;

	if (x < 0 || y < 0)
		return;
	if (x + width > surface->width)
		width = surface->width - x;
	if (y + height > surface->height)
		height = surface->height - y;
	if (width < 0 || height < 0)
		return;

	while (height--)
	{
		unsigned int w = width;
		unsigned int *pDst = pDstMain;
		pDstMain += surface->width;
		while (w--)
			*pDst++ = color;
	}
}

//////////////////////////////////////////////////////////////////////////
// FONT STUFF
bool FontCreate(Font *font, FontType type, Color32 foreColor, Color32 backColor)
{
	_FontStructure *pFont = (_FontStructure*)MemoryAlloc(sizeof(_FontStructure));
	const unsigned char *fontData;
	switch (type)
	{
	case Font_Msx:
		pFont->width = 8;
		pFont->height = 8;
		pFont->startch = 0;
		pFont->count = 0x100;
		fontData = fontmsx;
		break;
	default:
		MemoryFree(pFont);
		return false;
	}
	pFont->foreColor = foreColor;
	pFont->backColor = backColor;
	pFont->size = pFont->width * pFont->height;
	pFont->data = (unsigned int*)MemoryAlloc(pFont->size * pFont->count * sizeof(unsigned int));
	_UnpackFont8_1bpp(pFont->data, fontData, foreColor, backColor);
	*font = pFont;
	return true;
}
void FontDestroy(Font font)
{
	if (font != DefaultFont)
	{
		MemoryFree(((_FontStructure*)font)->data);
		MemoryFree(font);
	}
}
int FontGetMaxWidth(Font font)
{
	_FontStructure *f;
	if (font == DefaultFont)
		f = &DefaultFontStruture;
	else
		f = (_FontStructure*)font;
	return f->width;
}
int FontGetWidth(Font font, unsigned int ch)
{
	return FontGetMaxWidth(font);
}
int FontGetHeight(Font font)
{
	_FontStructure *f;
	if (font == DefaultFont)
		f = &DefaultFontStruture;
	else
		f = (_FontStructure*)font;
	return f->height;
}
FontType FontGetType(Font font)
{
	_FontStructure *f;
	if (font == DefaultFont)
		f = &DefaultFontStruture;
	else
		f = (_FontStructure*)font;
	return f->type;
}

//////////////////////////////////////////////////////////////////////////
// CHARACTER DRAWING STUFF

void DrawChar8(Surface *surface, int x, int y, unsigned int ch)
{
	if (x < 0 || x > surface->width - CHAR_SIZE ||
		y < 0 || y > surface->height - CHAR_SIZE)
		return;
	if (ch < NCHARS_ROW * NCHARS_COL)
	{
		int i, j;
		unsigned int *pSrc = font8_un32 + ch * CHAR_SIZE2x2;
		unsigned int *pDst = (unsigned int*)surface->data + x + y * surface->width;

		for (j = 0; j < CHAR_SIZE; j++)
		{
			for (i = 0; i < CHAR_SIZE; i++)
				pDst[i] = *pSrc++;
			pDst += surface->width;
		}
	}
}
void DrawCharColored8(Surface *surface, int x, int y, unsigned int color, unsigned int ch)
{
	if (x < 0 || x > surface->width - CHAR_SIZE ||
		y < 0 || y > surface->height - CHAR_SIZE)
		return;
	if (ch < NCHARS_ROW * NCHARS_COL)
	{
		int i, j;
		unsigned int *pSrc = font8_un32 + ch * CHAR_SIZE2x2;
		unsigned int *pDst = (unsigned int*)surface->data + x + y * surface->width;

		for (j = 0; j < CHAR_SIZE; j++)
		{
			for (i = 0; i < CHAR_SIZE; i++)
				pDst[i] = *pSrc++ & color;
			pDst += surface->width;
		}
	}
}
void DrawChar(Surface *surface, Font font, int x, int y, unsigned int ch)
{
	if (font != DefaultFont)
	{
		_FontStructure *pFont = (_FontStructure*)font;
		if (x < 0 || x > surface->width - (int)pFont->width ||
			y < 0 || y > surface->height - (int)pFont->height)
			return;

		ch -= pFont->startch;
		if (ch >= 0 || ch < pFont->count)
		{
			unsigned int i, j;
			unsigned int *pSrc = pFont->data + pFont->size * ch;
			unsigned int *pDst = (unsigned int*)surface->data + x + y * surface->width;

			unsigned int w = pFont->width; // put width on stack to improve access speed
			for (j = 0; j < pFont->height; j++)
			{
				for (i = 0; i < w; i++)
					pDst[i] = *pSrc++;
				pDst += surface->width;
			}
		}
	}
	else
		DrawChar8(surface, x, y, ch);
}
void DrawString8(Surface *surface, int x, int y, const char *str)
{
	int ch;
	int cx = x;
	while ((ch = *str++) != '\0')
	{
		// avoiding useless switch statement if we are sure that it's a char
		if (ch < 0x20)
		{
			switch (ch)
			{
			case '\n':
				cx = x;
				y += CHAR_SIZE;
				continue;
			default:
				ch = '.'; // default character if it's not recognized correctly
				break;
			}
		}
		DrawChar8(surface, cx, y, ch);
		cx += CHAR_SIZE;
	}
}
void DrawStringColored8(Surface *surface, int x, int y, unsigned int color, const char *str)
{
	if (color != 0xFFFFFFFF)
	{
		int ch;
		int cx = x;
		while ((ch = *str++) != '\0')
		{
			// avoiding useless switch statement if we are sure that it's a char
			if (ch < 0x20)
			{
				switch (ch)
				{
				case '\n':
					cx = x;
					y += CHAR_SIZE;
					continue;
				default:
					ch = '.'; // default character if it's not recognized correctly
					break;
				}
			}
			DrawCharColored8(surface, cx, y, color, ch);
			cx += CHAR_SIZE;
		}
	}
	else
		DrawString8(surface, x, y, str);
}
void DrawString(Surface *surface, Font font, int x, int y, const char *str)
{
	if (font != DefaultFont)
	{
		int ch;
		int cx = x;
		while ((ch = *str++) != '\0')
		{
			// avoiding useless switch statement if we are sure that it's a char
			if (ch < 0x20)
			{
				switch (ch)
				{
				case '\n':
					cx = x;
					y += CHAR_SIZE;
					continue;
				default:
					ch = '.'; // default character if it's not recognized correctly
					break;
				}
			}
			DrawChar(surface, font, cx, y, ch);
			cx += CHAR_SIZE;
		}
	}
	else
		DrawString8(surface, x, y, str);
}
void ClearSurface(Surface *surface, Color32 color)
{
	unsigned int count;
	unsigned int *pDst;

	// clear color cannot be transparent
	color |= 0xFF000000;
	switch (color)
	{
	case RGB8(0xFF, 0xFF, 0xFF):
		memset(surface->data, 0xFF, surface->pitch * surface->height * sizeof(int));
		break;
	case RGB8(0x00, 0x00, 0x00):
		memset(surface->data, 0x00, surface->pitch * surface->height * sizeof(int));
		break;
	default:
		pDst = surface->data;
		for (count = surface->pitch * surface->height; count; count--)
			*pDst++ = color;
		break;
	}
}
