#include "GlobalScope.h"

LAME_BEGIN2(Script)

ScopePtr GlobalScope::Create(Buffer name, Uint32 flags) {

	static ObjectPtr rootScope = !(flags & kFlagClass) ?
		ObjectPtr(new Script::Method(name, NULL, NULL, NULL)) :
		ObjectPtr(new Script::Class(name, NULL));

	if (Scope::classChar != NULL) {
		goto _ReturnScope;
	}

	Scope::classChar = rootScope->Scope::Add(new Script::Class("char", rootScope, 2))->GetClass();
	Scope::classByte = rootScope->Scope::Add(new Script::Class("byte", rootScope, 1))->GetClass();
	Scope::classBoolean = rootScope->Scope::Add(new Script::Class("boolean", rootScope, 1))->GetClass();
	Scope::classShort = rootScope->Scope::Add(new Script::Class("short", rootScope, 2))->GetClass();
	Scope::classInt = rootScope->Scope::Add(new Script::Class("int", rootScope, 4))->GetClass();
	Scope::classLong = rootScope->Scope::Add(new Script::Class("long", rootScope, 8))->GetClass();
	Scope::classFloat = rootScope->Scope::Add(new Script::Class("float", rootScope, 4))->GetClass();
	Scope::classDouble = rootScope->Scope::Add(new Script::Class("double", rootScope, 8))->GetClass();
	Scope::classVoid = rootScope->Scope::Add(new Script::Class("void", rootScope, 0))->GetClass();
	Scope::classString = rootScope->Scope::Add(new Script::Class("String", rootScope))->GetClass();
	Scope::classObject = rootScope->Scope::Add(new Script::Class("Object", rootScope))->GetClass();
	Scope::classClass = rootScope->Scope::Add(new Script::Class("Class", rootScope))->GetClass();
	Scope::classUnknown = rootScope->Scope::Add(new Script::Class("?", rootScope))->GetClass();
	Scope::classArray = rootScope->Scope::Add(new Script::Class("Array", rootScope))->GetClass();

	Scope::classChar
		->SetModificator(Script::Object::Modificator::Primitive)
		->SetModificator(Script::Object::Modificator::Internal)
		->SetModificator(Script::Object::Modificator::Integer);
	Scope::classByte
		->SetModificator(Script::Object::Modificator::Primitive)
		->SetModificator(Script::Object::Modificator::Internal)
		->SetModificator(Script::Object::Modificator::Integer);
	Scope::classBoolean
		->SetModificator(Script::Object::Modificator::Primitive)
		->SetModificator(Script::Object::Modificator::Internal)
		->SetModificator(Script::Object::Modificator::Boolean);
	Scope::classShort
		->SetModificator(Script::Object::Modificator::Primitive)
		->SetModificator(Script::Object::Modificator::Internal)
		->SetModificator(Script::Object::Modificator::Integer);
	Scope::classInt
		->SetModificator(Script::Object::Modificator::Primitive)
		->SetModificator(Script::Object::Modificator::Internal)
		->SetModificator(Script::Object::Modificator::Integer);
	Scope::classLong
		->SetModificator(Script::Object::Modificator::Primitive)
		->SetModificator(Script::Object::Modificator::Internal)
		->SetModificator(Script::Object::Modificator::Integer);
	Scope::classFloat
		->SetModificator(Script::Object::Modificator::Primitive)
		->SetModificator(Script::Object::Modificator::Internal)
		->SetModificator(Script::Object::Modificator::Float);
	Scope::classDouble
		->SetModificator(Script::Object::Modificator::Primitive)
		->SetModificator(Script::Object::Modificator::Internal)
		->SetModificator(Script::Object::Modificator::Float);
	Scope::classVoid
		->SetModificator(Script::Object::Modificator::Primitive)
		->SetModificator(Script::Object::Modificator::Internal);

	Scope::classString
		->SetModificator(Script::Object::Modificator::Internal)
		->SetModificator(Script::Object::Modificator::Object2)
		->SetModificator(Script::Object::Modificator::String);
	Scope::classObject
		->SetModificator(Script::Object::Modificator::Internal)
		->SetModificator(Script::Object::Modificator::Object2);
	Scope::classClass
		->SetModificator(Script::Object::Modificator::Internal)
		->SetModificator(Script::Object::Modificator::Object2);
	Scope::classUnknown
		->SetModificator(Script::Object::Modificator::Internal)
		->SetModificator(Script::Object::Modificator::Object2);
	Scope::classArray
		->SetModificator(Script::Object::Modificator::Internal)
		->SetModificator(Script::Object::Modificator::Object2);

	Scope::classChar->SetPriority(0);
	Scope::classByte->SetPriority(0);
	Scope::classBoolean->SetPriority(0);
	Scope::classShort->SetPriority(1);
	Scope::classInt->SetPriority(2);
	Scope::classLong->SetPriority(3);
	Scope::classFloat->SetPriority(4);
	Scope::classDouble->SetPriority(5);
	Scope::classVoid->SetPriority(0);
	Scope::classString->SetPriority(6);
	Scope::classObject->SetPriority(7);
	Scope::classClass->SetPriority(7);
	Scope::classUnknown->SetPriority(7);
	Scope::classArray->SetPriority(7);

	Scope::classClass->SetTemplate(
		Scope::classObject);

	Scope::classArray->Add(new Variable("length", Scope::classArray, Scope::classInt))
		->GetVariable()->SetInteger(0)->SetModificator(Object::Modificator::Internal);

	goto _SkipPrimitive;
_ReturnScope:
	rootScope->Add(Scope::classChar);
	rootScope->Add(Scope::classByte);
	rootScope->Add(Scope::classBoolean);
	rootScope->Add(Scope::classShort);
	rootScope->Add(Scope::classInt);
	rootScope->Add(Scope::classLong);
	rootScope->Add(Scope::classFloat);
	rootScope->Add(Scope::classDouble);
	rootScope->Add(Scope::classVoid);
	rootScope->Add(Scope::classString);
	rootScope->Add(Scope::classObject);
	rootScope->Add(Scope::classClass);
	rootScope->Add(Scope::classUnknown);
	rootScope->Add(Scope::classArray);
_SkipPrimitive:

	if (rootScope->Find("true")) {
		return rootScope;
	}

	rootScope->Add(new Variable("true", rootScope, Scope::classBoolean))
		->GetVariable()->SetBoolean(TRUE)
		->SetModificator(Object::Modificator::Internal)
		->SetModificator(Object::Modificator::Constant);

	rootScope->Add(new Variable("false", rootScope, Scope::classBoolean))
		->GetVariable()->SetBoolean(FALSE)
		->SetModificator(Object::Modificator::Internal)
		->SetModificator(Object::Modificator::Constant);

	rootScope->Add(new Variable("null", rootScope, Scope::classObject))
		->GetVariable()->SetObject(NULL)
		->SetModificator(Object::Modificator::Internal)
		->SetModificator(Object::Modificator::Constant);

	return rootScope;
}

Void GlobalScope::Release(ScopePtr rootScope) {
}

LAME_END2