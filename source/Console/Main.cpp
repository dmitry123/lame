#include "Main.h"

using namespace Lame::Core;
using namespace Lame::ResourceManager;
using namespace Lame::Script;
using namespace Lame::VirtualMachine;
using namespace Lame;

Void DeclareIntOperators(ScriptClassPtr intClass) {

	intClass->Overload(ScriptClass::Operator::Move, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue = right->v.intValue; });
	intClass->Overload(ScriptClass::Operator::Add, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue += right->v.intValue; });
	intClass->Overload(ScriptClass::Operator::Sub, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue -= right->v.intValue; });
	intClass->Overload(ScriptClass::Operator::Mul, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue *= right->v.intValue; });
	intClass->Overload(ScriptClass::Operator::Div, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue /= right->v.intValue; });
	intClass->Overload(ScriptClass::Operator::Mod, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue %= right->v.intValue;});
	intClass->Overload(ScriptClass::Operator::ShiftL, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue <<= right->v.intValue; });
	intClass->Overload(ScriptClass::Operator::ShiftR, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue <<= right->v.intValue; });
	intClass->Overload(ScriptClass::Operator::Above, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue = left->v.intValue > right->v.intValue; });
	intClass->Overload(ScriptClass::Operator::Bellow, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue = left->v.intValue < right->v.intValue; });
	intClass->Overload(ScriptClass::Operator::AboveE, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue = left->v.intValue >= right->v.intValue; });
	intClass->Overload(ScriptClass::Operator::BellowE, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue = left->v.intValue <= right->v.intValue; });
	intClass->Overload(ScriptClass::Operator::Equal, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue = left->v.intValue == right->v.intValue; });
	intClass->Overload(ScriptClass::Operator::NotEqual, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue = left->v.intValue != right->v.intValue; });
	intClass->Overload(ScriptClass::Operator::And, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue = left->v.intValue && right->v.intValue; });
	intClass->Overload(ScriptClass::Operator::Or, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue = left->v.intValue || right->v.intValue; });
	intClass->Overload(ScriptClass::Operator::BitAnd, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue &= right->v.intValue; });
	intClass->Overload(ScriptClass::Operator::BitOr, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue |= right->v.intValue; });
	intClass->Overload(ScriptClass::Operator::BitXor, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue ^= right->v.intValue; });
	intClass->Overload(ScriptClass::Operator::Inc, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue++; });
	intClass->Overload(ScriptClass::Operator::Dec, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue--; });
	intClass->Overload(ScriptClass::Operator::Sizeof, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue = left->GetSizeOf(); });
	intClass->Overload(ScriptClass::Operator::BitNot, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue = ~left->v.intValue; });
	intClass->Overload(ScriptClass::Operator::Not, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue = !left->v.intValue; });
}

Void DeclareFloatOperators(ScriptClassPtr floatClass) {

	floatClass->Overload(ScriptClass::Operator::Move, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.floatValue = right->v.floatValue; });
	floatClass->Overload(ScriptClass::Operator::Add, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.floatValue += right->v.floatValue;});
	floatClass->Overload(ScriptClass::Operator::Sub, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.floatValue -= right->v.floatValue; });
	floatClass->Overload(ScriptClass::Operator::Mul, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.floatValue *= right->v.floatValue; });
	floatClass->Overload(ScriptClass::Operator::Div, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.floatValue /= right->v.floatValue; });
	floatClass->Overload(ScriptClass::Operator::Above, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue = left->v.floatValue > right->v.floatValue; });
	floatClass->Overload(ScriptClass::Operator::Bellow, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue = left->v.floatValue < right->v.floatValue; });
	floatClass->Overload(ScriptClass::Operator::AboveE, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue = left->v.floatValue >= right->v.floatValue; });
	floatClass->Overload(ScriptClass::Operator::BellowE, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue = left->v.floatValue <= right->v.floatValue; });
	floatClass->Overload(ScriptClass::Operator::Equal, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue = left->v.floatValue == right->v.floatValue; });
	floatClass->Overload(ScriptClass::Operator::NotEqual, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue = left->v.floatValue != right->v.floatValue; });
	floatClass->Overload(ScriptClass::Operator::And, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue = left->v.floatValue && right->v.floatValue; });
	floatClass->Overload(ScriptClass::Operator::Or, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue = left->v.floatValue || right->v.floatValue; });
	floatClass->Overload(ScriptClass::Operator::Sizeof, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.intValue = left->GetSizeOf(); });
	floatClass->Overload(ScriptClass::Operator::Not, [](ScriptVarPtr left, ScriptVarPtr right) { left->v.floatValue = !left->v.floatValue; });
}

ScriptObjectPtr DeclareInt(StringC intName, Uint32 sizeOf) {

	ScriptObjectPtr intClass = ScriptGlobalScope()->GetClassScope()->Add(new ScriptClass(intName))
		->SetModificator(ScriptClass::Modificator::Static)
		->SetModificator(ScriptClass::Modificator::Final)
		->SetModificator(ScriptClass::Modificator::Public)
		->SetModificator(ScriptClass::Modificator::Primitive);

	DeclareIntOperators(intClass->GetClass());
	intClass->SetSizeOf(sizeOf);

	return intClass;
}

ScriptObjectPtr DeclareFloat(StringC floatName, Uint32 sizeOf) {

	ScriptObjectPtr floatClass = ScriptGlobalScope()->GetClassScope()->Add(new ScriptClass(floatName))
		->SetModificator(ScriptClass::Modificator::Static)
		->SetModificator(ScriptClass::Modificator::Final)
		->SetModificator(ScriptClass::Modificator::Public)
		->SetModificator(ScriptClass::Modificator::Primitive);

	DeclareFloatOperators(floatClass->GetClass());
	floatClass->SetSizeOf(sizeOf);

	return floatClass;
}

void TestInterface() {

	ScriptGlobalScope globalScope;

	ScriptObjectPtr interfaceA;

	ScriptObjectPtr classR;
	ScriptObjectPtr classL;
	ScriptObjectPtr classQ;
	ScriptObjectPtr objectR;
	ScriptObjectPtr objectL;
	ScriptObjectPtr objectQ;

	ScriptObjectPtr methodRA;
	ScriptObjectPtr methodLA;
	ScriptObjectPtr methodQA;

	ScriptObjectPtr charClass = DeclareInt("char", 8);
	ScriptObjectPtr shortClass = DeclareInt("short", 16);
	ScriptObjectPtr intClass = DeclareInt("int", 32);
	ScriptObjectPtr longClass = DeclareInt("long", 64);

	ScriptObjectPtr floatClass = DeclareFloat("float", 32);
	ScriptObjectPtr doubleClass = DeclareFloat("double", 64);

	ScriptObjectPtr voidClass = globalScope->GetClassScope()->Add(new ScriptClass("void"));

	// declare interface A with one empty method "a"
	interfaceA = globalScope->GetClassScope()->Add(new ScriptInterface("A"));
	interfaceA->GetScopeController()->GetMethodScope()->Add(new ScriptMethod("a", interfaceA, voidClass));

	// declare class R, which implements interface A
	classR = globalScope->GetClassScope()->Add(new ScriptClass("R"));
	classR->GetClass()->Implement(interfaceA->GetInterface());

	// declare class L, which also implements interface A
	classL = globalScope->GetClassScope()->Add(new ScriptClass("L"));
	classL->GetClass()->Implement(interfaceA->GetInterface());

	// declare interface object r/l
	objectR = globalScope->GetVarScope()->Add(new ScriptVar("r", interfaceA));
	objectL = globalScope->GetVarScope()->Add(new ScriptVar("l", interfaceA));

	// find methods in classes L/R
	methodRA = classR->GetScopeController()->GetMethodScope()->Find("a");
	methodLA = classL->GetScopeController()->GetMethodScope()->Find("a");

	methodRA->GetMethod()->SetNativeMethod([](ScriptMethodPtr m) {
		printf("R Method : "); m->Trace(0); puts("");
	});
	methodLA->GetMethod()->SetNativeMethod([](ScriptMethodPtr m) {
		printf("L Method : "); m->Trace(0); puts("");
	});

	// invoke operator new for l/r objects
	classR->GetClass()->New(objectR);
	classL->GetClass()->New(objectL);

	// create new class Q, which extends R
	classQ = globalScope->GetClassScope()->Add(new ScriptClass("Q"));
	classQ->GetClass()->Extend(classR->GetClass());

	// create Q object
	objectQ = globalScope->GetVarScope()->Add(new ScriptVar("q", interfaceA));

	// get q's method
	methodQA = classQ->GetScopeController()->GetMethodScope()->Find("a");

	methodQA->GetMethod()->SetNativeMethod([](ScriptMethodPtr m) {
		printf("Q Method : "); m->Trace(0); puts("");
	});

	// create new Q object
	classQ->GetClass()->New(objectQ);

	// trace global scope
	globalScope->Trace(); puts("\n");

	// invoke l/r methods
	objectR->GetVariable()->GetObject()->FindMethod("a")->Invoke();
	objectL->GetVariable()->GetObject()->FindMethod("a")->Invoke();
	objectQ->GetVariable()->GetObject()->FindMethod("a")->Invoke();
}

int main(int argc, char** argv) {

	ScriptError::SetErrorHandler([](ScriptError error) {
		printf("Script Error : %d (%s)\n", error, error.GetDescription());
		ScriptGlobalScope()->Clear();
#ifdef LAME_WINDOWS
		system("pause");
#endif
		exit(0);
	});

#ifdef LAME_VLD
	puts("");
#endif

	ScriptGlobalScope globalScope;

	ScriptObjectPtr charClass = DeclareInt("char", 8);
	ScriptObjectPtr shortClass = DeclareInt("short", 16);
	ScriptObjectPtr intClass = DeclareInt("int", 32);
	ScriptObjectPtr longClass = DeclareInt("long", 64);
	ScriptObjectPtr floatClass = DeclareFloat("float", 32);
	ScriptObjectPtr doubleClass = DeclareFloat("double", 64);
	ScriptObjectPtr voidClass = globalScope->GetClassScope()->Add(new ScriptClass("void"))
		->SetModificator(ScriptObject::Modificator::Primitive);

	ScriptObjectPtr abstractA;
	ScriptObjectPtr interfaceI;
	ScriptObjectPtr classB;
	ScriptObjectPtr classC;
	ScriptObjectPtr objectB;
	ScriptObjectPtr objectC;

	// create classes A, B, C
	interfaceI = globalScope->GetClassScope()->Add(new ScriptInterface("I"));
	abstractA = globalScope->GetClassScope()->Add(new ScriptAbstract("A"));
	classB = globalScope->GetClassScope()->Add(new ScriptClass("B"));
	classC = globalScope->GetClassScope()->Add(new ScriptClass("C"));

	// create A's method
	interfaceI->GetScopeController()->GetMethodScope()->Add(new ScriptMethod("a", interfaceI, voidClass))
		->SetModificator(ScriptClass::Modificator::Abstract);

	abstractA->GetClass()->Implement(interfaceI->GetInterface());

	// extend B and C by A
	classB->GetClass()->Extend(abstractA->GetClass());
	classC->GetClass()->Extend(abstractA->GetClass());

	// create objects
	objectB = globalScope->GetVarScope()->Add(new ScriptVar("b", abstractA));
	objectC = globalScope->GetVarScope()->Add(new ScriptVar("c", abstractA));

	// create object's methods
	classB->GetClass()->GetScopeController()->GetMethodScope()->Find("a")
		->GetMethod()->SetNativeMethod([] (ScriptMethodPtr m) {
			printf("%s Method : ", m->GetThis()->GetClass()->GetName().data()); m->Trace(0); puts("");
	});
	classC->GetClass()->GetScopeController()->GetMethodScope()->Find("a")
		->GetMethod()->SetNativeMethod([](ScriptMethodPtr m) {
		printf("%s Method : ", m->GetThis()->GetClass()->GetName().data()); m->Trace(0); puts("");
	});

	// allocate memory for B and C objects
	classB->GetClass()->New(objectB);
	classC->GetClass()->New(objectC);

	// trace
	globalScope->Trace();

	// invoke methods
	objectB->GetVariable()->GetObject()->FindMethod("a")->Invoke();
	objectC->GetVariable()->GetObject()->FindMethod("a")->Invoke();

	// trace and clear
	globalScope->Clear();

#ifdef LAME_WINDOWS
	puts("");
	system("pause");
	puts("");
#endif

	return 0;
}

int main0(int argc, char** argv) {

	Clock time;
	StringC fileName;

	time = Time::GetTime();

	if (argc > 1) {
		fileName = argv[1];
	} else {
		fileName = "main.ls";
	}

	ScriptParser parser;
	ScriptBuilder builder;

	try {
		parser.Load(fileName);
		builder.Build(&parser);
        time = Time::GetTime();
        time = Time::GetTime() - time;
	}
	catch (SyntaxException& e) {
		puts("------------------------");
		e.Debug();
	}
	catch (Exception& e) {
		e.Debug();
		puts("\n------------------------");
	}
    
#pragma push_macro("printf")
#pragma push_macro("puts")

#undef printf
#undef puts

	puts("\n------------------------");
	printf("Elapsed Time : %d ms", Uint32(time));
	puts("\n------------------------");

#pragma pop_macro("printf")
#pragma pop_macro("puts")

#ifdef LAME_WINDOWS
	if (argc == 1) {
		system("pause");
	}
#endif
	return 0;
}