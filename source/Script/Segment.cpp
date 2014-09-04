#include "Segment.h"

LAME_BEGIN2(Script)

static Uint32 ZeroByte32 = -1;
static Uint64 ZeroByte64 = -1;

Segment::Segment(StringC name) :
	name(name), data(NULL), offset(0)
{
	this->Clear();
}

Segment::~Segment(Void) {
	this->Clear();
}

Void Segment::Write(VoidP block, Uint32 size) {

	static Uint32 GROW = 64;

	if (!this->data) {
		this->Allocate(size);
	}

	if (this->size + size > this->capacity) {

		if (size > GROW) {
			GROW = size;
		}

		this->data = (Uint8P)realloc(this->data, this->capacity + GROW);
		this->capacity += GROW;
	}

	memcpy(this->data + this->size, block, size);

	this->history.push_back({ "", this->size, size });

	this->lastSize = size;
	this->size += size;
}

Void Segment::Write(VariablePtr var) {

	Uint8P copyBuffer = 0;
	Uint32 bufferSize = 0;
	Buffer hexString;
	Float32 float32;

	bufferSize = var->GetClass()->Size();

	if (var->GetVarType() == Variable::Var::String) {
		copyBuffer = Uint8P(var->stringValue.data());
	}
	else if (var->GetVarType() == Variable::Var::Integer) {
		copyBuffer = Uint8P(&var->v.intValue);
	}
	else if (var->GetVarType() == Variable::Var::Float) {
		if (var->GetClass()->IsFloat()) {
			float32 = Float32(var->v.floatValue);
			copyBuffer = Uint8P(&float32);
		}
		else {
			copyBuffer = Uint8P(&var->v.floatValue);
		}
	}
	else if (var->GetVarType() == Variable::Var::Object) {
		copyBuffer = Uint8P(&ZeroByte64);
	}
	else if (var->GetVarType() == Variable::Var::Array) {
		copyBuffer = Uint8P(&ZeroByte64);
		bufferSize = 8;
	}


	if (var->GetVarType() == Variable::Var::String && !var->GetNode()) {
		bufferSize = Uint32(var->stringValue.length()) + 1;
	}

	if (copyBuffer) {
		this->Write(copyBuffer, bufferSize);
		this->history.back().name = var->GetName();
		this->history.back().object = var;
	}
	else {
		this->size += bufferSize;
	}
}

Void Segment::Trace(Bool asFlat) {

	Uint32 i = 0;

	puts("+----------------------------+");
	printf("| Segment \"%s\" : %d", this->name.data(), this->size);
	printf(" bytes");
	if (this->size < 10) {
		printf("  ");
	}
	else if (this->size < 100) {
		printf(" ");
	}
	printf(" |");
	puts("\n+----------------------------+");

	static StringC whiteSpaces = "\t\n\a\b\v";

	if (asFlat && this->size) {

		printf("| ");

		for (i = 0; i < this->size; i++) {
			if (i && !(i % 9)) {
				printf("| \n| ");
			}
			printf("%.2X ", this->data[i]);
		}

		while (i++ % 9) {
			printf("   ");
		}

		puts("|\n+----------------------------+");
	}
	else {
		printf("| name   | address  | size   |");
		puts("\n+----------------------------+");

		for (History& h : this->history) {

			printf("| ");

			Uint32 nameLength = Uint32(h.name.length());

			if (nameLength == 1 && strchr(whiteSpaces, h.name[0])) {
				printf("%d    ", h.name[0]);
			}
			else {
				if (nameLength > 5) {
					for (Uint32 i = 0; i < 4; i++) {
						printf("%c", h.name.data()[i]);
					}
					printf("..");
				}
				else {
					printf("%s", h.name.data());
					for (Uint32 i = 0; i < 6 - nameLength; i++) {
						printf(" ");
					}
				}
			}

			printf(" | %.8X | %d", h.offset, h.size);

			if (h.size < 10) {
				printf("      ");
			}
			else if (h.size < 100) {
				printf("     ");
			}
			else if (h.size < 1000) {
				printf("    ");
			}
			else if (h.size < 10000) {
				printf("   ");
			}

			printf("| ");

			for (Uint32 i = 0; i < h.size; i++) {
				if (i && !(i % 16)) {
					printf("\n|        |          |        | ");
				}
				printf("%.2X ", Uint8P(this->data + h.offset - this->offset)[i]);
			}

			if (h.size > 16 || &h == &this->history.back()) {
				puts("\n+----------------------------+");
			}
			else {
				puts("");
			}
		}
	}
}

Void Segment::Allocate(Uint32 size) {

	if (!size) {
		size = this->capacity;
	}
	else {
		this->capacity = size;
	}

	if (!size) {
		size = 64;
	}

	this->size = 0;
	this->data = Uint8P(malloc(size));
}

Void Segment::Flush(Void) {

	if (this->capacity == this->size) {
		return;
	}

	this->data = (Uint8P)realloc(this->data, this->size);
	this->capacity = this->size;
}

Uint8P Segment::GetBlockAt(Uint32 offset) {

	if (offset > this->size) {
		return NULL;
	}

	return this->data + offset;
}

Void Segment::Clear(Void) {

	if (this->data) {
		delete this->data;
	}

	for (History& h : this->history) {
		if (h.object) {
			h.object->SetAddress(-1);
		}
	}

	this->history.clear();

	this->capacity = 0;
	this->data = 0;
	this->lastSize = 0;
	this->size = 0;
}

Void Segment::Merge(SegmentPtr segment) {

	if (this == segment) {
		return;
	}

	Uint32 mergeOffset = this->GetSize();

	for (History& h : segment->history) {
		this->history.push_back(h.Offset(mergeOffset));
	}

	if (segment->size) {
		this->Write(segment->data, segment->size);
	}

	segment->Clear();
}

ObjectPtr Segment::Fetch(Uint32 address) {

	for (History& h : this->history) {
		if (h.offset == address) {
			return h.object;
		}
	}

	return NULL;
}

Segment::HistoryPtr Segment::FetchHistory(Uint32 address) {

	for (History& h : this->history) {
		if (h.offset == address) {
			return &h;
		}
	}

	return NULL;
}

LAME_END2