#ifndef __LAME_SCRIPT__CORE_SEGMENT_LINKER__
#define __LAME_SCRIPT__CORE_SEGMENT_LINKER__

#include "Segment.h"

LAME_BEGIN2(Script)

class LAME_API SegmentLinker {
public:
	SegmentLinker();
	~SegmentLinker();
public:
	Bool Add(SegmentPtr segment);
	Bool Remove(SegmentPtr segment);
	Void Load(StringC fileName);
	Void Save(StringC fileName);
public:
	typedef struct {
		Uint64 singature;
		Uint32 segmentCount;
	} HeaderInfo;
public:
	typedef struct {
		Uint32 position;
		Uint32 length;
		Uint32 nameLength;
	} SegmentInfo;
public:
	inline Vector<SegmentPtr>& GetSegmentList() {
		return this->segmentList;
	}
	inline Uint32 GetPosition() {
		return this->position;
	}
private:
	Uint32 position;
	Vector<SegmentPtr> segmentList;
	Bool isLoaded;
private:
	static StringC signature;
};

LAME_END2

#endif // ~__LAME_SCRIPT__CORE_SEGMENT_LINKER__