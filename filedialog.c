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
#include <string.h>
#include "filedilaog.h"
#include "system.h"
#include "graphics.h"
#include "Input.h"

#if defined(_WIN32)
#include <windows.h>
#elif defined(PLATFORM_PSP2)
#endif

#define FILEDIALOG_USE_NATIVE 0

static const char *FILEDIALOG_SIZEID[] =
{
	" B", "KB", "MB", "GB",
};

FileDialogResult _NativeFileDialogOpen(char *filename, const char *extfilter, const char *directory)
{
#if defined(_WIN32)
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GetConsoleWindow();
	ofn.lpstrFile = filename;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = "All\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn) == TRUE)
		return FileDialogResult_Ok;
	DWORD err = CommDlgExtendedError();
	if (err == 0)
		return FileDialogResult_Cancel;
#endif
	return FileDialogResult_Error;
}

#define TITLEBAR_BG_COLOR RGB8(0xC0, 0xC0, 0xC0)
#define TITLEBAR_FG_COLOR RGB8(0x00, 0x00, 0x00)
#define SELECTION_COLOR RGB8(0xFF, 0x00, 0x00)

FileDialogResult FileDialogOpen(Surface *surface, Font font, char *filename, const char *extfilter, const char *directory)
{
	int i;
	int fonth = FontGetHeight(font);
	bool cycle = true;
	Directory curDir = DirectoryInvalid;
	FileDialogResult result = FileDialogResult_Error;
	DirectoryEntry entry[1024];
	int entriesCount = 0;
	char strCurDir[MAX_PATH];
	Font titleBarFont = DefaultFont;
	Font selectedFont = DefaultFont;
	int curSelection = 1;

	FontCreate(&titleBarFont, FontGetType(font), TITLEBAR_FG_COLOR, TITLEBAR_BG_COLOR);
	FontCreate(&selectedFont, FontGetType(font), SELECTION_COLOR, 0);
	strcpy(strCurDir, directory);
	do 
	{
		InputData input;
		GetCurrentBuffer(surface);
		ClearSurface(surface, 0);

		// Re-open current directory, if needed
		if (curDir == DirectoryInvalid)
		{
			entriesCount = 0;
			curDir = DirectoryOpen(strCurDir);
			if (curDir == DirectoryInvalid)
				break;
			for (i = 0; i < sizeof(entry) / sizeof(*entry) && DirectoryNext(curDir, &entry[i]) == Directory_Continue; i++);
			entriesCount = i;
			DirectoryClose(curDir);
		}

		// Draw title bar
		FillRectangle(surface, 0, 0, surface->width, fonth * 2, TITLEBAR_BG_COLOR);
		DrawString(surface, titleBarFont, 0, 0, "Please select a file to open:");
		DrawString(surface, titleBarFont, 0, fonth, strCurDir);

		// Draw file list
		for (i = 1; i < surface->height / fonth && i < entriesCount; i++)
		{
			DirectoryEntry *e = &entry[i];
			int y = (i + 2) * fonth;
			Font curFont = curSelection == i ? selectedFont : font;

			if (e->length > 0)
			{
				char buf[10];
				int size = e->length;
				int sizeId;
				for (sizeId = 0; size >= 1000; size /= 1024, sizeId++);
				if (size == 0)
					size = 1;
				sprintf(buf, "%3i%s", size, FILEDIALOG_SIZEID[sizeId]);
				DrawString(surface, curFont, 1 * 8, y, buf);
			}
			DrawString(surface, curFont, 7 * 8, y, entry[i].name);
		}

		// Input management
		InputUpdate(&input);
		if (input.repeat.inPs.up)
		{
			if (--curSelection < 1)
				curSelection = 1;
		}
		else if (input.repeat.inPs.down)
		{
			if (++curSelection >= entriesCount)
				curSelection = entriesCount;
		}
		if (input.repeat.inPs.cross)
		{
			cycle = false;
			sprintf(filename, "%s/%s", strCurDir, entry[curSelection].name);
			return FileDialogResult_Ok;

		}
		else if (input.repeat.inPs.circle)
		{
			cycle = false;
			return FileDialogResult_Cancel;
		}

		// Swap buffer
		GraphicsSwapBuffers(true);
	} while (cycle);

	FontDestroy(titleBarFont);
	FontDestroy(selectedFont);
	return result;
}