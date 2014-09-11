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
    //SegmentLinker segmentLinker;
    //SegmentBuilder segmentBuilder;
    //CodeTranslator codeTranslator;
    NodePtr rootNode;
	CodeBuilder codeBuilder;
	Package packageManager;
    
	try {
		/* Launch timer */
		time = Time::GetTime();

		/* Set path to Java Core */
		packageManager.SetJavaCorePath("d:/code/c++/projects/lame/java/");

		/* Initialize root scope */
		rootScope = GlobalScope::CreateScope(
			"Main", FALSE);

		/* Load, parse file and build syntax tree */
		fileParser.Load(fileName);
		syntaxBuilder.Build(&fileParser, &packageManager);

		/* Get syntax's root node */
		rootNode = syntaxBuilder.GetRootNode();
        
		/* Run scope and code builders */
		scopeBuilder.Build(rootNode, rootScope);
		codeBuilder.Build(&syntaxBuilder, rootScope);

		/* Trace root scope */
		rootScope->Trace(0);

		printf("\n\n");
		puts("+---------------------------+");
		for (NodePtr n : rootNode->blockList) {
			printf("[%s] ", n->word.data());
		}
		printf("\n");
		puts("+---------------------------+");

#if 0
		Set<ObjectPtr> neverRead = rootScope->Filter([] (ObjectPtr object) {
			return !object->GetReads() && object->CheckType(Object::Type::Variable) &&
				!object->CheckModificator(Object::Modificator::Internal) &&
				!object->CheckModificator(Object::Modificator::Constant);
		}, TRUE);

		for (ObjectPtr c : neverRead) {
			PostSyntaxWarning(c->GetNode()->lex->line, "Variable (%s) never read",
				c->GetName().data());
		}
#endif

#if 0
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