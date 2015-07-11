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

#include "menu.h"
#include "system.h"

typedef struct 
{
	MenuItem *item;
	Font font;
	int Width;
	int Height;
	int Border;
	int VerticalDistance;
} MenuPrivate;

Menu MenuCreate(MenuItem *item, Font font, int width)
{
	MenuPrivate *menu = (MenuPrivate*)MemoryAlloc(sizeof(MenuPrivate));
	menu->item = item;
	menu->font = font;
	menu->Border = 6;
	menu->VerticalDistance = 2;
	menu->Width = width;
	menu->Height = (item->EntryCount + 1) * (FontGetHeight(font) + menu->VerticalDistance) + menu->Border * 2;
	return menu;
}
void MenuDestroy(Menu menu)
{
	MemoryFree(menu);
}
void MenuDraw(Menu menu, Surface *surface, int x, int y)
{
	MenuPrivate *pMenu = (MenuPrivate*)menu;
	int fontw = FontGetMaxWidth(pMenu->font);
	int fonth = FontGetHeight(pMenu->font) + pMenu->VerticalDistance;
	int titlex = (pMenu->Width - strlen(pMenu->item->Name) * fontw) / 2;
	if (x == MENU_DEFAULT_POSITION)
		x = (surface->width - pMenu->Width) / 2;
	if (y == MENU_DEFAULT_POSITION)
		y = (surface->height - pMenu->Height) / 2;

	FillRectangle(surface, x, y, pMenu->Width, pMenu->Height, RGB8(0x00, 0x00, 0x00));
	DrawRectangle(surface, x, y, pMenu->Width, pMenu->Height, RGB8(0xA0, 0xA0, 0xA0));

	x += pMenu->Border;
	y += pMenu->Border;
	DrawString(surface, pMenu->font, x + titlex, y, pMenu->item->Name);
	y += fonth;
	for (int i = 0; i < pMenu->item->EntryCount; i++)
	{
		DrawString(surface, pMenu->font, x, y, pMenu->item->Entry[i].Name);
		y += fonth;
	}
}
void MenuProcess(InputData *input)
{

}