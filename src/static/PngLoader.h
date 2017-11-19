#ifndef __LAME_STATIC_PNG_LOADER__
#define __LAME_STATIC_PNG_LOADER__

#include "TextureLoader.h"

typedef class LameStaticPngLoader : public LameStaticTextureLoader {
public:
	LameStaticPngLoader(StringPtrC filename) :
		LameStaticTextureLoader(filename)
	{
	}
public:
	Logic load() override;
	Logic save() override;
} *LameStaticPngLoaderPtr;

#endif // ~__LAME_STATIC_PNG_LOADER__