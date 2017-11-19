#ifndef __LAME_STATIC_TEXTURE_RESOURCE__
#define __LAME_STATIC_TEXTURE_RESOURCE__

#include "Resource.h"
#include "TextureLoader.h"

class LameStaticTextureResource : public LameStaticResource<LameStaticTextureContainer> {
public:
	LameStaticTextureResource(StringPtrC filename) :
		LameStaticResource(filename, this->TYPE_TEXTURE)
	{
	}
};

#endif // ~__LAME_STATIC_TEXTURE_RESOURCE__