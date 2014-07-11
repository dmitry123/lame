#ifndef __LAME_SCRIPT__CLASS__
#define __LAME_SCRIPT__CLASS__

#include "ScopeController.h"
#include "Error.h"

LAME_BEGIN2(Script)

class LAME_API Class : public Object {
	friend class Variable;
	friend class Abstract;
	friend class Interface;
public:
	typedef Void(*OperatorCallback)(
		VariablePtr source,
		VariablePtr left,
		VariablePtr right);
public:
	enum class Operator : Uint32 {
		Unkown, Add,     Sub,    Mul,    Div,
		Mod,    Above,   Bellow, Equal,  NotEqual,
		Move,   ShiftR,  ShiftL, BitAnd, BitOr,
		BitXor, BellowE, AboveE, And,    Or,
		Inc,    Dec,     Sizeof, BitNot, Not,
		Cast
	};
private:
	enum {
		OPERATOR_AMOUNT = 26
	};
private:
	Class(BufferRefC name, Type type, NodePtr node = NULL) : Object(name, type, node),
		extended(0),
		priority(0)
	{
		this->references = new Uint32(0);
		this->EnableScopeController();
	}
public:
	Class(BufferRefC name, NodePtr node = NULL) : Object(name, Type::Class, node),
		extended(0),
		priority(0)
	{
		this->references = new Uint32(0);
		this->EnableScopeController();
	}
public:
	~Class() {
		if (!(*this->references)) {
			delete this->references;
		}
	}
public:
	Error Clone(ObjectPtrC object) override;
	Void Trace(Uint32 offset) override;
public:
	inline VariablePtr FindVariable(BufferRefC name) {
		return (VariablePtr) this->GetScopeController()->GetVarScope()->Find(name);
	}
	inline VariablePtr FindVariable(Uint32 hash) {
		return (VariablePtr) this->GetScopeController()->GetVarScope()->Find(hash);
	}
	inline ClassPtr FindClass(BufferRefC name) {
		return (ClassPtr) this->GetScopeController()->GetClassScope()->Find(name);
	}
	inline ClassPtr FindClass(Uint32 hash) {
		return (ClassPtr) this->GetScopeController()->GetClassScope()->Find(hash);
	}
	inline MethodPtr FindMethod(BufferRefC name, Uint32 invokeHash = -1) {
		return (MethodPtr) this->GetScopeController()->GetMethodScope()->Find(name, invokeHash);
	}
	inline MethodPtr FindMethod(Uint32 hash) {
		return (MethodPtr) this->GetScopeController()->GetMethodScope()->Find(hash);
	}
public:
	inline Uint32 GetReferences() { return *this->references; }
	inline ClassPtr GetExtended() { return this->extended; }
	inline ClassPtr GetClass() { return this; }
	inline Vector<InterfacePtr>& GetImplements() { return this->implemented; }
public:
	Error New(ObjectPtr object);
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
	inline Vector<OperatorCallback>& GetOperators() {
		return this->operators;
	}
public:
	Error Extend(ClassPtr object);
	Error Implement(InterfacePtr object);
	Error Overload(Operator op, OperatorCallback callback);
	Error Evaluate(Operator op, VariablePtr source, VariablePtr left, VariablePtr right, LexPtrC lex);
	Void ComputeSizeOf(Void);
private:
	ClassPtr extended;
	Vector<InterfacePtr> implemented;
	Uint32P references;
	Vector<OperatorCallback> operators;
	Uint32 priority;
};

LAME_END2

#endif // ~__LAME_SCRIPT__CLASS__