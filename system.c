#include <stdlib.h>
#include "system.h"

#if defined(_WIN32)
#include <windows.h>
#elif defined(PLATFORM_PSP2)
#include <psp2/types.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/dirent.h>
#endif

File FileInvalid = (File)-1;
Directory DirectoryInvalid = (Directory)-1;

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
void FileSeek(File file, signed long long offset, unsigned int mode)
{
	if (file == FileInvalid)
		return;
#if defined(_WIN32)
	LARGE_INTEGER large;
	large.QuadPart = offset;
	SetFilePointerEx((HANDLE)file, large, NULL, mode);
#elif defined(PLATFORM_PSP2)
	sceIoLseek((SceUID)file, offset, mode);
#endif
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
#endif
	return Directory_Error;
}
void DirectoryClose(Directory directory)
{
	if (directory == DirectoryInvalid)
		return;
#if defined(_WIN32)
#elif defined(PLATFORM_PSP2)
#endif
}