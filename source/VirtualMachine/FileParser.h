#ifndef __LAME_VIRTUAL_MACHINE__FILE_PARSER__
#define __LAME_VIRTUAL_MACHINE__FILE_PARSER__

#include "Define.h"

LAME_BEGIN2(VirtualMachine)

enum {
	kByteUnknown,
	kByteInteger,
	kByteHex,
	kByteFloat,
	kByteColon,
	kByteBracketL,
	kByteBracketR
};

class LAME_API FileParser {
public:
	 FileParser(Void);
	~FileParser(Void);
public:
	Void LoadFile(StringC fileName);
private:

};

LAME_END2

#endif // ~__LAME_VIRTUAL_MACHINE__FILE_PARSER__