#ifndef __LAME_SCRIPT__INTERFACE__
#define __LAME_SCRIPT__INTERFACE__

#include "Object.h"
#include "Class.h"

LAME_BEGIN2(Script)

class LAME_API Interface : public Object {
public:
	Interface(BufferRefC name, ScopePtr parent);
public:
	Void Implement(InterfacePtr object);
public:
	Bool Equal(ObjectPtrC object) override;
	ObjectPtr Clone(BufferRefC name, ObjectPtr parent) override;
	Void Trace(Uint32 offset) override;
	HashType Hash(Void) override;
	Uint32 Size(Void) override;
	Void Release(Void) override;
public:
	inline Void SetInheritor(ClassPtr inheritor) {
		this->inheritor = inheritor;
	}
	inline ClassPtr GetInheritor() {
		return this->inheritor;
	}
	inline InterfacePtr GetInterface() {
		return this;
	}
private:
	ClassPtr inheritor;
	Vector<InterfacePtr> implemented;
};

LAME_END2

#endif // ~__LAME_SCRIPT__INTERFACE__