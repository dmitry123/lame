#ifndef __LAME_RESOURCE_MANAGER__VIRTUAL_LOADER__
#define __LAME_RESOURCE_MANAGER__VIRTUAL_LOADER__

#include "Define.h"

LAME_BEGIN2(ResourceManager)

using namespace Core;

template <class LoaderInfo> class IVirtualLoader {
    typedef LoaderInfo* LoaderInfoPtr;
public:
    typedef IVirtualLoader<LoaderInfo> Self;
    typedef IVirtualLoader<LoaderInfo>* SelfPtr;
    typedef LoaderInfo Info;
    typedef LoaderInfoPtr InfoPtr;
public:
    IVirtualLoader(ResourceType resourceType) {
		this->loaderInfo_.fileType = resourceType;
    }
public:
    virtual ~IVirtualLoader() {
    }
public:
    virtual Bool Check(Buffer fileExtension) {
        return LAME_FALSE;
    }
public:
    virtual Bool Save(StringC fileName) = 0;
    virtual Bool Load(StringC fileName) = 0;
public:
    inline LoaderInfoPtr GetInfo() {
        return &this->loaderInfo_;
    }
    inline Void SetInfo(LoaderInfoPtr loaderInfo) {
        this->loaderInfo_ = *loaderInfo;
    }
public:
    LoaderInfo loaderInfo_;
};

LAME_END2

#endif // ~__LAME_RESOURCE_MANAGER__VIRTUAL_LOADER__