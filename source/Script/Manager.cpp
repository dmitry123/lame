#include "Script.h"

LAME_BEGIN

ScriptManager::ScriptManager() {

	this->Push();

    this->_DeclareInternalType(kScriptTypeVoid, "void");
	this->_DeclareInternalType(kScriptTypeInt, "int");
	this->_DeclareInternalType(kScriptTypeBool, "boolean");
	this->_DeclareInternalType(kScriptTypeFloat, "float");
	this->_DeclareInternalType(kScriptTypeString, "String");
	this->_DeclareInternalType(kScriptTypeArray, "Array");
	this->_DeclareInternalType(kScriptTypeList, "List");
	this->_DeclareInternalType(kScriptTypeFunction, "Function");
	this->_DeclareInternalType(kScriptTypeAuto, "Auto");
    this->_DeclareInternalType(kScriptTypeClass, "Class");
	this->_DeclareInternalType(kScriptTypeJson, "Json");
    
	ScriptVarPtr trueVar;
	ScriptVarPtr falseVar;
	ScriptVarPtr undeclaredVar;
    ScriptVarPtr nullVar;
    
	trueVar = this->_DeclareInternalVar(kScriptTypeBool, "True");
	falseVar = this->_DeclareInternalVar(kScriptTypeBool, "False");
	undeclaredVar = this->_DeclareInternalVar(kScriptTypeAuto, "Undeclared");
    nullVar = this->_DeclareInternalVar(kScriptTypeClass, "Null");

	trueVar->intValue = LAME_TRUE;
	falseVar->intValue = LAME_FALSE;
	undeclaredVar->classValue = LAME_NULL;
}

ScriptManager::~ScriptManager(Void) {

	while (this->varMapQueue.size() > 1) {
		this->Pop();
	}

	for (auto i = this->typeMap->begin(); i != this->typeMap->end(); i++) {
		delete i->second;
	}
	for (auto i = this->varMap->begin(); i != this->varMap->end(); i++) {
		if (i->first == "a") {
			//__asm int 3
		}
		if (i->second->classValue) {
			for (ScriptNodePtr n : i->second->classValue->childList) {
				//n->var = 0;
				//n->childList.clear();
				//delete n;
			}
			i->second->classValue->childList.clear();
		}
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

	if (newVar->name != newVar->typeName) {
		newVar->flags &= ~kScriptFlagType;
	}

	(*this->varMap)[var->name] = newVar;

	return LAME_TRUE;
}

ScriptVarPtr ScriptManager::Find(BufferRefC name) {
	
	ScriptVarPtr var = 0;

    for (List<Map>::reverse_iterator i = this->varMapQueue.rbegin(); i != this->varMapQueue.rend(); i++) {
        
		if (!i->count(name)) {
			continue;
		}
        
		if ((var = i->at(name))) {
			break;
		}
    }
    
    if (var && var->name != var->typeName) {
        var->flags &= ~kScriptFlagType;
    }

	return var;
}

ScriptVarPtr ScriptManager::_Find(ScriptTypeID type) {

	ScriptVarPtr var = 0;

	for (const Map& m : this->varMapQueue) {

		for (auto& n : m) {
			if (n.second->type == type) {
				if ((var = n.second)) {
					break;
				}
			}
		}

		if (var) {
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

ScriptVarPtr ScriptManager::_DeclareInternalType(ScriptTypeID type, BufferRefC name) {

    ScriptVar var(type, name, name, 0);
    this->Declare(&var);
    ScriptVarPtr result = this->Find(name);
    result->MakeType();
    return result;
}

ScriptVarPtr ScriptManager::_DeclareInternalVar(ScriptTypeID type, BufferRefC name) {

	ScriptVarPtr typeVar = this->_Find(type);

	if (!typeVar) {
		PostErrorMessage("Invalid type's id (%d)", type);
	}
    
	ScriptVar var(type, name, typeVar->typeName, 0);
	this->Declare(&var);
	typeVar = this->Find(name);
	typeVar->MakeFinal();
	return typeVar;
}

LAME_END