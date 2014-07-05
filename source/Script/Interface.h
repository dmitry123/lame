#ifndef __LAME_SCRIPT__INTERFACE__
#define __LAME_SCRIPT__INTERFACE__

#include "Object.h"
#include "Class.h"

LAME_BEGIN2(Script)

class LAME_API Interface : public Class {
public:
	Interface(BufferRefC name, NodePtr node = NULL) : Class(name, Type::Interface, node),
		inheritor(0)
	{
	}
public:
	Error Implement(InterfacePtr object);
	Void Trace(Uint32 offset) override;
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