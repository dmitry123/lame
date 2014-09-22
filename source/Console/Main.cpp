#include "Main.h"

#include <conio.h>

using namespace Lame::Core;
using namespace Lame::Compiler;
using namespace Lame::ResourceManager;
using namespace Lame::Script;
using namespace Lame;

void AnalizeJavaCore() {

	StringC javaCorePath = "d:/code/c++/projects/lame/java";
	List<Buffer> list = Directory::GetFiles(javaCorePath, TRUE, "java");
	Uint32 javaCorePathLength = strlen(javaCorePath);
	Uint32 totalFiles = list.size();
	Uint32 i = 0;
	Uint32 failed = 0;

	for (BufferRefC b : list) {

		printf("%.2f%% : %s", Float32(i) / totalFiles * 100,
			b.data() + javaCorePathLength);

		FileParser fp;
		SyntaxBuilder sb;

		try {
			fp.Load(b);
			sb.Build(&fp, NULL);
		}
		catch (...) {
			printf(" - FAILED");
			failed++;
		}

		printf("\n");

		++i;
	}

	printf("\nFailed : %d [%.4f%%]", failed, Float32(failed) / totalFiles);
	printf("\nCompleted : %d [%.4f%%]\n\n", totalFiles - failed, Float32(failed) / totalFiles);
}

void NeverReadWarning(ScopePtr rootScope) {

	Set<ObjectPtr> neverRead = rootScope->Filter([](ObjectPtr object) {
		return !object->GetReads() && object->CheckType(Object::Type::Variable) &&
			!object->CheckModificator(Object::Modificator::Internal) &&
			!object->CheckModificator(Object::Modificator::Constant);
	}, TRUE);

	for (ObjectPtr c : neverRead) {
		PostSyntaxWarning(c->GetNode()->lex->line, "Variable (%s) never read",
			c->GetName().data());
	}
}

int main(int argc, char** argv) {

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
	NodePtr rootNode;
	CodeBuilder codeBuilder;
	Package packageManager;
	AbstractCompiler abstractCompiler;

	try {
		/* Launch timer */
		time = Time::GetTime();

		/* Set path to Java Core */
		packageManager.SetJavaCorePath("d:/code/c++/projects/lame/java/");

		/* Initialize root scope */
		rootScope = GlobalScope::Create(
			"Main", FALSE);

		/* Load, parse file and build syntax tree */
		fileParser.Load(fileName);
		syntaxBuilder.Build(&fileParser, NULL);

		/* Get syntax's root node */
		rootNode = syntaxBuilder.GetRootNode();

		/* Run scope and code builders */
		scopeBuilder.Build(rootNode, rootScope);
		codeBuilder.Build(&syntaxBuilder, rootScope);

#if 0
		/* Trace root scope */
		rootScope->Trace(0);
#endif

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

		/* Create new inspector and append to compiler */
		//abstractCompiler.Inspect(new LameCompiler(&abstractCompiler));

		/* Compile code */
		//abstractCompiler.Run(&codeBuilder, &syntaxBuilder,
			//rootScope, segmentBuilder.GetCodeSegment());

		/* Link code segment */
		segmentLinker.Add(segmentBuilder.GetCodeSegment());

#if 0
		/* Trace segments */
		segmentBuilder.GetTextSegment()->Trace(FALSE);
		printf("\n");
		segmentBuilder.GetDataSegment()->Trace(FALSE);
		printf("\n");
		segmentBuilder.GetCodeSegment()->Trace(TRUE);
		printf("\n");
#endif

		/* Trace opcode */
		ByteCode::Trace(&segmentBuilder);
	}
	catch (Throwable& e) {
		puts("\n+---------------------------+");
		e.Debug(Console::GetInstance());
		puts("");
	}

	time = Time::GetTime() - time;

	puts("+---------------------------+");
	printf("Elapsed Time : %d ms", Uint32(time));
	printf("\n+---------------------------+\n");

#ifdef LAME_WINDOWS
	if (argc == 1) {
		system("pause");
	}
	puts("");
#endif

	return 0;
}