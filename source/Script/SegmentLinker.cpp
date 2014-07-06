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

	this->segmentList.push_back(segment);

	for (Segment::History& h : segment->history) {
		h.offset += this->position;
	}

	this->position += segment->size;

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

Void SegmentLinker::Load(BufferRefC fileName) {

	HeaderInfo header;
	SegmentInfo segment;
	File file;

	file.Open(fileName.data(), "rb");
	file.Read(&header, sizeof(HeaderInfo));

	if (header.singature != Uint32(signature)) {
		PostErrorMessage("File isn't LameScript class (%s)", fileName.data());
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
		file.Read(segmentObject->data, segmentObject->size);
		file.SetPosition(filePosition);

		this->segmentList.push_back(segmentObject);
	}

	file.Close();
	isLoaded = TRUE;
}

Void SegmentLinker::Save(BufferRefC fileName) {

	HeaderInfo header = {
		Uint32(signature),
		Uint32(segmentList.size())
	};

	File file;
	Uint32 positionOffset = sizeof(HeaderInfo) +
		sizeof(SegmentInfo) * this->segmentList.size();

	file.Open(fileName.data(), "wb");
	file.Write(&header, sizeof(HeaderInfo));

	for (SegmentPtr s : this->segmentList) {
		positionOffset += s->name.length() + 1;
	}

	for (SegmentPtr s : this->segmentList) {

		Uint32 nameLength = s->name.length() + 1;

		SegmentInfo segment = {
			positionOffset,
			s->size,
			nameLength
		};

		file.Write(&segment, sizeof(SegmentInfo));
		file.Write(VoidP(s->name.data()), nameLength);

		positionOffset += s->size;
	}

	for (SegmentPtr s : this->segmentList) {
		file.Write(s->data, s->size);
	}

	file.Close();
}

StringC SegmentLinker::signature = "LSCL";

LAME_END2