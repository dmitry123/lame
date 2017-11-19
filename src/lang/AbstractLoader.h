#ifndef __LAME_LANG__ABSTRACT_LOADER__
#define __LAME_LANG__ABSTRACT_LOADER__

#include "Foundation.h"

LAME_LANG_BEGIN

typedef class AbstractLoader {
public:
	virtual SInt8P load() = 0;
} *AbstractLoaderPtr;

LAME_LANG_END

#endif // ~__LAME_LANG__ABSTRACT_LOADER__