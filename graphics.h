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

#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include <stdbool.h>
#include "draw.h"

//! \brief create graphics context
/** \return -1 if an error occured, else how instances of graphics are
 * currently initializated.
 */
/** \details this function is needed for every function of graphics.h.
 * For every GraphicsInit called, should be called the same number of times
 * function GraphicsDestroy.
 */
int GraphicsInit();

//! \brief destroy graphics context
//! \return -1 if an error occured, else how instances remains to destroy
int GraphicsDestroy();

//! \brief set the title of console
//! \param[in] title to set
void GraphicsSetTitle(const char *title);

//! \brief pause current thread until vsync interrupt
/** \details await time is usually 1000/60 milliseconds. It's reccomended
 * to call this right after GraphicsSwapBuffers.
 */
void GraphicsWaitVSync();

//! \brief present current buffer to screen
void GraphicsSwapBuffers();

//! \brief get current buffer as a surface item
//! \param[out] surface direct access of current buffer
//! \details returned value will change everytime that SwapBuffer is called
void GetCurrentBuffer(Surface *surface);

#endif
