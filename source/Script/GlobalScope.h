#ifndef __LAME_SCRIPT__GLOBAL_SCOPE__
#define __LAME_SCRIPT__GLOBAL_SCOPE__

#include "ScopeController.h"

LAME_BEGIN2(Script)

class LAME_API ScriptGlobalScope {
public:
	inline ScriptScopeControllerPtr operator -> () {
		return globalScope_;
	}
private:
	static ScriptScopeControllerPtr globalScope_;
};

LAME_END2

#endif // ~__LAME_SCRIPT__GLOBAL_SCOPE__