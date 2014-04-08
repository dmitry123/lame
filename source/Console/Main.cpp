#include "Main.h"

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
	lame::ScriptPerformer performer;

	try {
		parser
			.Load(fileName)
            .Build(&performer);
		performer
			.Evaluate()
			.Trace();
	}
	catch (lame::SyntaxException& e) {
		puts("\n------------------------");
		e.Debug();
		puts("");
	}
	catch (lame::Exception& e) {
		e.Debug();
	}

	puts("------------------------");
	printf("Elapsed Time : %d ms", lame::Uint32(lame::Time::GetTime() - time));
	puts("\n------------------------");

#ifdef LAME_WINDOWS
	if (argc == 1) {
		system("pause");
	}
#endif

    return 0;
}