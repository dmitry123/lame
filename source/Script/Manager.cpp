#include "Script.h"

LAME_BEGIN

Bool ScriptTypeManager::Declare(ScriptTypePtr type) {

	// return false if we've just registered type in current typespace
	if (this->spaceMap->count(type->name)) {
		return LAME_FALSE;
	}

	// declare type for current typespace
	(*this->spaceMap)[type->name] = type;

	return LAME_TRUE;
}

ScriptTypePtr ScriptTypeManager::Find(const Buffer& typeName) {

	// find type with name in all typespaces and return ir
	for (const TypeMap& tm : this->spaceMapQueue) {
		if (tm.count(typeName)) {
			return tm.at(typeName);
		}
	}

	return LAME_NULL;
}

Void ScriptTypeManager::Push(Void) {

	this->spaceMapQueue.push_back(TypeMap());
	this->spaceMap = &this->spaceMapQueue.back();
}

Void ScriptTypeManager::Pop(Void) {

	if (this->spaceMapQueue.size() == 1) {
		PostErrorMessage("You can't pop global type space from queue", 1);
	}

	this->spaceMapQueue.pop_back();
	this->spaceMap = &this->spaceMapQueue.back();
}

Void ScriptTypeManager::_DeclareLanguageTypes(Void) {

	ScriptTypePtr list = ScriptType::GetList();

	while (*list != kScriptTypeDefault) {
		this->Declare(list++);
	}
}

Bool ScriptVarManager::Declare(ScriptVariablePtr var) {

	if (var->declared) {
		return LAME_TRUE;
	}

	// return false if we've just registered type in current typespace
	if (this->spaceMap->count(var->object->word)) {
		return LAME_FALSE;
	}

	// declare type for current typespace
	(*this->spaceMap)[var->object->word] = var;

	// set declared flag to TRUE
	var->declared = LAME_TRUE;

	return LAME_TRUE;
}

ScriptVariablePtr ScriptVarManager::Find(const Buffer& varName) {

	// find type with name in all typespaces and return ir
	for (const VarMap& tm : this->spaceMapQueue) {
		if (tm.count(varName)) {
			return tm.at(varName);
		}
	}

	return LAME_NULL;
}

Void ScriptVarManager::Push(Void) {

	this->spaceMapQueue.push_back(VarMap());
	this->spaceMap = &this->spaceMapQueue.back();
}

Void ScriptVarManager::Pop(Void) {

	if (this->spaceMapQueue.size() == 1) {
		PostErrorMessage("You can't pop global var space from queue", 1);
	}

	this->spaceMapQueue.pop_back();
	this->spaceMap = &this->spaceMapQueue.back();
}

LAME_END