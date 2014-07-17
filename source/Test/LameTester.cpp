#include "LameTester.h"

ScopePtr LameTester::Test(StringC fileName) {

	this->rootScope = Scope::GetRootScope();

	try {
		fileParser.Load(fileName);
		nodeBuilder.Build(&fileParser);
		lowLevelStack = new LowLevelStack();
		scopeBuilder.Build(&nodeBuilder, rootScope);
		codeAnalizer.Analize(lowLevelStack, &nodeBuilder, rootScope);
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