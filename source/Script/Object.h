#ifndef __LAME_SCRIPT__OBJECT__
#define __LAME_SCRIPT__OBJECT__

#include "Define.h"
#include "Error.h"

LAME_BEGIN2(Script)

class LAME_API Object {
	friend class Scope;
public:
	enum {
		SizeOf = 8
	};
public:
	enum class Type : Uint32 {
		Unknown,
		Class,
		Variable,
		Method,
		Interface,
		Abstract,
		Array
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
		Abstract = 0x0080,
		Register = 0x0100,
		Internal = 0x0200
	};
private:
	const Buffer name;
	const Uint32 hash;
	const Type type;
	const NodePtr node;
public:
	Object(BufferRefC name, Type type, NodePtr node) :
		name(name),
		hash(name.GetHash()),
		type(type),
		node(node)
	{
		this->scopeController_ = 0;
		this->modificators_ = 0;
		this->sizeOf_ = SizeOf;
		this->segment_ = 0;
		this->address_ = 0;
		this->size_ = 0;
	}
public:
	Object(Object& object);
public:
	virtual ~Object();
public:
	virtual Bool Equal(ObjectPtrC object) const;
	virtual Error Clone(ObjectPtrC object);
	virtual Void Trace(Uint32 offset);
	virtual Uint64 Hash(Void) const;
public:
	inline virtual ClassPtr GetClass() { return NULL; }
	inline virtual VariablePtr GetVariable() { return NULL; }
	inline virtual MethodPtr GetMethod() { return NULL; }
	inline virtual InterfacePtr GetInterface() { return NULL; }
	inline virtual ArrayPtr GetArray() { return NULL; }
public:
	ObjectPtr SetModificator(Modificator modificator, Bool state = TRUE);
	Void SetSegment(SegmentPtr segment, Uint32P address);
	Void PrintModificators(Void);
public:
	inline Void SetSizeOf(Uint32 sizeOf) {
		this->sizeOf_ = sizeOf;
	}
	inline Void SetCountOfArguments(Uint32 count) {
		this->countOfArguments_ = count;
	}
	inline Uint32 GetCountOfArguments() {
		return this->countOfArguments_;
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
	inline Uint32 GetNameHash() const { return this->hash; }
	inline Type GetType() const { return this->type; }
	inline Uint32 GetSizeOf() const { return this->sizeOf_; }
	inline NodePtr GetNode() const { return this->node; }
	inline SegmentPtr GetSegment() const { return this->segment_; }
	inline Uint32 GetAddress() const { return *this->address_; }
	inline Uint32 GetSize() const { return this->size_; }
	inline Uint32P GetAddressPtr() { return this->address_; }
public:
	inline ScopeControllerPtr GetScopeController() {
		return scopeController_;
	}
public:
	Bool EnableScopeController(Void);
private:
	Void SetScopeController(ScopeControllerPtr scopeController) {
		this->scopeController_ = scopeController;
	}
private:
	Uint32 modificators_;
	Uint32 sizeOf_;
	ScopeControllerPtr scopeController_;
	Bool isScopeControllerEnabled_;
	Uint32 countOfArguments_;
	SegmentPtr segment_;
	Uint32P address_;
	Uint32 size_;
};

LAME_END2

#endif // ~__LAME_SCRIPT__OBJECT__