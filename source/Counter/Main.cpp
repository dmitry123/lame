#include "Main.h"

#if defined(LAME_WINDOWS)
#define SOURCE_LINK "d:/code/C++/Projects/lame/source/"
#else
#define SOURCE_LINK "/Storage/code/C++/Projects/lame/source/"
#endif

#define CORE_LINK SOURCE_LINK "Core"
#define SCRIPT_LINK SOURCE_LINK "Script"
#define RM_LINK SOURCE_LINK "ResourceManager"
#define CONSOLE_LINK SOURCE_LINK "Console"
#define COUNTER_LINK SOURCE_LINK "Counter"
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
	printf("%s : %d\n", name, lines);
}

int main() {

	Uint32 totalLines = 0;
	Uint32 lines = 0;

	try {
		List<Buffer> coreList = Directory::GetFilesAtPath(CORE_LINK, EXTENSION_FILTER);

		lines = GetCountOfLines(&coreList);
		totalLines += lines;
		PrintCountOfLines("Core", lines);

		List<Buffer> scriptList = Directory::GetFilesAtPath(SCRIPT_LINK, EXTENSION_FILTER);

		lines = GetCountOfLines(&scriptList);
		totalLines += lines;
		PrintCountOfLines("Script", lines);

		List<Buffer> consoleList = Directory::GetFilesAtPath(CONSOLE_LINK, EXTENSION_FILTER);

		lines = GetCountOfLines(&consoleList);
		totalLines += lines;
		PrintCountOfLines("Console", lines);

		List<Buffer> counterList = Directory::GetFilesAtPath(COUNTER_LINK, EXTENSION_FILTER);

		lines = GetCountOfLines(&counterList);
		totalLines += lines;
		PrintCountOfLines("Counter", lines);

		List<Buffer> rmList = Directory::GetFilesAtPath(RM_LINK, EXTENSION_FILTER);

		lines = GetCountOfLines(&rmList);
		totalLines += lines;
		PrintCountOfLines("ResourceManager", lines);

		List<Buffer> compilerList = Directory::GetFilesAtPath(RM_LINK, EXTENSION_FILTER);

		lines = GetCountOfLines(&compilerList);
		totalLines += lines;
		PrintCountOfLines("Compiler", lines);
	}
	catch (Exception& e) {
		e.Debug();
	}

	PrintCountOfLines("Total", totalLines);

#if defined(LAME_WINDOWS)
	puts("");
	system("pause");
#endif

	return 0;
}