#include "Main.h"
#include "Test.h"

using namespace Lame::Core;
using namespace Lame::Compiler;
using namespace Lame::ResourceManager;
using namespace Lame::Script;
using namespace Lame;

int main(int argc, char** argv) {

#ifdef LAME_VLD
	puts("");
#endif

	StringC fileName;
	Clock time;

	fileName = argc > 1 ?
		argv[1] : "main.ls";

	NodeBuilder nodeBuilder;
	FileParser fileParser;
	ScopeBuilder scopeBuilder;
	ScopePtr rootScope;
	CodeAnalizer codeAnalizer;
	LowLevelStackPtr lowLevelStack;
	SegmentLinker segmentLinker;
	SegmentBuilderPtr segmentBuilder;

	rootScope = Scope::GetRootScope();
	time = Time::GetTime();

	try {
		/* Load and parser file */
		fileParser.Load(fileName);

		/* File node and scope trees */
		nodeBuilder.Build(&fileParser);
		scopeBuilder.Build(&nodeBuilder, rootScope);

		puts("");
		for (Uint32 i = 0; i < 80; i++) {
			printf("-");
		}

		/* Allocate memory for segment builder and low level stack */
		lowLevelStack = new LowLevelStack();
		segmentBuilder = new SegmentBuilder(rootScope);

		/* Build segments */
		segmentBuilder->BuildTextSegment();
		segmentBuilder->BuildDataSegment();
		segmentBuilder->BuildCodeSegment();

		/* Link segments */
		segmentLinker.Add(segmentBuilder->GetTextSegment());
		segmentLinker.Add(segmentBuilder->GetDataSegment());
		segmentLinker.Add(segmentBuilder->GetCodeSegment());

		/* Trace segments */
		printf("\n");
		segmentBuilder->GetTextSegment()->Trace();
		printf("\n");
		segmentBuilder->GetDataSegment()->Trace();

		//codeAnalizer.Analize(lowLevelStack, &nodeBuilder, rootScope);
	}
	catch (SyntaxException& e) {
		printf("\n---------------------------");
		rootScope->Trace(0);
		puts("\n---------------------------");
		e.Debug();
		puts("");
		goto _AvoidTrace;
	}
	catch (Exception& e) {
		printf("\n---------------------------");
		rootScope->Trace(0);
		e.Debug(Console::GetInstance());
		puts("\n---------------------------");
		goto _AvoidTrace;
	}
	catch (ThrowableAdapter& e) {
		printf("\n---------------------------");
		rootScope->Trace(0);
		puts("\n---------------------------");
		e.Debug(Console::GetInstance());
		puts("\n---------------------------");
		goto _AvoidTrace;
	}

	rootScope->Trace(0);
	puts("\n");
_AvoidTrace:

#pragma push_macro("printf")
#pragma push_macro("puts")

#undef printf
#undef puts

	time = Time::GetTime() - time;

	puts("+---------------------------+");
	printf("Elapsed Time : %d ms", Uint32(time));
	printf("\n+---------------------------+\n");

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