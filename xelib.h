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

#ifndef _XELIB_H
#define _XELIB_H

//! \brief convert an integer value to a sequence of numbers
//! \param[in] value the input value
//! \param[in] str the output string; if the output value is NULL, it will not processed
//! \param[in] base the output base number; if it's 0 the base is set to 10
//! \returns how character has been written
/** \details the base supported are 2, 8, 10 and 16; you can set as output string
 * a NULL value to get only how characters will be used during the conversion;
 * it doesn't add the '\0' to the end of the string to help the concatenation,
 * but you can implement it doing str[Itoa(value, str, 10)] = '\0'.
 */
int Itoa(int value, char *str, int base);

//! \brief convert a long value to a sequence of numbers
//! \param[in] value the input value
//! \param[in] str the output string; if the output value is NULL, it will not processed
//! \param[in] base the output base number; if it's 0 the base is set to 10
//! \returns how character has been written
/** \details the base supported are 2, 8, 10 and 16; you can set as output string
 * a NULL value to get only how characters will be used during the conversion;
 * it doesn't add the '\0' to the end of the string to help the concatenation,
 * but you can implement it doing str[Itoa(value, str, 10)] = '\0'.
 */
int Ltoa(long long value, char *str, int base);

#endif