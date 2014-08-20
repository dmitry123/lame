#ifndef __LAME_COMPILER_REGISTER__
#define __LAME_COMPILER_REGISTER__

#include "Define.h"

LAME_BEGIN2(Compiler)

class LAME_API Register : public Variable {
public:
	Register(BufferRefC name, ClassPtr classType) : Variable(name, NULL, classType) {
		this->isAvailable = TRUE;
	}
public:
	Void Release() {
		this->isAvailable = TRUE;
	}
	Bool Reserve() {
		if (!this->isAvailable) {
			return FALSE;
		}
		this->isAvailable = FALSE;
		return TRUE;
	}
private:
	Bool isAvailable;
};

LAME_END2

#endif // ~__LAME_COMPILER_REGISTER__