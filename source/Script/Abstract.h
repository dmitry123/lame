#ifndef __LAME_SCRIPT__ABSTRACT__
#define __LAME_SCRIPT__ABSTRACT__

#include "Class.h"

LAME_BEGIN2(Script)

class LAME_API ScriptAbstract : public ScriptClass {
public:
	ScriptAbstract(BufferRefC name) :
		ScriptClass(name, Type::Abstract)
	{
		this->SetModificator(Modificator::Abstract);
	}
};

LAME_END2

#endif // ~__LAME_SCRIPT__ABSTRACT__