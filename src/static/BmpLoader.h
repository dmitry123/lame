#ifndef __LAME_STATIC_BMP_LOADER__
#define __LAME_STATIC_BMP_LOADER__

#include "TextureLoader.h"

typedef class LameStaticBmpLoader : public LameStaticTextureLoader {
public:
	LameStaticBmpLoader(StringPtrC filename) :
		LameStaticTextureLoader(filename)
	{
	}
public:
	Logic load() override;
	Logic save() override;
} *LameStaticBmpLoaderPtr;

#endif // ~__LAME_STATIC_BMP_LOADER__