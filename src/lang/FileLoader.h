#ifndef __LAME_LANG__FILE_LOADER__
#define __LAME_LANG__FILE_LOADER__

#include "AbstractLoader.h"

LAME_LANG_BEGIN

typedef class FileLoader : public AbstractLoader {
public:
	FileLoader(StringPtrC file) :
		name(strdup(file))
	{
	}
	~FileLoader() {
		delete this->name;
	}
public:
	SInt8P load();
public:
	inline StringPtrC getName() const {
		return this->name;
	}
private:
	StringPtr name;
} *FileLoaderPtr;

LAME_LANG_END

#endif // ~__LAME_LANG__FILE_LOADER__