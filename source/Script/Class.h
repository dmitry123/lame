#ifndef __LAME_SCRIPT__CLASS__
#define __LAME_SCRIPT__CLASS__

#include "Object.h"

LAME_BEGIN2(Script)

class LAME_API Class : public Object {
	friend class Variable;
public:
	typedef Void(*OperatorCallback)(
		VariablePtr source,
		VariablePtr left,
		VariablePtr right);
	typedef OperatorCallback* OperatorCallbackPtr;
public:
	enum class Operator : Uint32 {
		Unkown, Add,     Sub,    Mul,    Div,
		Mod,    Above,   Bellow, Equal,  NotEqual,
		Move,   ShiftR,  ShiftL, BitAnd, BitOr,
		BitXor, BellowE, AboveE, And,    Or,
		Inc,    Dec,     Sizeof, BitNot, Not,
		Cast
	};
public:
	Class(BufferRefC name, ScopePtr parent, Uint32 size = SizeOf);
	~Class();
public:
	Bool Equal(ObjectPtrC object) override;
	ObjectPtr Clone(BufferRefC name) override;
	Void Trace(Uint32 offset) override;
	HashType Hash(Void) override;
	Uint32 Size(Void) override;
	Void Release(Void) override;
	ClassPtr GetClass(Void) override;
public:
	inline Uint32 GetReferences() const { return *this->references; }
	inline ObjectPtr GetExtend() { return this->extendClass; }
	inline Set<ObjectPtr>& GetImplements() { return this->implementClass; }
	inline Uint32 GetPriority() const { return this->priority; }
public:
	inline Bool DecRef(Void) {
		return --(*this->references) == 0;
	}
	inline Void IncRef(Void) {
		++(*this->references);
	}
	inline Void SetPriority(Uint32 priority) {
		this->priority = priority;
	}
	inline Uint32 GetPriority() {
		return this->priority;
	}
	inline OperatorCallback* GetOperators() {
		return this->operators;
	}
public:
	Void Extend(ObjectPtr object);
	Void Implement(ObjectPtr object);
	Void New(ObjectPtr object);
	Void Evaluate(Operator op, VariablePtr source, VariablePtr left, VariablePtr right, LexPtrC lex);
	Void Overload(Operator op, OperatorCallback callback);
private:
	enum {
		OperatorAmount = 26
	};
private:
	ObjectPtr extendClass;
	Set<ObjectPtr> implementClass;
	Uint32P references;
	OperatorCallback operators[OperatorAmount];
	Uint32 priority;
	Uint32 size;
};

LAME_END2

#endif // ~__LAME_SCRIPT__CLASS__