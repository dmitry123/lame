#ifndef __LAME_RESOURCE_MANAGER__RESOURCE__
#define __LAME_RESOURCE_MANAGER__RESOURCE__

#include "TextureLoader.h"
#include "AudioLoader.h"
#include "VirtualLoader.h"

LAME_BEGIN2(ResourceManager)

typedef class LAME_API Resource {
public:
	Resource() {
		this->resourceLoader_.audioLoader_ = 0;
	}
public:
	~Resource();
public:
	Bool Load(StringC fileName);
	Bool Save(StringC fileName);
public:
	ResourceAudioInfoPtr GetAudioInfo() {
        return this->resourceLoader_.audioLoader_->GetInfo();
    }
	ResourceTextureInfoPtr GetTextureInfo() {
        return this->resourceLoader_.textureLoader_->GetInfo();
    }
private:
	union {
		IAudioLoaderPtr audioLoader_;
		ITextureLoaderPtr textureLoader_;
	} resourceLoader_;
} *ResourcePtr;

LAME_END2

#endif // ~__LAME_RESOURCE_MANAGER__RESOURCE__