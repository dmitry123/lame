#include "Script.h"

LAME_BEGIN

ScriptManager::ScriptManager() {

	this->Push();

	ScriptVar var;

	var.name = "Void";
	var.MakeVoid();
	var.lex = ScriptLex::Find(kScriptObjectType);
	this->DeclareType(&var);

	var.name = "Int";
	var.MakeInt();
	var.lex = ScriptLex::Find(kScriptObjectType);
	this->DeclareType(&var);

	var.name = "Float";
	var.MakeFloat();
	var.lex = ScriptLex::Find(kScriptObjectType);
	this->DeclareType(&var);

	var.name = "Bool";
	var.MakeBool();
	var.lex = ScriptLex::Find(kScriptObjectType);
	this->DeclareType(&var);

	var.name = "String";
	var.MakeString();
	var.lex = ScriptLex::Find(kScriptObjectType);
	this->DeclareType(&var);

	var.name = "Auto";
	var.MakeVar();
	var.lex = ScriptLex::Find(kScriptObjectType);
	this->DeclareType(&var);

	var.name = "Function";
	var.MakeFunction();
	var.lex = ScriptLex::Find(kScriptObjectType);
	this->DeclareType(&var);

	var.name = "Class";
	var.MakeClass();
	var.lex = ScriptLex::Find(kScriptObjectType);
	this->DeclareType(&var);
}

ScriptManager::~ScriptManager(Void) {

	while (this->varMapQueue.size() > 1) {
		this->Pop();
	}

	for (auto i = this->typeMap->begin(); i != this->typeMap->end(); i++) {
		delete i->second;
	}
	for (auto i = this->varMap->begin(); i != this->varMap->end(); i++) {
		delete i->second;
	}

	this->typeMapQueue.pop_back();
	this->varMapQueue.pop_back();
}

Bool ScriptManager::_Declare(MapPtr map, ScriptVarPtr var) {

	if (this->varMap->count(var->name)) {
		return LAME_FALSE;
	}

	// allocate variable and push in map
	(*map)[var->name] = new ScriptVar(*var);

	return LAME_TRUE;
}

ScriptVarPtr ScriptManager::_FindQueue(List<Map>* queue, const Buffer& name) {
	
	ScriptVarPtr var = 0;

	for (const Map& m : *queue) {
		if ((var = this->_Find((MapPtr)&m, name))) {
			break;
		}
	}

	return var;
}

ScriptVarPtr ScriptManager::_Find(MapPtr map, const Buffer& name) {

	if (!map->count(name)) {
		return LAME_NULL;
	}
	return map->at(name);
}

Void ScriptManager::Push(Void) {
	
	this->typeMapQueue.push_back(Map());
	this->typeMap = &this->typeMapQueue.back();
	this->varMapQueue.push_back(Map());
	this->varMap = &this->varMapQueue.back();
}

Void ScriptManager::Pop(Void) {

	if (this->varMapQueue.size() == 1 ||
		this->typeMapQueue.size() == 1
	) {
		PostErrorMessage("You can't pop global space", 1);
	}

	for (auto i = this->typeMap->begin(); i != this->typeMap->end(); i++) {
		delete i->second;
	}
	for (auto i = this->varMap->begin(); i != this->varMap->end(); i++) {
		delete i->second;
	}

	this->typeMapQueue.pop_back();
	this->typeMap = &this->typeMapQueue.back();
	this->varMapQueue.pop_back();
	this->varMap = &this->varMapQueue.back();
}

LAME_END