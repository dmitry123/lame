#include "Variable.h"
#include "Internal.h"
#include "Lex.h"
#include "Node.h"
#include "Exception.h"

LAME_BEGIN2(Script)

Variable::Variable(BufferRefC name, ScopePtr parent, ClassPtr classType, Bool isArray) : Object(name, parent, Type::Variable),
	classType(classType)
{
	this->wasInStack = FALSE;
	this->objectValue = NULL;
	this->registerType = NULL;

	if (classType->IsFloat() ||
		classType->IsDouble()
	) {
		this->varType = Var::Float;
		this->SetModificator(Modificator::Float);
	}
	else if (
		classType->IsInt() ||
		classType->IsChar() ||
		classType->IsShort() ||
		classType->IsLong()
	) {
		this->varType = Var::Integer;
		this->SetModificator(Modificator::Integer);
	}
	else if (classType->IsString()) {
		this->varType = Var::String;
		this->SetModificator(Modificator::String);
		this->SetModificator(Modificator::Object2);
	}
	else {
		this->varType = Var::Object;
		this->SetModificator(Modificator::Object2);
	}

	if (isArray) {
		this->varType = Var::Array;
	}

	this->v.intValue = 0;
	this->objectValue = 0;
}

Variable::~Variable() {

	if (this->objectValue != NULL) {
		delete this->objectValue;
	}
}

VariablePtr Variable::SetInteger(ScriptNativeInt i) {

	this->v.intValue = i;
	this->varType = Var::Integer;

	return this;
}

VariablePtr Variable::SetFloat(ScriptNativeFloat f) {

	this->v.floatValue = f;
	this->varType = Var::Float;

	return this;
}

VariablePtr Variable::SetObject(ObjectPtr c) {

	if (this->objectValue) {
		delete this->objectValue;
	}

	this->objectValue = c;
	this->varType = Var::Object;

	return this;
}

VariablePtr Variable::SetString(ScriptNativeString s) {

	this->stringValue = s;
	this->varType = Var::String;

	return this;
}

VariablePtr Variable::SetBoolean(ScriptNativeBool b) {

	this->v.intValue = b;
	this->varType = Var::Boolean;

	return this;
}

ScriptNativeInt Variable::GetInteger(Void) {

	if (this->varType == Var::Float) {
		return (ScriptNativeInt) this->v.floatValue;
	}

	if (this->varType != Var::Float) {
		PostSyntaxError(this->GetNode()->lex->line, "Invalid type cast from (%s) to Integer", this->GetClass()->GetName().data());
	}

	return this->v.intValue;
}

ScriptNativeFloat Variable::GetFloat(Void) {

	if (this->varType == Var::Integer) {
		return (ScriptNativeFloat) this->v.intValue;
	}

	if (this->varType == Var::Object) {
		throw ClassInvalidCastException("Invalid class cast");
	}

	return this->v.floatValue;
}

ScriptNativeBool Variable::GetBoolean(Void) {

	if (this->varType != Var::Boolean) {
		PostSyntaxError(this->GetNode()->lex->line, "Invalid type cast from (%s) to Boolean", this->GetClass()->GetName().data());
	}

	return ScriptNativeBool(this->v.intValue);
}

Bool Variable::Equal(ObjectPtrC object) {

	if (this == object) {
		return TRUE;
	}
	if (this->GetType() != object->GetType() ||
		this->varType != object->GetVariable()->varType ||
		this->classType->Hash() != object->GetVariable()->classType->Hash()
		) {
		return FALSE;
	}
	if (this->varType == Var::Integer) {
		return this->v.intValue == object->GetVariable()->v.intValue;
	}
	else if (this->varType == Var::Float) {
		return this->v.floatValue == object->GetVariable()->v.floatValue;
	}
	else {
		return this->objectValue == object->GetVariable()->objectValue;
	}
}

ObjectPtr Variable::Clone(BufferRefC name, ObjectPtr parent) {

	VariablePtr newVariable = new Variable(name, parent, this->GetClass());

	if (this->varType == Var::Object) {
		newVariable->SetObject(this->objectValue);
	}
	else if (this->varType == Var::Float) {
		newVariable->SetFloat(this->v.floatValue);
	}
	else {
		newVariable->SetInteger(this->v.intValue);
	}

	return newVariable;
}

Void Variable::Trace(Uint32 offset) {

	printf(this->GetModificatorString().data());

	if (this->GetClass()) {
		if (this->GetName().size() > 0) {
			if (this->GetTemplate()) {
				printf("%s<%s> %s = ", this->GetClass()->GetName().data(),
					this->GetTemplate()->GetName().data(), this->GetName().data());
			}
			else {
				printf("%s %s = ", this->GetClass()->GetName().data(), this->GetName().data());
			}
		}
		else {
			printf("%s = ", this->GetClass()->GetName().data());
		}
	}
	else {
		printf("null %s = ", this->GetName().data());
	}

	if (varType == Var::Float) {
		printf("%.2f", this->v.floatValue);
	}
	else if (varType == Var::Integer) {
		if (this->GetClass()->IsChar()) {
			printf("%lld", this->v.intValue);
		}
		else {
			if (sizeof(this->v.intValue) == 4) {
				printf("%d", Uint32(this->v.intValue));
			}
			else {
				printf("%lld", this->v.intValue);
			}
		}
	}
	else {
		if (this->objectValue && this->GetName() != "this") {
			this->objectValue->Trace(offset);
		}
		else {
			if (this->varType == Var::Float) {
				printf("%.2f", this->v.floatValue);
			}
			else if (this->varType == Var::Integer) {
				printf("%lld", this->v.intValue);
			}
			else if (this->varType == Var::String) {
				printf("\"%s\"", this->stringValue.data());
			}
			else if (this->varType == Var::Object) {
				printf("null");
			}
		}
	}
}

Variable::HashType Variable::Hash(Void) {
	return this->GetHash64();
}

Uint32 Variable::Size(Void) {

	//if (this->varType == Var::Integer) {
	//	return sizeof(ScriptNativeInt);
	//}
	//else if (this->varType == Var::Float) {
	//	return sizeof(ScriptNativeFloat);
	//}
	//else {
	//	return Object::SizeOf;
	//}

	return this->classType->Size();
}

ClassPtr Variable::GetClass() {

	if (this->CheckType(Type::Class)) {
		return ClassPtr(this);
	}

	return this->classType;
}

VariablePtr Variable::GetVariable() {
	return this;
}

LAME_END2
