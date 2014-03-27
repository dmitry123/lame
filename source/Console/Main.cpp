#include "Main.h"

int main() {

	lame::ScriptParser parser;

	parser.Parse("int value = 123;");

#ifdef LAME_WINDOWS
	system("pause");
#endif

    return 0;
}