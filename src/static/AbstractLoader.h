#ifndef __LAME_STATIC_ABSTRACT_LOADER__
#define __LAME_STATIC_ABSTRACT_LOADER__

#include "Foundation.h"

typedef class LameStaticAbstractLoader {
public:
	LameStaticAbstractLoader(StringPtrC filename):
		filename(filename)
	{
	}
public:
	virtual Logic load() = 0;
	virtual Logic save() = 0;
public:
	inline StringPtrC getName() const {
		return this->filename;
	}
private:
	StringPtrC filename;
} *LameStaticAbstractLoaderPtr;

#endif // ~__LAME_STATIC_ABSTRACT_LOADER__