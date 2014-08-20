#include "LameTester.h"

static Void _TestScript(StringC fileName) {

	LameTester lameTester;
	ScopePtr rootScope;

	try {
		rootScope = lameTester.Test((Buffer("../../test/") + fileName).data());

		if (!rootScope->Find("in") || !rootScope->Find("out")) {
			throw ScriptException("Test script must have 'in/out' variables (%s)", fileName);
		}

		if (!rootScope->Find("in")->GetVariable()->Equal(rootScope->Find("out")->GetVariable())) {
			Assert::Fail(_StrToWstr("Invalid expression's result"));
		}
	}
	catch (ThrowableAdapter e) {
		Assert::Fail(_StrToWstr(e.GetErrorBuffer()));
	}
}

TEST_CLASS(TestScript) {

	TEST_METHOD(TestParser) {

		Lame::Script::LexID lexResult[] = {
			Lame::Script::kScriptLexClass,
			Lame::Script::kScriptLexDefault,
			Lame::Script::kScriptLexBraceL,
			Lame::Script::kScriptLexPublic,
			Lame::Script::kScriptLexDefault,
			Lame::Script::kScriptLexDefault,
			Lame::Script::kScriptLexSet,
			Lame::Script::kScriptLexInt,
			Lame::Script::kScriptLexAdd,
			Lame::Script::kScriptLexInt,
			Lame::Script::kScriptLexAdd,
			Lame::Script::kScriptLexInt,
			Lame::Script::kScriptLexSemicolon,
			Lame::Script::kScriptLexBraceR
		};

		Lame::Script::FileParser fileParser;
		Lame::Script::LexID* ptrLexResult = lexResult;

		fileParser.Parse("class A { public int value_1 = 100 + 2 + 3; }");

		for (Lame::Script::LexNodePtrC lexNode : fileParser.GetLexList()) {
			if ((*ptrLexResult++) != lexNode->lex->id) {
				Assert::Fail();
			}
		}
	}

	TEST_METHOD(TestBuilder) {

		Lame::Script::FileParser fileParser;
		Lame::Script::NodeBuilder nodeBuilder;

		fileParser.Parse("class A { public int value = 100 + 2 + 3; }");

		try {
			nodeBuilder.Build(&fileParser);
		}
		catch (Lame::Script::SyntaxException& e) {
			Assert::Fail(_StrToWstr(e.GetErrorBuffer()));
		}
	}

	TEST_METHOD(TestMath1) {
		_TestScript("test-math-1.ls");
	}

	TEST_METHOD(TestMath2) {
		_TestScript("test-math-2.ls");
	}

	TEST_METHOD(TestMath3) {
		_TestScript("test-math-3.ls");
	}

	TEST_METHOD(TestMath4) {
		_TestScript("test-math-4.ls");
	}

	TEST_METHOD(TestMath5) {
		_TestScript("test-math-5.ls");
	}

	TEST_METHOD(TestMath6) {
		_TestScript("test-math-6.ls");
	}

	TEST_METHOD(TestScriptFiles) {

		Core::List<Buffer> list = Core::Directory::GetFiles("../../test/", FALSE, "ls");

		for (BufferRefC b : list) {
			_TestScript(Core::File::GetFileName(b.data()));
		}
	}
};