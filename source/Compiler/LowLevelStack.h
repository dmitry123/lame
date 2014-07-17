#ifndef __LAME_SCRIPT_LOW_LEVEL_STACK__
#define __LAME_SCRIPT_LOW_LEVEL_STACK__

#include "Register.h"

LAME_BEGIN2(Compiler)

class LAME_API LowLevelStack {
public:
	LowLevelStack();
public:
	typedef enum {
		Integer,
		Float,
		Pointer
	} Type;
public:
	RegisterPtr FindRegister(Type type);
	Void ReleaseRegister(RegisterPtr var = NULL);
	Void Push(RegisterPtr var);
	RegisterPtr Pop(Void);
public:
	RegisterPtr p0;
	RegisterPtr p1;
	RegisterPtr p2;
	RegisterPtr p3;
	RegisterPtr p4;
	RegisterPtr p5;
	RegisterPtr p6;
	RegisterPtr p7;
	RegisterPtr p8;
	RegisterPtr p9;
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
public:
	inline Script::ScopePtr GetRegisterScope() {
		return &this->registerScope;
	}
private:
	Core::USet<RegisterPtr> iRegList;
	Core::USet<RegisterPtr> fRegList;
	Core::USet<RegisterPtr> pRegList;
	Script::Scope registerScope;
};

LAME_END2

#endif // ~__LAME_SCRIPT_LOW_LEVEL_STACK__