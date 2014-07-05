#include "Variable.h"
#include "Internal.h"
#include "GlobalScope.h"
#include "Lex.h"

LAME_BEGIN2(Script)

#define __EvalError(_operation) \
	PostSyntaxError(0, "Unable to apply %s operation to this type", #_operation, left->name.data());

Variable::Variable(BufferRefC name, ObjectPtrC classType, Type type, NodePtr node) : Object(name, type, node),
	classType((ClassPtrC)classType)
{
	this->objectValue = NULL;

	if (classType == GlobalScope::classString) {
		this->SetString("");
	}
	else if (!classType->CheckModificator(Modificator::Primitive)) {
		this->SetObject(NULL);
	}
	else if (classType == GlobalScope::classFloat) {
		this->SetFloat(0.0f);
	}
	else {
		this->SetInteger(0);
	}

	this->SetSizeOf(Object::SizeOf);
}

Variable::Variable(BufferRefC name, ObjectPtrC classType, NodePtr node) : Object(name, Type::Variable, node),
	classType((ClassPtrC)classType)
{
	this->objectValue = NULL;

	if (classType == GlobalScope::classString) {
		this->SetString("");
	}
	else if (!classType->CheckModificator(Modificator::Primitive)) {
		this->SetObject(NULL);
	}
	else if (classType == GlobalScope::classFloat) {
		this->SetFloat(0.0f);
	}
	else {
		this->SetInteger(0);
	}

	if (!classType->GetSizeOf()) {
		this->SetSizeOf(Object::SizeOf);
	} else {
		this->SetSizeOf(classType->GetSizeOf());
	}
}

Variable::~Variable() {

	if (this->objectValue != NULL && this->objectValue->DecRef()) {
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

VariablePtr Variable::SetObject(ClassPtr c) {

	if (this->objectValue) {
		if (this->objectValue->DecRef()) {
			delete this->objectValue;
		}
	}

	if (c != NULL) {
		c->IncRef();
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

ScriptNativeInt Variable::GetInteger(Void) {

	if (this->varType == Var::String) {
		return ParseIntValue(this->stringValue.data());
	}

	if (this->varType == Var::Float) {
		return (ScriptNativeInt) this->v.floatValue;
	}

	if (this->varType == Var::Object) {
		throw ClassInvalidCastException();
	}

	return this->v.intValue;
}

ScriptNativeFloat Variable::GetFloat(Void) {

	if (this->varType == Var::String) {
		return ParseFloatValue(this->stringValue.data());
	}

	if (this->varType == Var::Float) {
		return (ScriptNativeFloat) this->v.intValue;
	}

	if (this->varType == Var::Object) {
		throw ClassInvalidCastException();
	}

	return this->v.floatValue;
}

ScriptNativeString Variable::GetString(Void) {

	Buffer b;

	if (this->varType == Var::Integer) {
		if (sizeof(this->v.intValue) == 4) {
			b.Format("%d", this->v.intValue);
		}
		else {
			b.Format("%lld", this->v.intValue);
		}
	}
	else if (this->varType == Var::Float) {
		b.Format("%.2f", this->v.floatValue);
	}
	else if (this->varType == Var::Object) {
		b.Format("%s", this->objectValue->GetName().data());
	}
	else {
		return this->stringValue;
	}

	return b;
}

Error Variable::Clone(ObjectPtrC object) {

	Object::Clone(object);

	if (!object->CheckType(Type::Variable)) {
		return Error::Class_ObjectNotVariable;
	}

	if (object->GetVariable()->varType == Var::Object) {
		object->GetVariable()->SetObject(this->GetVariable()->objectValue);
	}
	else if (object->GetVariable()->varType == Var::Float) {
		object->GetVariable()->SetFloat(this->GetVariable()->v.floatValue);
	}
	else if (object->GetVariable()->varType == Var::String) {
		object->GetVariable()->SetString(this->GetVariable()->stringValue);
	}
	else{
		object->GetVariable()->SetInteger(this->GetVariable()->v.intValue);
	}

	return Error::NoError;
}

Void Variable::Trace(Uint32 offset) {

	this->PrintModificators();

	if (this->GetClass()) {
		if (this->GetName().size() > 0) {
			printf("%s %s = ", this->GetClass()->GetName().data(), this->GetName().data());
		} else {
			printf("%s = ", this->GetClass()->GetName().data());
		}
	} else {
		printf("null %s = ", this->GetName().data());
	}

	if (varType == Var::Float) {
		printf("%.2f", this->v.floatValue);
	}
	else if (varType == Var::Integer) {
		printf("%d", this->v.intValue);
	}
	else {
		if (this->objectValue && this->GetName() != "this") {
			this->objectValue->Trace(offset);
		}
		else {
			if (this->varType == Var::Float) {
				printf("%.2f", this->v.intValue);
			}
			else if (this->varType == Var::Integer) {
				printf("%lld", this->v.intValue);
			}
			else if (this->varType == Var::Object) {
				printf("null");
			}
			else if (this->varType == Var::String) {
				printf("\"%s\"", this->stringValue.data());
			}
		}
	}
}

Bool Variable::IsChar(Void) {
	return this->classType == GlobalScope::classChar;
}

Bool Variable::IsByte(Void) {
	return this->classType == GlobalScope::classByte;
}

Bool Variable::IsShort(Void) {
	return this->classType == GlobalScope::classShort;
}

Bool Variable::IsInt(Void) {
	return this->classType == GlobalScope::classInt;
}

Bool Variable::IsLong(Void) {
	return this->classType == GlobalScope::classLong;
}

Bool Variable::IsFloat(Void) {
	return this->classType == GlobalScope::classFloat;
}

Bool Variable::IsDouble(Void) {
	return this->classType == GlobalScope::classDouble;
}

Bool Variable::IsObject(Void) {
	return this->classType == GlobalScope::classObject;
}

Bool Variable::IsClass(Void) {
	return this->classType == GlobalScope::classClass;
}

Bool Variable::IsBoolean(Void) {
	return this->classType == GlobalScope::classBoolean;
}

Bool Variable::IsVoid(Void) {
	return this->classType == GlobalScope::classVoid;
}

Bool Variable::IsString(Void) {
	return this->classType == GlobalScope::classString;
}

LAME_END2
