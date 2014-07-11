#include "GlobalScope.h"
#include "Class.h"
#include "Variable.h"
#include "Exception.h"
#include "Internal.h"
#include "Interface.h"
#include "Method.h"

#define OVERLOAD(_operator, _class, _code) \
	_class->Overload(Class::Operator::_operator, [] (VariablePtr left, VariablePtr right) _code); \

LAME_BEGIN2(Script)

ObjectPtr DeclareInt(StringC intName, Uint32 sizeOf) {
    
	ObjectPtr intClass = GlobalScope()->GetClassScope()->Add(new Class(intName))
        ->SetModificator(Class::Modificator::Static)
        ->SetModificator(Class::Modificator::Final)
        ->SetModificator(Class::Modificator::Public)
        ->SetModificator(Class::Modificator::Primitive);
    
	intClass->SetSizeOf(sizeOf);
    
	return intClass;
}

ObjectPtr DeclareFloat(StringC floatName, Uint32 sizeOf) {
    
	ObjectPtr floatClass = GlobalScope()->GetClassScope()->Add(new Class(floatName))
        ->SetModificator(Class::Modificator::Static)
        ->SetModificator(Class::Modificator::Final)
        ->SetModificator(Class::Modificator::Public)
        ->SetModificator(Class::Modificator::Primitive);
    
	floatClass->SetSizeOf(sizeOf);
    
	return floatClass;
}

Void GlobalScope::_InitializeGlobalScope(Void) {
    
    isGlobalScopeInitialized_ = TRUE;
    
	/* Initialize Global Scope & Language Classes */

	_InitializeCharClass();
	_InitializeByteClass();
	_InitializeShortClass();
	_InitializeIntClass();
	_InitializeLongClass();
	_InitializeFloatClass();
	_InitializeDoubleClass();
	_InitializeBooleanClass();
	_InitializeVoidClass();
	_InitializeStringClass();
	_InitializeObjectClass();
	_InitializeClassClass();

	classBoolean->SetPriority(0);
	classChar->SetPriority(1);
	classByte->SetPriority(2);
	classShort->SetPriority(3);
	classInt->SetPriority(4);
	classLong->SetPriority(5);
	classFloat->SetPriority(6);
	classDouble->SetPriority(7);
	classString->SetPriority(8);
	classObject->SetPriority(0);
	classClass->SetPriority(0);
	classVoid->SetPriority(0);

	/* Base Registers */

	r0 = globalScope_->GetVarScope()->Add(new Variable("r0", classInt))
		->SetModificator(Class::Modificator::Register)->GetVariable();

	r1 = globalScope_->GetVarScope()->Add(new Variable("r1", classInt))
		->SetModificator(Class::Modificator::Register)->GetVariable();

	r2 = globalScope_->GetVarScope()->Add(new Variable("r2", classInt))
		->SetModificator(Class::Modificator::Register)->GetVariable();

	r3 = globalScope_->GetVarScope()->Add(new Variable("r3", classInt))
		->SetModificator(Class::Modificator::Register)->GetVariable();

	r4 = globalScope_->GetVarScope()->Add(new Variable("r4", classInt))
		->SetModificator(Class::Modificator::Register)->GetVariable();

	r5 = globalScope_->GetVarScope()->Add(new Variable("r5", classInt))
		->SetModificator(Class::Modificator::Register)->GetVariable();

	r6 = globalScope_->GetVarScope()->Add(new Variable("r6", classInt))
		->SetModificator(Class::Modificator::Register)->GetVariable();

	r7 = globalScope_->GetVarScope()->Add(new Variable("r7", classInt))
		->SetModificator(Class::Modificator::Register)->GetVariable();

	r8 = globalScope_->GetVarScope()->Add(new Variable("r8", classInt))
		->SetModificator(Class::Modificator::Register)->GetVariable();

	r9 = globalScope_->GetVarScope()->Add(new Variable("r9", classInt))
		->SetModificator(Class::Modificator::Register)->GetVariable();

	/* Float Registers */

	f0 = globalScope_->GetVarScope()->Add(new Variable("f0", classFloat))
		->SetModificator(Class::Modificator::Register)->GetVariable();

	f1 = globalScope_->GetVarScope()->Add(new Variable("f1", classFloat))
		->SetModificator(Class::Modificator::Register)->GetVariable();

	f2 = globalScope_->GetVarScope()->Add(new Variable("f2", classFloat))
		->SetModificator(Class::Modificator::Register)->GetVariable();

	f3 = globalScope_->GetVarScope()->Add(new Variable("f3", classFloat))
		->SetModificator(Class::Modificator::Register)->GetVariable();

	f4 = globalScope_->GetVarScope()->Add(new Variable("f4", classFloat))
		->SetModificator(Class::Modificator::Register)->GetVariable();

	f5 = globalScope_->GetVarScope()->Add(new Variable("f5", classFloat))
		->SetModificator(Class::Modificator::Register)->GetVariable();

	f6 = globalScope_->GetVarScope()->Add(new Variable("f6", classFloat))
		->SetModificator(Class::Modificator::Register)->GetVariable();

	f7 = globalScope_->GetVarScope()->Add(new Variable("f7", classFloat))
		->SetModificator(Class::Modificator::Register)->GetVariable();

	f8 = globalScope_->GetVarScope()->Add(new Variable("f8", classFloat))
		->SetModificator(Class::Modificator::Register)->GetVariable();

	f9 = globalScope_->GetVarScope()->Add(new Variable("f9", classFloat))
		->SetModificator(Class::Modificator::Register)->GetVariable();
}

Void GlobalScope::_InitializeCharClass(Void) {
    classChar = DeclareInt("char", 1)->GetClass();
}

Void GlobalScope::_InitializeByteClass(Void) {
    classByte = DeclareInt("byte", 1)->GetClass();
}

Void GlobalScope::_InitializeShortClass(Void) {
    classShort = DeclareInt("short", 2)->GetClass();
}

Void GlobalScope::_InitializeIntClass(Void) {
    classInt = DeclareInt("int", 4)->GetClass();
}

Void GlobalScope::_InitializeLongClass(Void) {
    classLong = DeclareInt("long", 8)->GetClass();
}

Void GlobalScope::_InitializeFloatClass(Void) {
    classFloat = DeclareFloat("float", 4)->GetClass();
}

Void GlobalScope::_InitializeDoubleClass(Void) {
    classDouble = DeclareFloat("double", 8)->GetClass();
}

Void GlobalScope::_InitializeBooleanClass(Void) {
    classBoolean = DeclareInt("boolean", 1)->GetClass();
}

Void GlobalScope::_InitializeObjectClass(Void) {
    
	ObjectPtr toString;

    classObject = globalScope_->GetClassScope()->Add(new Class("Object"))
        ->SetModificator(Class::Modificator::Final)
		->SetModificator(Class::Modificator::Internal)->GetClass();

	toString = classObject->GetClass()->GetScopeController()->GetMethodScope()
		->Add(new Method("toString", NULL, classObject, classString));

	classObjectT = globalScope_->GetClassScope()->Add(new Class("ObjectT"))
		->SetModificator(Class::Modificator::Final)
		->SetModificator(Class::Modificator::Internal)->GetClass();

	classObjectT->Extend(classObject);
}

Void GlobalScope::_InitializeClassClass(Void) {
    
    classClass = globalScope_->GetClassScope()->Add(new Class("Class"))
        ->SetModificator(Class::Modificator::Final)
		->SetModificator(Class::Modificator::Internal)->GetClass();
}

Void GlobalScope::_InitializeVoidClass(Void) {

    classVoid = globalScope_->GetClassScope()->Add(new Class("void"))
        ->SetModificator(Class::Modificator::Primitive)->GetClass();
}

Void GlobalScope::_InitializeStringClass(Void) {

	classString = globalScope_->GetClassScope()->Add(new Class("String"))
		->SetModificator(Class::Modificator::Final)
		->SetModificator(Class::Modificator::Internal)->GetClass();

	classString->Overload(Class::Operator::Move, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		left->stringValue = right->stringValue;
	});

	classString->Overload(Class::Operator::Add, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		left->stringValue.append(right->stringValue);
	});

	classString->Overload(Class::Operator::Cast, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		left->SetString(left->GetString());
	});
}

ClassPtr GlobalScope::classChar = NULL;
ClassPtr GlobalScope::classByte = NULL;
ClassPtr GlobalScope::classShort = NULL;
ClassPtr GlobalScope::classInt = NULL;
ClassPtr GlobalScope::classLong = NULL;
ClassPtr GlobalScope::classFloat = NULL;
ClassPtr GlobalScope::classDouble = NULL;
ClassPtr GlobalScope::classObject = NULL;
ClassPtr GlobalScope::classObjectT = NULL;
ClassPtr GlobalScope::classClass = NULL;
ClassPtr GlobalScope::classBoolean = NULL;
ClassPtr GlobalScope::classVoid = NULL;
ClassPtr GlobalScope::classString = NULL;

VariablePtr GlobalScope::r0 = NULL;
VariablePtr GlobalScope::r1 = NULL;
VariablePtr GlobalScope::r2 = NULL;
VariablePtr GlobalScope::r3 = NULL;
VariablePtr GlobalScope::r4 = NULL;
VariablePtr GlobalScope::r5 = NULL;
VariablePtr GlobalScope::r6 = NULL;
VariablePtr GlobalScope::r7 = NULL;
VariablePtr GlobalScope::r8 = NULL;
VariablePtr GlobalScope::r9 = NULL;
VariablePtr GlobalScope::r10 = NULL;
VariablePtr GlobalScope::r11 = NULL;
VariablePtr GlobalScope::r12 = NULL;
VariablePtr GlobalScope::r13 = NULL;
VariablePtr GlobalScope::r14 = NULL;
VariablePtr GlobalScope::r15 = NULL;

VariablePtr GlobalScope::f0 = NULL;
VariablePtr GlobalScope::f1 = NULL;
VariablePtr GlobalScope::f2 = NULL;
VariablePtr GlobalScope::f3 = NULL;
VariablePtr GlobalScope::f4 = NULL;
VariablePtr GlobalScope::f5 = NULL;
VariablePtr GlobalScope::f6 = NULL;
VariablePtr GlobalScope::f7 = NULL;
VariablePtr GlobalScope::f8 = NULL;
VariablePtr GlobalScope::f9 = NULL;

Bool GlobalScope::isGlobalScopeInitialized_
	= FALSE;

ScopeControllerPtr GlobalScope::globalScope_
	= new ScopeController();

SegmentPtr GlobalScope::codeSegment_;
static GlobalScope globalScope;

LAME_END2
