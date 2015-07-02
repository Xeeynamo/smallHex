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
#include "defines.h"
#include "graphics.h"
#include "input.h"

#include "smallhex.h"

#define APP_TITLE "smallHex"

#if defined(PLATFORM_PSP2)
PSP2_MODULE_INFO(0, 1, APP_TITLE)
#endif

int main()
{
	if (CALL_FAILED(DrawInit()))
		return -1;
	else
	{
		Surface surface;
		InputData input;
		GraphicsSetTitle(APP_TITLE);
		InputInit();
		shInit();
#if defined(_DEBUG) && defined(_WIN32)
		shOpenFile("D:\\SONIC1.BIN");
#else
		shOpenFileDialog(surface, DefaultFont);
#endif
		while (1)
		{
			GetCurrentBuffer(&surface);
			ClearSurface(&surface, 0);
			InputUpdate(&input);
			shDrawTitleBar(&surface);
			shDrawBody(&surface);
			GraphicsSwapBuffers(true);
		}
		shDestroy();
		DrawDestroy();
	}
	return 0;
}