#include "Script.h"

LAME_BEGIN

ScriptManager::ScriptManager() {

	this->Push();

	this->Declare(&ScriptVar(kScriptTypeVoid, "Void", "Void", 0));
	this->Declare(&ScriptVar(kScriptTypeInt, "Int", "Int", 0));
	this->Declare(&ScriptVar(kScriptTypeBool, "Bool", "Bool", 0));
	this->Declare(&ScriptVar(kScriptTypeFloat, "Float", "Float", 0));
	this->Declare(&ScriptVar(kScriptTypeString, "String", "String", 0));
	this->Declare(&ScriptVar(kScriptTypeArray, "Array", "Array", 0));
	this->Declare(&ScriptVar(kScriptTypeList, "List", "List", 0));
	this->Declare(&ScriptVar(kScriptTypeFunction, "Function", "Function", 0));
	this->Declare(&ScriptVar(kScriptTypeAuto, "Auto", "Auto", 0));
	this->Declare(&ScriptVar(kScriptTypeBool, "True", "True", 0));
	this->Declare(&ScriptVar(kScriptTypeBool, "False", "False", 0));
	this->Declare(&ScriptVar(kScriptTypeClass, "Null", "Null", 0));
	this->Declare(&ScriptVar(kScriptTypeAuto, "Undeclared", "Undeclared", 0));

	ScriptVarPtr trueVar = this->Find("True");
	ScriptVarPtr falseVar = this->Find("False");
	ScriptVarPtr nullVar = this->Find("Null");
	ScriptVarPtr undeclaredVar = this->Find("Undeclared");

	trueVar->intValue = LAME_TRUE;
	falseVar->intValue = LAME_FALSE;
	nullVar->classValue = LAME_NULL;
	undeclaredVar->classValue = LAME_NULL;

	trueVar->MakeFinal();
	falseVar->MakeFinal();
	nullVar->MakeFinal();
	undeclaredVar->MakeFinal();
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

Bool ScriptManager::Declare(ScriptVarPtr var) {

	if (this->varMap->count(var->name)) {
		return LAME_FALSE;
	}

	ScriptVarPtr newVar = new ScriptVar(*var);
	newVar->flags &= ~kScriptFlagType;
	(*this->varMap)[var->name] = newVar;

	return LAME_TRUE;
}

ScriptVarPtr ScriptManager::Find(BufferRefC name) {
	
	ScriptVarPtr var = 0;

	for (const Map& m : this->varMapQueue) {

		if (!m.count(name)) {
			continue;
		}

		if ((var = var = m.at(name)) && !var->IsTemp()) {
			break;
		}
	}

	return var;
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
	this->typeMap->clear();
	for (auto i = this->varMap->begin(); i != this->varMap->end(); i++) {
		delete i->second;
	}
	this->varMap->clear();

	this->typeMapQueue.pop_back();
	this->typeMap = &this->typeMapQueue.back();
	this->varMapQueue.pop_back();
	this->varMap = &this->varMapQueue.back();
}

ScriptTypeID ScriptManager::GetTypeID(BufferRefC typeName) {

	ScriptVarPtr var = this->Find(typeName);

	if (!var) {
		return kScriptTypeDefault;
	}

	return var->type;
}

LAME_END