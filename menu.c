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

#include <stdlib.h>
#include <string.h>
#include "menu.h"
#include "system.h"
#include "xelib.h"

typedef struct 
{
	MenuItem *Item;
	Font Font;
	Font FontSelected;
	int Width;
	int Height;
	int Border;
	int VerticalDistance;
	int RightX;
	int Cursor;
	Color32 BkColor;
} MenuPrivate;

Menu MenuCreate(const MenuItem *item, Font font, int width)
{
	MenuPrivate *menu = (MenuPrivate*)MemoryAlloc(sizeof(MenuPrivate));
	menu->Item = item;
	menu->Border = 6;
	menu->VerticalDistance = 3;
	menu->Width = width;
	menu->Height = (item->EntryCount + 1) * (FontGetHeight(font) + menu->VerticalDistance) + menu->Border * 2;
	menu->RightX = 6;
	menu->Cursor = 0;
	menu->BkColor = RGB8(0x00, 0x00, 0x10);
	FontCreate(&menu->Font, FontGetType(font), RGB8(0xFF, 0xFF, 0xFF), menu->BkColor);
	FontCreate(&menu->FontSelected, FontGetType(font), RGB8(0xFF, 0x30, 0x00), menu->BkColor);
	return menu;
}
void MenuDestroy(Menu menu)
{
	MenuPrivate *pMenu = (MenuPrivate*)menu;
	FontDestroy(pMenu->Font);
	FontDestroy(pMenu->FontSelected);
	MemoryFree(pMenu);
}
void MenuDraw(Menu menu, Surface *surface, int x, int y)
{
	int i;
	MenuPrivate *pMenu = (MenuPrivate*)menu;
	Font font = pMenu->Font;
	int fontw = FontGetMaxWidth(font);
	int fonth = FontGetHeight(font) + pMenu->VerticalDistance;
	int titlex = (pMenu->Width - strlen(pMenu->Item->Name) * fontw) / 2;
	int valx = pMenu->Width - pMenu->Border - (pMenu->RightX + 3) * fontw;
	if (x == MENU_DEFAULT_POSITION)
		x = (surface->width - pMenu->Width) / 2;
	if (y == MENU_DEFAULT_POSITION)
		y = (surface->height - pMenu->Height) / 2;

	FillRectangle(surface, x, y, pMenu->Width, pMenu->Height, pMenu->BkColor);
	DrawRectangle(surface, x, y, pMenu->Width, pMenu->Height, RGB8(0xA0, 0xA0, 0xA0));

	x += pMenu->Border;
	y += pMenu->Border;
	DrawString(surface, font, x + titlex, y, pMenu->Item->Name);
	y += fonth;
	for (i = 0; i < pMenu->Item->EntryCount; i++)
	{
		char buf[11];
		const MenuEntry *e = &pMenu->Item->Entry[i];
		Font font = pMenu->Cursor == i ? pMenu->FontSelected : pMenu->Font;

		DrawString(surface, font, x, y, e->Name);
		switch (e->Type)
		{
		case MenuType_Dummy:
			break;
		case MenuType_ValueRange:
			DrawChar(surface, font, x + valx, y, '<');
			buf[Itoa(e->ValueRange.Min + *e->ValueRange.Value, buf, 10)] = '\0';
			DrawString(surface, font, x + valx + fontw, y, buf);
			DrawChar(surface, font, x + pMenu->Width - pMenu->Border - fontw * 2, y, '>');
			break;
		case MenuType_ValueSet:
			DrawChar(surface, font, x + valx, y, '<');
			DrawChar(surface, font, x + pMenu->Width - pMenu->Border - fontw * 2, y, '>');
			break;
		case MenuType_ValueStringSet:
			DrawChar(surface, font, x + valx, y, '<');
			DrawString(surface, font, x + valx + fontw, y, (*e->ValueStringSet.Strings)[*e->ValueStringSet.Value]);
			DrawChar(surface, font, x + pMenu->Width - pMenu->Border - fontw * 2, y, '>');
			break;
		case MenuType_Function:
			break;
		case MenuType_Submenu:
			DrawChar(surface, font, x + pMenu->Width - pMenu->Border - fontw * 2, y, '>');
			break;
		}
		y += fonth;
	}
}
int MenuProcess(Menu menu, InputData *input)
{
	MenuPrivate *pMenu = (MenuPrivate*)menu;

	int ret = 0;
	if (input->repeat.inPs.circle)
		return -1;
	if (input->repeat.inPs.up)
	{
		pMenu->Cursor--;
		if (pMenu->Cursor < 0)
			pMenu->Cursor = pMenu->Item->EntryCount - 1;
		ret = 1;
	}
	else if (input->repeat.inPs.down)
	{
		pMenu->Cursor++;
		if (pMenu->Cursor >= pMenu->Item->EntryCount)
			pMenu->Cursor = 0;
		ret = 1;
	}
	else if (input->repeat.inPs.left)
	{
		MenuEntry *e = &pMenu->Item->Entry[pMenu->Cursor];
		int v;
		switch (e->Type)
		{
		case MenuType_Dummy:
			break;
		case MenuType_ValueRange:
			v = *e->ValueRange.Value;
			v--;
			if (v < e->ValueRange.Min)
				v = e->ValueRange.Min + e->ValueRange.Count - 1;
			*e->ValueRange.Value = v;
			e->ValueRange.OnChanged(v);
			ret = 1;
			break;
		case MenuType_ValueSet:
		case MenuType_ValueStringSet:
			v = *e->ValueRange.Value;
			v--;
			if (v < 0)
				v = e->ValueRange.Count - 1;
			*e->ValueRange.Value = v;
			e->ValueRange.OnChanged(v);
			ret = 1;
			break;
		case MenuType_Function:
			break;
		case MenuType_Submenu:
			break;
		}
	}
	else if (input->repeat.inPs.right)
	{
		MenuEntry *e = &pMenu->Item->Entry[pMenu->Cursor];
		int v;
		switch (e->Type)
		{
		case MenuType_Dummy:
			break;
		case MenuType_ValueRange:
			v = *e->ValueRange.Value;
			v++;
			if (v >= e->ValueRange.Min + e->ValueRange.Count)
				v = e->ValueRange.Min;
			*e->ValueRange.Value = v;
			e->ValueRange.OnChanged(v);
			ret = 1;
			break;
		case MenuType_ValueSet:
		case MenuType_ValueStringSet:
			v = *e->ValueRange.Value;
			v++;
			if (v >= e->ValueRange.Count)
				v = 0;
			*e->ValueRange.Value = v;
			e->ValueRange.OnChanged(v);
			ret = 1;
			break;
		case MenuType_Function:
			break;
		case MenuType_Submenu:
			break;
		}
	}
	return ret;
}