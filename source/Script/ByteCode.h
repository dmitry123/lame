#ifndef __LAME_COMPILER_BYTE_CODE_PRINTER__
#define __LAME_COMPILER_BYTE_CODE_PRINTER__

#include "Assembler.h"
#include "Segment.h"

LAME_BEGIN2(Script)

class LAME_API ByteCode {
public:
	ByteCode(SegmentPtr segment) :
		segment(segment)
	{
	}
public:
	ByteCodePtr New(Asm command);
	ByteCodePtr Write(Uint32 address);
	ByteCodePtr Flush(Void);
public:
	static Void Trace(SegmentBuilderPtr segmentBuilder);
public:
	inline Uint32 GetPosition() const {
		return this->segment->GetPosition();
	}
	inline SegmentPtr GetSegment()  {
		return this->segment;
	}
public:
	inline Void SetSegment(SegmentPtr segment) {
		this->segment  = segment;
	}
public:
	inline ByteCodePtr operator -> () {
		return this;
	}
private:
	List<Buffer> lineList;
	Vector<Uint32> infoList;
	Assembler::AsmInfoPtr asmInfo;
	SegmentPtr segment;
};

LAME_END2

#endif // ~__LAME_COMPILER_BYTE_CODE_PRINTER__