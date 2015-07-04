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

#define APPLY_DEADZONE(i) if (abs(i) < InputDeadZone) i = 0

int InputDeadZone = 12288;
int InputWait = 30;
int InputRepeat = 0;
unsigned int InputDataPrevious = 0;

int InputRepeatValues[16] = { 0 };
unsigned int _InputProcessRepeat(unsigned int value)
{
	unsigned int i = 0;
	unsigned int data = 0;
	int *pRepeatValues = InputRepeatValues;
	for (unsigned int i = 0; i < sizeof(InputRepeatValues) / sizeof(*InputRepeatValues); i++)
	{
		unsigned int mask = (1 << i);
		if (value & mask)
		{
			if (*pRepeatValues > 0)
			{
				if ((*pRepeatValues)++ >= InputWait)
				{
					*pRepeatValues = -1;
					data |= mask;
				}
			}
			else if (*pRepeatValues < 0)
			{
				if (-(*pRepeatValues)-- > InputRepeat)
				{
					*pRepeatValues = -1;
					data |= mask;
				}
			}
			else
			{
				*pRepeatValues = 1;
				data |= mask;
			}
		}
		else
			*pRepeatValues = 0;
		pRepeatValues++;
	}
	return data;
}

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

void InputGetDeadZone(unsigned int *value)
{
	*value = InputDeadZone;
}
void InputSetDeadZone(unsigned int value)
{
	if (value > 32767)
		value = 32767;
	InputDeadZone = value;
}
void InputGetRepeat(int *wait, int *repeat)
{
	*wait = InputWait;
	*repeat = InputRepeat;
}
void InputSetRepeat(int wait, int repeat)
{
	InputWait = wait;
	InputRepeat = repeat;
}
void InputUpdate(InputData *data)
{
	data->raw.data = 0;
	data->lx = data->ly = data->rx = data->ry = 0;
#if defined(_WIN32)
	data->raw.inPc.info = GetAsyncKeyState(VK_MENU) != 0;
	data->raw.inPc.dummy1 = 0;
	data->raw.inPc.dummy2 = 0;
	data->raw.inPc.menu = GetAsyncKeyState(VK_RETURN) != 0;
	data->raw.inPc.up = GetAsyncKeyState(VK_UP) != 0;
	data->raw.inPc.right = GetAsyncKeyState(VK_RIGHT) != 0;
	data->raw.inPc.down = GetAsyncKeyState(VK_DOWN) != 0;
	data->raw.inPc.left = GetAsyncKeyState(VK_LEFT) != 0;
	data->raw.inPc.pgup = GetAsyncKeyState(VK_PRIOR) != 0;
	data->raw.inPc.pgdown = GetAsyncKeyState(VK_NEXT) != 0;
	data->raw.inPc.operation = GetAsyncKeyState(VK_ESCAPE) != 0;
	data->raw.inPc.back = GetAsyncKeyState(VK_BACK) != 0;
	data->raw.inPc.select = GetAsyncKeyState(VK_SPACE) != 0;
	data->raw.inPc.options = GetAsyncKeyState(VK_SHIFT) != 0;
#elif defined(PLATFORM_PSP2)
	SceCtrlData ctrlData;
	sceCtrlPeekBufferPositive(0, &ctrlData, 1);
	data->raw.data = ctrlData.buttons;
	data->lx = (ctrlData.lx - 0x80) << 8;
	data->ly = (ctrlData.ly - 0x80) << 8;
	data->rx = (ctrlData.rx - 0x80) << 8;
	data->ry = (ctrlData.ry - 0x80) << 8;
#endif

	data->prev.data = InputDataPrevious;
	InputDataPrevious = data->raw.data;
	data->repeat.data = _InputProcessRepeat(data->raw.data);
	APPLY_DEADZONE(data->lx);
	APPLY_DEADZONE(data->ly);
	APPLY_DEADZONE(data->rx);
	APPLY_DEADZONE(data->ry);
}