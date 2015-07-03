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

#ifndef _SYSTEM_H
#define _SYSTEM_H

#define FILE_ACCESS_READ 0x0001
#define FILE_ACCESS_WRITE 0x0002
#define FILE_ACCESS_RW FILE_ACCESS_READ | FILE_ACCESS_WRITE
#define FILE_OPEN_APPEND 0x0100
#define FILE_OPEN_CREATE 0x0200
#define FILE_OPEN_TRUNCATE 0x0400

typedef void *File;
typedef void *Directory;

typedef enum
{
	Seek_Begin,
	Seek_Current,
	Seek_End,
} SeekMode;

typedef enum
{
	Directory_Continue = 1,
	Directory_EndOfEntries = 0,
	Directory_Error = -1
} DirectoryResult;

typedef struct  
{
	//! \brief name of entry
	char name[0x100];
	//! \brief length of the item in byte, if it's a file, else -1
	long long length;

} DirectoryEntry;

extern const File FileInvalid;
extern const Directory DirectoryInvalid;

void *MemoryAlloc(unsigned int size);
void *MemoryFree(void *mem);

File FileOpen(const char *strFilename, unsigned int flags);
void FileClose(File file);
unsigned int FileRead(File file, void *data, unsigned int size);
unsigned int FileWrite(File file, void *data, unsigned int size);
signed long long FileSeek(File file, signed long long offset, unsigned int mode);


Directory DirectoryOpen(const char *strDirectoryname);
DirectoryResult DirectoryNext(Directory directory, DirectoryEntry *entry);
void DirectoryClose(Directory directory);

#endif