#include "ResourceManager.h"

LAME_BEGIN

ResourceManager::Resource ResourceManager::Load(StringC fileName, StringC tagName) {

	if (this->resourceMap_.count(tagName)) {
		PostErrorMessage("Resource with that name already exists (%s)", tagName);
	}

	Resource resource(new ::lame::Resource);

	resource->Load(fileName);
	this->resourceMap_[tagName] = resource;;

	return resource;
}

Bool ResourceManager::Erase(StringC tagName) {

	if (!this->resourceMap_.count(tagName)) {
		return LAME_FALSE;
	}

	this->resourceMap_.erase(
		this->resourceMap_.find(tagName));

	return LAME_TRUE;
}

ResourceManager::Resource ResourceManager::Find(StringC tagName) {

	if (!this->resourceMap_.count(tagName)) {
		return Resource();
	}

	return this->resourceMap_.at(tagName);
}

LAME_END