#ifndef __LAME_COMPILER_SEGMENT_CONTAINER__
#define __LAME_COMPILER_SEGMENT_CONTAINER__

#include "Define.h"

LAME_BEGIN2(Compiler)

class LAME_API SegmentContainer {
	typedef Script::SegmentPtr SegmentPtr;
public:
	SegmentContainer(SegmentLinkerPtr linker) :
		segmentLinker(linker)
	{
	}
public:
	Void Compile(Void);
	SegmentPtr Add(SegmentPtr segment);
	Bool Erase(SegmentPtr segment);
public:
	inline SegmentLinkerPtr GetLinker() {
		return this->segmentLinker;
	}
	inline Set<SegmentPtr> GetList() {
		return this->segmentList;
	}
private:
	Set<SegmentPtr> segmentList;
	SegmentLinkerPtr segmentLinker;
};

LAME_END2

#endif // ~__LAME_COMPILER_SEGMENT_CONTAINER__