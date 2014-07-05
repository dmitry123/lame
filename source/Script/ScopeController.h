#ifndef __LAME_SCRIPT__SCOPE_CONTROLLER__
#define __LAME_SCRIPT__SCOPE_CONTROLLER__

#include "Define.h"
#include "Scope.h"
#include "Object.h"

LAME_BEGIN2(Script)

class LAME_API ScopeController {
	typedef Object::Type Type;
public:
	ScopePtr Get(Type type);
public:
	inline ObjectPtr Add(Type type, ObjectPtr object) {
		return this->Get(type)->Add(object);
	}
	inline Void Remove(Type type, ObjectPtr object) {
		this->Get(type)->Remove(object);
	}
	inline ObjectPtr Find(Type type, StringC name) {
		return this->Get(type)->Find(name);
	}
public:
	inline ScopePtr GetClassScope() {
		return &classScope_;
	}
	inline ScopePtr GetVarScope() {
		return &varScope_;
	}
	inline ScopePtr GetMethodScope() {
		return &methodScope_;
	}
	inline ScopePtr GetTempScope() {
		return &tempScope_;
	}
public:
	Void Merge(ScopeControllerPtr sc);
	Void Clone(ScopeControllerPtr sc);
	Void Move(ScopeControllerPtr sc);
	Uint32 Amount(Void);
	Void Trace(Uint32 offset = 0, Bool temp = 1);
	Void Clear(Void);
	Uint32 Size(Void);
	Void Write(Uint8P buffer, Uint32P offset);
private:
	Scope classScope_;
	Scope varScope_;
	Scope methodScope_;
	Scope tempScope_;
};

LAME_END2

#endif // ~__LAME_SCRIPT__SCOPE_CONTROLLER__