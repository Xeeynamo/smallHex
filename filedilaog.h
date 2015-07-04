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

#ifndef _FILEDIALOG_H
#define _FILEDIALOG_H

#include "draw.h"

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

typedef enum
{
	FileDialogResult_Error = -1,
	FileDialogResult_Ok = 0,
	FileDialogResult_Cancel = 1,
} FileDialogResult;

//! \brief open a window where it's possible to select a single file
//! \param[in] surface where the file dialog will be displayed, if necessary
//! \param[in] font used by file dialog, if necessary
//! \param[out] filename that was selected; it's recommended to use an array not shorter than MAX_PATH
//! \param[in] extfilter RESERVED FOR FUTURE USE. Set it to NULL
//! \param[in] directory RESERVED FOR FUTURE USE. Set it to NULL
//! \return result of operation
FileDialogResult FileDialogOpen(Surface *surface, Font font, char *filename, const char *extfilter, const char *directory);

#endif