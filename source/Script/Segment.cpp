#include "Segment.h"
#include "Variable.h"
#include "Method.h"

LAME_BEGIN2(Script)

static Uint32 ZeroByte32 = 0;

Segment::Segment(StringC name) :
	name(name)
{
	this->size = 0;
	this->data = 0;
	this->lastSize = 0;
}

Segment::~Segment(Void) {

	if (this->data) {
		delete this->data;
	}
}

Void Segment::Write(VoidP block, Uint32 size) {

	if (!this->data) {
		throw SegmentException("Segment not allocated", 0);
	}
	if (this->position + size > this->size) {
		this->data = (Uint8P) realloc(this->data, this->size + 64);
		this->size += 64;
	}

	memcpy(this->data + this->position, block, size);

	this->lastSize = size;
	this->position += size;
}

Uint32P Segment::Write(VariablePtr var) {

	VoidP copyBuffer = 0;
	Uint32 bufferSize = 0;
	Buffer hexString;

	bufferSize = var->GetSizeOf();

	if (var->GetVarType() == Variable::Var::String) {
		copyBuffer = (VoidP)var->stringValue.data();
	} else if (var->GetVarType() == Variable::Var::Integer) {
		copyBuffer = &var->v.intValue;
	} else if (var->GetVarType() == Variable::Var::Float) {
		copyBuffer = &var->v.floatValue;
	}

	if (var->GetVarType() == Variable::Var::String && !var->GetNode()) {
		bufferSize = var->stringValue.length() + 1;
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

	if (!this->size) {
		printf("| Segment \"%s\" is empty   |\n", this->name.data());
		puts("-----------------------------");
		return;
	}

	puts("-----------------------------");
	printf("| Segment \"%s\" : %d bytes |", this->name.data(), this->size);
	puts("\n-----------------------------");
	printf("| name   | address  | size  |");
	puts("\n-----------------------------");

	for (History& h : this->history) {

		printf("| ");

		Uint32 nameLength = h.name.length();

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

		printf(" | %.8X | %d", h.offset, h.size);

		if (h.size < 10) {
			printf("     ");
		} else if (h.size < 100) {
			printf("    ");
		} else if (h.size < 1000) {
			printf("   ");
		} else if (h.size < 10000) {
			printf("  ");
		}

		printf("| ");

		if (h.block) {
			for (Uint32 i = 0; i < (h.size > 10 ? 10 : h.size); i++) {
				printf("%.2X ", Uint8P(h.block)[i]);
			}
			if (h.size > 10) {
				printf("...");
			}
		}

		puts("");
	}

	puts("-----------------------------");

	for (Uint32 i = 0; i < this->size; i++) {

		if (i && !(i % 10)) {
			puts("");
		}
		printf("%.2X ", Uint8P(this->data)[i]);
	}

	puts("\n-----------------------------");
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

LAME_END2