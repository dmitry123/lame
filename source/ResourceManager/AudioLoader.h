#ifndef __LAME_RESOURCE_MANAGER__AUDIO_LOADER__
#define __LAME_RESOURCE_MANAGER__AUDIO_LOADER__

#include "VirtualLoader.h"

LAME_BEGIN2(ResourceManager)

class IAudioLoader : public IVirtualLoader<ResourceAudioInfo> {
protected:
	IAudioLoader(ResourceType resourceType) :
        IVirtualLoader(resourceType)
    {
	}
public:
	virtual ~IAudioLoader() {
		Free(this->loaderInfo_.audioData);
	}
};

typedef IAudioLoader::SelfPtr IAudioLoaderPtr;

typedef class WavLoader : public IAudioLoader {
public:
	WavLoader() :
		IAudioLoader(kResourceTypeWav)
	{
	}
public:
    inline Bool Check(Buffer fileExtension) {
        return fileExtension == "wav";
    }
public:
	Bool Load(StringC fileName);
	Bool Save(StringC fileName);
} *WavLoaderPtr;

LAME_END2

#endif // ~__LAME_RESOURCE_MANAGER__AUDIO_LOADER__