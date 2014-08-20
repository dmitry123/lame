#include "LameTester.h"

ScopePtr LameTester::Test(StringC fileName) {

	this->rootScope = Scope::CreateRootScope();

	try {
		fileParser.Load(fileName);
		nodeBuilder.Build(&fileParser);
		lowLevelStack = new LowLevelStack(rootScope);
		scopeBuilder.Build(&nodeBuilder, rootScope);
	}
	catch (SyntaxException& e) {
		Assert::Fail(_StrToWstr(e.GetErrorBuffer()));
	}
	catch (Exception& e) {
		Assert::Fail(_StrToWstr(e.GetErrorBuffer()));
	}
	catch (ThrowableAdapter& e) {
		Assert::Fail(_StrToWstr(e.GetErrorBuffer()));
	}

	return this->rootScope;
}