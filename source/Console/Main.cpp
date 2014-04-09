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
	lame::ScriptPerformer performer;
	lame::ScriptBuilder builder;
	lame::ScriptVirtualMachine machine;

	try {
		parser
			.Load(fileName);
		builder
			.Analyze(&parser, &performer);
		machine
			.Execute(&performer);
		
		lame::Vector<lame::ScriptNodePtr> result; performer.Evaluate(&performer.root->next->block[1]->block, &result);

		performer
			.Trace();
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

	printf("Elapsed Time : %d ms", lame::Uint32(lame::Time::GetTime() - time));
	puts("\n------------------------");

#ifdef LAME_WINDOWS
	if (argc == 1) {
		system("pause");
	}
#endif

    return 0;
}