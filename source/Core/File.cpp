#include "FileSystem.h"

#include <stdio.h>
#include <stdlib.h>

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

typedef struct stat Stat;

LAME_BEGIN

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

	new (this) File();
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

	new (this) File ();
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

LAME_END