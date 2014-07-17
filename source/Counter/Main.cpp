#include "Main.h"

#if defined(LAME_WINDOWS)
#define SOURCE_LINK "d:/code/C++/Projects/lame/source/"
#else
#define SOURCE_LINK "/Storage/code/C++/Projects/lame/source/"
#endif

#define CORE_LINK SOURCE_LINK     "Core"
#define SCRIPT_LINK SOURCE_LINK   "Script"
#define RM_LINK SOURCE_LINK       "ResourceManager"
#define CONSOLE_LINK SOURCE_LINK  "Console"
#define COUNTER_LINK SOURCE_LINK  "Counter"
#define COMPILER_LINK SOURCE_LINK "Compiler"
#define TEST_LINK SOURCE_LINK     "Test"
#define VM_LINK SOURCE_LINK       "VirtualMachine"

#define EXTENSION_FILTER "cpp&h&c"

using namespace Lame::Core;
using namespace Lame;

Uint32 GetCountOfLines(List<Buffer>* list) {

	Uint32 lines = 0;
	File file;
	Buffer code;

	for (const Buffer& b : *list) {

		code.clear();
		file.Open(b.data(), "rt");
		code.resize(file.GetSize());
		file.Read((String)code.data(), file.GetSize());
		file.Close();

		for (Sint8 s : code) {
			if (s == '\n' || s == '\r') {
				++lines;
			}
		}
	}

	return lines;
}

Void PrintCountOfLines(StringC name, Uint32 lines) {
	printf("| %s | %d", name, lines);
	if (lines < 10) {
		printf("     ");
	}
	else if (lines < 100) {
		printf("    ");
	}
	else if (lines < 1000) {
		printf("   ");
	}
	else if (lines < 10000) {
		printf("  ");
	}
	else if (lines < 100000) {
		printf(" ");
	}
	printf("|\n");
}

int main() {

	Uint32 totalLines = 0;
	Uint32 lines = 0;
	List<Buffer> list;

	puts("+------------------+");

	try {
		list = Directory::GetFilesAtPath(CORE_LINK, EXTENSION_FILTER);
		lines = GetCountOfLines(&list);
		totalLines += lines;
		PrintCountOfLines("Core    ", lines);

		list = Directory::GetFilesAtPath(SCRIPT_LINK, EXTENSION_FILTER);
		lines = GetCountOfLines(&list);
		totalLines += lines;
		PrintCountOfLines("Script  ", lines);

		list = Directory::GetFilesAtPath(CONSOLE_LINK, EXTENSION_FILTER);
		lines = GetCountOfLines(&list);
		totalLines += lines;
		PrintCountOfLines("Console ", lines);

		list = Directory::GetFilesAtPath(COUNTER_LINK, EXTENSION_FILTER);
		lines = GetCountOfLines(&list);
		totalLines += lines;
		PrintCountOfLines("Counter ", lines);

		list = Directory::GetFilesAtPath(RM_LINK, EXTENSION_FILTER);
		lines = GetCountOfLines(&list);
		totalLines += lines;
		PrintCountOfLines("Resource", lines);

		list = Directory::GetFilesAtPath(COMPILER_LINK, EXTENSION_FILTER);
		lines = GetCountOfLines(&list);
		totalLines += lines;
		PrintCountOfLines("Compiler", lines);

		list = Directory::GetFilesAtPath(TEST_LINK, EXTENSION_FILTER);
		lines = GetCountOfLines(&list);
		totalLines += lines;
		PrintCountOfLines("Test    ", lines);

		list = Directory::GetFilesAtPath(VM_LINK, EXTENSION_FILTER);
		lines = GetCountOfLines(&list);
		totalLines += lines;
		PrintCountOfLines("Machine ", lines);
	}
	catch (Exception& e) {
		e.Debug(Console::GetInstance());
	}

	puts("+------------------+");
	PrintCountOfLines("Total   ", totalLines);
	puts("+------------------+");

#if defined(LAME_WINDOWS)
	puts("");
	system("pause");
#endif

	return 0;
}