#ifndef __LAME_SCRIPT__SCOPE_CONTROLLER__
#define __LAME_SCRIPT__SCOPE_CONTROLLER__

#include "Define.h"
#include "Scope.h"
#include "Object.h"

LAME_BEGIN2(Script)

class LAME_API ScriptScopeController {
	typedef ScriptObject::Type Type;
public:
	ScriptScopePtr Get(Type type);
public:
	inline ScriptObjectPtr Add(Type type, ScriptObjectPtr object) {
		return this->Get(type)->Add(object);
	}
	inline Void Remove(Type type, ScriptObjectPtr object) {
		this->Get(type)->Remove(object);
	}
	inline ScriptObjectPtr Find(Type type, StringC name) {
		return this->Get(type)->Find(name);
	}
public:
	inline ScriptScopePtr GetClassScope() {
		return &classScope_;
	}
	inline ScriptScopePtr GetVarScope() {
		return &varScope_;
	}
	inline ScriptScopePtr GetMethodScope() {
		return &methodScope_;
	}
	inline ScriptScopePtr GetTempScope() {
		return &tempScope_;
	}
public:
	Void Merge(ScriptScopeControllerPtr sc);
	Void Clone(ScriptScopeControllerPtr sc);
	Uint32 Amount(Void);
	Void Trace(Uint32 offset = 0);
	Void Clear(Void);
private:
	ScriptScope classScope_;
	ScriptScope varScope_;
	ScriptScope methodScope_;
	ScriptScope tempScope_;
};

LAME_END2

#endif // ~__LAME_SCRIPT__SCOPE_CONTROLLER__