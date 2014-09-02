#ifndef __LAME_CORE_BUFFER__
#define __LAME_CORE_BUFFER__

#include "Types.h"
#include "List.h"

#include <string>

LAME_BEGIN2(Core)

class Buffer : public std::string {
public:
	Buffer() {
	}
	~Buffer() {
	}
public:
	Buffer(const char* string) : std::string(string) {}
	Buffer(char* string) : std::string(string) {}
	Buffer(const char  symbol) : std::string(&symbol, 1) {}
	Buffer(const std::string& string) : std::string(string) {}
	Buffer(const char* string, Uint32 length) : std::string(string, length) {}
	Buffer(char* string, Uint32 length) : std::string(string, length) {}
	Buffer(const std::string& string, Uint32 length) : std::string(string, length) {}
public:
	Void LAME_API Format(StringC format, ...);
	List <Buffer> LAME_API Split(Sint8 symbol) const;
	Void LAME_API Get(Void);
	Uint32 LAME_API GetHash32(Void) const;
	Uint64 LAME_API GetHash64(Void) const;
public:
	static Uint32 LAME_API GetHash32(StringC string);
	static Uint64 LAME_API GetHash64(StringC string);
};

typedef       Buffer& BufferRef;
typedef const Buffer& BufferRefC;

LAME_END2

#endif // ~__LAME_CORE_BUFFER__