#include "Main.h"

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
		argv[1] : "main.lame";

	NodeBuilder nodeBuilder;
	FileParser fileParser;
	ScopeBuilder scopeBuilder;
	ScopePtr rootScope;
	LowLevelStackPtr lowLevelStack;
	SegmentLinker segmentLinker;
	SegmentBuilderPtr segmentBuilder;
	CodeTranslator codeTranslator;
	CodeAnalizer codeAnalizer;

	rootScope = Scope::CreateRootScope();
	time = Time::GetTime();

	try {
		/* Load and parser file */
		fileParser.Load(fileName);

		/* File node and scope trees */
		nodeBuilder.Build(&fileParser);
		scopeBuilder.Build(&nodeBuilder, rootScope);

		/* Run code analizer */
		//        codeAnalizer.Analize(&nodeBuilder, rootScope);

		puts("");
		for (Uint32 i = 0; i < 79; i++) {
			printf("-");
		}
		puts("\n");

		/* Allocate memory for segment builder and low level stack */
		lowLevelStack = new LowLevelStack(rootScope);
		segmentBuilder = new SegmentBuilder(rootScope);

		/* Build segments */
		segmentBuilder->BuildTextSegment();
		segmentBuilder->BuildDataSegment();

		/* Link segments */
		segmentLinker.Add(segmentBuilder->GetTextSegment());
		segmentLinker.Add(segmentBuilder->GetDataSegment());

		/* Trace segments */
		//segmentBuilder->GetTextSegment()->Trace();
		//printf("\n");
		//segmentBuilder->GetDataSegment()->Trace();
		//printf("\n");

		/* Set code segment offset */
		ByteCodePrinter::GetInstance()
			->SetPosition(segmentLinker.GetPosition());

		/* Run translator */
		codeTranslator.Analize(&nodeBuilder, rootScope);
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

	//rootScope->Trace(0);
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