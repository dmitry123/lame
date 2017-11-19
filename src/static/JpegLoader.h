#ifndef __LAME_STATIC_JPEG_LOADER__
#define __LAME_STATIC_JPEG_LOADER__

#include "TextureLoader.h"

typedef class LameStaticJpegLoader : public LameStaticTextureLoader {
public:
	LameStaticJpegLoader(StringPtrC filename) :
		LameStaticTextureLoader(filename)
	{
	}
public:
	Logic load() override;
	Logic save() override;
} *LameStaticJpegLoaderPtr;

#endif // ~__LAME_STATIC_JPEG_LOADER__