#ifndef __LAME_SCRIPT_REGISTER__
#define __LAME_SCRIPT_REGISTER__

#include "Define.h"

LAME_BEGIN2(Compiler)

class LAME_API Register : public Script::Variable {
public:
	Register(Core::BufferRefC name, Script::ClassPtr classType) : Script::Variable(name, NULL, classType) {
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

#endif // ~__LAME_SCRIPT_REGISTER__