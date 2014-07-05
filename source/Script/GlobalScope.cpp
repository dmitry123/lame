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

Void DeclareIntOperators(ClassPtr intClass) {

	intClass->Overload(Class::Operator::Move, [](VariablePtr left, VariablePtr right) {
		if (left->CheckType(Class::Type::Variable)) {
			left->v.intValue = right->v.intValue;
		} else {
			right->Clone(left);
		}
	});

	intClass->Overload(Class::Operator::Add, [](VariablePtr left, VariablePtr right) {
		left->v.intValue += right->v.intValue;
	});

	//OVERLOAD(Add, intClass, {
	//	left->v.intValue += right->v.intValue;
	//});

	OVERLOAD(Sub, intClass, {
		left->v.intValue -= right->v.intValue;
	});

	OVERLOAD(Mul, intClass, {
		left->v.intValue *= right->v.intValue;
	});

	OVERLOAD(Div, intClass, {
		left->v.intValue /= right->v.intValue;
	});

	OVERLOAD(Mod, intClass, {
		left->v.intValue %= right->v.intValue;
	});

	OVERLOAD(ShiftL, intClass, {
		left->v.intValue <<= right->v.intValue;
	});

	OVERLOAD(ShiftR, intClass, {
		left->v.intValue >>= right->v.intValue;
	});

	OVERLOAD(Above, intClass, {
		left->v.intValue = left->v.intValue > right->v.intValue;
	});

	OVERLOAD(Bellow, intClass, {
		left->v.intValue = left->v.intValue <= right->v.intValue;
	});

	OVERLOAD(AboveE, intClass, {
		left->v.intValue = left->v.intValue >= right->v.intValue;
	});

	OVERLOAD(BellowE, intClass, {
		left->v.intValue = left->v.intValue <= right->v.intValue;
	});

	OVERLOAD(Equal, intClass, {
		left->v.intValue = left->v.intValue == right->v.intValue;
	});

	OVERLOAD(NotEqual, intClass, {
		left->v.intValue = left->v.intValue != right->v.intValue;
	});

	OVERLOAD(And, intClass, {
		left->v.intValue = left->v.intValue && right->v.intValue;
	});

	OVERLOAD(Or, intClass, {
		left->v.intValue = left->v.intValue || right->v.intValue;
	});

	OVERLOAD(BitAnd, intClass, {
		left->v.intValue = left->v.intValue & right->v.intValue;
	});

	OVERLOAD(BitOr, intClass, {
		left->v.intValue = left->v.intValue | right->v.intValue;
	});

	OVERLOAD(BitXor, intClass, {
		left->v.intValue = left->v.intValue ^ right->v.intValue;
	});

	OVERLOAD(Inc, intClass, {
		++left->v.intValue;
	});

	OVERLOAD(Dec, intClass, {
		--left->v.intValue;
	});

	OVERLOAD(Sizeof, intClass, {
		left->v.intValue = left->GetClass()->GetSizeOf();
	});

	OVERLOAD(BitNot, intClass, {
		left->v.intValue = ~left->v.intValue;
	});

	OVERLOAD(Not, intClass, {
		left->v.intValue = !left->v.intValue;
	});

	OVERLOAD(Cast, intClass, {
		left->SetInteger(left->GetInteger());
	});
}

Void DeclareFloatOperators(ClassPtr floatClass) {
    
	OVERLOAD(Move, floatClass, {
		left->v.floatValue = right->v.floatValue;
	});

	OVERLOAD(Add, floatClass, {
		left->v.floatValue += right->v.floatValue;
	});

	OVERLOAD(Sub, floatClass, {
		left->v.floatValue -= right->v.floatValue;
	});

	OVERLOAD(Mul, floatClass, {
		left->v.floatValue *= right->v.floatValue;
	});

	OVERLOAD(Div, floatClass, {
		left->v.floatValue /= right->v.floatValue;
	});

	OVERLOAD(Above, floatClass, {
		left->v.intValue = left->v.floatValue > right->v.floatValue;
	});

	OVERLOAD(Bellow, floatClass, {
		left->v.intValue = left->v.floatValue < right->v.floatValue;
	});

	OVERLOAD(AboveE, floatClass, {
		left->v.intValue = left->v.floatValue >= right->v.floatValue;
	});

	OVERLOAD(BellowE, floatClass, {
		left->v.intValue = left->v.floatValue <= right->v.floatValue;
	});

	OVERLOAD(Equal, floatClass, {
		left->v.intValue = left->v.floatValue == right->v.floatValue;
	});

	OVERLOAD(NotEqual, floatClass, {
		left->v.intValue = left->v.floatValue != right->v.floatValue;
	});

	OVERLOAD(And, floatClass, {
		left->v.intValue = left->v.floatValue && right->v.floatValue;
	});

	OVERLOAD(Or, floatClass, {
		left->v.intValue = left->v.floatValue || right->v.floatValue;
	});

	OVERLOAD(Or, floatClass, {
		left->v.intValue = left->GetClass()->GetSizeOf();
	});

	OVERLOAD(Or, floatClass, {
		left->v.floatValue = !left->v.floatValue;
	});

	OVERLOAD(Cast, floatClass, {
		left->SetFloat(left->GetFloat());
	});
}

ObjectPtr DeclareInt(StringC intName, Uint32 sizeOf) {
    
	ObjectPtr intClass = GlobalScope()->GetClassScope()->Add(new Class(intName))
        ->SetModificator(Class::Modificator::Static)
        ->SetModificator(Class::Modificator::Final)
        ->SetModificator(Class::Modificator::Public)
        ->SetModificator(Class::Modificator::Primitive);
    
	DeclareIntOperators(intClass->GetClass());
	intClass->SetSizeOf(sizeOf);
    
	return intClass;
}

ObjectPtr DeclareFloat(StringC floatName, Uint32 sizeOf) {
    
	ObjectPtr floatClass = GlobalScope()->GetClassScope()->Add(new Class(floatName))
        ->SetModificator(Class::Modificator::Static)
        ->SetModificator(Class::Modificator::Final)
        ->SetModificator(Class::Modificator::Public)
        ->SetModificator(Class::Modificator::Primitive);
    
	DeclareFloatOperators(floatClass->GetClass());
	floatClass->SetSizeOf(sizeOf);
    
	return floatClass;
}

Void GlobalScope::_InitializeGlobalScope(Void) {
    
    isGlobalScopeInitialized_ = TRUE;
    
	/* Initrialize Global Scope & Language Classes */

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

	/* Lists with Registers */

	rList.push_back(r0);
	rList.push_back(r1);
	rList.push_back(r2);
	rList.push_back(r3);
	rList.push_back(r4);
	rList.push_back(r5);
	rList.push_back(r6);
	rList.push_back(r7);
	rList.push_back(r8);
	rList.push_back(r9);

	fList.push_back(f0);
	fList.push_back(f1);
	fList.push_back(f2);
	fList.push_back(f3);
	fList.push_back(f4);
	fList.push_back(f5);
	fList.push_back(f6);
	fList.push_back(f7);
	fList.push_back(f8);
	fList.push_back(f9);
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

	toString->GetMethod()->SetNativeMethod([](MethodPtr m){
		r9->GetVariable()->SetString(m->GetThis()->GetName());
	});

	OVERLOAD(Move, classObject, {
		right->Clone(left);
	});

	classObject->Overload(Class::Operator::Cast, [](VariablePtr left, VariablePtr right) {

		if (right->GetClass() == left->GetClass()) {
			goto __AllowCast;
		}

		if (left->GetClass()->GetExtended() == right->GetClass()) {
			goto __AllowCast;
		}

		for (InterfacePtr i : left->GetClass()->GetImplements()) {
			if (right->GetClass() == i->GetClass()) {
				goto __AllowCast;
			}
		}

		throw ClassInvalidCastException();

	__AllowCast:
		;
	});
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

	classString->Overload(Class::Operator::Move, [] (VariablePtr left, VariablePtr right) {
		left->stringValue = right->stringValue;
	});

	classString->Overload(Class::Operator::Add, [](VariablePtr left, VariablePtr right) {
		left->stringValue.append(right->stringValue);
	});

	classString->Overload(Class::Operator::Cast, [](VariablePtr left, VariablePtr right) {
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

Vector<VariablePtr> GlobalScope::rList;
Vector<VariablePtr> GlobalScope::fList;

Bool GlobalScope::isGlobalScopeInitialized_
    = FALSE;

ScopeControllerPtr GlobalScope::globalScope_
	= new ScopeController();

static GlobalScope globalScope;

LAME_END2
