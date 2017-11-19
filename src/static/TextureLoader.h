#ifndef __LAME_STATIC_TEXTURE_LOADER__
#define __LAME_STATIC_TEXTURE_LOADER__

#include "AbstractLoader.h"

typedef class LameStaticTextureLoader : public LameStaticAbstractLoader {
	typedef LameStaticTextureLoader* LameStaticTextureLoaderPtr;
public:
	LameStaticTextureLoader(StringPtrC filename) :
		LameStaticAbstractLoader(filename)
	{
		memset(&this->data, 0, sizeof(this->data));
	}
public:
	inline static LameStaticTextureLoaderPtr cast(LameStaticAbstractLoaderPtr loader) {
		return dynamic_cast<LameStaticTextureLoaderPtr>(loader);
	}
public:
	inline LameStaticTextureContainer* getData() {
		return &this->data;
	}
	inline void setData(LameStaticTextureContainer* data) {
		memcpy(&this->data, data, sizeof(this->data));
	}
public:
	inline LameStaticTextureLoaderPtr copyDataTo(LameStaticAbstractLoaderPtr dst) {
		LameStaticTextureLoaderPtr copy = cast(dst);
		if (copy != NULL) {
			memcpy(&copy->data, &this->data, sizeof(this->data));
		}
		return copy;
	}
private:
	LameStaticTextureContainer data;
} *LameStaticTextureLoaderPtr;

#endif // ~__LAME_STATIC_TEXTURE_LOADER__