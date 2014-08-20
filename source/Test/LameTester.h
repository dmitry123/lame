#ifndef __TEST_LAME_TESTER__
#define __TEST_LAME_TESTER__

#include <Core/Core.h>
#include <Script/Script.h>
#include <Compiler/Compiler.h>

#include "Test.h"

using namespace Lame::Core;
using namespace Lame::Compiler;
using namespace Lame::Script;
using namespace Lame;

class LameTester {
public:
	ScopePtr Test(StringC fileName);
private:
	NodeBuilder nodeBuilder;
	FileParser fileParser;
	ScopeBuilder scopeBuilder;
	ScopePtr rootScope;
	LowLevelStackPtr lowLevelStack;
};

#endif // ~__TEST_LAME_TESTER__