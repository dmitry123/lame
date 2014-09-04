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

    SyntaxBuilder syntaxBuilder;
    FileParser fileParser;
    ScopeBuilder scopeBuilder;
    ScopePtr rootScope;
    SegmentLinker segmentLinker;
    SegmentBuilder segmentBuilder;
    CodeTranslator codeTranslator;
    NodePtr rootNode;
	CodeBuilder codeBuilder;

	try {
		/* Launch timer */
		time = Time::GetTime();

		/* Load, parse file and build syntax tree */
		fileParser.Load(fileName);
		syntaxBuilder.Build(&fileParser);

		/* Initialize root scope */
		rootScope = Scope::CreateRootScope(
			/* Root class name */
			"Main",
			/* Shall root be a class */
			FALSE);

		/* Get syntax's root node */
		rootNode = syntaxBuilder.GetRootNode();

		/* Build scope trees */
		scopeBuilder.Build(rootNode, rootScope);

		/* Check code for syntax errors */
		codeBuilder.Run(&syntaxBuilder, rootScope,
			segmentBuilder.GetCodeSegment());

		for (CodeMethod& m : codeBuilder.GetMethodList()) {

			MethodPtr m2 = m.GetMethod();

			printf("%s %s %s%s(%s)\n", m2->GetModificatorString().data(), m2->GetReturnType()->GetName().data(),
				m2->GetPath().data(), m2->GetName().data(), m2->GetFormattedArguments().data());

			for (CodeNodePtr n : m.GetList()) {

				printf("%4d : (%6s) : ", n->GetLine(), n->GetName().data());

				if (n->GetLeft() && n->GetRight()) {
					printf("%s, %s", n->GetLeft()->GetName().data(), n->GetRight()->GetName().data());
				}
				else if (n->GetLeft()) {
					printf("%s", n->GetLeft()->GetName().data());
				}

				if (n->GetOffset() != -1) {
					printf("%d", n->GetOffset());
				}

				printf("\n");
			}
		}

#if 0
		/* Trace root scope */
		rootScope->Trace(0);

		/* Build segments */
		segmentBuilder.BuildTextSegment(rootScope);
		segmentBuilder.BuildDataSegment(rootScope);
		segmentBuilder.BuildCodeSegment(rootScope);

		/* Link data and text segments */
		segmentLinker.Add(segmentBuilder.GetTextSegment());
		segmentLinker.Add(segmentBuilder.GetDataSegment());

		/* Move code segment to nessesary position */
		segmentBuilder.GetCodeSegment()->SetOffset(
			segmentLinker.GetPosition());

		/* Compile code */
		codeTranslator.Run(&syntaxBuilder, rootScope,
			segmentBuilder.GetCodeSegment());

		/* Link code segment */
		segmentLinker.Add(segmentBuilder.GetCodeSegment());

		/* Trace segments */
		segmentBuilder.GetTextSegment()->Trace(FALSE);
		printf("\n");
		segmentBuilder.GetDataSegment()->Trace(FALSE);
		printf("\n");
		segmentBuilder.GetCodeSegment()->Trace(TRUE);
		printf("\n");

		/* Trace opcode */
		ByteCode::Trace(&segmentBuilder);
#endif
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

	return 0;
}