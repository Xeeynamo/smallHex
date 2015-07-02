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

#ifndef _SMALLHEX_H
#define _SMALLHEX_H

#include "draw.h"

void shInit();
void shDestroy();
void shDrawTitleBar(Surface *surface);
bool shOpenFileDialog(Surface *surface, Font font);
bool shOpenFile(const char *strFilename);
void shFlushFile();
void shCloseFile();
void shDrawBody(Surface *surface);

#endif