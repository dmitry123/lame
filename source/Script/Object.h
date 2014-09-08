#ifndef __LAME_SCRIPT_OBJECT__
#define __LAME_SCRIPT_OBJECT__

#include "Scope.h"

LAME_BEGIN2(Script)

class LAME_API Object :
	public Hashable64,
	public Scope
{
	friend class Segment;
	friend class ScopeBuilder;
public: /* Flags & States */
	enum class Modificator : Uint32 {
		Unknown      = 0x00000000,
		Public       = 0x00000001,
		Private      = 0x00000002,
		Protected    = 0x00000004,
		Static       = 0x00000008,
		Final        = 0x00000010,
		Native       = 0x00000020,
		Primitive    = 0x00000040,
		Abstract     = 0x00000080,
		Register     = 0x00000100,
		Internal     = 0x00000200,
		Constant     = 0x00000400,
		Override     = 0x00000800,
		Deprecated   = 0x00001000,
		Array        = 0x00002000,
        Float        = 0x00004000,
        Integer      = 0x00008000,
        Boolean      = 0x00010000,
        String       = 0x00020000,
        Object2      = 0x00040000,
		Enum         = 0x00080000,
		Synchronized = 0x00100000,
		Construction = 0x00200000,
		Implemented  = 0x00400000
	};
	enum class Type : Uint32 {
		Unknown      = 0x00000000,
		Class        = 0x00000001,
		Variable     = 0x00000002,
		Method       = 0x00000003,
		Interface    = 0x00000004
	};
private: /* Constant Variables */
	      Buffer name;
	      Buffer path;
	const Type   type;
public:
	Object(
		/* Name of the scope */
		BufferRefC name,
		/* Object's parent scope */
		ScopePtr   parent,
		/* Class type */
		Type       type);
public: /* Strict Virtual Methods */
	virtual Bool Equal(ObjectPtrC object) = 0;
	virtual ObjectPtr Clone(BufferRefC name, ObjectPtr parent) = 0;
	virtual Void Trace(Uint32 offset) = 0;
	virtual HashType Hash(Void) = 0;
	virtual Uint32 Size(Void) = 0;
public: /* Weak virtual Methods */
	virtual ClassPtr     GetClass()     { return NULL; }
	virtual VariablePtr  GetVariable()  { return NULL; }
	virtual MethodPtr    GetMethod()    { return NULL; }
	virtual InterfacePtr GetInterface() { return NULL; }
public: /* Setters */
	ObjectPtr SetModificator(Modificator modificator,
		Bool state = TRUE);
public:
	inline Void SetTemplate(ClassPtr tamplate) { this->template_ = tamplate; }
	inline Void SetNode(NodePtr node)          { this->node_     = node;     }
	inline Void SetPosition(Uint32 position)   { this->position_ = position; }
	inline Void SetAddress(Uint32 address)     { this->address_  = address;  }
	inline Void SetSegment(SegmentPtr segment) { this->segment_  = segment;  }
	inline Void SetSize(Uint32 size)           { this->size_     = size;     }
	inline Void SetFieldID(Uint32 id)          { this->fieldId_  = id;       }
	inline Void SetThis(ObjectPtr self)        { this->this_     = self;     }
public: /* Getters */
	Buffer GetModificatorString(Void);
	Uint64 GetHash64(Void);
	Uint32 GetHash32(Void);
public: /* Checkers */
	inline Bool CheckModificator(Modificator modificator) const {
		return (this->modificators_ & (Uint32)modificator) != 0;
	}
	inline Bool CheckType(Type type) const {
		return this->type == type;
	}
public:
    inline Bool IsIntegerLike() const { return this->CheckModificator(Modificator::Integer); }
    inline Bool IsFloatLike()   const { return this->CheckModificator(Modificator::Float);   }
    inline Bool IsStringLike()  const { return this->CheckModificator(Modificator::String);  }
    inline Bool IsBooleanLike() const { return this->CheckModificator(Modificator::Boolean); }
    inline Bool IsObjectLike()  const { return this->CheckModificator(Modificator::Object2); }
public:
	Bool IsInt()     const;
	Bool IsChar()    const;
	Bool IsByte()    const;
	Bool IsBoolean() const;
	Bool IsShort()   const;
	Bool IsLong()    const;
	Bool IsFloat()   const;
	Bool IsDouble()  const;
	Bool IsString()  const;
	Bool IsVoid()    const;
	Bool IsObject()  const;
public: /* Inline Getters */
	inline BufferRef  GetName()             { return this->name;      }
	inline Type       GetType()       const { return this->type;      }
	inline SegmentPtr GetSegment()    const { return this->segment_;  }
	inline Uint32     GetAddress()    const { return this->address_;  }
	inline Uint32     GetSize()       const { return this->size_;     }
	inline ClassPtr   GetTemplate()         { return this->template_; }
	inline BufferRefC GetPath()       const { return this->path;      }
	inline NodePtr    GetNode()             { return this->node_;     }
	inline Uint32     GetPosition()   const { return this->position_; }
	inline Uint32     GetFieldID()    const { return this->fieldId_;  }
	inline ObjectPtr  GetThis()       const { return this->this_;     }
public:
	typedef Void(*OnScopeUpdate)(
		ScopePtr scope, ObjectPtr object);
private: /* Private Variables */
	Uint32        modificators_;
	SegmentPtr    segment_;
	Uint32        address_;
	Uint32        size_;
	ClassPtr      template_;
	NodePtr       node_;
	Uint32        position_;
	Uint32        fieldId_;
	ObjectPtr     this_;
	OnScopeUpdate callback_;
public:
	inline Void SetOnScopeUpdate(OnScopeUpdate callback) { this->callback_ = callback; }
	inline OnScopeUpdate GetOnScopeUpdate() { return this->callback_; }
public:
	enum {
		SizeOf = 8
	};
};

LAME_END2

#endif // ~__LAME_SCRIPT_OBJECT__