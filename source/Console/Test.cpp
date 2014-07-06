#include "Test.h"

#include <Script/Script.h>

using namespace Lame::Script;

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
    
	// declare interface A with one empty method "a"
	interfaceA = globalScope->GetClassScope()->Add(new ScriptInterface("A"));
	interfaceA->GetScopeController()->GetMethodScope()->Add(new ScriptMethod("a", interfaceA, ScriptGlobalScope::classVoid));
    
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

void TestAbstract() {
    
	ScriptGlobalScope globalScope;
    
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
	interfaceI->GetScopeController()->GetMethodScope()->Add(new ScriptMethod("a", interfaceI, ScriptGlobalScope::classVoid))
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
    ->GetMethod()->SetNativeMethod([](ScriptMethodPtr m) {
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
}