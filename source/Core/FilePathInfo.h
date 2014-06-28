#ifndef __LAME_CORE__FILE_SYSTEM__
#define __LAME_CORE__FILE_SYSTEM__

#include "File.h"

LAME_BEGIN2(Core)

class LAME_API FilePathInfo {
public:
	FilePathInfo() {
	}
public:
	FilePathInfo(StringC buffer) {
		this->Parse(buffer);
	}
public:
	Void Parse(StringC buffer);
	Void Create(Void);
public:
	inline operator StringC() const {
		return this->buffer.data();
	}
public:
	Buffer buffer;
	Buffer path;
	Buffer name;
	Buffer extension;
};

LAME_END2

#endif // ~__LAME_CORE__FILE_SYSTEM__