#ifndef __LAME_CORE_STREAM__
#define __LAME_CORE_STREAM__

#include "Types.h"

LAME_BEGIN2(Core)

class Stream {
public:
	virtual Void Skip(Uint32 length) = 0;
	virtual Uint32 Available() = 0;
	virtual Void Close() = 0;
	virtual Void Reset() = 0;
};

class InputStream : public Stream {
public:
	virtual Void Read() = 0;
	virtual Void Read(VoidP buffer, Uint32 length) = 0;
};

class OutputStream : public Stream {
public:
	virtual Void Write() = 0;
	virtual Void Write(VoidP buffer, Uint32 length) = 0;
};

class PrintStream : public Stream {
public:
	virtual Void Print() = 0;
	virtual Void Print(StringC string, ...) = 0;
};

LAME_END2

#endif // ~__LAME_CORE_INPUT_STREAM__