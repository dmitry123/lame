#ifndef __LAME_STATIC_TGA_LOADER__
#define __LAME_STATIC_TGA_LOADER__

#include "TextureLoader.h"

typedef class LameStaticTgaLoader : public LameStaticTextureLoader {
public:
	LameStaticTgaLoader(StringPtrC filename) :
		LameStaticTextureLoader(filename)
	{
	}
public:
	Logic load() override;
	Logic save() override;
} *LameStaticTgaLoaderPtr;

#endif // ~__LAME_STATIC_TGA_LOADER__