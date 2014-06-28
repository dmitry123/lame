#include "GlobalScope.h"

LAME_BEGIN2(Script)

ScriptScopeControllerPtr ScriptGlobalScope::globalScope_
	= new ScriptScopeController();

LAME_END2