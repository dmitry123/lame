#include "Main.h"

using namespace Lame::Core;
using namespace Lame::Compiler;
using namespace Lame::ResourceManager;
using namespace Lame::Script;
using namespace Lame;

#include <iostream>

void tprintf(const char* format) {
	std::cout << format;
}

template<typename T, typename... Targs>
void tprintf(const char* format, T value, Targs... Fargs) {
	while (*format) {
		if (*format == '/' && *(format + 1) == '@') {
			std::cout << '@';
			format += 2;
		}
		if (*format == '@') {
			std::cout << value;
			tprintf(format + 1, Fargs...);
			return;
		}
		std::cout << *format++;
	}
}

template <class ...Args> Uint32 getArgCount(Args... a) {
	return sizeof...a;
}

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
	CodeAnalizer codeAnalizer;
	NodePtr rootNode;

	try {
		/* Launch timer */
		time = Time::GetTime();

		/* Load, parse file and build syntax tree */
		fileParser.Load(fileName);
		syntaxBuilder.Build(&fileParser);

		/* Initialize root scope and fetch root node */
		rootScope = Scope::CreateRootScope("Main", TRUE);
		rootNode = syntaxBuilder.GetRootNode();

		/* Build scope trees */
		scopeBuilder.Build(rootNode, rootScope);

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

		segmentLinker.Save("main.lc");

		/* Trace opcode */
		ByteCode::Trace(&segmentBuilder);
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