#include "core/core.h"
#include "static/static.h"
#include "lang/lang.h"

#include <iostream>
#include <ctime>

extern "C" {
#include "lua/lualib.h"
#include "lua/lauxlib.h"
}

void reportErrors(lua_State *L, int status) {

	if (status != 0) {
		std::cerr << "-- " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}
}

int testTable_Test(lua_State* state) {

	int argc = lua_gettop(state),
		result = 0;

	while (argc > 0) {
		result += lua_tointeger(state, argc--);
	}

	lua_pushinteger(state, result);

	return 1;
}

static const luaL_Reg testLib[] = {
	{ "my_function",   testTable_Test },
	{ NULL, NULL }
};

int testTable(lua_State* state) {

	luaL_register(state, "test", testLib);

	return 1;
}

int main(int argc, char** argv) {

	lua_State* state = lua_open();

	lua_pushcfunction(state, testTable);
	lua_pushstring(state, "test");
	lua_call(state, 1, 0);

	luaL_openlibs(state);
	int status;
	if ((status = luaL_loadfile(state, "main.lua")) == 0) {
		status = lua_pcall(state, 0, LUA_MULTRET, 0);
	}
	reportErrors(state, status);
	lua_close(state);

	system("pause");
}

int main2(int argc, char** argv) {

	/* create script loader, which loads file from filesystem */
	lame::lang::FileLoaderPtr loader = new lame::lang::FileLoader("1.lame");

	/* create script parser to parse script via loader */
	lame::lang::ScriptParserPtr parser = new lame::lang::ScriptParser(loader,
		lame::lang::java::TokenManager::getManager()
	);

	std::clock_t t = std::clock();

	/* parse file */
	try {
		parser->parse();
	}
	catch (std::exception* e) {
		lame::lang::SyntaxErrorPtr error = dynamic_cast<lame::lang::SyntaxErrorPtr>(e);
		if (error != nullptr) {
			std::cout << "An error occurred while parsing file : \"" << error->what() << " (" << error->getCode() << ")\"" << std::endl;
		}
		else {
			std::cout << "An error occurred while parsing file : \"" << e->what() << "\"" << std::endl;
		}
		return 1;
	}

	/* create syntax builder to build language tree */
	lame::lang::java::SyntaxBuilderPtr syntax = new lame::lang::java::SyntaxBuilder(
		lame::lang::java::TokenManager::getManager(), parser
	);

	/* run building process */
	try {
		syntax->build();
	}
	catch (std::exception* e) {
		lame::lang::SyntaxErrorPtr error = dynamic_cast<lame::lang::SyntaxErrorPtr>(e);
		if (error != nullptr) {
			std::cout << "An error occurred while building language : \"" << loader->getName() << "\" (" << error->getLine() << ") : " << error->what() << std::endl;
		}
		else {
			std::cout << "An error occurred while building language : \"" << e->what() << "\"" << std::endl;
		}
		return 1;
	}

	/* create scope builder to build scope tree */
	lame::lang::ScopeBuilderPtr scope = new lame::lang::java::ScopeBuilder(syntax->getRoot());

	/* run building process */
	try {
		scope->build();
	}
	catch (std::exception* e) {
		lame::lang::SyntaxErrorPtr error = dynamic_cast<lame::lang::SyntaxErrorPtr>(e);
		if (error != nullptr) {
			std::cout << "An error occurred while building scopes : \"" << loader->getName() << "\" (" << error->getLine() << ") : " << error->what() << std::endl;
		}
		else {
			std::cout << "An error occurred while building scopes: \"" << e->what() << "\"" << std::endl;
		}
		return 1;
	}

	/* create analyzer to analyze syntax */
	lame::lang::AnalyzerPtr analyzer = new lame::lang::java::Analyzer(scope);

	/* run analyzing process */
	try {
		analyzer->analyze();
	}
	catch (std::exception* e) {
		lame::lang::SyntaxErrorPtr error = dynamic_cast<lame::lang::SyntaxErrorPtr>(e);
		if (error != nullptr) {
			std::cout << "An error occurred while analyzing file: \"" << loader->getName() << "\" (" << error->getLine() << ") : " << error->what() << std::endl;
		}
		else {
			std::cout << "An error occurred while analyzing file: \"" << e->what() << "\"" << std::endl;
		}
		return 1;
	}

	auto root = scope->getRoot();

	/* release alocated memory */
	delete analyzer;
	delete scope;
	delete syntax;
	delete parser;
	delete loader;

	printf("Elapsed Time : %.2f\n", (std::clock() - t) / 1000.0f);

	return 0;
}