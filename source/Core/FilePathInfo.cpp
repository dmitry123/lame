#include "FilePathInfo.h"
#include "File.h"

LAME_BEGIN2(Core)

Void FilePathInfo::Parse(StringC buffer) {

	this->name = File::GetFileNameWithoutExtension(buffer);
	this->extension = File::GetFileExtension(buffer);
	this->path = File::GetPathWithoutEnd(buffer);
	this->buffer = File::NormalizePath(buffer);
}

Void FilePathInfo::Create() {

	this->buffer.clear();
	this->buffer += this->path;
	this->buffer += LAME_SLASH;
	this->buffer += this->name;
	this->buffer += '.';
	this->buffer += this->extension;
}

LAME_END2
