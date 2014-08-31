#include "SegmentLinker.h"

LAME_BEGIN2(Script)

SegmentLinker::SegmentLinker() {
	this->position = 0;
	this->isLoaded = 0;
}

SegmentLinker::~SegmentLinker() {

	if (this->isLoaded) {
		for (SegmentPtr s : this->segmentList) {
			delete s;
		}
	}
}

Bool SegmentLinker::Add(SegmentPtr segment) {

	if (std::find(this->segmentList.begin(), this->segmentList.end(), segment) != this->segmentList.end()) {
		return FALSE;
	}

	if (this->GetPosition() == segment->GetOffset()) {
		segment->SetOffset(0);
	}

	this->segmentList.push_back(segment);

	for (Segment::History& h : segment->history) {
		h.Offset(this->position);
	}

	segment->SetOffset(this->position);
	this->position += segment->capacity;

	return TRUE;
}

Bool SegmentLinker::Remove(SegmentPtr segment) {

	Vector<SegmentPtr>::iterator i;

	if ((i = std::find(this->segmentList.begin(), this->segmentList.end(), segment)) == this->segmentList.end()) {
		return FALSE;
	}

	this->segmentList.erase(i);

	return TRUE;
}

Void SegmentLinker::Load(StringC fileName) {

	HeaderInfo header;
	SegmentInfo segment;
	File file;

	file.Open(fileName, "rb");
	file.Read(&header, sizeof(HeaderInfo));

	if (header.singature != *Uint32P(&signature)) {
		throw SegmentLinkerException("File isn't LameScript class (%s)", fileName);
	}

	for (Uint32 i = 0; i < header.segmentCount; i++) {

		Buffer segmentName;
		SegmentPtr segmentObject;

		file.Read(&segment, sizeof(SegmentInfo));
		segmentName.resize(segment.nameLength);
		file.Read(VoidP(segmentName.data()), segment.nameLength);

		segmentObject = new Segment(segmentName.data());
		segmentObject->Allocate(segment.length);

		Uint32 filePosition = file.GetPosition();
		file.SetPosition(segment.position);
		file.Read(segmentObject->data, segmentObject->capacity);
		file.SetPosition(filePosition);

		this->segmentList.push_back(segmentObject);
	}

	file.Close();
	isLoaded = TRUE;
}

Void SegmentLinker::Save(StringC fileName) {

	HeaderInfo header = {
		*Uint32P(&signature),
		Uint32(segmentList.size())
	};

	File file;
	Uint32 positionOffset = sizeof(HeaderInfo) +
		sizeof(SegmentInfo) * Uint32(this->segmentList.size());

	file.Open(fileName, "wb");
	file.Write(&header, sizeof(HeaderInfo));

	for (SegmentPtr s : this->segmentList) {
		positionOffset += s->name.length() + 1;
	}

	for (SegmentPtr s : this->segmentList) {

		Uint32 nameLength = Uint32(s->name.length()) + 1;

		SegmentInfo segment = {
			positionOffset,
			s->capacity,
			nameLength
		};

		file.Write(&segment, sizeof(SegmentInfo));
		file.Write(VoidP(s->name.data()), nameLength);

		positionOffset += s->capacity;
	}

	for (SegmentPtr s : this->segmentList) {
		file.Write(s->data, s->size);
	}

	file.Close();
}

StringC SegmentLinker::signature = "LAME_CLS";

LAME_END2