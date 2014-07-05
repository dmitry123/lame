#include "Main.h"
#include "Test.h"

using namespace Lame::Core;
using namespace Lame::ResourceManager;
using namespace Lame::Script;
using namespace Lame;

int main(int argc, char** argv) {

#if 0
	ScriptError::SetErrorHandler([](ScriptError error) {
		printf("Script Error : %u (%s)\n", error.GetCode(), error.GetDescription())
	});
#endif

#ifdef LAME_VLD
	puts("");
#endif
    
	StringC fileName;
	Clock time;
	ScopeControllerPtr g;

	fileName = argc > 1 ? argv[1] : "main.ls";

	NodeBuilder nodeBuilder;
	FileParser fileParser;
	NodePerformer nodePerformer;

	g = GlobalScope::GetInstance();

	try {
		time = Time::GetTime();
		fileParser.Load(fileName);
		nodeBuilder.Build(&fileParser);
		nodePerformer.Perform(&nodeBuilder);
		time = Time::GetTime() - time;
	}
	catch (SyntaxException& e) {
		printf("\n-------------------------");
		GlobalScope::GetInstance()->Trace(0, FALSE);
		puts("\n-------------------------");
		e.Debug();
		goto _AvoidTrace;
	}
	catch (Exception& e) {
		printf("\n-------------------------");
		GlobalScope::GetInstance()->Trace(0, FALSE);
		e.Debug();
		puts("\n-------------------------");
		goto _AvoidTrace;
	}

	printf("\n-------------------------");
	g->Trace(0, FALSE);
	puts("\n---------------------------");
_AvoidTrace:

	Uint32 dataSegmentSize;
	Uint8P dataSegmentBuffer;

	printf("| %s\t| %s   | %s |\n", "name", "offset", "size");
	puts("---------------------------");

	dataSegmentSize = g->Size();
	dataSegmentBuffer = new Uint8[dataSegmentSize];
	memset(dataSegmentBuffer, -1, dataSegmentSize);
	Uint32 offset = 0;
	g->Write(dataSegmentBuffer, &offset);

	puts("---------------------------");
	printf("Memory : %d bytes", dataSegmentSize);
	puts("\n-------------------------");

	Buffer b;
	for (Sint32 i = 0; i < (Sint32) dataSegmentSize; i++) {
		b.Format("%.8X", Uint8P(dataSegmentBuffer)[i]);
		if (i && !(i % 10)) {
			puts("");
		}
		printf("%c%c ", b[b.length() - 2], b[b.length() - 1]);
	}

#pragma push_macro("printf")
#pragma push_macro("puts")

#undef printf
#undef puts

	puts("\n-------------------------");
	printf("Elapsed Time : %d ms", Uint32(time));
	printf("\n-------------------------\n");

#pragma pop_macro("printf")
#pragma pop_macro("puts")

#ifdef LAME_WINDOWS
	if (argc == 1) {
		system("pause");
	}
#  ifdef LAME_VLD
	puts("");
#  endif
#endif

	return 0;
}