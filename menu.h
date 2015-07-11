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

#ifndef _MENU_H
#define _MENU_H

#include "defines.h"
#include "draw.h"
#include "Input.h"

#define MENU_DEFAULT_POSITION 0x80000000

typedef enum
{
	MenuType_Dummy,
	MenuType_ValueRange,
	MenuType_ValueSet,
	MenuType_ValueStringSet,
	MenuType_Function,
	MenuType_Submenu,
} MenuType;
typedef struct
{
	int *Value;
	int(*OnChanged)(int value, int previous);
	int Count;
	int Min;
} MenuTypeValueRange;
typedef struct
{
	int *Value;
	int(*OnChanged)(int value, int previous);
	int Count;
	const int *Set;
} MenuTypeValueSet;
typedef struct
{
	int *Value;
	int(*OnChanged)(int value, int previous);
	int Count;
	const char *Strings[];
} MenuTypeValueStringsSet;
typedef struct
{
	int(*Function)();
} MenuTypeFunction;
typedef struct _MenuEntry
{
	const char *Name;
	MenuType Type;
	union
	{
		MenuTypeValueRange ValueRange;
		MenuTypeValueSet ValueSet;
		MenuTypeValueStringsSet ValueStringSet;
		MenuTypeFunction Function;
		struct _MenuEntry *Submenu;
	};
} MenuEntry;
typedef struct _MenuItem
{
	const char *Name;
	const struct _MenuItem *Parent;
	const int EntryCount;
	const struct _MenuEntry *Entry;
} MenuItem;

typedef void *Menu;

Menu MenuCreate(MenuItem *item, Font font, int width);
void MenuDestroy(Menu menu);
void MenuDraw(Menu menu, Surface *surface, int x, int y);
void MenuProcess(InputData *input);

#endif