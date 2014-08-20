#ifndef __LAME_SCRIPT_OBJECT__
#define __LAME_SCRIPT_OBJECT__

#include "Scope.h"

LAME_BEGIN2(Script)

class LAME_API Object : public Hashable64, public Observable<Object>, public Scope {
public:
	enum class Modificator : Uint32 {
		Unknown    = 0x000000,
		Public     = 0x000001,
		Private    = 0x000002,
		Protected  = 0x000004,
		Static     = 0x000008,
		Final      = 0x000010,
		Native     = 0x000020,
		Primitive  = 0x000040,
		Abstract   = 0x000080,
		Register   = 0x000100,
		Internal   = 0x000200,
		Constant   = 0x000400,
		Override   = 0x000800,
		Deprecated = 0x001000,
		Array      = 0x002000,
        Float      = 0x004000,
        Integer    = 0x008000,
        Boolean    = 0x010000,
        String     = 0x020000,
        Object2    = 0x040000
	};
	enum class Type : Uint32 {
		Unknown    = 0x000000,
		Class      = 0x000001,
		Variable   = 0x000002,
		Method     = 0x000003,
		Interface  = 0x000004
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
	virtual ObjectPtr Clone(BufferRefC name, ObjectPtr parent) = 0;
	virtual Void Trace(Uint32 offset) = 0;
	virtual HashType Hash(Void) = 0;
	virtual Uint32 Size(Void) = 0;
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
	inline Void SetNewNode(NodePtr node) {
		this->newNode_ = node;
	}
public:
    inline Bool IsIntegerLike() const {
        return this->CheckModificator(Modificator::Integer);
    }
    inline Bool IsFloatLike() const {
        return this->CheckModificator(Modificator::Float);
    }
    inline Bool IsStringLike() const {
        return this->CheckModificator(Modificator::String);
    }
    inline Bool IsBooleanLike() const {
        return this->CheckModificator(Modificator::Boolean);
    }
    inline Bool IsObjectLike() const {
		return this->CheckModificator(Modificator::Object2);
    }
public:
	inline Bool IsInt() const {
		return
			ClassPtr(this) == this->classChar ||
			ClassPtr(this) == this->classByte ||
			ClassPtr(this) == this->classBoolean ||
			ClassPtr(this) == this->classShort ||
			ClassPtr(this) == this->classInt;
	}
	inline Bool IsChar() const {
		return ClassPtr(this) == this->classChar;
	}
	inline Bool IsByte() const {
		return ClassPtr(this) == this->classByte;
	}
	inline Bool IsBoolean() const {
		return ClassPtr(this) == this->classBoolean;
	}
	inline Bool IsShort() const {
		return ClassPtr(this) == this->classShort;
	}
	inline Bool IsInt8() const {
		return
			ClassPtr(this) == this->classChar ||
			ClassPtr(this) == this->classByte;
	}
	inline Bool IsInt16() const {
		return ClassPtr(this) == this->classShort;
	}
	inline Bool IsInt32() const {
		return ClassPtr(this) == this->classInt;
	}
	inline Bool IsInt64() const {
		return ClassPtr(this) == this->classLong;
	}
	inline Bool IsLong() const {
		return ClassPtr(this) == this->classLong;
	}
	inline Bool IsFloat() const {
		return ClassPtr(this) == this->classFloat;
	}
	inline Bool IsDouble() const {
		return ClassPtr(this) == this->classDouble;
	}
	inline Bool IsString() const {
		return ClassPtr(this) == this->classString;
	}
	inline Bool IsVoid() const {
		return ClassPtr(this) == this->classVoid;
	}
	inline Bool IsObject() const {
		return
			!this->IsInt() &&
			!this->IsLong() &&
			!this->IsFloat() &&
			!this->IsDouble() &&
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
	inline NodePtr GetNewNode() { return this->newNode_; }
private:
	Uint32 modificators_;
	Uint32 arguments_;
	SegmentPtr segment_;
	Uint32P address_;
	Uint32 size_;
	ClassPtr templateClass;
	NodePtr node_;
	NodePtr newNode_;
public:
	Uint32 segmentCodePosition;
public:
	enum {
		SizeOf = 8
	};
};

LAME_END2

#endif // ~__LAME_SCRIPT_OBJECT__