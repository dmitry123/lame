#include "Main.h"

int main() {

	lame::ScriptParser parser;
	lame::ScriptPerformer performer;
	lame::Clock time;

	time = lame::Time::GetTime();
    
	try {
		parser
			.Load("main.ls")
            .Sort();
		performer
            .RegisterVariables(parser)
			.FixBraces(parser)
			.ComputeJumps(parser);
		parser
			.Trace();
		performer
			.EvaluateScript(parser)
			.Trace();
	}
	catch (lame::SyntaxException e) {
		puts("\n------------------------");
		e.Debug();
		puts("");
	}
	catch (lame::Exception e) {
		puts("\n------------------------");
		e.Debug();
	}

	puts("------------------------");
	printf("Elapsed Time : %d ms", lame::Uint32(lame::Time::GetTime() - time));
	puts("\n------------------------");

#ifdef LAME_WINDOWS
	system("pause");
#endif

    return 0;
}