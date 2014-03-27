#include "FileSystem.h"
#include "Utility.h"

#include <stdio.h>
#include <stdlib.h>

#ifndef LAME_WINDOWS
#  include <unistd.h>
#else
#  include <Windows.h>
#endif

#include <sys/stat.h>
#include <fstream>

#ifdef LAME_WINDOWS
#  undef CreateDirectory
#  undef RemoveDirectory
#  undef GetCurrentDirectory
#  undef CreateFile
#  undef MoveFile
#  undef CopyFile
#endif

#ifdef LAME_WINDOWS
#  define strcasecmp _stricmp
#endif

typedef struct stat Stat;

LAME_BEGIN

namespace internal {
	char __CurrentDirectoryBuffer[LAME_FILEPATH_MAX];
}

Bool IsDirectory(StringC name) {

	Stat st;

	if (stat(name, &st) != 0) {
		return LAME_FALSE;
	}

	return st.st_mode & S_IFDIR;
}

Bool IsFile(StringC name) {

	Stat st;

	if (stat(name, &st) != 0) {
		return LAME_FALSE;
	}

	return !(st.st_mode & S_IFDIR);
}

Bool IsContent(StringC name) {

	Stat st;

	return stat(name, &st) == 0;
}

Bool CreateDirectory(StringC name) {

	return
#ifdef LAME_WINDOWS
		CreateDirectoryA(name, NULL);
#else
		mkdir(name, S_IRWXU | S_IRWXG | S_IRWXO) == 0;
#endif
}

Bool CreateFile(StringC name) {

	return
		fclose(fopen(name, "wb")) == 0;
}

Bool RemoveDirectory(StringC name) {
	return
#ifdef LAME_WINDOWS
		RemoveDirectoryA(name);
#else
		rmdir(name) == 0;
#endif
}

Bool RemoveFile(StringC name) {
	return remove(name) == 0;
}

Bool RemoveContent(StringC name) {
	return IsDirectory(name) ? RemoveDirectory(name) : RemoveFile(name);
}

Bool RenameDirectory(StringC name, StringC result) {
	return rename(name, result) == 0;
}

Bool RenameContent(StringC name, StringC result) {
	return IsDirectory(name) ? RenameDirectory(name, result) : RemoveFile(result);
}

Bool RenameFile(StringC name, StringC result) {
	return rename(name, result) == 0;
}

Bool MoveDirectory(StringC name, StringC result) {
	return RenameDirectory(name, result);
}

Bool MoveFile(StringC name, StringC result) {
	return RenameFile(name, result);
}

Bool MoveContent(StringC name, StringC result) {
	return IsDirectory(name) ? MoveDirectory(name, result) : MoveFile(name, result);
}

Bool CopyDirectory(StringC name, StringC result) {
	return CopyDirectoryTree(name, result);
}

Bool CopyFile(StringC name, StringC result) {

	std::ifstream f1(name, std::fstream::binary);
	std::ofstream f2(result, std::fstream::trunc | std::fstream::binary);

	try {
		f2 << f1.rdbuf();
	}
	catch (...) {
		return LAME_FALSE;
	}

	return LAME_TRUE;
}

Bool CopyContent(StringC name, StringC result) {
	return IsDirectory(name) ? CopyDirectory(name, result) : CopyFile(name, result);
}

Bool CreateDirectoryTree(StringC tree) {

	Buffer buffer;
	List <Buffer> list;
	List <Buffer> subdir;

	list = NormalizePath(tree).Split(LAME_SLASH);

	for (Buffer dir : list) {

		buffer += dir;

		if (!CreateDirectory(buffer.data())) {
			PostWarningMessage("Unable to create directory (%s)", buffer.data());
		}

		buffer += LAME_SLASH;
	}

	return LAME_FALSE;
}

Bool RemoveDirectoryTree(StringC name) {

	Directory dir;
	StringC file;
	List <Buffer> dirlist;

	dir.Open(name, dir.kFiles | dir.kDeep);
	while ((file = dir.Next())) {
		RemoveFile(file);
	}
	dir.Close();

	dirlist = GetDirectoriesAtPathWithDeep(name);

	for (List <Buffer>::reverse_iterator i = dirlist.rbegin(); i != dirlist.rend(); i++) {
		RemoveDirectory(i->data());
	}

	return LAME_TRUE;
}

Bool RenameDirectoryTree(StringC name, StringC result) {
	return RenameDirectory(name, result);
}

Bool MoveDirectoryTree(StringC name, StringC result) {
	return RenameDirectory(name, result);
}

Bool CopyDirectoryTree(StringC name, StringC result) {

	List <Buffer> fileList = GetFilesAtPathWithDeep(name);
	List <Buffer> directoryList = GetDirectoriesAtPathWithDeep(name);

	CreateDirectory(result);

	for (const Buffer& dir : directoryList) {
		CreateDirectory((Buffer(result) + LAME_SLASH + GetPathWithoutBegin(dir.data())).data());
	}

	for (const Buffer& file : fileList) {
		CopyFile(file.data(), (Buffer(result) + LAME_SLASH + GetPathWithoutBegin(file.data())).data());
	}

	return LAME_TRUE;
}

Uint32 GetFileSize(StringC filename) {

	Stat st;

	if (stat(filename, &st) != 0) {
		return 0;
	}

	return (Uint32)st.st_size;
}

Uint32 GetFileSize64(StringC filename) {

	Stat st;

	if (stat(filename, &st) != 0) {
		return 0;
	}

	return (Uint32)st.st_size;
}

StringC GetCurrentDirectory(Void) {

#ifdef LAME_WINDOWS
	GetCurrentDirectoryA(LAME_FILEPATH_MAX, internal::__CurrentDirectoryBuffer);
#else
	getcwd(internal::__CurrentDirectoryBuffer, LAME_FILEPATH_MAX);
#endif

	return internal::__CurrentDirectoryBuffer;
}

Bool ChangeDirectory(StringC name) {
	return
#ifdef LAME_WINDOWS
		SetCurrentDirectoryA(name);
#else
		chdir(name) == 0;
#endif
}

Buffer NormalizePath(StringC string) {

	Buffer buffer = string;

#if !defined(LAME_WINDOWS) && 0
	buffer.resize(buffer.length() + 1, '\0');
	for (Sint32 i = (Sint32)buffer.length() - 2; i >= 0; i--) {
		buffer[i + 1] = buffer[i];
	}
	buffer[0] = LAME_SLASH;
#endif

	for (Sint8& symbol : buffer) {
		if (symbol == '\\' || symbol == '/') {
			symbol = LAME_SLASH;
		}
	}

	return buffer;
}

Void NormalizePath(Sint8P buffer) {
    
    size_t length = strlen(buffer);
    
	for (size_t i = 0; i < length; i++) {
		if (buffer[i] == '/' || buffer[i] == '\\') {
			buffer[i] = LAME_SLASH;
		}
	}
}

StringC GetFileExtension(StringC filename) {

	Sint32 length = (Sint32)strlen(filename);

	for (Sint32 i = length - 1; i >= 0; i--) {
		if (filename[i] == '.') {
			return filename + i + 1;
		}
	}

	return filename;
}

Bool CheckFileExtension(StringC filename, StringC exts) {
	return CheckFileExtension(filename, Buffer(exts).Split('&'));
}

Bool CheckFileExtension(StringC filename, const List <Buffer>& extList) {

	StringC ext = GetFileExtension(filename);
	Uint32 extlen = strlen(ext);

	for (const Buffer& ext_ : extList) {
		if (ext_ == ext && extlen > 0 && ext_.length() > 0) {
			return 1;
		}
	}

	return 0;
}

StringC GetFileName(StringC filename) {

	Sint32 length = (Sint32)strlen(filename);

	for (Sint32 i = length - 1; i >= 0; i--) {
		if (filename[i] == '\\' || filename[i] == '/') {
			return filename + i + 1;
		}
	}

	return filename;
}

Buffer GetFileNameWithoutExtension(StringC filename) {

	Buffer result = GetFileName(filename[0] == LAME_SLASH ? filename + 1 : filename);

	for (Sint32 i = (Sint32)result.length() - 1; i >= 0; i--) {
		if (result[i] == '.') {
			result[i] = '\0';
			break;
		}
	}

	return result;
}

StringC GetPathWithoutBegin(StringC filename) {

	Sint32 length = (Sint32)strlen(filename);
	StringC pointer = filename;

	for (Sint32 i = 0; i < length; i++) {
		if (pointer[i] == '\\' || pointer[i] == '/') {
			return pointer + i + 1;
		}
	}

	return filename;
}

Buffer GetPathWithoutEnd(StringC filename) {

	Sint32 length = (Sint32)strlen(filename);
	Buffer result = filename;

	for (Sint32 i = length - 1; i >= 0; i--) {
		if (filename[i] == '\\' || filename[i] == '/') {
			result[i] = '\0';
			break;
		}
	}

	return result;
}

static List <Buffer> __GetContentAtPath(StringC path, StringC ext, Uint32 flags) {

	Directory dir;
	List <Buffer> list;
	List <Buffer> extList;

	if (ext) {
		extList = Buffer(ext).Split('&');
	}

	dir.Open(path, flags);
	while (dir.Next()) {
		if (ext) {
			Bool state = LAME_FALSE;
			for (const Buffer& ext_ : extList) {
				if (strcasecmp(GetFileExtension(dir.GetLast()), ext_.data()) == 0) {
					state = LAME_TRUE;
				}
			}
			if (!state) {
				continue;
			}
		}
		list.push_back(dir.GetLast());
	}
	dir.Close();

	return list;
}

List <Buffer> GetContentAtPath(StringC path) {
	return __GetContentAtPath(path, LAME_NULL, Directory::kFiles | Directory::kDirectories);
}

List <Buffer> GetDirectoriesAtPath(StringC path) {
	return __GetContentAtPath(path, LAME_NULL, Directory::kDirectories);
}

List <Buffer> GetFilesAtPath(StringC path, StringC extension) {
	return __GetContentAtPath(path, extension, Directory::kFiles);
}

List <Buffer> GetContentAtPathWithDeep(StringC path) {
	return __GetContentAtPath(path, LAME_NULL, Directory::kFiles | Directory::kDirectories | Directory::kDeep);
}

List <Buffer> GetDirectoriesAtPathWithDeep(StringC path) {
	return __GetContentAtPath(path, LAME_NULL, Directory::kDirectories | Directory::kDeep);
}

List <Buffer> GetFilesAtPathWithDeep(StringC path, StringC extension) {
	return __GetContentAtPath(path, extension, Directory::kFiles | Directory::kDeep);
}

static Buffer __FindFileAtPath(StringC path, StringC name, StringC extensions, Uint32 flags) {

	Directory dir;
	List <Buffer> extList;

	if (extensions) {
		extList = Buffer(extensions).Split('&');
	}

	dir.Open(path, flags);
	while (dir.Next()) {
		if (extensions) {
			Bool state = LAME_FALSE;
			for (const Buffer& ext_ : extList) {
				if (strcasecmp(GetFileExtension(dir.GetLast()), ext_.data()) == 0) {
					state = LAME_TRUE;
				}
			}
			if (!state) {
				continue;
			}
		}
		if (strcasecmp(GetFileNameWithoutExtension(dir.GetLast() + 1).data(), name) == 0) {
			return Buffer(dir.GetLast());
		}
	}
	dir.Close();

	return Buffer();
}

static List <Buffer> __FindFilesAtPath(StringC path, StringC name, StringC extensions, Uint32 flags) {

	Directory dir;
	List <Buffer> result;
	List <Buffer> extList;

	if (extensions) {
		extList = Buffer(extensions).Split('&');
	}

	dir.Open(path, flags);
	while (dir.Next()) {
		if (extensions) {
			Bool state = LAME_FALSE;
			for (const Buffer& ext_ : extList) {
				if (strcasecmp(GetFileExtension(dir.GetLast()), ext_.data()) == 0) {
					state = LAME_TRUE;
				}
			}
			if (!state) {
				continue;
			}
		}
		if (strcasecmp(GetFileNameWithoutExtension(dir.GetLast() + 1).data(), name) == 0) {
			result.push_back(dir.GetLast());
		}
	}
	dir.Close();

	return result;
}

Buffer FindFileAtPath(StringC path, StringC name, StringC extensions) {
	return __FindFileAtPath(path, name, extensions, Directory::kFiles);
}

Buffer FindFileAtPathWithDeep(StringC path, StringC name, StringC extensions) {
	return __FindFileAtPath(path, name, extensions, Directory::kFiles | Directory::kDeep);
}

List <Buffer> FindFilesAtPath(StringC path, StringC name, StringC extensions) {
	return __FindFilesAtPath(path, name, extensions, Directory::kFiles);
}

List <Buffer> FindFilesAtPathWithDeep(StringC path, StringC name, StringC extensions) {
	return __FindFilesAtPath(path, name, extensions, Directory::kFiles | Directory::kDeep);
}

static List <Buffer> __FindContainingFilesAtPath(StringC path, StringC name, StringC extensions, Uint32 flags) {

	Directory dir;
	List <Buffer> result;
	List <Buffer> extList;

	if (extensions) {
		extList = Buffer(extensions).Split('&');
	}

	dir.Open(path, flags);
	while (dir.Next()) {
		if (extensions) {
			Bool state = LAME_FALSE;
			for (const Buffer& ext_ : extList) {
				if (strcasecmp(GetFileExtension(dir.GetLast()), ext_.data()) == 0) {
					state = LAME_TRUE;
				}
			}
			if (!state) {
				continue;
			}
		}
		if (strstr(dir.GetLast(), name)) {
			result.push_back(dir.GetLast());
		}
	}
	dir.Close();

	return result;
}

List <Buffer> FindContainingFilesAtPath(StringC path, StringC name, StringC extensions) {
	return __FindContainingFilesAtPath(path, name, extensions, Directory::kFiles);
}
List <Buffer> FindContainingFilesAtPathWithDeep(StringC path, StringC name, StringC extensions) {
	return __FindContainingFilesAtPath(path, name, extensions, Directory::kFiles | Directory::kDeep);
}
//
//FilePathInfo ParseFilePath(StringC path) {
//
//	FilePathInfo info;
//
//	info.name = GetFileNameWithoutExtension(path);
//	info.extension = GetFileExtension(path);
//	info.path = GetPathWithoutEnd(path);
//	info.path = NormalizePath(info.path.data());
//
//	return info;
//}
//
//Buffer CreateFilePathFromInfo(FilePathInfo info) {
//
//	FilePath path = { 0 };
//
//	strcat(path, info.path.data());
//	strcat(path, LAME_SLASH_STR);
//	strcat(path, info.name.data());
//	strcat(path, ".");
//	strcat(path, info.extension.data());
//
//	return Buffer(path);
//}

LAME_END