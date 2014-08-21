#ifndef __LAME_SCRIPT_OBJECT__
#define __LAME_SCRIPT_OBJECT__

#include "Scope.h"

LAME_BEGIN2(Script)

class LAME_API Object :
	public Hashable64,
	public Observable<Object>,
	public Scope
{
	friend class Segment;
public: /* Flags & States */
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
        Object2    = 0x040000,
		Enum       = 0x080000
	};
	enum class Type : Uint32 {
		Unknown    = 0x000000,
		Class      = 0x000001,
		Variable   = 0x000002,
		Method     = 0x000003,
		Interface  = 0x000004
	};
private: /* Constant Variables */
	const Buffer name;
	const Buffer path;
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
	inline BufferRefC GetName()       const { return this->name;      }
	inline Type       GetType()       const { return this->type;      }
	inline SegmentPtr GetSegment()    const { return this->segment_;  }
	inline Uint32     GetAddress()    const { return this->address_;  }
	inline Uint32     GetSize()       const { return this->size_;     }
	inline ClassPtr   GetTemplate()         { return this->template_; }
	inline BufferRefC GetPath()       const { return this->path;      }
	inline NodePtr    GetNode()             { return this->node_;     }
	inline Uint32     GetPosition()   const { return this->position_; }
private: /* Private Variables */
	Uint32     modificators_;
	SegmentPtr segment_;
	Uint32     address_;
	Uint32     size_;
	ClassPtr   template_;
	NodePtr    node_;
	Uint32     position_;
public:
	Bool wasInStack;
public:
	enum {
		SizeOf = 8
	};
};

LAME_END2

#endif // ~__LAME_SCRIPT_OBJECT__