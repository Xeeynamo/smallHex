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

#if defined(_DEBUG)
#define FILEDIALOG_USE_NATIVE 0
#else
#define FILEDIALOG_USE_NATIVE 1
#endif

#define TITLEBAR_BG_COLOR RGB8(0xC0, 0xC0, 0xC0)
#define TITLEBAR_FG_COLOR RGB8(0x00, 0x00, 0x00)
#define SELECTION_COLOR RGB8(0xFF, 0x00, 0x00)

#define MESSAGE_TIMER 120
#define MESSAGE_TIMERINFINITE -1
#define MESSAGE_DEFAULT 0
#define MESSAGE_DIRERROR 1
#define MESSAGE_DIREMPTY 2

static const char *FILEDIALOG_SIZEID[] =
{
	" B", "KB", "MB", "GB",
};

void _ReduceString(char *path, int count)
{
	char *pStr1 = path;
	char *pStr2 = path + count;
	while (*pStr2)
		*pStr1++ = *pStr2++;
	*pStr1 = '\0';
}
void _ReduceArrayString(char **v, int count, int length, int toremove)
{
	int i;
	for (i = 0; i < count; i++)
	{
		v[0] = v[toremove];
		*v -= length;
		v++;
	}
}

//! \brief take a path and removes ./ and ../ when possible
//! \param[in,out] path to check and modify
void _CompressPath(char *path)
{
	int i;
	char *pStr = path;
	char *pFolder[64];
	int found = 0;

	while (*pStr && found < sizeof(pFolder) / sizeof(*pFolder) - 1)
	{
		pFolder[found++] = pStr;
		while (*pStr != '\0' && *pStr != '/') pStr++;
		while (*pStr != '\0' && *pStr == '/') pStr++;
	}
	pFolder[found] = pStr;

	i = 1;
	while (i < found)
	{
		if (pFolder[i][0] == '.')
		{
			if (pFolder[i][1] == '/' || pFolder[i][1] == '\0')
			{
				int len = pFolder[i + 1] - pFolder[i];
				_ReduceString(pFolder[i], len);
				_ReduceArrayString(pFolder + i, found - i, len, 1);
				found--;
			}
			else if (pFolder[i][1] == '.' && (pFolder[i][2] == '/' || pFolder[i][2] == '\0'))
			{
				if (strncmp(pFolder[i - 1], "./", 2) == 0 || strncmp(pFolder[i - 1], "../", 3) == 0)
				{
					i++;
					continue;
				}
				int len = pFolder[i + 1] - pFolder[i - 1];
				_ReduceString(pFolder[i - 1], len);
				_ReduceArrayString(pFolder + i, found - i, len, 2);
				found -= 2;
				i--;
			}
			else
				i++;
		}
		else
			i++;
	}
}

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

void _FolderEnter(char *curpath, const char *folder)
{
	int len = strlen(curpath);
	if (curpath[len - 1] != '/')
		curpath[len++] = '/';
	strcpy(curpath + len, folder);
}
void _FolderLeave(char *curpath)
{
	int len = strlen(curpath);
	if (curpath[len - 1] != '/')
		curpath[len++] = '/';
	curpath[len++] = '.';
	curpath[len++] = '.';
	curpath[len++] = '\0';
}

FileDialogResult FileDialogOpen(Surface *surface, Font font, char *filename, const char *extfilter, const char *directory)
{
	const char *MSG[] =
	{
		"Please select a file to open:",
		"Selected path cannot be accessed.",
		"No files or directories found in this folder.",
	};

	int i; // used for cycles
	int fonth = FontGetHeight(font); // do not calculate every time the height of current font
	bool cycle = false; // used to validate 'directory' as a valid folder
	FileDialogResult result = FileDialogResult_Error; // default return value
	int curSelection = 0; // current entry selected
	bool invalidate = true; // demand a new rendering

	char strCurDir[MAX_PATH]; // current directory path
	char tmpCurDir[MAX_PATH]; // temporarely directory path
	Directory curDir = DirectoryInvalid; // current directory pointer
	DirectoryEntry entry[1024]; // entries of current folder
	int entriesCount = 0; // number of entries found

	int entriesPerPage = 1;
	int curPage = 0;
	int curMsg = 0; // current message showed; 0 by default
	int timerMsg = 0; // when a message is prompt, it's displayed only temporarely

	Font titleBarFont = DefaultFont;
	Font selectedFont = DefaultFont;
	FontCreate(&titleBarFont, FontGetType(font), TITLEBAR_FG_COLOR, TITLEBAR_BG_COLOR);
	FontCreate(&selectedFont, FontGetType(font), SELECTION_COLOR, 0);

	strcpy(strCurDir, directory);
	do 
	{
		InputData input;
		GetCurrentBuffer(surface);
		ClearSurface(surface, 0);
		entriesPerPage = surface->height / fonth - 3;

		// Re-open current directory, if needed
		if (curDir == DirectoryInvalid)
		{
			_CompressPath(strCurDir);
			curDir = DirectoryOpen(strCurDir);
			if (curDir == DirectoryInvalid)
			{
				if (cycle == false)
					continue;
				curMsg = MESSAGE_DIRERROR;
				timerMsg = MESSAGE_TIMER;
				strcpy(strCurDir, tmpCurDir);
			}
			else
			{
				curPage = 0;
				curSelection = 0;
				cycle = true;
				strcpy(tmpCurDir, strCurDir);

				for (i = 0; i < sizeof(entry) / sizeof(*entry) && DirectoryNext(curDir, &entry[i]) == Directory_Continue; i++);
				DirectoryClose(curDir);

				entriesCount = i;
				if (entriesCount == 0)
				{
					curMsg = MESSAGE_DIREMPTY;
					timerMsg = MESSAGE_TIMERINFINITE;
				}
				else if (timerMsg < 0)
					curMsg = MESSAGE_DEFAULT;
			}
			invalidate = true;
		}

		// Update message
		if (timerMsg > 0)
		{
			timerMsg--;
			if (timerMsg <= 0)
			{
				curMsg = MESSAGE_DEFAULT;
				invalidate = true;
			}
		}

		// Input management
		InputUpdate(&input);
		int move = 0;
		if (input.repeat.inPs.up)
			move--;
		if (input.repeat.inPs.down)
			move++;
		if (input.repeat.inPs.ltrigger)
			move -= entriesPerPage;
		if (input.repeat.inPs.rtrigger)
			move += entriesPerPage;
		else if (input.ly != 0)
			move += input.ly * 4 / 32767;
		if (move != 0)
		{
			int old = curSelection;
			curSelection += move;

			if (curSelection >= entriesCount)
				curSelection = entriesCount - 1;
			if (curSelection < 1)
				curSelection = 0;
			if (old != curSelection)
			{
				invalidate = true;
				if (curPage > curSelection)
					curPage = curSelection;
				else if (entriesPerPage <= curSelection - curPage)
					curPage = curSelection - entriesPerPage + 1;
			}

		}
		else if (input.repeat.inPs.left)
		{
			_FolderLeave(strCurDir);
			curDir = DirectoryInvalid;
			invalidate = true;
		}
		if (input.repeat.inPs.cross || input.repeat.inPs.right)
		{
			DirectoryEntry *e = &entry[curSelection];
			if (e->length >= 0)
			{
				if (input.repeat.inPs.cross)
				{
					cycle = false;
					sprintf(filename, "%s/%s", strCurDir, e->name);
					result = FileDialogResult_Ok;
				}
			}
			else
			{
				_FolderEnter(strCurDir, e->name);
				curDir = DirectoryInvalid;
				invalidate = true;
			}
		}
		else if (input.repeat.inPs.circle)
		{
			cycle = false;
			result = FileDialogResult_Cancel;
		}

		if (invalidate)
		{
			invalidate = false;
			// Draw title bar
			FillRectangle(surface, 0, 0, surface->width, fonth * 2, TITLEBAR_BG_COLOR);
			DrawString(surface, titleBarFont, 0, 0, MSG[curMsg]);
			DrawString(surface, titleBarFont, 0, fonth, strCurDir);

			// Draw file list
			for (i = 0; i < surface->height / fonth && (curPage + i) < entriesCount; i++)
			{
				DirectoryEntry *e = &entry[curPage + i];
				int y = (i + 3) * fonth;
				Font curFont = (curSelection - curPage) == i ? selectedFont : font;

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
				else
					DrawChar(surface, curFont, 5 * 8, y, 'D');
				DrawString(surface, curFont, 7 * 8, y, e->name);
			}
			// Swap buffer
			GraphicsSwapBuffers();
		}
		GraphicsWaitVSync();
	} while (cycle);

	FontDestroy(titleBarFont);
	FontDestroy(selectedFont);
	return result;
}