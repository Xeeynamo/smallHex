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

#include <stdbool.h>

#define FILE_ACCESS_READ 0x0001
#define FILE_ACCESS_WRITE 0x0002
#define FILE_ACCESS_RW FILE_ACCESS_READ | FILE_ACCESS_WRITE
#define FILE_OPEN_APPEND 0x0100
#define FILE_OPEN_CREATE 0x0200
#define FILE_OPEN_TRUNCATE 0x0400

//! \breif pointer that represents a valid file
typedef void *File;
//! \breif pointer that represents a valid directory
typedef void *Directory;

typedef enum
{
	Seek_Begin,
	Seek_Current,
	Seek_End,
} SeekMode;

typedef enum
{
	//! \brief there are other entries obtainable with DirectoryNext
	Directory_Continue = 1,
	//! \brief current entry is the last entry of DirectoryNext
	Directory_EndOfEntries = 0,
	//! \brief an error occureed
	Directory_Error = -1
} DirectoryResult;

typedef struct  
{
	//! \brief name of entry
	char name[0x100];
	//! \brief length of the item in byte, if it's a file, else -1
	long long length;

} DirectoryEntry;

//! \brief represents an invalid file pointer
extern const File FileInvalid;
//! \brief represents an invalid directory pointer
extern const Directory DirectoryInvalid;

void *MemoryAlloc(unsigned int size);
void *MemoryFree(void *mem);

//! \brief open a file
//! \param[in] strFilename name of the file to open
//! \param[in] flags modes to open a file; check FILE_ACCESS_* and FILE_OPEN_*
//! \return a file or FileInvalidd
File FileOpen(const char *strFilename, unsigned int flags);

//! \brief close the specified file
//! \param[in] file to close; can be FileInvalid
void FileClose(File file);

//! \brief read from specified file file
//! \param[in] file where the content will be read; can be FileInvalid
//! \param[in] data pointer where the content read will be written
//! \param[in] size in bytes of the content to read
//! \return how bytes were read
unsigned int FileRead(File file, void *data, unsigned int size);

//! \brief write from specified file file
//! \param[in] file where the content will be written; can be FileInvalid
//! \param[in] data pointer that contains the information that will be written
//! \param[in] size in bytes of the content to write
//! \return how bytes were written
unsigned int FileWrite(File file, void *data, unsigned int size);

//! \brief move through the file
//! \param[in] file where to re-set position; can be FileInvalid
//! \param[in] offset of new position
//! \param[in] mode of positioning of the offset
//! \return current position
/** \details to retrieve current file position, you can just get the return
 * value of FileSeek(file, 0, Seek_End).
 */
signed long long FileSeek(File file, signed long long offset, unsigned int mode);

//! \brief remove the specified file from file system
//! \param[in] strFilename file name of the file to remove
//! \return true if the file was correctly removed from file system
bool FileRemove(const char *strFilename);

//! \brief rename the specified file
//! \param[in] strFilename file name of the file to rename
//! \param[in] strNewFilename new file name to assign
//! \return true if the file was correctly renamed
bool FileRename(const char *strFilename, const char *strNewFilename);

//! \brief open a directory in order to query its content
//! \param[in] strDirectoryname name of directory to open
//! \return directory pointer or DirectoryInvalid
Directory DirectoryOpen(const char *strDirectoryname);

//! \brief get the next entry of specified directory
//! \param[in] directory to query; can be DirectoryInvalid
//! \param[out] entry currently found
//! \return status of the call
DirectoryResult DirectoryNext(Directory directory, DirectoryEntry *entry);

//! \brief close the specified directory
//! \param[in] directory to close; can be DirectoryInvalid
void DirectoryClose(Directory directory);

//! \brief create a directory
//! \param[in] strDirectoryName to create
//! \return true if the operation did not failed
bool DirectoryCreate(const char *strDirectoryName);

//! \brief remove a directory
//! \param[in] strDirectoryName to remove; directory content must be empty
//! \return true if the operation did not failed
bool DirectoryRemove(const char *strDirectoryName);

//! \brief rename a directory
//! \param[in] strDirectoryName to rename
//! \param[in] strNewDirectoryName new directory name
//! \return true if the operation did not failed
bool DirectoryRename(const char *strDirectoryName, const char *strNewDirectoryName);

//! \brief enter to specifeid directory
//! \param[in] strDirectoryName to enter
//! \return true if the operation did not failed
bool DirectoryChange(const char *strDirectoryName);

//! \brief get name of device entry point
//! \param[in,out] strDevName array of string pointers
//! \param[in] maxcount maximum number of elements that can be write on strDevName
//! \return number of devices found
int DeviceNameEnumerate(char **strDevName, int maxcount);

#endif