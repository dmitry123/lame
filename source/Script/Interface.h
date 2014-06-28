#ifndef __LAME_SCRIPT__INTERFACE__
#define __LAME_SCRIPT__INTERFACE__

#include "Object.h"

LAME_BEGIN2(Script)

class LAME_API ScriptInterface : public ScriptObject {
public:
	ScriptInterface(BufferRefC name) : ScriptObject(name, Type::Interface),
		inheritor(0)
	{
		this->EnableScopeController();
	}
public:
	ScriptError Implement(ScriptInterfacePtr object);
	Void Trace(Uint32 offset) override;
public:
	inline Void SetInheritor(ScriptClassPtr inheritor) {
		this->inheritor = inheritor;
	}
	inline ScriptClassPtr GetInheritor() {
		return this->inheritor;
	}
	inline ScriptInterfacePtr GetInterface() {
		return this;
	}
private:
	ScriptClassPtr inheritor;
	Vector<ScriptInterfacePtr> implemented;
};

LAME_END2

#endif // ~__LAME_SCRIPT__INTERFACE__