#include "Scope.h"
#include "Exception.h"
#include "Lex.h"
#include "Object.h"
#include "Class.h"
#include "Method.h"

#include <utility>

LAME_BEGIN2(Script)

ObjectPtr Scope::Add(ObjectPtr object) {

	HashMap::iterator i;
	Hash hash = object->Hash();

	if (Lex::Find(object->GetName()) != NULL) {
		PostSyntaxError(0, "Reserved name (%s)", object->GetName().data());
	}

	if ((i = this->hashMap_.find(hash)) != this->hashMap_.end() && object->GetName().length() > 0) {
		return LAME_NULL;
	}

	this->hashMap_.insert(std::make_pair(hash, object));
	this->stringMap_.insert(std::make_pair(object->GetName(), object));

	object->GetObservable()
		->Add(this);

	this->Update(object);

	return object;
}

Void Scope::Remove(ObjectPtr object) {

	HashMap::iterator i;
	StringMap::iterator j;

	if ((i = this->hashMap_.find(object->Hash())) == this->hashMap_.end() ||
		(j = this->stringMap_.find(object->GetName())) == this->stringMap_.end()
	) {
		return;
	}

	object->GetObservable()
		->Remove(this);

	if ((object = i->second) != NULL) {
		object->Release();
	}

	if (this->publicSet_.count(object)) {
		this->publicSet_.erase(object);
	}
	if (this->staticSet_.count(object)) {
		this->staticSet_.erase(object);
	}
	if (this->methodSet_.count(object)) {
		this->methodSet_.erase(object);
	}
	if (this->variableSet_.count(object)) {
		this->variableSet_.erase(object);
	}
	if (this->classSet_.count(object)) {
		this->classSet_.erase(object);
	}

	this->hashMap_.erase(i);
	this->stringMap_.erase(j);
}

ObjectPtr Scope::Find(Hash hash) {

	HashMap::iterator i;
	ScopePtr scope;

	scope = this;

	while (scope) {

		if ((i = scope->hashMap_.find(hash)) != scope->hashMap_.end()) {
			return i->second;
		}

		scope = scope->parentScope_;
	}

	return NULL;
}

ObjectPtr Scope::Find(BufferRefC name) {

	StringMap::iterator i;
	ScopePtr scope;

	scope = this;

	while (scope) {

		if ((i = scope->stringMap_.find(name)) != scope->stringMap_.end()) {
			return i->second;
		}

		scope = scope->parentScope_;
	}

	return NULL;
}

Void Scope::Merge(ScopePtrC scope) {

	if (this == scope) {
		return;
	}

	for (auto& i : scope->hashMap_) {
		this->Add(i.second->Clone(i.second->GetName(), ObjectPtr(scope)));
	}
}

Void Scope::Clone(ScopePtrC scope) {

	this->Clear();
	this->Merge(scope);
}

Void Scope::Move(ScopePtrC scope) {

	this->Clear();

	if (this == scope) {
		return;
	}

	for (auto& i : scope->hashMap_) {

		if (i.second->CheckModificator(Object::Modificator::Static)) {
			this->Add(i.second);
		}
		else {
			if (i.second->CheckType(Object::Type::Method)) {
				this->Add(i.second);
			}
			else {
				this->Add(i.second);
			}
		}
	}

	isOwner_ = FALSE;
}

Uint32 Scope::Amount(Void) {
	return (Uint32) this->hashMap_.size();
}

Void Scope::Clear(Void) {

	while (this->hashMap_.size()) {
		this->Remove(this->hashMap_.begin()->second);
	}

	this->hashMap_.clear();
	this->stringMap_.clear();
	this->publicSet_.clear();
	this->staticSet_.clear();
	this->methodSet_.clear();
	this->variableSet_.clear();
	this->classSet_.clear();
}

Void Scope::Trace(Uint32 offset) {

	for (auto& i : this->hashMap_) {

		if (i.second->CheckModificator(Object::Modificator::Primitive) ||
			i.second->CheckModificator(Object::Modificator::Register) ||
			i.second->CheckModificator(Object::Modificator::Internal) ||
			i.second->CheckModificator(Object::Modificator::Constant)
		) {
			continue;
		}

		Lex::PrintLine(offset);

		i.second->Trace(offset);
	}
}

Uint32 Scope::Size(Void) {

	Uint32 size = 0;

	for (auto& i : this->hashMap_) {

		if (i.second->CheckModificator(Object::Modificator::Register) ||
			i.second->CheckModificator(Object::Modificator::Internal)
		) {
			continue;
		}

		size += i.second->Size();
	}

	return size;
}

Void Scope::Update(ObjectPtr object) {

	if (this->publicSet_.count(object)) {
		this->publicSet_.erase(object);
	}
	if (this->staticSet_.count(object)) {
		this->staticSet_.erase(object);
	}
	if (this->methodSet_.count(object)) {
		this->methodSet_.erase(object);
	}
	if (this->variableSet_.count(object)) {
		this->variableSet_.erase(object);
	}
	if (this->classSet_.count(object)) {
		this->classSet_.erase(object);
	}

	if (object->CheckModificator(Object::Modificator::Private)) {
		// ignore
	}
	else if (object->CheckModificator(Object::Modificator::Protected)) {
		// ignore
	}
	else {
		this->publicSet_.insert(object);
	}

	if (object->CheckModificator(Object::Modificator::Static)) {
		this->staticSet_.insert(object);
	}

	if (object->CheckType(Object::Type::Class)) {
		this->classSet_.insert(object);
	}
	else if (object->CheckType(Object::Type::Variable)) {
		this->variableSet_.insert(object);
	}
	else {
		this->methodSet_.insert(object);
	}
}

ScopePtr Scope::Root(Void) {

	ScopePtr root = this;

	while (root->parentScope_ != NULL) {
		root = root->parentScope_;
	}

	return root;
}

Buffer Scope::Path(Void) {

	ScopePtr root = this;
	Buffer resultPath;

	while (root && root->parentScope_) {
		resultPath += root->GetName() + '@';
		root = root->parentScope_;
	}

	return resultPath;
}

Void Scope::Flush(Void) {

	this->publicSet_.clear();
	this->staticSet_.clear();
	this->methodSet_.clear();
	this->variableSet_.clear();
	this->classSet_.clear();

	for (auto i : this->stringMap_) {
		this->Update(i.second);
	}
}

Scope::Scope(BufferRefC name, ScopePtr parent) {

	this->scopeName_ = name;
	this->parentScope_ = parent;
	this->isOwner_ = TRUE;
}

Scope::~Scope() {
	this->Clear();
}

ScopePtr Scope::GetRootScope(Void) {

	Script::ClassPtr rootScope = new Script::Class("Root", NULL, 0);

	classChar = rootScope->Scope::Add(new Script::Class("char", rootScope, 1))->GetClass();
	classByte = rootScope->Scope::Add(new Script::Class("byte", rootScope, 1))->GetClass();
	classBoolean = rootScope->Scope::Add(new Script::Class("boolean", rootScope, 1))->GetClass();
	classShort = rootScope->Scope::Add(new Script::Class("short", rootScope, 2))->GetClass();
	classInt = rootScope->Scope::Add(new Script::Class("int", rootScope, 4))->GetClass();
	classLong = rootScope->Scope::Add(new Script::Class("long", rootScope, 8))->GetClass();
	classFloat = rootScope->Scope::Add(new Script::Class("float", rootScope, 4))->GetClass();
	classDouble = rootScope->Scope::Add(new Script::Class("double", rootScope, 8))->GetClass();
	classVoid = rootScope->Scope::Add(new Script::Class("void", rootScope, 0))->GetClass();
	classString = rootScope->Scope::Add(new Script::Class("String", rootScope))->GetClass();
	classObject = rootScope->Scope::Add(new Script::Class("Object", rootScope))->GetClass();
	classClass = rootScope->Scope::Add(new Script::Class("Class", rootScope))->GetClass();
	classUnknown = rootScope->Scope::Add(new Script::Class("?", rootScope))->GetClass();

	classChar
		->SetModificator(Script::Object::Modificator::Primitive)
		->SetModificator(Script::Object::Modificator::Internal);
	classByte
		->SetModificator(Script::Object::Modificator::Primitive)
		->SetModificator(Script::Object::Modificator::Internal);
	classBoolean
		->SetModificator(Script::Object::Modificator::Primitive)
		->SetModificator(Script::Object::Modificator::Internal);
	classShort
		->SetModificator(Script::Object::Modificator::Primitive)
		->SetModificator(Script::Object::Modificator::Internal);
	classInt
		->SetModificator(Script::Object::Modificator::Primitive)
		->SetModificator(Script::Object::Modificator::Internal);
	classLong
		->SetModificator(Script::Object::Modificator::Primitive)
		->SetModificator(Script::Object::Modificator::Internal);
	classFloat
		->SetModificator(Script::Object::Modificator::Primitive)
		->SetModificator(Script::Object::Modificator::Internal);
	classDouble
		->SetModificator(Script::Object::Modificator::Primitive)
		->SetModificator(Script::Object::Modificator::Internal);
	classVoid
		->SetModificator(Script::Object::Modificator::Primitive)
		->SetModificator(Script::Object::Modificator::Internal);

	classString->SetModificator(Script::Object::Modificator::Internal);
	classObject->SetModificator(Script::Object::Modificator::Internal);
	classClass->SetModificator(Script::Object::Modificator::Internal);
	classUnknown->SetModificator(Script::Object::Modificator::Internal);

	classObject->Script::Scope::Add(new Script::Method("toString", classObject, classObject, classString))
		->GetMethod()->SetNativeMethod([](Script::MethodPtr method) { /* Nothing */ });

	classChar->SetPriority(0);
	classByte->SetPriority(0);
	classBoolean->SetPriority(0);
	classShort->SetPriority(1);
	classInt->SetPriority(2);
	classLong->SetPriority(3);
	classFloat->SetPriority(4);
	classDouble->SetPriority(5);
	classVoid->SetPriority(0);
	classString->SetPriority(6);
	classObject->SetPriority(7);
	classClass->SetPriority(0);
	classUnknown->SetPriority(0);

	return ScopePtr(rootScope);
}

Script::ClassPtr Scope::classChar = NULL;
Script::ClassPtr Scope::classByte = NULL;
Script::ClassPtr Scope::classBoolean = NULL;
Script::ClassPtr Scope::classShort = NULL;
Script::ClassPtr Scope::classInt = NULL;
Script::ClassPtr Scope::classLong = NULL;
Script::ClassPtr Scope::classFloat = NULL;
Script::ClassPtr Scope::classDouble = NULL;
Script::ClassPtr Scope::classVoid = NULL;
Script::ClassPtr Scope::classString = NULL;
Script::ClassPtr Scope::classObject = NULL;
Script::ClassPtr Scope::classClass = NULL;
Script::ClassPtr Scope::classUnknown = NULL;

LAME_END2
