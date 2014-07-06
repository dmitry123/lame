#ifndef __LAME_RESOURCE_MANAGER__
#define __LAME_RESOURCE_MANAGER__

#include "Define.h"
#include "AudioLoader.h"
#include "Define.h"
#include "Resource.h"
#include "TextureLoader.h"

LAME_BEGIN2(ResourceManager)

typedef SharedPtr<Resource> SharedResourcePtr;

typedef class LAME_API ResourceContainer {
	typedef std::map<Buffer, SharedPtr<Resource>> ResourceMap;
public:
	typedef SharedPtr<Resource> Resource;
public:
	Resource Load(StringC fileName, StringC tagName);
	Bool Erase(StringC tagName);
	Resource Find(StringC tagName);
private:
	ResourceMap resourceMap_;
} *ResourceManagerPtr;

LAME_END2

#endif // ~__LAME_RESOURCE_MANAGER__