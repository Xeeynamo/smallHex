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

#ifndef _DRAW_H
#define _DRAW_H

#include <stdbool.h>

#if defined(_WIN32)
#define RGBA8(r, g, b, a) ((((a)&0xFF)<<24) | (((r)&0xFF)<<16) | (((g)&0xFF)<<8) | (((b)&0xFF)<<0))
#elif defined(PLATFORM_PSP2)
#define RGBA8(r, g, b, a) ((((a)&0xFF)<<24) | (((b)&0xFF)<<16) | (((g)&0xFF)<<8) | (((r)&0xFF)<<0))
#endif
#define RGB8(r, g, b) (RGBA8(r, g, b, 0xFF))

typedef unsigned int Color32;
typedef void *Font;

extern Font DefaultFont;

// surface where every pixel is 32-bit length
typedef struct
{
	int width;
	int height;
	int pitch;
	unsigned int *data;
} Surface;

typedef enum
{
	//! \brief 8x8 font
	Font_Msx,
} FontType;

int DrawInit();
void DrawDestroy();

//! \brief draw a rectangle with the content filled
//! \param[in] surface destination
//! \param[in] x coordinate in pixel where the rectangle will be drawn
//! \param[in] y coordinate in pixel where the rectangle will be drawn
//! \param[in] width size in pixel of how horizontally big is the rectangle to draw
//! \param[in] width size in pixel of how vertically big is the rectangle to draw
//! \param[in] color of rectangle to draw
void FillRectangle(Surface *surface, unsigned int x, unsigned int y, unsigned int width, unsigned int height, Color32 color);

//! \brief create a font
//! \param[out] font created
//! \param[in] type of font to create
//! \param[in] foreColor color of font; cannot be changed after font creation
//! \param[in] backColor background of font; cannot be changed after font creation
//! \return false if font is not created correctly; in that case, font var will be left untouched
bool FontCreate(Font *font, FontType type, Color32 foreColor, Color32 backColor);

//! \brief destroy a font previously created
void FontDestroy(Font font);

//! \brief faster way to draw a character; background black, foreground white
//! \param[in] surface destination
//! \param[in] x coordinate in pixel where the character will be drawn
//! \param[in] y coordinate in pixel where the character will be drawn
//! \param[in] ch character to write in 8-bit ASCII format
void DrawChar8(Surface *surface, unsigned int x, unsigned int y, unsigned int ch);

//! \brief fast way to draw a character; can select a foreground color
//! \param[in] surface destination
//! \param[in] x coordinate in pixel where the character will be drawn
//! \param[in] y coordinate in pixel where the character will be drawn
//! \param[in] ch character to write in 8-bit ASCII format
void DrawCharColored8(Surface *surface, unsigned int x, unsigned int y, unsigned int color, unsigned int ch);

//! \brief fast way to draw a character; must specify a font data
//! \param[in] surface destination
//! \param[in] x coordinate in pixel where the character will be drawn
//! \param[in] y coordinate in pixel where the character will be drawn
//! \param[in] ch character to write in 8-bit ASCII format
void DrawChar(Surface *surface, Font font, unsigned int x, unsigned int y, unsigned int ch);

//! \brief faster way to draw a string; background black, foreground white
//! \param[in] surface destination
//! \param[in] x coordinate in pixel where the string will be drawn
//! \param[in] y coordinate in pixel where the string will be drawn
//! \param[in] str string 8-bit ASCII format; must terminate with '\0'
void DrawString8(Surface *surface, unsigned int x, unsigned int y, char *str);

//! \brief fast way to draw a character; can select a foreground color
//! \param[in] surface destination
//! \param[in] x coordinate in pixel where the string will be drawn
//! \param[in] y coordinate in pixel where the string will be drawn
//! \param[in] str string 8-bit ASCII format; must terminate with '\0'
void DrawStringColored8(Surface *surface, unsigned int x, unsigned int y, unsigned int color, char *str);

//! \brief fast way to draw a character; must specify a font data
//! \param[in] surface destination
//! \param[in] x coordinate in pixel where the string will be drawn
//! \param[in] y coordinate in pixel where the string will be drawn
//! \param[in] str string 8-bit ASCII format; must terminate with '\0'
void DrawString(Surface *surface, Font font, unsigned int x, unsigned int y, char *str);

//! \brief clear an entire surface
//! \param[in] surface to clear
//! \param[in] color of clean-up. Black and white are faster
void ClearSurface(Surface *surface, Color32 color);

#endif