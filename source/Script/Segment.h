#ifndef __LAME_SCRIPT_CORE_SEGMENT__
#define __LAME_SCRIPT_CORE_SEGMENT__

#include "Object.h"
#include "Variable.h"
#include "Method.h"

LAME_BEGIN2(Script)

class LAME_API Segment {
	friend class SegmentLinker;
	friend class SegmentBuilder;
public:
	inline Segment(Buffer name) : Segment(name.data()) {
	}
public:
	Segment(StringC name);
	~Segment(Void);
public:
	Void Write(VoidP block, Uint32 size);
	Uint32P Write(VariablePtr var);
	Void Trace(Bool asFlat = FALSE);
	Void Allocate(Uint32 size = 0);
	Void Flush(Void);
	Uint8P GetBlockAt(Uint32 offset);
	Void Clear(Void);
	Void Move(SegmentPtr segment);
public:
	inline Void SetOffset(Uint32 offset) {
		this->offset = offset;
	}
public:
	inline Uint32 GetPosition() {
		return this->position;
	}
	inline Uint32 GetLastSize() {
		return this->lastSize;
	}
private:
	typedef struct {
		Buffer name;
		Uint32 offset;
		Uint32 size;
	} History;
private:
	Buffer name;
	Uint32 position;
	Uint32 offset;
	Uint8P data;
	Uint32 size;
	Uint32 lastSize;
private:
	List<History> history;
};

LAME_END2

#endif // ~__LAME_SCRIPT_CORE_SEGMENT__