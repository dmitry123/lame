#include "Main.h"
#include "Test.h"

using namespace Lame::Compiler;
using namespace Lame::Core;
using namespace Lame::ResourceManager;
using namespace Lame::Script;
using namespace Lame;

int main(int argc, char** argv) {

#ifdef LAME_VLD
	puts("");
#endif

	StringC fileName;
	Clock time;
	ScopeControllerPtr g;

	fileName = argc > 1 ? argv[1] : "main.ls";

	g = GlobalScope::GetInstance();

	NodeBuilder nodeBuilder;
	FileParser fileParser;
	NodePerformer nodePerformer;
	ScopeBuilder scopeBuilder;
	SegmentBuilder segmentBuilder(g);
	SegmentLinker segmentLinker;

	try {
		time = Time::GetTime();
		fileParser.Load(fileName);
		nodeBuilder.Build(&fileParser);
		nodePerformer.Overload();
		scopeBuilder.Build(&nodeBuilder, g);
		time = Time::GetTime() - time;
	}
	catch (SyntaxException& e) {
		printf("\n---------------------------");
		GlobalScope::GetInstance()->Trace(0, FALSE);
		puts("\n---------------------------");
		e.Debug();
		goto _AvoidTrace;
	}
	catch (Exception& e) {
		printf("\n---------------------------");
		GlobalScope::GetInstance()->Trace(0, FALSE);
		e.Debug();
		puts("\n---------------------------");
		goto _AvoidTrace;
	}

	printf("\n-----------------------------");
	g->Trace(0, FALSE);
	puts("\n-----------------------------");
_AvoidTrace:

	try {
		segmentLinker.Add(
			segmentBuilder.BuildDataSegment());
		segmentLinker.Add(
			segmentBuilder.BuildTextSegment());
		segmentLinker.Add(
			segmentBuilder.BuildCodeSegment());

		GlobalScope::SetCodeSegment(
			segmentBuilder.GetCodeSegment());

		segmentBuilder.GetCodeSegment()->SetOffset(
			segmentLinker.GetPosition());

		VirtualMachine::ByteCodePrinter::GetInstance()
			->SetPosition(segmentLinker.GetPosition());

		nodePerformer.Perform(&nodeBuilder);

		//segmentBuilder.GetDataSegment()->Trace();
		//segmentBuilder.GetTextSegment()->Trace();
		//segmentBuilder.GetCodeSegment()->Trace();

		Buffer fcName = File::GetFileNameWithoutExtension(fileName) + ".lc";

		segmentLinker.Save(fcName);
	}
	catch (SyntaxException e) {
		puts("-----------------------------");
		e.Debug();
		puts("");
	}
	catch (Exception e) {
		puts("-----------------------------");
		e.Debug();
		puts("");
	}

#pragma push_macro("printf")
#pragma push_macro("puts")

#undef printf
#undef puts

	puts("-----------------------------");
	printf("Elapsed Time : %d ms", Uint32(time));
	printf("\n---------------------------\n");

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