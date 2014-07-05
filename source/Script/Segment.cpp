#include "Segment.h"

LAME_BEGIN2(Script)

Segment::Segment(Uint32 size) :
	size(size)
{
	if (!size) {
		throw InvalidArgumentException("Size mustn't be 0");
	}


}

Void Segment::Write(VoidP block, Uint32 size) {

}

Void Segment::Write(VariablePtr var) {

}

LAME_END2