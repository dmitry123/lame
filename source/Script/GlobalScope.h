#ifndef __LAME_SCRIPT_GLOBAL_SCOPE__
#define __LAME_SCRIPT_GLOBAL_SCOPE__

#include "Scope.h"
#include "Object.h"
#include "Method.h"
#include "Variable.h"
#include "Class.h"

LAME_BEGIN2(Script)

class LAME_API GlobalScope {
public:
	typedef enum {
		kFlagDefault = 0x0000,
		kFlagClass = 0x0001
	} Flag;
public:
	static ScopePtr Create(Buffer name = "", Uint32 flags = 0);
	static Void Release(ScopePtr rootScope);
};

LAME_END2

#endif // ~__LAME_SCRIPT_GLOBAL_SCOPE__