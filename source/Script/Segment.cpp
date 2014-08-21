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
		throw SegmentException("Segment not allocated", 0);
	}
	if (this->position + size > this->size) {
		if (size > GROW) {
			GROW = size;
		}
		this->data = (Uint8P)realloc(this->data, this->size + GROW);
		this->size += GROW;
	}

	memcpy(this->data + this->position, block, size);

	this->lastSize = size;
	this->position += size;
}

Uint32P Segment::Write(VariablePtr var) {

	VoidP copyBuffer = 0;
	Uint32 bufferSize = 0;
	Buffer hexString;

	bufferSize = var->GetClass()->Size();

	if (var->GetVarType() == Variable::Var::String) {
		copyBuffer = (VoidP)var->stringValue.data();
	} else if (var->GetVarType() == Variable::Var::Integer) {
		copyBuffer = &var->v.intValue;
	} else if (var->GetVarType() == Variable::Var::Float) {
		copyBuffer = &var->v.floatValue;
	}

	if (var->GetVarType() == Variable::Var::String && !var->GetNode()) {
		bufferSize = Uint32(var->stringValue.length()) + 1;
	}
    
	this->history.push_back({ var->GetName().data(),
		position, bufferSize, copyBuffer
	});

	if (copyBuffer) {
		this->Write(copyBuffer, bufferSize);
	} else {
		this->position += bufferSize;
	}

	return &this->history.back().offset;
}

Uint32P Segment::Write(MethodPtr method) {

	this->history.push_back({ method->GetName().data(),
		position, Object::SizeOf, &ZeroByte32
	});

	this->Write(&ZeroByte32, Object::SizeOf);

	return &this->history.back().offset;
}

Void Segment::Trace(Uint32 columns) {

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
	printf("| name   | address  | size   |");
	puts("\n+----------------------------+");

    static StringC whiteSpaces = "\t\n\a\b\v";
    
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
			} else {
				printf("%s", h.name.data());
				for (Uint32 i = 0; i < 6 - nameLength; i++) {
					printf(" ");
				}
			}
		}

		printf(" | %.8X | %d", h.offset, h.size);

		if (h.size < 10) {
			printf("      ");
		} else if (h.size < 100) {
			printf("     ");
		} else if (h.size < 1000) {
			printf("    ");
		} else if (h.size < 10000) {
			printf("   ");
		}

		printf("| ");

		if (h.block) {
			for (Uint32 i = 0; i < h.size; i++) {
				if (i && !(i % 16)) {
					printf("\n|        |          |        | ");
				}
				printf("%.2X ", Uint8P(h.block)[i]);
			}
		}

		if (h.size > 16 || &h == &this->history.back()) {
			puts("\n+----------------------------+");
		}
		else {
			puts("");
		}
	}
}

Void Segment::Allocate(Uint32 size) {

	if (!size) {
		size = this->size;
	} else {
		this->size = size;
	}

	if (!size) {
		size = 64;
	}

	this->position = 0;
	this->offset = 0;

	this->data =
		new Uint8[size];

	memset(this->data, 0, this->size);
}

Void Segment::Grow(Uint32 size) {
	this->size += size;
}

Void Segment::Flush(Void) {
	this->size = this->position;
}

Uint8P Segment::GetBlockAt(Uint32 offset) {
	return this->data + offset;
}

Void Segment::Clear(Void) {

	if (this->data) {
		delete this->data;
	}

	this->size = 0;
	this->data = 0;
	this->lastSize = 0;
}

Void Segment::Merge(SegmentPtr segment) {

	Uint32 mergeOffset = this->GetPosition();

	for (History& h : segment->history) {

		h.offset += mergeOffset;

		this->history.push_back(h);
		this->position += h.size;
	}

	for (History& h : segment->history) {
		h.offset -= mergeOffset;
	}
}

LAME_END2