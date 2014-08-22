#include "Segment.h"

LAME_BEGIN2(Script)

static Uint32 ZeroByte32 = 0;

Segment::Segment(StringC name) :
name(name), data(NULL)
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

	if (this->position + size > this->size) {

		if (size > GROW) {
			GROW = size;
		}

		this->data = (Uint8P)realloc(this->data, this->size + GROW);
		this->size += GROW;
	}

	memcpy(this->data + this->position, block, size);

	this->history.push_back({ "", this->position, size });

	this->lastSize = size;
	this->position += size;
}

Uint32P Segment::Write(VariablePtr var) {

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

	if (var->GetVarType() == Variable::Var::String && !var->GetNode()) {
		bufferSize = Uint32(var->stringValue.length()) + 1;
	}

	if (copyBuffer) {
		this->Write(copyBuffer, bufferSize);
		this->history.back().name = var->GetName();
	}
	else {
		this->position += bufferSize;
	}

	return &this->history.back().offset;
}

Void Segment::Trace(Bool asFlat) {

	Uint32 i = 0;

	puts("+----------------------------+");
	printf("| Segment \"%s\" : %d", this->name.data(), this->position);
	printf(" bytes");
	if (this->position < 10) {
		printf("  ");
	}
	else if (this->position < 100) {
		printf(" ");
	}
	printf(" |");
	puts("\n+----------------------------+");

	static StringC whiteSpaces = "\t\n\a\b\v";

	if (asFlat && this->data) {

		printf("| ");

		for (i = 0; i < this->position; i++) {
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
				printf("%.2X ", Uint8P(this->data + h.offset)[i]);
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
		size = this->size;
	}
	else {
		this->size = size;
	}

	if (!size) {
		size = 64;
	}

	this->position = 0;
	this->offset = 0;

	this->data = ZeroMemory(new Uint8[size], size);
}

Void Segment::Flush(Void) {
	this->size = this->position;
}

Uint8P Segment::GetBlockAt(Uint32 offset) {

	if (offset > this->position) {
		return NULL;
	}

	return this->data + offset;
}

Void Segment::Clear(Void) {

	if (this->data) {
		delete this->data;
	}

	this->size = 0;
	this->data = 0;
	this->lastSize = 0;
	this->position = 0;
}

Void Segment::Move(SegmentPtr segment) {

	Uint32 mergeOffset = this->GetPosition();

	for (History& h : segment->history) {

		h.offset += mergeOffset;

		this->history.push_back(h);
		this->position += h.size;
	}

	for (History& h : segment->history) {
		h.offset -= mergeOffset;
	}

	segment->Clear();
}

LAME_END2