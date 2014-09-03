#ifndef __LAME_COMPILER_SEGMENT_LINKER__
#define __LAME_COMPILER_SEGMENT_LINKER__

#include "Define.h"

LAME_BEGIN2(Compiler)

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
		Sint8 singature[10];
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
	static const StringC signature;
};

LAME_END2

#endif // ~__LAME_COMPILER_SEGMENT_LINKER__