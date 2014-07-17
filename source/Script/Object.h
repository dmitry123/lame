#ifndef __LAME_SCRIPT_OBJECT__
#define __LAME_SCRIPT_OBJECT__

#include "Scope.h"

LAME_BEGIN2(Script)

class LAME_API Object : public Hashable64, public Observable<Object>, public Scope {
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
		Internal = 0x0200,
		Constant = 0x0400,
		Override = 0x0800,
		Deprecated = 0x1000
	};
	enum class Type : Uint32 {
		Unknown = 0x0000,
		Class = 0x0001,
		Variable = 0x0002,
		Method = 0x0003,
		Interface = 0x0004
	};
private:
	const Buffer name;
	const Buffer path;
	const Type type;
public:
	Object(BufferRefC name, ScopePtr parent, Type type);
public:
	virtual ~Object();
public:
	virtual Bool Equal(ObjectPtrC object) = 0;
	virtual ObjectPtr Clone(BufferRefC name) = 0;
	virtual Void Trace(Uint32 offset) = 0;
	virtual HashType Hash(Void) = 0;
	virtual Uint32 Size(Void) = 0;
	virtual Void Release(Void) = 0;
public:
	virtual ClassPtr GetClass() { return NULL; }
	virtual VariablePtr GetVariable() { return NULL; }
	virtual MethodPtr GetMethod() { return NULL; }
	virtual InterfacePtr GetInterface() { return NULL; }
public:
	ObjectPtr SetModificator(Modificator modificator, Bool state = TRUE);
	ObjectPtr SetSegment(SegmentPtr segment, Uint32P address);
public:
	Void PrintModificators(Void);
	Uint64 GetPathHash64(Void);
	Uint32 GetPathHash32(Void);
	Uint64 GetHash64(Void);
	Uint32 GetHash32(Void);
public:
	inline Bool CheckModificator(Modificator modificator) const {
		return (this->modificators_ & (Uint32)modificator) != 0;
	}
	inline Bool CheckType(Type type) const {
		return this->type == type;
	}
	inline Void SetCountOfArguments(Uint32 count) {
		this->arguments_ = count;
	}
	inline Void SetTemplateClass(ClassPtr templateClass) {
		this->templateClass = templateClass;
	}
	inline Void SetNode(NodePtr node) {
		this->node_ = node;
	}
	inline NodePtr GetNode() {
		return this->node_;
	}
public:
	inline Bool IsInt() const {
		return
			ClassPtr(this) == this->classChar ||
			ClassPtr(this) == this->classByte ||
			ClassPtr(this) == this->classBoolean ||
			ClassPtr(this) == this->classShort ||
			ClassPtr(this) == this->classInt ||
			ClassPtr(this) == this->classLong;
	}
	inline Bool IsFloat() const {
		return
			ClassPtr(this) == this->classFloat ||
			ClassPtr(this) == this->classDouble;
	}
	inline Bool IsString() const {
		return
			ClassPtr(this) == this->classString;
	}
	inline Bool IsVoid() const {
		return
			ClassPtr(this) == this->classVoid;
	}
	inline Bool IsObject() const {
		return
			!this->IsInt() &&
			!this->IsFloat() &&
			!this->IsString() &&
			!this->IsVoid();
	}
public:
	inline BufferRefC GetName() const { return this->name; }
	inline Type GetType() const { return this->type; }
	inline SegmentPtr GetSegment() const { return this->segment_; }
	inline Uint32 GetAddress() const { return *this->address_; }
	inline Uint32 GetSize() const { return this->size_; }
	inline Uint32P GetAddressPtr() { return this->address_; }
	inline Uint32 GetCountOfArguments() const { return this->arguments_; }
	inline ClassPtr GetTemplateClass() { return this->templateClass; }
	inline BufferRefC GetPath() const { return this->path; }
private:
	Uint32 modificators_;
	Uint32 arguments_;
	SegmentPtr segment_;
	Uint32P address_;
	Uint32 size_;
	ClassPtr templateClass;
	NodePtr node_;
public:
	enum {
		SizeOf = 8
	};
};

LAME_END2

#endif // ~__LAME_SCRIPT_OBJECT__