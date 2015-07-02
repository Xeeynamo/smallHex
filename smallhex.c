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

#include <string.h>
#include "smallhex.h"
#include "system.h"
#include "filedilaog.h"

#define BUFFER_LENGTH (4 * 1024)

#define TITLEBAR_BG_COLOR RGB8(0xC0, 0xC0, 0xC0)
#define FONT_W 8
#define FONT_H 8

#define OFFSET_LENGTH_MIN 5
#define OFFSET_LENGTH_MAX 9
#define BYTES_VERTICALSPACE 3

// Resources
Font fontBar;

// File management
File shFile;
bool shFileReadonly;
char shFilename[1024];
char shFilenameTitle[MAX_PATH];

// Buffer management
unsigned char shBuffer[BUFFER_LENGTH];
int shBufferIndex;
int shBufferShowed;
int shFilePosition;

// Settings variables
bool setShowBar = true;
bool setOffsetShow = false;
bool setOffsetLenght = 7;
bool setBytesPerLine = 16;
bool setBytesGroup = 1;

static const char Hex2Char[] =
{
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
};

void _shDrawHex(Surface* surface, Font font, unsigned int x, unsigned int y, unsigned int value, unsigned int units)
{
	unsigned int sh = 0;
	x += units * 8;
	while (units--)
	{
		unsigned int n = (value >> sh) & 0xF;
		sh += 4;
		DrawChar8(surface, x, y, Hex2Char[n]);
		x -= 8;
	}
}
unsigned int _shHexToChar(unsigned int value)
{
	return value;
}
void _shAssignTitle(int maxlength, const char *title)
{
	int len = strlen(title);
	if (len > maxlength)
	{
		int bonus = maxlength & 2;
		maxlength = (maxlength - 3) / 2;
		memcpy(shFilenameTitle, title, maxlength);
		memcpy(shFilenameTitle + maxlength, "...", 3);
		memcpy(shFilenameTitle + maxlength + 3, title + len - maxlength, maxlength + bonus + 1);
	}
	else
		memcpy(shFilenameTitle, title, len + 1);
}
void _shResetBuffer()
{
	shBufferIndex = 0;
	FileSeek(shFile, shFilePosition, Seek_Begin);
	FileRead(shFile, shBuffer, BUFFER_LENGTH);
}


void shInit()
{
	shFile = FileInvalid;
	FontCreate(&fontBar, Font_Msx, RGB8(0x00, 0x00, 0x00), TITLEBAR_BG_COLOR);
}
void shDestroy()
{
	FontDestroy(fontBar);
}
void shDrawTitleBar(Surface *surface)
{
	FillRectangle(surface, 0, 0, surface->width, 8, TITLEBAR_BG_COLOR);
	DrawString(surface, fontBar, 0, 0, "smallHex");
	if (shFile != FileInvalid)
	{
		if (shFilenameTitle[0] == '\0')
			_shAssignTitle(surface->width / FONT_W - 10, shFilename);
		DrawString(surface, fontBar, 10 * FONT_W, 0, shFilenameTitle);
	}
}
void shDrawBody(Surface *surface)
{
	unsigned char *bufHex = shBuffer;
	unsigned char *bufChar = shBuffer;
	int i, j;
	int x = 1;
	int y = (setShowBar ? 2 : 1) * FONT_H;
	int ycount = (surface->height - y - FONT_H - 1) / (FONT_H + BYTES_VERTICALSPACE);
	int xcount = (surface->width / FONT_W) - (setOffsetShow ? setOffsetLenght + 1 : 0) - 3;
	xcount /= (3 + 1);

	for (j = 0; j < ycount; j++)
	{
		int curX = x;
		int curY = y + j * (FONT_H + BYTES_VERTICALSPACE);
		int group = setBytesGroup;

		if (setOffsetShow)
		{
			_shDrawHex(surface, 0, 0, curY, 0x12345678, setOffsetLenght);
			curX += setOffsetLenght + 1;
		}
		for (i = 0; i < xcount; i++)
		{
			_shDrawHex(surface, 0, curX * 8, curY, *bufHex++, 2);
			if (--group)
				curX += 2;
			else
			{
				group = setBytesGroup;
				curX += 3;
			}
		}
		curX += 1;

		for (i = 0; i < xcount; i++)
		{
			DrawChar8(surface, curX++ * 8, curY, _shHexToChar(*bufChar++));
		}
	}
}


bool shOpenFileDialog(Surface *surface, Font font)
{
	char fn[MAX_PATH];
	FileDialogResult r = FileDialogOpen(surface, font, fn, 0, 0);
	switch (r)
	{
	case FileDialogResult_Ok:
		return shOpenFile(fn);
	case FileDialogResult_Cancel:
		return true;
	case FileDialogResult_Error:
		return false;
	}
	return false;
}
bool shOpenFile(const char *strFilename)
{
	int len;
	shCloseFile();
	shFile = FileOpen(strFilename, FILE_ACCESS_RW);
	if (shFile == FileInvalid)
	{
		shFile = FileOpen(strFilename, FILE_ACCESS_RW);
		if (shFile == FileInvalid)
			return false;
		else
			shFileReadonly = true;
	}
	else
		shFileReadonly = false;

	// rare case when filename path is too long
	len = strlen(strFilename);
	if (len >= sizeof(shFilename))
	{
		FileClose(shFile);
		return false;
	}
	memcpy(shFilename, strFilename, len);
	shFilenameTitle[0] = '\0';
	shFilePosition = 0;
	_shResetBuffer();
	return true;
}
void shFlushFile()
{

}
void shCloseFile()
{
	FileClose(shFile);
	shFile = FileInvalid;
}