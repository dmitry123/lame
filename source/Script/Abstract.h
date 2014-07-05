#ifndef __LAME_SCRIPT__ABSTRACT__
#define __LAME_SCRIPT__ABSTRACT__

#include "Class.h"

LAME_BEGIN2(Script)

class LAME_API Abstract : public Class {
public:
	Abstract(BufferRefC name, NodePtr node = NULL) :
		Class(name, Type::Abstract, node)
	{
		this->SetModificator(Modificator::Abstract);
	}
};

LAME_END2

#endif // ~__LAME_SCRIPT__ABSTRACT__