#include "SegmentLinker.h"

LAME_BEGIN2(Compiler)

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
	this->position += segment->GetCapacity();

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

	if (!strcmp(header.singature, signature)) {
		throw SegmentLinkerException("File isn't Lame class (%s)", fileName);
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
		file.Read(segmentObject->GetBlockAt(0),
                  segmentObject->GetCapacity());
		file.SetPosition(filePosition);

		this->segmentList.push_back(segmentObject);
	}

	file.Close();
	isLoaded = TRUE;
}

Void SegmentLinker::Save(StringC fileName) {

	HeaderInfo header;

	strcpy(header.singature, SegmentLinker::signature);
	header.segmentCount = (Uint32)segmentList.size();

	File file;
	Uint32 positionOffset = sizeof(HeaderInfo) +
		sizeof(SegmentInfo) * Uint32(this->segmentList.size());

	file.Open(fileName, "wb");
	file.Write(&header, sizeof(HeaderInfo));

	for (SegmentPtr s : this->segmentList) {
		positionOffset += s->GetName().length() + 1;
	}

	for (SegmentPtr s : this->segmentList) {

		Uint32 nameLength = Uint32(s->GetName().length()) + 1;

		SegmentInfo segment = {
			positionOffset,
			s->GetCapacity(),
			nameLength
		};

		file.Write(&segment, sizeof(SegmentInfo));
		file.Write(VoidP(s->GetName().data()), nameLength);

		positionOffset += s->GetCapacity();
	}

	for (SegmentPtr s : this->segmentList) {
		file.Write(s->GetBlockAt(0), s->GetSize());
	}

	file.Close();
}

const StringC SegmentLinker::signature = "LAME_CLASS";

LAME_END2