#include "File.h"
#include "Directory.h"

#include <stdio.h>
#include <stdlib.h>

#ifndef LAME_WINDOWS
#  include <unistd.h>
#else
#  include <Windows.h>
#endif

#include <sys/stat.h>
#include <fstream>

#include <sys/stat.h>

#ifndef LAME_WINDOWS
#  define _S_IFDIR S_IFDIR
#  define _S_IREAD S_IREAD
#  define _S_IWRITE S_IWRITE
#  define _S_IEXEC S_IEXEC
#endif

#ifndef LAME_WINDOWS
#  define _fseeki64 fseek
#  define _ftelli64 ftell
#endif

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

LAME_BEGIN2(Core)

File::File() {

	this->size_ = 0;
	this->handle_ = 0;
	this->flags_ = 0;
	this->permission_ = kPermissionDefault;
	this->type_ = kTypeDefault;
}

File::~File() {

	if (this->handle_) {
		this->Close();
	}
		
	this->size_ = 0;
	this->handle_ = 0;
	this->flags_ = 0;
	this->permission_ = kPermissionDefault;
	this->type_ = kTypeDefault;
}

Void File::Open(StringC filename, Uint32 flags) {

	Sint8 mode[4] = { 0 };

	if (!filename) {
		PostErrorMessage("filename mustn't be NULL", 1);
	}

	if (!flags) {
		PostErrorMessage("flags mustn't be 0", 1);
	}

	Stat st;
	Uint32 stf;

	// get file's stat information
	if (!stat(filename, &st)) {

		stf = st.st_mode;

		if (stf & _S_IFDIR) {
			this->type_ = kTypeDirectory;
		} else {
			this->type_ = kTypeFile;
		}

		if (stf & _S_IREAD) {
			this->permission_ = kPermissionRead;
		}
		if (stf & _S_IWRITE) {
			this->permission_ = kPermissionWrite;
		}
		if (stf & _S_IEXEC) {
			this->permission_ = kPermissionExecute;
		}
	}

	this->filename_ = filename;
	this->flags_ = flags;

	// generate fopen read/write mode
	if (flags & kRead && flags & kWrite) {
		mode[0] = 'a';
	} else if (flags & kRead) {
		mode[0] = 'r';
	} else if (flags & kWrite) {
		mode[0] = 'w';
	} else {
		mode[0] = 'w';
	}

	// generate fopen binary/text mode
	if (flags & kBinary && flags & kText) {
		PostErrorMessage("Unable to open file with BINARY and TEXT flags", 1);
	} else if (flags & kText) {
		mode[1] = 't';
	} else {
		mode[1] = 'b';
	}

	// check append flag
	if (flags & kAppend) {
		mode[2] = '+';
	}

	// open file
	this->handle_ = fopen(filename, mode);

	if (!this->handle_) {
		PostErrorMessage("Unable to open file (%s)", this->filename_.data());
	}

	_fseeki64((FILE*)this->handle_, 0, SEEK_END);
	this->size_ = _ftelli64((FILE*)this->handle_);
	rewind((FILE*)this->handle_);
}

Void File::Open(StringC filename, StringC mode) {

	if (!filename) {
		PostErrorMessage("filename mustn't be NULL", 1);
	}

	if (!mode) {
		PostErrorMessage("mode mustn't be NULL", 1);
	}

	Stat st;
	Uint32 stf;

	// get file's stat information
	if (!stat(filename, &st)) {

		stf = st.st_mode;

		if (stf & _S_IFDIR) {
			this->type_ = kTypeDirectory;
		}
		else {
			this->type_ = kTypeFile;
		}

		if (stf & _S_IREAD) {
			this->permission_ = kPermissionRead;
		}
		if (stf & _S_IWRITE) {
			this->permission_ = kPermissionWrite;
		}
		if (stf & _S_IEXEC) {
			this->permission_ = kPermissionExecute;
		}
	}

	this->filename_ = filename;

	// open file
	this->handle_ = fopen(filename, mode);

	if (!this->handle_) {
		PostErrorMessage("Unable to open file (%s)", this->filename_.data());
	}

	_fseeki64((FILE*)this->handle_, 0, SEEK_END);
	this->size_ = _ftelli64((FILE*)this->handle_);
	rewind((FILE*)this->handle_);
}

Uint32 File::Write(VoidP buffer, Uint32 length) {

	if (!this->handle_) {
		PostErrorMessage("Unable to write buffer to file with NULL handle", 1);
	}

	return
		(Uint32)fwrite(buffer, length, 1, (FILE*)this->handle_);
}

Uint32 File::Read(VoidP buffer, Uint32 length) {

	Uint32 position = this->GetPosition();

	if (!this->handle_) {
		PostErrorMessage("Unable to read buffer from file with NULL handle", 1);
	}

	fread(buffer, length, 1, (FILE*)this->handle_);

	return this->GetPosition() - position;
}

Void File::Close(Void) {

	if (this->handle_) {
		if (fclose((FILE*)this->handle_) != 0) {
			PostErrorMessage("Unable to close file (%s)", this->filename_.data());
		}
	}

	this->size_ = 0;
	this->handle_ = 0;
	this->flags_ = 0;
	this->permission_ = kPermissionDefault;
	this->type_ = kTypeDefault;
}

Uint32 File::GetPosition(Void) {

	return
		(Uint32)GetPosition64();
}

Uint64 File::GetPosition64(Void) {

	fpos_t _position;

	if (!this->handle_) {
		PostErrorMessage("Unable to get file's position with NULL handle", 1);
	}

	if (fgetpos((FILE*)this->handle_, &_position) != 0) {
		PostErrorMessage("Unable to get file's position", 1);
	}

	return (Uint64)_position;
}

Uint32 File::SetPosition(Uint32 position) {

	return
		(Uint32)this->SetPosition64((Uint64)position);
}

Uint64 File::SetPosition64(Uint64 position) {

	fpos_t _previous = this->GetPosition64();
	fpos_t _position = position;

	if (fsetpos((FILE*)this->handle_, &_position) != 0) {
		PostErrorMessage("Unable to set file's position", 1);
	}

	return _previous;
}

Bool File::Rewind(Void) {

	if (!this->handle_) {
		return LAME_FALSE;
	}

	rewind((FILE*)this->handle_);

	return LAME_FALSE;
}

Time File::GetModificationDate(Void) {

	Time time;
	Stat st;

	if (stat(this->filename_.data(), &st)) {
		PostErrorMessage("Unable to get file's modification date", 1);
	}

	time.BuildForDate(st.st_mtime);

	return time;
}

Time File::GetCreationDate(Void) {

	Time time;
	Stat st;

	if (stat(this->filename_.data(), &st)) {
		PostErrorMessage("Unable to get file's modification date", 1);
	}

	time.BuildForDate(st.st_ctime);

	return time;
}

Time File::GetAccessDate(Void) {

	Time time;
	Stat st;

	if (stat(this->filename_.data(), &st) != 0) {
		PostErrorMessage("Unable to get file's modification date", 1);
	}

	time.BuildForDate(st.st_atime);

	return time;
}

namespace internal {
	char __CurrentDirectoryBuffer[LAME_FILEPATH_MAX];
}

Bool File::IsDirectory(StringC name) {

	Stat st;

	if (stat(name, &st) != 0) {
		return LAME_FALSE;
	}

	return st.st_mode & S_IFDIR;
}

Bool File::IsFile(StringC name) {

	Stat st;

	if (stat(name, &st) != 0) {
		return LAME_FALSE;
	}

	return !(st.st_mode & S_IFDIR);
}

Bool File::IsContent(StringC name) {

	Stat st;

	return stat(name, &st) == 0;
}

Bool File::CreateDirectory(StringC name) {

	return
#ifdef LAME_WINDOWS
		CreateDirectoryA(name, NULL);
#else
		mkdir(name, S_IRWXU | S_IRWXG | S_IRWXO) == 0;
#endif
}

Bool File::CreateFile(StringC name) {

	return
		fclose(fopen(name, "wb")) == 0;
}

Bool File::RemoveDirectory(StringC name) {
	return
#ifdef LAME_WINDOWS
		RemoveDirectoryA(name);
#else
		rmdir(name) == 0;
#endif
}

Bool File::RemoveFile(StringC name) {
	return remove(name) == 0;
}

Bool File::RemoveContent(StringC name) {
	return IsDirectory(name) ? RemoveDirectory(name) : RemoveFile(name);
}

Bool File::RenameDirectory(StringC name, StringC result) {
	return rename(name, result) == 0;
}

Bool File::RenameContent(StringC name, StringC result) {
	return IsDirectory(name) ? RenameDirectory(name, result) : RemoveFile(result);
}

Bool File::RenameFile(StringC name, StringC result) {
	return rename(name, result) == 0;
}

Bool File::MoveDirectory(StringC name, StringC result) {
	return RenameDirectory(name, result);
}

Bool File::MoveFile(StringC name, StringC result) {
	return RenameFile(name, result);
}

Bool File::MoveContent(StringC name, StringC result) {
	return IsDirectory(name) ? MoveDirectory(name, result) : MoveFile(name, result);
}

Bool File::CopyDirectory(StringC name, StringC result) {
	return CopyDirectoryTree(name, result);
}

Bool File::CopyFile(StringC name, StringC result) {

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

Bool File::CopyContent(StringC name, StringC result) {
	return IsDirectory(name) ? CopyDirectory(name, result) : CopyFile(name, result);
}

Bool File::CreateDirectoryTree(StringC tree) {

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

Bool File::RemoveDirectoryTree(StringC name) {

	Directory dir;
	StringC file;
	List <Buffer> dirlist;

	dir.Open(name, dir.kFiles | dir.kDeep);
	while ((file = dir.Next())) {
		RemoveFile(file);
	}
	dir.Close();

	dirlist = Directory::GetDirectoriesAtPathWithDeep(name);

	for (List <Buffer>::reverse_iterator i = dirlist.rbegin(); i != dirlist.rend(); i++) {
		RemoveDirectory(i->data());
	}

	return LAME_TRUE;
}

Bool File::RenameDirectoryTree(StringC name, StringC result) {
	return RenameDirectory(name, result);
}

Bool File::MoveDirectoryTree(StringC name, StringC result) {
	return RenameDirectory(name, result);
}

Bool File::CopyDirectoryTree(StringC name, StringC result) {

	List <Buffer> fileList = Directory::GetFilesAtPathWithDeep(name);
	List <Buffer> directoryList = Directory::GetDirectoriesAtPathWithDeep(name);

	CreateDirectory(result);

	for (const Buffer& dir : directoryList) {
		CreateDirectory((Buffer(result) + LAME_SLASH + GetPathWithoutBegin(dir.data())).data());
	}

	for (const Buffer& file : fileList) {
		CopyFile(file.data(), (Buffer(result) + LAME_SLASH + GetPathWithoutBegin(file.data())).data());
	}

	return LAME_TRUE;
}

Uint32 File::GetFileSize(StringC filename) {

	Stat st;

	if (stat(filename, &st) != 0) {
		return 0;
	}

	return (Uint32)st.st_size;
}

Uint32 File::GetFileSize64(StringC filename) {

	Stat st;

	if (stat(filename, &st) != 0) {
		return 0;
	}

	return (Uint32)st.st_size;
}

StringC File::GetCurrentDirectory(Void) {

#ifdef LAME_WINDOWS
	GetCurrentDirectoryA(LAME_FILEPATH_MAX, internal::__CurrentDirectoryBuffer);
#else
	getcwd(internal::__CurrentDirectoryBuffer, LAME_FILEPATH_MAX);
#endif

	return internal::__CurrentDirectoryBuffer;
}

Bool File::ChangeDirectory(StringC name) {
	return
#ifdef LAME_WINDOWS
		SetCurrentDirectoryA(name);
#else
		chdir(name) == 0;
#endif
}

Buffer File::NormalizePath(StringC string) {

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

Void File::NormalizePath(Sint8P buffer) {

	size_t length = strlen(buffer);

	for (size_t i = 0; i < length; i++) {
		if (buffer[i] == '/' || buffer[i] == '\\') {
			buffer[i] = LAME_SLASH;
		}
	}
}

StringC File::GetFileExtension(StringC filename) {

	Sint32 length = (Sint32)strlen(filename);

	for (Sint32 i = length - 1; i >= 0; i--) {
		if (filename[i] == '.') {
			return filename + i + 1;
		}
	}

	return filename;
}

Bool File::CheckFileExtension(StringC filename, StringC exts) {
	return CheckFileExtension(filename, Buffer(exts).Split('&'));
}

Bool File::CheckFileExtension(StringC filename, const List <Buffer>& extList) {

	StringC ext = GetFileExtension(filename);
	Uint32 extlen = (Uint32)strlen(ext);

	for (const Buffer& ext_ : extList) {
		if (ext_ == ext && extlen > 0 && ext_.length() > 0) {
			return 1;
		}
	}

	return 0;
}

StringC File::GetFileName(StringC filename) {

	Sint32 length = (Sint32)strlen(filename);

	for (Sint32 i = length - 1; i >= 0; i--) {
		if (filename[i] == '\\' || filename[i] == '/') {
			return filename + i + 1;
		}
	}

	return filename;
}

Buffer File::GetFileNameWithoutExtension(StringC filename) {

	Buffer result = GetFileName(filename[0] == LAME_SLASH ? filename + 1 : filename);

	for (Sint32 i = (Sint32)result.length() - 1; i >= 0; i--) {
		if (result[i] == '.') {
			result[i] = '\0';
			break;
		}
	}

	return result;
}

StringC File::GetPathWithoutBegin(StringC filename) {

	Sint32 length = (Sint32)strlen(filename);
	StringC pointer = filename;

	for (Sint32 i = 0; i < length; i++) {
		if (pointer[i] == '\\' || pointer[i] == '/') {
			return pointer + i + 1;
		}
	}

	return filename;
}

Buffer File::GetPathWithoutEnd(StringC filename) {

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

LAME_END2
