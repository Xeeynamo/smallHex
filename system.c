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
#include "system.h"

#if defined(_WIN32)
#include <windows.h>
#elif defined(PLATFORM_PSP2)
#include <psp2/types.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/dirent.h>
#endif

const File FileInvalid = (File)-1;
const Directory DirectoryInvalid = (Directory)-1;

void *MemoryAlloc(unsigned int size)
{
#if defined(_WIN32)
	return HeapAlloc(GetProcessHeap(), 0, size);
#elif defined(PLATFORM_PSP2)
	return malloc(size);
#endif
}
void *MemoryResize(void *mem, unsigned int newsize)
{
#if defined(_WIN32)
	return HeapReAlloc(GetProcessHeap(), 0, mem, newsize);
#elif defined(PLATFORM_PSP2)
	return realloc(mem, newsize);
#endif
	return NULL;
}
void *MemoryFree(void *mem)
{
#if defined(_WIN32)
	HeapFree(GetProcessHeap(), 0, mem);
#elif defined(PLATFORM_PSP2)
	free(mem);
#endif
	return NULL;
}

File FileOpen(const char *strFilename, unsigned int flags)
{
#if defined(_WIN32)
	DWORD access;
	DWORD creation = 0;
	switch (flags & FILE_ACCESS_RW)
	{
	case 0:
		access = 0;
		break;
	case FILE_ACCESS_READ:
		access = GENERIC_READ;
		break;
	case FILE_ACCESS_WRITE:
		access = GENERIC_WRITE;
		break;
	case FILE_ACCESS_RW:
		access = GENERIC_READ | GENERIC_WRITE;
		break;
	}
	if (flags & FILE_OPEN_CREATE)
		creation |= CREATE_ALWAYS;
	else if (flags & FILE_OPEN_TRUNCATE)
		creation |= TRUNCATE_EXISTING;
	else
		creation |= OPEN_EXISTING;
	HANDLE hFile = CreateFile(strFilename, access, FILE_SHARE_READ | FILE_SHARE_DELETE,
		NULL, creation, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FileInvalid;
	if (flags & FILE_OPEN_APPEND)
		FileSeek((File)hFile, 0, Seek_End);
	return (File)hFile;
#elif defined(PLATFORM_PSP2)
	SceUID id = sceIoOpen(strFilename, flags, 0777);
	if (id < 0)
		return FileInvalid;
	else
		return (File)id;
#endif
	return FileInvalid;
}
void FileClose(File file)
{
	if (file == FileInvalid)
		return;
#if defined(_WIN32)
	CloseHandle((HANDLE)file);
#elif defined(PLATFORM_PSP2)
	sceIoClose((SceUID)file);
#endif
}
unsigned int FileRead(File file, void *data, unsigned int size)
{
	if (file == FileInvalid)
		return 0;
#if defined(_WIN32)
	DWORD read = 0;
	ReadFile((HANDLE)file, data, size, &read, NULL);
	return read;
#elif defined(PLATFORM_PSP2)
	return (unsigned int)sceIoRead((SceUID)file, data, (SceSize)size);
#endif
	return 0;
}
unsigned int FileWrite(File file, void *data, unsigned int size)
{
	if (file == FileInvalid)
		return 0;
#if defined(_WIN32)
	DWORD writed = 0;
	WriteFile((HANDLE)file, data, size, &writed, NULL);
	return writed;
#elif defined(PLATFORM_PSP2)
	return (unsigned int)sceIoWrite((SceUID)file, data, (SceSize)size);
#endif
	return 0;
}
signed long long FileSeek(File file, signed long long offset, unsigned int mode)
{
	if (file == FileInvalid)
		return -1;
#if defined(_WIN32)
	LARGE_INTEGER large;
	LARGE_INTEGER large2;
	large.QuadPart = offset;
	SetFilePointerEx((HANDLE)file, large, &large2, mode);
	return (signed long long)large2.QuadPart;
#elif defined(PLATFORM_PSP2)
	return (signed long long)sceIoLseek((SceUID)file, offset, mode);
#endif
	return -1;
}

Directory DirectoryOpen(const char *strDirectoryname)
{
#if defined(_WIN32)
#elif defined(PLATFORM_PSP2)
	SceUID id = sceIoDopen(strDirectoryname);
	if (id < 0)
		return DirectoryInvalid;
	else
		return (Directory)id;
#endif
	return DirectoryInvalid;
}
DirectoryResult DirectoryNext(Directory directory, DirectoryEntry *entry)
{
	if (directory == DirectoryInvalid)
		return Directory_Error;
#if defined(_WIN32)
#elif defined(PLATFORM_PSP2)
	SceIoDirent dir;
	int res = sceIoDread((SceUID)directory, &dir);
	if (res < 0)
		return Directory_Error;
	memcpy(entry->name, dir.d_name);
	if ((dir.d_stat.st_mode & 0x0010) != 0)
		entry->length = -1;
	else
		entry->length = dir.d_stat.st_size;
	return res == 0 ? Directory_EndOfEntries : Directory_Continue;
#endif
	return Directory_Error;
}
void DirectoryClose(Directory directory)
{
	if (directory == DirectoryInvalid)
		return;
#if defined(_WIN32)
#elif defined(PLATFORM_PSP2)
	sceIoDclose((SceUID)directory);
#endif
}