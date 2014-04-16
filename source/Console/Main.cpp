#include "Main.h"

#include <memory>

int main(int argc, char** argv) {

	lame::Clock time;
	lame::StringC fileName;

	time = lame::Time::GetTime();

	if (argc > 1) {
		fileName = argv[1];
	} else {
		fileName = "main.ls";
	}

	lame::ScriptParser parser;
	lame::ScriptBuilder builder;
	lame::ScriptVirtualMachine machine;

	try {
		parser.Load(fileName);
		builder.Build(&parser, &machine);
        
        time = lame::Time::GetTime();
        machine.Execute();
        time = lame::Time::GetTime() - time;
        
        machine.Trace();
	}
	catch (lame::SyntaxException& e) {
		puts("\n------------------------");
		e.Debug();
		puts("\n------------------------");
	}
	catch (lame::Exception& e) {
		e.Debug();
		puts("\n------------------------");
	}
    
	printf("Elapsed Time : %d ms", lame::Uint32(time));
	puts("\n------------------------");

#ifdef LAME_WINDOWS
	if (argc == 1) {
		system("pause");
	}
#endif

    return 0;
}