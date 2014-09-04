#include "LameTester.h"

ScopePtr LameTester::Test(StringC fileName) {

	this->rootScope = Scope::CreateRootScope();

	try {
		fileParser.Load(fileName);
		nodeBuilder.Build(&fileParser);
		scopeBuilder.Build(nodeBuilder.GetRootNode(), rootScope);
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