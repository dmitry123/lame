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
	inline Segment() : Segment("")
	{
	}
	inline Segment(Buffer name) :
		Segment(name.data())
	{
	}
public:
	Segment(StringC name);
	~Segment(Void);
public:
	Void Write(VoidP block, Uint32 size);
	Void Write(VariablePtr var);
	Void Trace(Bool asFlat = FALSE);
	Void Allocate(Uint32 size = 0);
	Void Flush(Void);
	Uint8P GetBlockAt(Uint32 offset);
	Void Clear(Void);
	Void Merge(SegmentPtr segment);
public:
	inline Void SetOffset(Uint32 offset) {
		this->offset = offset;
	}
public:
	inline Uint32 GetCapacity() const { return this->capacity; }
	inline Uint32 GetSize()     const { return this->size;     }
	inline Uint32 GetOffset()   const { return this->offset;   }
	inline Uint32 GetLastSize() const { return this->lastSize; }
public:
	inline Uint32 GetPosition() const {
		return this->GetSize() + this->GetOffset();
	}
private:
	struct History {
	public:
		Buffer name;
		Uint32 offset;
		Uint32 size;
	public:
		History& Offset(Uint32 offset) {

			this->offset += offset;

			if (this->object) {
				this->object->SetAddress(this->offset);
			}

			return *this;
		}
	public:
		ObjectPtr object;
	};
private:
	Buffer name;
	Uint32 size;
	Uint32 offset;
	Uint8P data;
	Uint32 capacity;
	Uint32 lastSize;
private:
	List<History> history;
};

LAME_END2

#endif // ~__LAME_SCRIPT_CORE_SEGMENT__