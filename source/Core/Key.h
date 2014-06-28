#ifndef __LAME_CORE__KEY__
#define __LAME_CORE__KEY__

#include "Types.h"

LAME_BEGIN2(Core)

class LAME_API Key {
public:
	typedef Void(*KeyProc)(VoidP);
public:
	Bool Set(VoidP value);
	VoidP Get(Void);
public:
	 Key();
	~Key();
public:
	Key(KeyProc destructor);
private:
	VoidP handle_;
};

LAME_END2

#endif // ~__LAME_CORE__KEY__