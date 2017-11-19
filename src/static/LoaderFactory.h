#ifndef __LAME_STATIC_LOADER_FACTORY__
#define __LAME_STATIC_LOADER_FACTORY__

#include "Foundation.h"
#include "AbstractLoader.h"

typedef class LameStaticLoaderFactory {
	typedef LameStaticLoaderFactory* LameStaticLoaderFactoryP;
public:
	LameStaticAbstractLoaderPtr createByName(StringPtrC filename);
public:
	static LameStaticLoaderFactoryP getFactory();
private:
	Vector<LameStaticAbstractLoaderPtr> loaders;
} *LameStaticLoaderFactoryPtr;

#endif // ~__LAME_STATIC_LOADER_FACTORY__