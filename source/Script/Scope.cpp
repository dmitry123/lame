#include "Scope.h"
#include "Exception.h"
#include "Variable.h"
#include "Interface.h"
#include "Method.h"

LAME_BEGIN2(Script)

ScriptObjectPtr ScriptScope::Add(ScriptObjectPtr object) {

	StringPair stringPair = std::make_pair(object->GetName(), object->GetHash());
	HashPair hashPair = std::make_pair(object->GetHash(), object->GetHash());

	if (this->hashMap_.count(hashPair) > 0) {
		return LAME_NULL;
	}

	this->stringMap_[stringPair] = object;
	this->hashMap_[hashPair] = object;

	return object;
}

Void ScriptScope::Remove(ScriptObjectPtr var) {

	StringMap::iterator i;
	HashMap::iterator j;

	if ((i = this->stringMap_.find(std::make_pair(var->GetName(), var->GetHash()))) == this->stringMap_.end() ||
		(j = this->hashMap_.find(std::make_pair(var->GetHash(), var->GetHash()))) == this->hashMap_.end()
	) {
		return;
	}

	ScriptObjectPtr object = i->second;

	if (object->CheckType(ScriptObject::Type::Class)) {
		
		ScriptClassPtr clss = (ScriptClassPtr) object;

		if (clss->DecRef()) {
			delete object;
		}
	}
	else {
		delete object;
	}

	this->stringMap_.erase(i);
	this->hashMap_.erase(j);
}

ScriptObjectPtr ScriptScope::Find(StringC name) {
	return this->Find(Buffer::GetHash(name));
}

ScriptObjectPtr ScriptScope::Find(Uint32 hash) {

	for (auto& hp : this->hashMap_) {
		if (hp.first.first == hash) {
			return hp.second;
		}
	}

	return LAME_NULL;
}

static void _CloneObject(ScriptScopePtr scope, ScriptObjectPtr i) {

	if (i->CheckType(ScriptObject::Type::Class)) {
		scope->Add(new ScriptClass(*(ScriptClassPtr)i));
	}
	else if (i->CheckType(ScriptObject::Type::Interface)) {
		scope->Add(new ScriptInterface(*(ScriptInterfacePtr)i));
	}
	else if (i->CheckType(ScriptObject::Type::Method)) {
		scope->Add(new ScriptMethod(*(ScriptMethodPtr)i));
	}
	else if (i->CheckType(ScriptObject::Type::Variable)) {
		scope->Add(new ScriptVar(*(ScriptVarPtr)i));
	}
}

Void ScriptScope::Merge(ScriptScopePtrC scope) {

	if (this == scope) {
		return;
	}

	for (auto& i : scope->stringMap_) {

		if (i.second->CheckModificator(ScriptObject::Modificator::Static)) {
			_CloneObject(this, i.second);
		}
		else {
			if (i.second->CheckType(ScriptObject::Type::Method)) {
				_CloneObject(this, i.second);
			}
			else {
				_CloneObject(this, i.second);
			}
		}
	}
}

Void ScriptScope::Clone(ScriptScopePtrC scope) {

	this->Clear();
	this->Merge(scope);
}

Uint32 ScriptScope::Amount(Void) {
	return this->hashMap_.size();
}

Void ScriptScope::Clear(Void) {

	while (this->stringMap_.size()) {
		this->Remove(this->stringMap_.begin()->second);
	}
}

Void ScriptScope::Trace(Uint32 offset) {

	for (auto& i : this->stringMap_) {

		if (i.second->CheckModificator(ScriptClass::Modificator::Primitive)) {
			continue;
		}

		for (Uint32 j = 0; j < offset; j++) {
			printf("  ");
		}

		i.second->Trace(offset);
		puts("");
	}
}

ScriptScope::~ScriptScope() {
	this->Clear();
}

LAME_END2