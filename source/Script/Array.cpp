#include "Array.h"
#include "Exception.h"
#include "GlobalScope.h"

LAME_BEGIN2(Script)

Array::Array(BufferRefC name, NodePtr node, ObjectPtr classType, Uint32 size) :
	Variable(name, classType, Type::Array, node)
{
	this->EnableScopeController();

	if (size > 0) {
		this->arrayList.resize(size);
	}

	if (classType->CheckModificator(Modificator::Primitive)) {
		if (classType == GlobalScope::classString) {
			this->varType = Var::StringPtr;
		} else if (classType == GlobalScope::classFloat) {
			this->varType = Var::FloatPtr;
		} else {
			this->varType = Var::IntegerPtr;
		}
	} else {
		this->varType = Var::ObjectPtr;
	}
}

ObjectPtr Array::At(Uint32 index) {

	if (index > this->arrayList.size()) {
		throw ArrayOutOfBoundsException ();
	}

	return this->arrayList[index];
}

Bool Array::Equal(ObjectPtrC object) const {

	if (!object->CheckType(Type::Array)) {
		return FALSE;
	}

	if (this == object) {
		return TRUE;
	}

	if (this->arrayList.size() != object->GetArray()->arrayList.size()) {
		return FALSE;
	}

	return this->arrayList == object->GetArray()->arrayList;
}

Error Array::Clone(ObjectPtrC object) {

	Object::Clone(object);

	if (!object->CheckType(Type::Array)) {
		return Error::Array_ObjectNotArray;
	}

	object->GetArray()->arrayList
		= Vector<ObjectPtr>(this->arrayList.begin(), this->arrayList.end());

	return Error::NoError;
}

Void Array::Trace(Uint32 offset) {

	printf("%s[] %s = { ", this->classType->GetName().data(), this->GetName().data());

	for (Uint32 i = 0; i < this->arrayList.size(); i++) {
		if (this->arrayList[i]) {
			this->arrayList[i]->Trace(offset);
		} else {
			printf("null ");
		}
	}

	printf("}");
}

LAME_END2
