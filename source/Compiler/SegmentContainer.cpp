#include "SegmentContainer.h"

LAME_BEGIN2(Compiler)

Void SegmentContainer::Compile(Void) {

}

SegmentPtr SegmentContainer::Add(SegmentPtr segment) {

	if (this->segmentList.count(segment)) {
		return NULL;
	}

	this->segmentList.insert(segment);

	return segment;
}

Bool SegmentContainer::Erase(SegmentPtr segment) {

	if (!this->segmentList.count(segment)) {
		return FALSE;
	}

	this->segmentList.erase(segment);

	return TRUE;
}

LAME_END2