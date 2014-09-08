#include "Scope.h"
#include "Lex.h"
#include "Object.h"
#include "Class.h"
#include "Method.h"
#include "Variable.h"

#include <utility>

LAME_BEGIN2(Script)

ObjectPtr Scope::Add(ObjectPtr object) {

	HashMap::iterator i;
	Hash hash = object->Hash();

	if (Lex::Find(object->GetName()) != NULL && Lex::Find(object->GetName())->id != kScriptLexTernary) {
		PostSyntaxError(0, "Reserved name (%s)", object->GetName().data());
	}

#if 0
	if (this->Find(object->GetName())) {
		return LAME_NULL;
	}
#endif

	if ((i = this->hashMap_.find(hash)) != this->hashMap_.end() && object->GetName().length() > 0) {
		return LAME_NULL;
	}

	this->hashMap_.insert(std::make_pair(hash, object));
	this->stringMap_.insert(std::make_pair(object->GetName(), object));

	if (object->CheckType(Object::Type::Class)) {
		this->classSet_.insert(object);
	}
	else if (object->CheckType(Object::Type::Variable)) {
		this->variableSet_.insert(object);
	}
	else if (object->CheckType(Object::Type::Method)) {
		this->methodSet_.insert(object);
	}

	object->ownerScope_ = this;

	if (this->callback_) {
		this->callback_(this, object);
	}

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

	delete object;
}

ObjectPtr Scope::Find(Hash hash, Bool withDepth, Uint32 objectType) {

	HashMap::iterator i;
	ScopePtr scope;

	scope = this;

	while (scope) {

		if ((i = scope->hashMap_.find(hash)) != scope->hashMap_.end()) {
			if (!objectType || i->second->CheckType((Object::Type)objectType)) {
				return i->second;
			}
		}

		if (!withDepth) {
			break;
		}

		scope = scope->parentScope_;
	}

	return NULL;
}

ObjectPtr Scope::Find(BufferRefC name, Bool withDepth, Uint32 objectType) {

	StringMap::iterator i;
	ScopePtr scope;

	scope = this;

	while (scope) {

		if ((i = scope->stringMap_.find(name)) != scope->stringMap_.end()) {
			if (!objectType || i->second->CheckType((Object::Type)objectType)) {
				return i->second;
			}
		}

		if (!withDepth) {
			break;
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

	scope->hashMap_.clear();
	scope->stringMap_.clear();
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

		if (i.second->CheckType(Object::Type::Variable)) {
			size += i.second->Size();
		}
	}

	return size;
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
	Core::Stack<ScopePtr> scopeStack;

	while (root && root->parentScope_) {
		scopeStack.push(root);
		root = root->parentScope_;
	}

	while (!scopeStack.empty()) {
		resultPath += scopeStack.top()->GetName() + '/';
		scopeStack.pop();
	}

	return resultPath;
}

Scope::Scope(BufferRefC name, ScopePtr parent) {

	this->scopeName_ = name;
	this->parentScope_ = parent;
	this->isOwner_ = TRUE;
	this->ownerScope_ = NULL;
	this->callback_ = NULL;
}

Scope::~Scope() {
	this->Clear();
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
