#ifndef __LAME_SCRIPT_LOW_LEVEL_STACK__
#define __LAME_SCRIPT_LOW_LEVEL_STACK__

#include "Register.h"

LAME_BEGIN2(Compiler)

class LAME_API LowLevelStack {
public:
	LowLevelStack(ScopePtr rootScope);
public:
	typedef enum {
		Integer,
		Float,
		Pointer
	} Type;
public:
	RegisterPtr FindRegister(Type type);
	Void ReleaseRegister(RegisterPtr var = NULL);
	Void Push(VariablePtr var);
	VariablePtr Pop(Void);
public:
	RegisterPtr r0;
	RegisterPtr r1;
	RegisterPtr r2;
	RegisterPtr r3;
	RegisterPtr r4;
	RegisterPtr r5;
	RegisterPtr r6;
	RegisterPtr r7;
	RegisterPtr r8;
	RegisterPtr r9;
public:
	RegisterPtr i0;
	RegisterPtr i1;
	RegisterPtr i2;
	RegisterPtr i3;
	RegisterPtr i4;
	RegisterPtr i5;
	RegisterPtr i6;
	RegisterPtr i7;
	RegisterPtr i8;
	RegisterPtr i9;
public:
	RegisterPtr f0;
	RegisterPtr f1;
	RegisterPtr f2;
	RegisterPtr f3;
	RegisterPtr f4;
	RegisterPtr f5;
	RegisterPtr f6;
	RegisterPtr f7;
	RegisterPtr f8;
	RegisterPtr f9;
private:
	Core::Set<RegisterPtr> rRegList;
	Core::Set<RegisterPtr> iRegList;
	Core::Set<RegisterPtr> fRegList;
	Core::Stack<VariablePtr> callStack;
	Script::MethodPtr registerClass;
};

LAME_END2

#endif // ~__LAME_SCRIPT_LOW_LEVEL_STACK__