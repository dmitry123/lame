#ifndef __LAME_SCRIPT__OBJECT__
#define __LAME_SCRIPT__OBJECT__

#include "Define.h"
#include "Error.h"

LAME_BEGIN2(Script)

class LAME_API ScriptObject {
	friend class ScriptScope;
public:
	enum class Type : Uint32 {
		Unknown,
		Class,
		Variable,
		Method,
		Interface,
		Abstract
	};
public:
	enum class Modificator : Uint32 {
		Unknown = 0x0000,
		Public = 0x0001,
		Private = 0x0002,
		Protected = 0x0004,
		Static = 0x0008,
		Final = 0x0010,
		Native = 0x0020,
		Primitive = 0x0040,
		Abstract = 0x0080
	};
private:
	const Buffer name;
	const Uint32 hash;
	const Type   type;
public:
	ScriptObject(BufferRefC name, Type type) :
		name(name),
		hash(name.GetHash()),
		type(type)
	{
		this->scopeController_ = 0;
		this->modificators_ = 0;
		this->sizeOf_ = 0;
	}
public:
	ScriptObject(ScriptObject& object);
public:
	virtual ~ScriptObject();
public:
	virtual Bool Equal(ScriptObjectPtrC object) const {
		if (this->type == Type::Class) {
			return this->type == object->type && this->hash == object->hash;
		} else {
			return this->type == object->type;
		}
	}
public:
	virtual ScriptError Cast(ScriptObjectPtrC object) {
		return ScriptError::Object_UnableToCast;
	}
	virtual ScriptError Clone(ScriptObjectPtrC object) {
		return ScriptError::Object_UnableToClone;
	}
	virtual Void Trace(Uint32 offset) {
		// ignore
	}
public:
	inline virtual ScriptClassPtr GetClass() {
		return NULL;
	}
	inline virtual ScriptVarPtr GetVariable() {
		return NULL;
	}
	inline virtual ScriptMethodPtr GetMethod() {
		return NULL;
	}
	inline virtual ScriptInterfacePtr GetInterface() {
		return NULL;
	}
public:
	ScriptObjectPtr SetModificator(Modificator modificator, Bool state = TRUE);
public:
	inline Void SetSizeOf(Uint32 sizeOf) {
		this->sizeOf_ = sizeOf;
	}
public:
	inline Bool CheckModificator(Modificator modificator) const {
		return (this->modificators_ & (Uint32)modificator) != 0;
	}
	inline Bool CheckType(Type type) const {
		return this->type == type;
	}
public:
	inline BufferRefC GetName() const { return this->name; }
	inline Uint32 GetHash() const { return this->hash; }
	inline Type GetType() const { return this->type; }
	inline Uint32 GetSizeOf() const { return this->sizeOf_; }
public:
	inline ScriptScopeControllerPtr GetScopeController() {
		return scopeController_;
	}
public:
	Bool EnableScopeController(Void);
private:
	Void SetScopeController(ScriptScopeControllerPtr scopeController) {
		this->scopeController_ = scopeController;
	}
private:
	Uint32 modificators_;
	Uint32 sizeOf_;
	ScriptScopeControllerPtr scopeController_;
	Bool isScopeControllerEnabled_;
};

LAME_END2

#endif // ~__LAME_SCRIPT__OBJECT__