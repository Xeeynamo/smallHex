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

#ifndef _INPUT_H
#define _INPUT_H

typedef struct
{
	unsigned int info : 1;
	unsigned int dummy1 : 1;
	unsigned int dummy2 : 1;
	unsigned int menu : 1;
	unsigned int up : 1;
	unsigned int right : 1;
	unsigned int down : 1;
	unsigned int left : 1;
	unsigned int pgup : 1;
	unsigned int pgdown : 1;
	unsigned int operation : 1;
	unsigned int back : 1;
	unsigned int select : 1;
	unsigned int options : 1;
} InputDataGeneric;
typedef struct
{
	unsigned int select : 1;
	unsigned int dummy1 : 1;
	unsigned int dummy2 : 1;
	unsigned int start : 1;
	unsigned int up : 1;
	unsigned int right : 1;
	unsigned int down : 1;
	unsigned int left : 1;
	unsigned int ltrigger : 1;
	unsigned int rtrigger : 1;
	unsigned int triangle : 1;
	unsigned int circle : 1;
	unsigned int cross : 1;
	unsigned int square : 1;
} InputDataPlaystation;
typedef struct
{
	union
	{
		InputDataGeneric inPc;
		InputDataPlaystation inPs;
		unsigned int data;
	};
} InputButton;
typedef struct 
{
	//! \brief raw button values
	InputButton raw;
	//! \brief holds previous button state
	InputButton prev;
	//! \brief holds raw values with repeat processor
	InputButton repeat;
	// values that goes between -32768 and +32767
	short lx, ly; // mapped on numeric pad on windows
	short rx, ry; // mapped on mouse wheel on windows
} InputData;

void InputInit();
void InputDestroy();

//! \brief get dead zone for analog or wheel movement
//! \param[out] value between 0 and 32767
//! \sa InputSetDeadZone
void InputGetDeadZone(unsigned int *value);
//! \brief set dead zone for analog or wheel movement
//! \param[in] value between 0 and 32767
//! \details inital value set by InputInit is 12288
void InputSetDeadZone(unsigned int value);

//! \brief get the repeat frequency
//! \param[out] wait how frames to wait until "repeating"
//! \param[out] repeat frequency
//! \sa InputSetRepeat
void InputGetRepeat(int *wait, int *repeat);
//! \brief set the repeat frequency
//! \param[in] wait how frames to wait until "repeating"; default 30
//! \param[in] repeat frequency; default 0
void InputSetRepeat(int wait, int repeat);

void InputUpdate(InputData *data);

#endif