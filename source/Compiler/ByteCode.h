#ifndef __LAME_COMPILER_BYTE_CODE_PRINTER__
#define __LAME_COMPILER_BYTE_CODE_PRINTER__

#include "Assembler.h"

LAME_BEGIN2(Compiler)

class LAME_API ByteCode {
public:
	ByteCode(SegmentPtr segment) :
		segment(segment), position(0)
	{
	}
public:
	ByteCodePtr New(Asm command);
	ByteCodePtr Write(Uint32 address);
	ByteCodePtr Flush(Void);
public:
	inline Uint32     GetPosition() { return this->position; }
	inline SegmentPtr GetSegment()  { return this->segment;  }
public:
	inline Void SetPosition(Uint32 position)   { this->position = position; }
	inline Void SetSegment(SegmentPtr segment) { this->segment  = segment; }
public:
	inline ByteCodePtr operator -> () {
		return this;
	}
private:
	List<Buffer> lineList;
	Vector<Uint32> infoList;
	Assembler assembler;
	AsmInfoPtr asmInfo;
	Uint32 position;
	SegmentPtr segment;
};

LAME_END2

#endif // ~__LAME_COMPILER_BYTE_CODE_PRINTER__