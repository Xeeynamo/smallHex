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

#include "Input.h"
#include "defines.h"
#include <math.h>

#if defined(_WIN32)
#include <Windows.h>
#elif defined(PLATFORM_PSP2)
#include <psp2/ctrl.h>
#endif

int InputDeadZone = 12288;
#define APPLY_DEADZONE(i) if (abs(i) < InputDeadZone) i = 0

void InputInit()
{
#if defined(_WIN32)
#elif defined(PLATFORM_PSP2)
	sceCtrlSetSamplingMode(PSP2_CTRL_MODE_ANALOG);
#endif
	InputDeadZone = 12288;
}
void InputDestroy()
{
#if defined(_WIN32)
#elif defined(PLATFORM_PSP2)
#endif
}
void InputSetDeadZone(short value)
{
	InputDeadZone = (int)abs(value);
}
void InputUpdate(InputData *data)
{
	data->data = 0;
	data->lx = data->ly = data->rx = data->ry = 0;
#if defined(_WIN32)
	data->inPc.info = GetAsyncKeyState(VK_MENU) != 0;
	data->inPc.dummy1 = 0;
	data->inPc.dummy2 = 0;
	data->inPc.menu = GetAsyncKeyState(VK_RETURN) != 0;
	data->inPc.up = GetAsyncKeyState(VK_UP) != 0;
	data->inPc.right = GetAsyncKeyState(VK_RIGHT) != 0;
	data->inPc.down = GetAsyncKeyState(VK_DOWN) != 0;
	data->inPc.left = GetAsyncKeyState(VK_LEFT) != 0;
	data->inPc.pgup = GetAsyncKeyState(VK_PRIOR) != 0;
	data->inPc.pgdown = GetAsyncKeyState(VK_NEXT) != 0;
	data->inPc.operation = GetAsyncKeyState(VK_ESCAPE) != 0;
	data->inPc.back = GetAsyncKeyState(VK_BACK) != 0;
	data->inPc.select = GetAsyncKeyState(VK_SPACE) != 0;
	data->inPc.options = GetAsyncKeyState(VK_SHIFT) != 0;
#elif defined(PLATFORM_PSP2)
	SceCtrlData ctrlData;
	sceCtrlPeekBufferPositive(0, &ctrlData, 1);
	data->data = ctrlData.buttons;
	data->lx = (ctrlData.lx - 0x80) << 8;
	data->ly = (ctrlData.ly - 0x80) << 8;
	data->rx = (ctrlData.rx - 0x80) << 8;
	data->ry = (ctrlData.ry - 0x80) << 8;
#endif

	APPLY_DEADZONE(data->lx);
	APPLY_DEADZONE(data->ly);
	APPLY_DEADZONE(data->rx);
	APPLY_DEADZONE(data->ry);
}