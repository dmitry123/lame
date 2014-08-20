#include "Object.h"
#include "Segment.h"
#include "Lex.h"

#include <stdint.h>

LAME_BEGIN2(Script)

static Buffer _GetRandomScopeName() {

	Uint64 result = 0;

	for (Uint32 i = 0; i < sizeof(result) * 8; i++) {
		if (rand() % 2) {
#ifdef LAME_MSVC
			result |= 0x01ui64 << i;
#else
            result |= Uint64(0x01) << i;
#endif
		}
	}

	Sint8 numberBuffer[20];

	if (sizeof(result) == 8) {
		sprintf(numberBuffer, "%.8x%.8x", Uint32(result >> 32), Uint32(result));
	}
	else if (sizeof(result) == 4) {
		sprintf(numberBuffer, "%.8x", Uint32(result));
	}
	else if (sizeof(result) == 2) {
		sprintf(numberBuffer, "%.4x", Uint32(result));
	}
	else {
		sprintf(numberBuffer, "%.2x", Uint32(result));
	}

	return Buffer(numberBuffer);
}

Object::Object(BufferRefC name, ScopePtr parent, Type type) : Scope(name.length() ? name : _GetRandomScopeName(), parent),
	name(this->Scope::GetName()),
	path(parent->Path()),
	type(type)
{
	this->modificators_ = 0;
	this->segment_ = 0;
	this->address_ = 0;
	this->size_ = 0;
	this->templateClass = 0;
	this->arguments_ = 0;
	this->node_ = 0;
	this->newNode_ = 0;
	this->segmentCodePosition = 0;
}

Object::~Object() {
}

ObjectPtr Object::SetModificator(Modificator modificator, Bool state) {

	state ?
		this->modificators_ |=  (Uint32)modificator :
		this->modificators_ &= ~(Uint32)modificator;

	this->SetChanged(TRUE);
	this->Notify();

	return this;
}

ObjectPtr Object::SetSegment(SegmentPtr segment, Uint32P address) {

	this->address_ = address;
	this->size_ = segment->GetLastSize();;
	this->segment_ = segment;

	return this;
}

Void Object::PrintModificators(Void) {

	ConsolePtr c = Console::GetInstance();

	if (this->CheckModificator(Modificator::Override)) {
		c->Print("override ");
	}
	if (this->CheckModificator(Modificator::Deprecated)) {
		c->Print("deprecated ");
	}
	if (this->CheckModificator(Modificator::Public)) {
		c->Print("public ");
	}
	if (this->CheckModificator(Modificator::Private)) {
		c->Print("private ");
	}
	if (this->CheckModificator(Modificator::Protected)) {
		c->Print("protected ");
	}
	if (this->CheckModificator(Modificator::Static)) {
		c->Print("static ");
	}
	if (this->CheckModificator(Modificator::Final)) {
		c->Print("final ");
	}
	if (this->CheckModificator(Modificator::Native)) {
		c->Print("native ");
	}
	if (this->CheckModificator(Modificator::Abstract)) {
		c->Print("abstract ");
	}
}

Uint64 Object::GetPathHash64(Void) {
	return Uint64(this->GetName().GetHash32()) << 32 | this->GetPath().GetHash32();
}

Uint32 Object::GetPathHash32(Void) {
	return Uint32(this->GetName().GetHash16()) << 16 | this->GetPath().GetHash16();
}

Uint64 Object::GetHash64(Void) {
	return this->Hash();
}

Uint32 Object::GetHash32(Void) {

	Uint64 hash = this->GetHash64();
	Uint32 result = 0;

	for (Uint32 i = 0; i < 64; i++) {
		result = 31 * result + (hash >> i) & 0xffffffff;
	}

	return result;
}

LAME_END2
