#include "ResourceManager.h"

LAME_BEGIN2(ResourceManager)

ResourceContainer::Resource ResourceContainer::Load(StringC fileName, StringC tagName) {

	if (this->resourceMap_.count(tagName)) {
		PostErrorMessage("Resource with that name already exists (%s)", tagName);
	}

	Resource resource(new ::LAME_NAMESPACE::ResourceManager::Resource());

	resource->Load(fileName);
	this->resourceMap_[tagName] = resource;;

	return resource;
}

Bool ResourceContainer::Erase(StringC tagName) {

	if (!this->resourceMap_.count(tagName)) {
		return LAME_FALSE;
	}

	this->resourceMap_.erase(
		this->resourceMap_.find(tagName));

	return LAME_TRUE;
}

ResourceContainer::Resource ResourceContainer::Find(StringC tagName) {

	if (!this->resourceMap_.count(tagName)) {
		return Resource();
	}

	return this->resourceMap_.at(tagName);
}

LAME_END2
