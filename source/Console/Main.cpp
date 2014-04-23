#include "Main.h"

using namespace lame;

int main(int argc, char** argv) {

	Clock time;
	StringC fileName;

	time = Time::GetTime();

	if (argc > 1) {
		fileName = argv[1];
	} else {
		fileName = "main.ls";
	}

	ScriptParser parser;
	ScriptBuilder builder;
	ScriptPerformer performer;

	try {
		parser.Load(fileName);
		builder.Build(&parser);
        time = Time::GetTime();
		performer.Evaluate(&builder);
        time = Time::GetTime() - time;
		performer.Trace();
	}
	catch (SyntaxException& e) {
		performer.Trace();
		puts("------------------------");
		e.Debug();
	}
	catch (Exception& e) {
		e.Debug();
		puts("\n------------------------");
	}
    
#pragma push_macro("printf")
#pragma push_macro("puts");

#undef printf
#undef puts

	puts("\n------------------------");
	printf("Elapsed Time : %d ms", Uint32(time));
	puts("\n------------------------");

#pragma pop_macro("printf")
#pragma pop_macro("puts");

#ifdef LAME_WINDOWS
	if (argc == 1) {
		system("pause");
	}
#endif

    return 0;
}