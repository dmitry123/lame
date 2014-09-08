#include "Object.h"
#include "Lex.h"

LAME_BEGIN2(Script)

static Buffer _GetRandomScopeName() {

	Uint64 result = 0;

	for (Uint32 i = 0; i < sizeof(result) * 8; i++) {
		if (rand() % 2) {
            result |= Uint64(0x01) << i;
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
	type(type)
{
	this->name = this->Scope::GetName();
	this->path = parent->Path();

	this->segment_ = NULL;
	this->template_ = NULL;
	this->node_ = NULL;
	this->this_ = NULL;
	this->callback_ = NULL;

	this->modificators_ = 0;
	this->address_ = -1;
	this->size_ = 0;
	this->position_ = 0;
}

ObjectPtr Object::SetModificator(Modificator modificator, Bool state) {

	state ?
		this->modificators_ |=  (Uint32)modificator :
		this->modificators_ &= ~(Uint32)modificator;

	if (this->callback_) {
		this->callback_(this->GetOwner(), this);
	}

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
	if (this->CheckModificator(Modificator::Synchronized)) {
		result.append("synchronized ");
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
