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

Object::Object(BufferRefC name, ScopePtr parent, Type type) :
		Scope(name.length() ? name : _GetRandomScopeName(), parent),
	name(this->Scope::GetName()),
	path(parent->Path()),
	type(type)
{
	this->modificators_ = 0;
	this->segment_ = 0;
	this->address_ = 0;
	this->size_ = 0;
	this->template_ = 0;
	this->node_ = 0;
	this->wasInStack = 0;
	this->position_ = 0;
}

ObjectPtr Object::SetModificator(Modificator modificator, Bool state) {

	state ?
		this->modificators_ |=  (Uint32)modificator :
		this->modificators_ &= ~(Uint32)modificator;

	this->Notify(TRUE);

	return this;
}

ObjectPtr Object::SetSegment(SegmentPtr segment, Uint32P address) {

	this->address_ = address;
	this->size_ = segment->GetLastSize();;
	this->segment_ = segment;

	return this;
}

Buffer Object::GetModificatorString(Void) {

	Buffer result;

	if (this->CheckModificator(Modificator::Override)) {
		result.append("override ");
	}
	if (this->CheckModificator(Modificator::Deprecated)) {
		result.append("deprecated ");
	}
	if (this->CheckModificator(Modificator::Public)) {
		result.append("public ");
	}
	if (this->CheckModificator(Modificator::Private)) {
		result.append("private ");
	}
	if (this->CheckModificator(Modificator::Protected)) {
		result.append("protected ");
	}
	if (this->CheckModificator(Modificator::Static)) {
		result.append("static ");
	}
	if (this->CheckModificator(Modificator::Final)) {
		result.append("final ");
	}
	if (this->CheckModificator(Modificator::Native)) {
		result.append("native ");
	}
	if (this->CheckModificator(Modificator::Abstract)) {
		result.append("abstract ");
	}

	return result;
}

Uint64 Object::GetHash64(Void) {
	return Buffer(this->GetPath() + this->GetName()).GetHash64();
}

Uint32 Object::GetHash32(Void) {
	return Buffer(this->GetPath() + this->GetName()).GetHash32();
}

Bool Object::IsInt() const {
	return
		ClassPtr(this) == this->classChar ||
		ClassPtr(this) == this->classByte ||
		ClassPtr(this) == this->classBoolean ||
		ClassPtr(this) == this->classShort ||
		ClassPtr(this) == this->classInt;
}

Bool Object::IsChar() const {
	return ClassPtr(this) == this->classChar;
}

Bool Object::IsByte() const {
	return ClassPtr(this) == this->classByte;
}

Bool Object::IsBoolean() const {
	return ClassPtr(this) == this->classBoolean;
}

Bool Object::IsShort() const {
	return ClassPtr(this) == this->classShort;
}

Bool Object::IsInt8() const {
	return
		ClassPtr(this) == this->classChar ||
		ClassPtr(this) == this->classByte;
}

Bool Object::IsInt16() const {
	return ClassPtr(this) == this->classShort;
}

Bool Object::IsInt32() const {
	return ClassPtr(this) == this->classInt;
}

Bool Object::IsInt64() const {
	return ClassPtr(this) == this->classLong;
}

Bool Object::IsLong() const {
	return ClassPtr(this) == this->classLong;
}

Bool Object::IsFloat() const {
	return ClassPtr(this) == this->classFloat;
}

Bool Object::IsDouble() const {
	return ClassPtr(this) == this->classDouble;
}

Bool Object::IsString() const {
	return ClassPtr(this) == this->classString;
}

Bool Object::IsVoid() const {
	return ClassPtr(this) == this->classVoid;
}

Bool Object::IsObject() const {
	return
		!this->IsInt() &&
		!this->IsLong() &&
		!this->IsFloat() &&
		!this->IsDouble() &&
		!this->IsString() &&
		!this->IsVoid();
}

LAME_END2
