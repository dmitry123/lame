#ifndef __LAME_CORE_CONSOLE_PRINTER__
#define __LAME_CORE_CONSOLE_PRINTER__

#include "Stream.h"

LAME_BEGIN2(Core)

class LAME_API Console : public PrintStream {
public:
	static ConsolePtr GetInstance() {
		return consolePrinter;
	}
public:
	Void Print() override;
	Void Print(StringC format, ...) override;
	Void Skip(Uint32 length) override;
	Uint32 Available() override;
	Void Close() override;
	Void Reset() override;
private:
	Console() {
	}
private:
	static ConsolePtr consolePrinter;
};

LAME_END2

#endif // ~__LAME_CORE_CONSOLE_PRINTER__