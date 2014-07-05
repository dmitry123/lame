#include "Scope.h"
#include "Exception.h"
#include "Variable.h"
#include "Interface.h"
#include "Method.h"
#include "Class.h"
#include "Abstract.h"
#include "Array.h"

#include <utility>

LAME_BEGIN2(Script)

ObjectPtr Scope::Add(ObjectPtr object) {

	HashMap::iterator i;
    StringPair stringPair;
    HashPair hashPair;
    
    stringPair.first = object->GetName();
	stringPair.second = object->Hash();
    
    hashPair.first = object->GetNameHash();
	hashPair.second = object->Hash();

	if ((i = this->hashMap_.find(hashPair)) != this->hashMap_.end() && object->GetName().length() > 0) {
		return LAME_NULL;
	}

	this->stringMap_.insert(MapStringPair(stringPair, object));
	this->hashMap_.insert(MapHashPair(hashPair, object));

	return object;
}

Void Scope::Remove(ObjectPtr var) {

	StringMap::iterator i;
	HashMap::iterator j;
    
    StringPair stringPair;
    HashPair hashPair;
    
	stringPair.first = var->GetName();
	stringPair.second = var->Hash();

	hashPair.first = var->GetNameHash();
	hashPair.second = var->Hash();

	if ((i = this->stringMap_.find(stringPair)) == this->stringMap_.end() ||
		(j = this->hashMap_.find(hashPair)) == this->hashMap_.end()
	) {
		return;
	}

	ObjectPtr object = i->second;

	if (object->CheckType(Object::Type::Variable) &&
		object->GetVariable()->GetVarType() == Variable::Var::Object
	) {
		ClassPtr clss = object->GetVariable()->GetObject();

		if (clss->DecRef()) {
			if (isOwner_) {
				delete object;
			}
		}
	}
	else {
		if (isOwner_) {
			delete object;
		}
	}

	this->stringMap_.erase(i);
	this->hashMap_.erase(j);
}

ObjectPtr Scope::Find(StringC name, Uint32 rightHash) {
	return this->Find(Buffer::GetHash(name), rightHash);
}

ObjectPtr Scope::Find(Uint32 hash, Uint32 rightHash) {

	HashMap::iterator i;
	HashPair hashPair;

	if (rightHash == -1) {
		rightHash = hash;
	}

	hashPair.first = hash;
	hashPair.second = (Uint64)hash << 32 | rightHash;

	if ((i = this->hashMap_.find(hashPair)) == this->hashMap_.end()) {
		return LAME_NULL;
	}

	return i->second;
}

static void _CloneObject(ScopePtr scope, ObjectPtr i) {

	if (i->CheckModificator(Class::Modificator::Static)) {
		scope->Add(i);
	}
	else {
		if (i->CheckType(Object::Type::Class)) {
			scope->Add(i);
		}
		else if (i->CheckType(Object::Type::Interface)) {
			scope->Add(new Interface(*InterfacePtr(i)));
		}
		else if (i->CheckType(Object::Type::Method)) {
			scope->Add(i);
		}
		else if (i->CheckType(Object::Type::Variable)) {
			scope->Add(new Variable(*(VariablePtr)i));
		}
		else if (i->CheckType(Object::Type::Abstract)) {
			scope->Add(new Abstract(*(AbstractPtr)i));
		}
		else if (i->CheckType(Object::Type::Array)) {
			scope->Add(new Array(*(ArrayPtr)i));
		}
	}
}

Void Scope::Merge(ScopePtrC scope) {

	if (this == scope) {
		return;
	}

	for (auto& i : scope->stringMap_) {

		if (i.second->CheckModificator(Object::Modificator::Static)) {
			_CloneObject(this, i.second);
		}
		else {
			if (i.second->CheckType(Object::Type::Method)) {
				_CloneObject(this, i.second);
			}
			else {
				_CloneObject(this, i.second);
			}
		}
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

	for (auto& i : scope->stringMap_) {

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

	while (this->stringMap_.size()) {
		this->Remove(this->stringMap_.begin()->second);
	}
}

Void Scope::Trace(Uint32 offset) {

	Uint32 count = 0;

	for (auto& i : this->stringMap_) {

		++count;

		if (i.second->CheckModificator(Class::Modificator::Primitive) ||
			i.second->CheckModificator(Class::Modificator::Register) ||
			i.second->CheckModificator(Class::Modificator::Internal)
		) {
			continue;
		}

		Lex::PrintLine(offset);
		i.second->Trace(offset);
	}
}

Uint32 Scope::Size(Void) {

	Uint32 size = 0;

	for (auto& i : this->stringMap_) {

		if (i.second->CheckModificator(Class::Modificator::Register) ||
			i.second->CheckModificator(Class::Modificator::Internal)
		) {
			continue;
		}

		size += i.second->GetSizeOf();
	}

	return size;
}

Void Scope::Write(Uint8P buffer, Uint32P offset) {

	for (auto& i : this->stringMap_) {

		if (i.second->CheckModificator(Class::Modificator::Register) ||
			i.second->CheckModificator(Class::Modificator::Internal)
		) {
			continue;
		}

		i.second->Write(buffer + *offset, offset);

		*offset += i.second->GetSizeOf();
	}
}

Scope::~Scope() {
	this->Clear();
}

LAME_END2
