#ifndef __LAME_SCRIPT__CLASS__
#define __LAME_SCRIPT__CLASS__

#include "ScopeController.h"
#include "Error.h"

LAME_BEGIN2(Script)

class LAME_API ScriptClass : public ScriptObject {
	friend class ScriptVar;
	friend class ScriptAbstract;
public:
	typedef Void(*OperatorCallback)(
		ScriptVarPtr left,
		ScriptVarPtr right);
public:
	enum class Operator : Uint32 {
		Unkown, Add,     Sub,    Mul,    Div,
		Mod,    Above,   Bellow, Equal,  NotEqual,
		Move,   ShiftR,  ShiftL, BitAnd, BitOr,
		BitXor, BellowE, AboveE, And,    Or,
		Inc,    Dec,     Sizeof, BitNot, Not
	};
private:
	enum {
		OPERATOR_AMOUNT = 25
	};
private:
	ScriptClass(BufferRefC name, Type type) : ScriptObject(name, type),
		extended(0),
		references(0)
	{
		this->EnableScopeController();
		references = new Uint32(1);
	}
public:
	ScriptClass(BufferRefC name) : ScriptObject(name, Type::Class),
		extended(0),
		references(0)
	{
		this->EnableScopeController();
		references = new Uint32(1);
	}
public:
	~ScriptClass() {
		if (this->references && !(*this->references)) {
			delete this->references;
		}
	}
public:
	ScriptError Clone(ScriptObjectPtrC object) override;
	ScriptError Cast(ScriptObjectPtrC object) override;
	Void Trace(Uint32 offset) override;
public:
	inline ScriptVarPtr FindVariable(BufferRefC name) {
		return (ScriptVarPtr) this->GetScopeController()->GetVarScope()->Find(name);
	}
	inline ScriptVarPtr FindVariable(Uint32 hash) {
		return (ScriptVarPtr) this->GetScopeController()->GetVarScope()->Find(hash);
	}
	inline ScriptClassPtr FindClass(BufferRefC name) {
		return (ScriptClassPtr) this->GetScopeController()->GetClassScope()->Find(name);
	}
	inline ScriptClassPtr FindClass(Uint32 hash) {
		return (ScriptClassPtr) this->GetScopeController()->GetClassScope()->Find(hash);
	}
	inline ScriptMethodPtr FindMethod(BufferRefC name) {
		return (ScriptMethodPtr) this->GetScopeController()->GetMethodScope()->Find(name);
	}
	inline ScriptMethodPtr FindMethod(Uint32 hash) {
		return (ScriptMethodPtr) this->GetScopeController()->GetMethodScope()->Find(hash);
	}
public:
	inline Uint32 GetReferences() { return *this->references; }
	inline ScriptClassPtr GetExtended() { return this->extended; }
	inline ScriptClassPtr GetClass() { return this; }
public:
	ScriptError New(ScriptObjectPtr object);
public:
	inline Bool DecRef(Void) {
		return --(*this->references) == 0;
	}
	inline Void IncRef(Void) {
		++(*this->references);
	}
public:
	ScriptError Extend(ScriptClassPtr object);
	ScriptError Implement(ScriptInterfacePtr object);
	ScriptError Overload(Operator op, OperatorCallback callback);
	ScriptError Evaluate(Operator op, ScriptObjectPtr left, ScriptObjectPtr right);
private:
	ScriptClassPtr extended;
	Vector<ScriptInterfacePtr> implemented;
	Uint32P references;
	Vector<OperatorCallback> operators;
};

LAME_END2

#endif // ~__LAME_SCRIPT__CLASS__