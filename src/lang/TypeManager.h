#ifndef __LAME_LANG__TYPE_MANAGER__
#define __LAME_LANG__TYPE_MANAGER__

#include "Object.h"

LAME_LANG_BEGIN

typedef class TypeManager {
public:
	TypeManager(ObjectPtr scope = nullptr);
public:
	~TypeManager() {
		delete this->scope;
	}
public:
	ObjectPtr declare(NodePtr node);
public:
	inline Logic has(StringPtrC name) {
		return this->scope->find(name) != nullptr;
	}
public:
	inline ObjectPtr declare(ObjectPtr object) {
		return this->declare(object->getNode());
	}
public:
	inline ObjectPtr operator -> () const {
		return this->scope;
	}
public:
	inline ObjectPtr getScope() {
		return this->scope;
	}
	inline void setScope(ObjectPtr scope) {
		if (this->scope != nullptr) {
			this->scope->merge(scope);
		}
		else {
			this->scope = scope;
		}
	}
public:
	ObjectPtr scope;
} *TypeManagerPtr;

LAME_LANG_END

#endif // ~__LAME_LANG__TYPE_MANAGER__