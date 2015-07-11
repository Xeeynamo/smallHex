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
#include "input.h"
#include "filedilaog.h"
#include "menu.h"

#if defined(_WIN32)
#define INITIAL_DIRECTORY "."
#elif defined(PLATFORM_PSP2)
#define INITIAL_DIRECTORY "pss0:/top/Documents/"
#endif

#define BUFFER_LENGTH 32768

#define TITLEBAR_BG_COLOR RGB8(0xC0, 0xC0, 0xC0)
#define HEXCURSOR_FORE RGB8(0xFF, 0x00, 0x00)
#define HEXCURSOR_BACK RGB8(0x10, 0x30, 0x30)
#define FONT_W 8
#define FONT_H 8

#define OFFSET_LENGTH_MIN 5
#define OFFSET_LENGTH_MAX 9
#define BYTES_VERTICALSPACE 3

// Resources
Font fontBar;
Font fontDefault;
Font fontHexSelected;

// File management
File shFile;
bool shFileReadonly;
long long shFileLength;
char shFilename[1024];
char shFilenameTitle[MAX_PATH];

// Buffer management
unsigned char shBuffer[BUFFER_LENGTH];
int shBufferIndex;
int shBufferShowed;
int shFilePosition;
bool shDrawInvalidate;

// Settings variables
bool shSetInvalidate;
bool setShowBar = true;
bool shSetOffsetShow = true;
int shSetOffsetLenght = 7;
int shSetBytesPerLine = 16;
int shSetBytesGroup = 2;

// Menu settings
Menu shCurrentMenu = 0;
Menu shMenuVisualSettings;
Menu shMenuFontSettings;
int shMenuSetOffset = 0;
int shMenuSetBytesGroup = 0;
int shMenuSetFontBgR = 0;
int shMenuSetFontBgG = 0;
int shMenuSetFontBgB = 0;

// Hex editor itself
int shBytesPerLine = 0;
int shLinesPerPage;
int shCursorPos;
int shPagePos;
int shPosxOffset;
int shPosxHex;
int shPosxChar;

static const char Hex2Char[] =
{
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
};

//////////////////////////////////////////////////////////////////////////
// Menu entries

int _shCreateFont(Color32 fgColor, Color32 bgColor)
{
	FontDestroy(fontDefault);
	FontCreate(&fontDefault, Font_Msx, fgColor, bgColor);
	shDrawInvalidate = true;
}
int _shMenuFontSettingsBg(int value)
{
	_shCreateFont(RGB8(0xFF, 0xFF, 0xFF),
		RGB8(shMenuSetFontBgR, shMenuSetFontBgG, shMenuSetFontBgB));
}
int _shMenuVisualSettingsOffset(int value)
{
	shSetInvalidate = true;
	shSetOffsetShow = true;
	switch (shMenuSetOffset)
	{
	case 0:
		shSetOffsetShow = false;
		break;
	case 1:
		shSetOffsetLenght = 6;
		break;
	case 2:
		shSetOffsetLenght = 7;
		break;
	case 3:
		shSetOffsetLenght = 8;
		break;
	case 4:
		shSetOffsetLenght = 9;
		break;
	case 5:
		shSetOffsetLenght = 10;
		break;
	}
	return 1;
}
int _shMenuVisualSettingsBytesgroup(int value)
{
	shSetInvalidate = true;
	switch (shMenuSetBytesGroup)
	{
	case 0:
		shSetBytesGroup = 1;
		break;
	case 1:
		shSetBytesGroup = 2;
		break;
	case 2:
		shSetBytesGroup = 4;
		break;
	case 3:
		shSetBytesGroup = 8;
		break;
	}
	return 1;
}

static const MenuEntry FontSettingsEntries[] =
{
	{ "Background RED", MenuType_ValueRange, { &shMenuSetFontBgR, _shMenuFontSettingsBg, 256, 0 } },
	{ "Background GREEN", MenuType_ValueRange, { &shMenuSetFontBgG, _shMenuFontSettingsBg, 256, 0 } },
	{ "Background BLUE", MenuType_ValueRange, { &shMenuSetFontBgB, _shMenuFontSettingsBg, 256, 0 } },
};
static const MenuItem FontSettings = { "Font settings", NULL, sizeof(FontSettingsEntries) / sizeof(MenuEntry), FontSettingsEntries };

static const char *VisualSettingsEntries_OffsetValues[] = { "Hide", "6", "7", "8", "9", "10" };
static const char *VisualSettingsEntries_GroupBytesValues[] = { "1", "2", "4", "8" };
static const MenuEntry VisualSettingsEntries[] =
{
	{ "Offset", MenuType_ValueStringSet, { &shMenuSetOffset, _shMenuVisualSettingsOffset,
	lengthof(VisualSettingsEntries_OffsetValues), VisualSettingsEntries_OffsetValues } },
	{ "Group bytes", MenuType_ValueStringSet, { &shMenuSetBytesGroup, _shMenuVisualSettingsBytesgroup,
	lengthof(VisualSettingsEntries_GroupBytesValues), VisualSettingsEntries_GroupBytesValues } },
	//{ "Font settings", MenuType_Submenu, VisualSettingsFont },
};
static const MenuItem VisualSettings = { "Visual settings", NULL, sizeof(VisualSettingsEntries) / sizeof(MenuEntry), VisualSettingsEntries };

//////////////////////////////////////////////////////////////////////////


void _shDrawHex(Surface* surface, Font font, unsigned int x, unsigned int y, unsigned int value, unsigned int units)
{
	unsigned int sh = 0;
	x += (units - 1) * FONT_W;
	while (units--)
	{
		unsigned int n = (value >> sh) & 0xF;
		sh += 4;
		DrawChar(surface, font, x, y, Hex2Char[n]);
		x -= FONT_W;
	}
}
void _shDrawLHex(Surface* surface, Font font, unsigned int x, unsigned int y, unsigned long long value, unsigned int units)
{
	unsigned int sh = 0;
	x += (units - 1) * FONT_W;
	while (units--)
	{
		unsigned int n = (value >> sh) & 0xF;
		sh += 4;
		DrawChar(surface, font, x, y, Hex2Char[n]);
		x -= FONT_W;
	}
}
void _shDrawCursor(Surface *surface, int pos, int xcount, int ycount)
{
	int xx = pos % xcount;
	int y = (pos / xcount - shPagePos / xcount) * (FONT_H + BYTES_VERTICALSPACE) + (setShowBar ? 2 : 1) * FONT_H;
	int group = shSetBytesGroup;
	int x = shPosxHex;
	while (xx--)
		if (--group)
			x += 2;
		else
		{
			group = shSetBytesGroup;
			x += 3;
		}
	x *= FONT_W;

	_shDrawHex(surface, fontHexSelected, x, y, shBuffer[pos], 2);
	FillRectangle(surface, x, y + FONT_H, FONT_W * 2, 1, HEXCURSOR_FORE);

	x = (shPosxChar + (pos % xcount)) * FONT_W;
	DrawChar(surface, fontHexSelected, x, y, shBuffer[pos]);
	FillRectangle(surface, x, y + FONT_H, FONT_W, 1, HEXCURSOR_FORE);
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
unsigned char *_shRecalculateBuffer(int position, int length)
{
	int startPos = position - shBufferIndex;
	if (startPos < 0)
	{
		shBufferIndex = shBufferIndex + startPos + length - BUFFER_LENGTH;
		if (shBufferIndex < 0)
			shBufferIndex = 0;
		FileSeek(shFile, shBufferIndex, Seek_Begin);
		FileRead(shFile, shBuffer, BUFFER_LENGTH);
		return shBuffer;
	}
	else if (startPos + length > BUFFER_LENGTH)
	{
		shBufferIndex = position;
		FileSeek(shFile, shBufferIndex, Seek_Begin);
		FileRead(shFile, shBuffer, BUFFER_LENGTH);
		return shBuffer;
	}
	else
		return shBuffer + startPos;
}

void _shResizeWindow(Surface *surface)
{
	int y = (setShowBar ? 2 : 1) * FONT_H;

	shPosxOffset = 0;
	shPosxHex = shSetOffsetShow ? shSetOffsetLenght + 2 : 1;
	shLinesPerPage = (surface->height - y - FONT_H - 1) / (FONT_H + BYTES_VERTICALSPACE);
	shBytesPerLine = (surface->width / FONT_W) - shPosxHex - (FONT_H + 2);
	shBytesPerLine = shBytesPerLine * shSetBytesGroup / (2 * (shSetBytesGroup + 1));
	shPosxChar = shPosxHex + 2 * shBytesPerLine + (shBytesPerLine / shSetBytesGroup);
}

void _shSetBytesGroup(int group)
{
	shSetBytesGroup = group;
	shSetInvalidate = true;
}

void shInit()
{
	shFile = FileInvalid;
	shSetInvalidate = true;
	shDrawInvalidate = true;
	fontDefault = DefaultFont;
	fontBar = DefaultFont;
	fontHexSelected = DefaultFont;
	FontCreate(&fontBar, Font_Msx, RGB8(0x00, 0x00, 0x00), TITLEBAR_BG_COLOR);
	FontCreate(&fontHexSelected, Font_Msx, HEXCURSOR_FORE, HEXCURSOR_BACK);
	shMenuVisualSettings = MenuCreate(&VisualSettings, DefaultFont, 300);
	shMenuFontSettings = MenuCreate(&FontSettings, DefaultFont, 300);
	InputInit();

	// default settings
	_shCreateFont(RGB8(0xFF, 0xFF, 0xFF), RGB8(0x00, 0x00, 0x00));
	_shMenuVisualSettingsOffset(shMenuSetOffset = 2);
	_shMenuVisualSettingsOffset(shMenuSetBytesGroup = 1);
}
void shDestroy()
{
	MenuDestroy(shMenuVisualSettings);
	FontDestroy(fontDefault);
	FontDestroy(fontHexSelected);
	FontDestroy(fontBar);
	InputDestroy();
}
SmallHexState shProcess()
{
	Surface surface;
	GetCurrentBuffer(&surface);
	ClearSurface(&surface, RGB8(shMenuSetFontBgR, shMenuSetFontBgG, shMenuSetFontBgB));

	InputData input;
	InputUpdate(&input);
	if (input.repeat.inPs.select)
	{
		if (input.repeat.inPs.start)
			return SmallHexState_Exit;
		else
			shOpenFileDialog(&surface, DefaultFont);
	}
	else
		shInputControl(&input);

	if (shDrawInvalidate)
	{
		shDrawInvalidate = false;
		if (setShowBar)
			shDrawTitleBar(&surface);
		shDrawBody(&surface);
		GraphicsSwapBuffers(true);
	}
	GraphicsWaitVSync();
	return SmallHexState_Loop;
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
	int i, j;
	int y = (setShowBar ? 2 : 1) * FONT_H;
	if (shSetInvalidate)
	{
		_shResizeWindow(surface);
		shSetInvalidate = false;
	}

	// Save these information on stack to speed-up for cycles
	int remainsBytes = shFileLength - shPagePos;
	int xcount = shBytesPerLine;
	int ycount = shLinesPerPage;
	unsigned char *bufHex = _shRecalculateBuffer(shPagePos, xcount * ycount);
	unsigned char *bufChar = bufHex;
	for (j = 0; j < ycount && remainsBytes >= 0; j++)
	{
		int curX;
		int curY = y + j * (FONT_H + BYTES_VERTICALSPACE);
		int group = shSetBytesGroup;

		if (shSetOffsetShow)
			_shDrawLHex(surface, fontDefault, shPosxOffset, curY, shPagePos + j * shBytesPerLine, shSetOffsetLenght);

		curX = shPosxHex;
		for (i = 0; i < xcount && remainsBytes > 0; i++, remainsBytes--)
		{
			_shDrawHex(surface, fontDefault, curX * FONT_W, curY, *bufHex++, 2);
			if (--group)
				curX += 2;
			else
			{
				group = shSetBytesGroup;
				curX += 3;
			}
		}

		// definitely not a fashion way, but at least it works...
		if (remainsBytes <= 0)
			remainsBytes = i;
		else
			remainsBytes += xcount;
		// Draw characters
		curX = shPosxChar;
		for (i = 0; i < xcount && remainsBytes > 0; i++, remainsBytes--)
			DrawChar(surface, fontDefault, curX++ * FONT_W, curY, *bufChar++);
	}
	_shDrawCursor(surface, shCursorPos, xcount, ycount);
	if (shCurrentMenu != 0)
		MenuDraw(shCurrentMenu, surface, MENU_DEFAULT_POSITION, MENU_DEFAULT_POSITION);
}


bool shOpenFileDialog(Surface *surface, Font font)
{
	char fn[MAX_PATH];
	FileDialogResult r = FileDialogOpen(surface, font, fn, 0, INITIAL_DIRECTORY);
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
	shCursorPos = 0;
	shPagePos = 0;
	shFileLength = FileSeek(shFile, 0, Seek_End);
	FileSeek(shFile, 0, Seek_Begin),
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
void shInputControl(InputData *input)
{
	if (shCurrentMenu == 0)
	{
		int move = 0;
		if (input->repeat.inPs.left)
			move -= 1;
		else if (input->repeat.inPs.right)
			move += 1;
		if (input->repeat.inPs.up)
			move -= shBytesPerLine;
		else if (input->repeat.inPs.down)
			move += shBytesPerLine;
		else if (input->repeat.inPc.pgup)
			move -= shBytesPerLine * shLinesPerPage;
		else if (input->repeat.inPc.pgdown)
			move += shBytesPerLine * shLinesPerPage;
		else if (input->lx != 0)
			move += input->lx * shBytesPerLine / 2 / 32767;
		else if (input->ly != 0)
			move += input->ly * shBytesPerLine * shLinesPerPage / 32767;
		else if (input->repeat.inPs.start)
		{
			shCurrentMenu = shMenuVisualSettings;
			shDrawInvalidate = true;
		}

		if (move != 0)
		{
			move = shCursorPos + move;
			if (move >= shFileLength)
				move = shFileLength - 1;
			if (move < 0) // not else if, just in case the file length is 0
				move = 0;
			if (shCursorPos != move)
			{
				shDrawInvalidate = true;
				shCursorPos = move;
				if (shCursorPos >= shPagePos + shBytesPerLine * shLinesPerPage)
				{
					while (shCursorPos >= shPagePos + shBytesPerLine * shLinesPerPage)
						shPagePos += shBytesPerLine;
				}
				else if (shCursorPos < shPagePos)
				{
					while (shCursorPos < shPagePos)
						shPagePos -= shBytesPerLine;
					if (shPagePos < 0)
						shPagePos = 0;
				}
			}
		}
	}
	else
	{
		int r = MenuProcess(shCurrentMenu, input);
		if (r != 0)
		{
			if (r < 0)
				shCurrentMenu = 0;
			shDrawInvalidate = true;
		}
	}
}