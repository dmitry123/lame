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
	SegmentLinker segmentLinker;
	SegmentBuilderPtr segmentBuilder;
	CodeTranslator codeTranslator;
	CodeAnalizer codeAnalizer;

	try {
		/* Launch timer */
		time = Time::GetTime();

		/* Initialize root scope and segment builder */
		rootScope = Scope::CreateRootScope();
		segmentBuilder = new SegmentBuilder(rootScope);

		/* Load and parser file */
		fileParser.Load(fileName);

		/* Build node and scope trees */
		nodeBuilder.Build(&fileParser);
		scopeBuilder.Build(&nodeBuilder, rootScope);

		/* Run code analizer */
		//codeAnalizer.Analize(&nodeBuilder, rootScope);

		puts("");
		for (Uint32 i = 0; i < 79; i++) {
			printf("-");
		}
		puts("\n");

		/* Build segments */
		segmentBuilder->BuildTextSegment();
		segmentBuilder->BuildDataSegment();
		segmentBuilder->BuildCodeSegment();

		/* Link data and text segments */
		segmentLinker.Add(segmentBuilder->GetTextSegment());
		segmentLinker.Add(segmentBuilder->GetDataSegment());

		/* Move code segment to nessesary position */
		segmentBuilder->GetCodeSegment()->SetOffset(
			segmentLinker.GetPosition());

		/* Run translator and compile code */
		codeTranslator.Run(&nodeBuilder, rootScope,
			segmentBuilder->GetCodeSegment());

		/* Link code segment */
		segmentLinker.Add(segmentBuilder->GetCodeSegment());

		/* Trace segments */
		segmentBuilder->GetTextSegment()->Trace();
		printf("\n");
		segmentBuilder->GetDataSegment()->Trace();
		printf("\n");
		segmentBuilder->GetCodeSegment()->Trace(TRUE);
		printf("\n");

		/* Trace opcode */
		ByteCode::Trace(segmentBuilder);
	}
	catch (SyntaxException& e) {
		puts("\n+---------------------------+");
		e.Debug();
		puts("");
	}
	catch (Exception& e) {
		printf("\n+---------------------------+\n");
		e.Debug(Console::GetInstance());
		printf("\n");
	}
	catch (ThrowableAdapter& e) {
		puts("\n+---------------------------+");
		e.Debug(Console::GetInstance());
		puts("\n+---------------------------+");
	}

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

	delete segmentBuilder;

	return 0;
}