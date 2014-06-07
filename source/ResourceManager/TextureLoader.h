#ifndef __LAME_RESOURCE_MANAGER__TEXTURE_LOADER__
#define __LAME_RESOURCE_MANAGER__TEXTURE_LOADER__

#include "VirtualLoader.h"

LAME_BEGIN

class ITextureLoader : public IVirtualLoader<ResourceTextureInfo> {
protected:
	ITextureLoader(ResourceType resourceType) :
        IVirtualLoader(resourceType)
    {
	}
public:
	virtual ~ITextureLoader() {
		Free(this->loaderInfo_.imageData);
	}
};

typedef ITextureLoader::SelfPtr ITextureLoaderPtr;

typedef class JpgLoader : public ITextureLoader {
public:
	JpgLoader() :
		ITextureLoader(kResourceTypeJpg)
	{
	}
public:
    inline Bool Check(Buffer fileExtension) {
        return fileExtension == "jpg";
    }
public:
	Bool Load(StringC fileName);
	Bool Save(StringC fileName);
} *JpgLoaderPtr;

typedef class PngLoader : public ITextureLoader {
public:
	PngLoader() :
		ITextureLoader(kResourceTypePng)
	{
	}
public:
    inline Bool Check(Buffer fileExtension) {
        return fileExtension == "png";
    }
public:
	Bool Load(StringC fileName);
	Bool Save(StringC fileName);
} *PngLoaderPtr;

typedef class TgaLoader : public ITextureLoader {
public:
	TgaLoader() :
		ITextureLoader(kResourceTypeTga)
	{
	}
public:
    inline Bool Check(Buffer fileExtension) {
        return fileExtension == "tga";
    }
public:
	Bool Load(StringC fileName);
	Bool Save(StringC fileName);
} *TgaLoaderPtr;

typedef class BmpLoader : public ITextureLoader {
public:
	BmpLoader() :
		ITextureLoader(kResourceTypeBmp)
	{
	}
public:
    inline Bool Check(Buffer fileExtension) {
        return fileExtension == "bmp";
    }
public:
	Bool Load(StringC fileName);
	Bool Save(StringC fileName);
} *BmpLoaderPtr;

LAME_END

#endif // ~__LAME_RESOURCE_MANAGER__TEXTURE_LOADER__