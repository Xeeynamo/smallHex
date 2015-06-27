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

#if defined(_WIN32)
#elif defined(PLATFORM_PSP2)
#include <psp2/ctrl.h>
#endif

//static int InputDeadZone = 12288;
#define APPLY_DEADZONE(i) if (abs(i) < InputDeadZone) i = 0

void InputInit()
{
#if defined(_WIN32)
#elif defined(PLATFORM_PSP2)
	sceCtrlSetSamplingMode(PSP2_CTRL_MODE_ANALOG);
#endif
	//InputDeadZone = 12288;
}
void InputDestroy()
{
#if defined(_WIN32)
#elif defined(PLATFORM_PSP2)
#endif
}
void InputSetDeadZone(short value)
{

}
void InputUpdate(InputData *data)
{
#if defined(_WIN32)

#elif defined(PLATFORM_PSP2)
	SceCtrlData ctrlData;
	sceCtrlPeekBufferPositive(0, &ctrlData, 1);
	data->data = ctrlData.buttons;
	data->lx = (ctrlData.lx - 0x80) << 8;
	data->ly = (ctrlData.ly - 0x80) << 8;
	data->rx = (ctrlData.rx - 0x80) << 8;
	data->ry = (ctrlData.ry - 0x80) << 8;
#endif

	//APPLY_DEADZONE(data->lx);
	//APPLY_DEADZONE(data->ly);
	//APPLY_DEADZONE(data->rx);
	//APPLY_DEADZONE(data->ry);
}