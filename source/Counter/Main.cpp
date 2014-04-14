#include "Main.h"

#if defined(LAME_WINDOWS)
#define SOURCE_LINK "d:/code/C++/Projects/lame/source/"
#else
#define SOURCE_LINK "/Storage/code/C++/Projects/lame/source/"
#endif

#define CORE_LINK SOURCE_LINK "Core"
#define ZLIB_LINK SOURCE_LINK "Core/Zlib"
#define SCRIPT_LINK SOURCE_LINK "Script"
#define CONSOLE_LINK SOURCE_LINK "Console"
#define COUNTER_LINK SOURCE_LINK "Counter"
#define EXTENSION_FILTER "cpp&h&c"

using namespace lame;

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
		List<Buffer> coreList = GetFilesAtPath(CORE_LINK, EXTENSION_FILTER);

		lines = GetCountOfLines(&coreList);
		totalLines += lines;
		PrintCountOfLines("Core", lines);

		List<Buffer> zlibList = GetFilesAtPath(ZLIB_LINK, EXTENSION_FILTER);

		lines = GetCountOfLines(&zlibList);
		totalLines += lines;
		PrintCountOfLines("Zlib", lines);
		
		List<Buffer> scriptList = GetFilesAtPath(SCRIPT_LINK, EXTENSION_FILTER);

		lines = GetCountOfLines(&scriptList);
		totalLines += lines;
		PrintCountOfLines("Script", lines);
		
		List<Buffer> consoleList = GetFilesAtPath(CONSOLE_LINK, EXTENSION_FILTER);

		lines = GetCountOfLines(&consoleList);
		totalLines += lines;
		PrintCountOfLines("Console", lines);

		List<Buffer> counterList = GetFilesAtPath(COUNTER_LINK, EXTENSION_FILTER);

		lines = GetCountOfLines(&counterList);
		totalLines += lines;
		PrintCountOfLines("Counter", lines);
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