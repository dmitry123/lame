#include "VirtualAnalizer.h"

LAME_BEGIN2(Compiler)

using namespace Core;
using namespace Script;

Void VirtualAnalizer::Treat(NodeListRef nodeList) {

	for (NodePtr n : nodeList) {
		this->currentNode = n;
		try {
			if (n->id == kScriptNodeClass || n->id == kScriptNodeInterface || n->id == kScriptNodeFunction) {
				continue;
			}
			else if (n->id == kScriptNodeInvoke) {
				if (!n->var) {
					PostSyntaxError(n->lex->line, "Undeclared class (%s)", n->word.data());
				}
				if (n->lex->args == 1 && n->var->CheckType(Class::Type::Class)) {
					this->_AnalizeCast(n);
				}
				else {
					if (!n->var->CheckType(Object::Type::Class)) {
						this->_AnalizeInvoke(n);
					}
					if (n->parent->id != kScriptNodeClass) {
						goto _SaveNode;
					}
				}
			}
			else if (n->id == kScriptNodeAlloc) {
				if (!n->var) {
					PostSyntaxError(n->lex->line, "Undeclared class (%s)", n->word.data());
				}
				this->_AnalizeInvoke(n);
				if (n->parent->id != kScriptNodeClass) {
					goto _SaveNode;
				}
			}
			else if (n->lex->lex->IsUnknown() || n->lex->lex->IsConst()) {
			_SaveNode:
				if (!n->var) {
					PostSyntaxError(n->lex->line, "Undeclared variable (%s)", n->word.data());
				}
				else {
					varStack.push_back(VariablePtr(n->var));
				}
				nameStack.push_back(n->word);
				if (n->var) {
					//n->var->GetNode()->templatesNew = n->templates;
				}
			}
			else if (n->lex->lex->id == kScriptLexSemicolon) {
				this->lowLevelStack->ReleaseRegister();
				this->varStack.clear();
			}
			else if (n->lex->lex->id == kScriptLexNew) {
				this->_AnalizeNew(n);
			}
			else if (n->lex->lex->id == kScriptLexDirected || n->lex->lex->id == kScriptLexMediated) {
				this->_AnalizeSelection(n);
			}
			else if (n->lex->lex->IsMath() || n->lex->lex->IsBool()) {
				if (n->lex->args == 1) {
					this->_AnalizeUnary(n);
				}
				else if (n->lex->args == 2) {
					this->_AnalizeBinary(n);
				}
				else {
					this->_AnalizeInvoke(n);
				}
			}
		}
		catch (ScriptException e) {
			throw SyntaxException(n->lex->line, e.GetErrorBuffer());
		}
	}
}

Void VirtualAnalizer::Read(NodePtr n, VariablePtr& left, VariablePtr& right) {

	if (this->varStack.size() < n->lex->args) {
		PostSyntaxError(n->lex->line, "Invalid operator parameters, \"%s\" requires %d arguments", n->word.data(), n->lex->args);
	}
	if (n->lex->args > 1) {
		right = this->varStack.back();
		this->varStack.pop_back();
		if (n->lex->lex->id != kScriptLexDirected) {
			if (!right) {
				PostSyntaxError(n->lex->line, "Undeclared variable (%s)", this->nameStack.back().data());
			}
			this->nameStack.pop_back();
		}
	}
	left = this->varStack.back();
	this->varStack.pop_back();
	if (!left) {
		PostSyntaxError(n->lex->line, "Undeclared variable (%s)", this->nameStack.back().data());
	}
	if (n->lex->lex->id != kScriptLexDirected) {
		this->nameStack.pop_back();
	}
}

Void VirtualAnalizer::Write(VariablePtr var) {

	this->varStack.push_back(var);
	this->nameStack.push_back(var->GetName());
}

Void VirtualAnalizer::Analize(LowLevelStackPtr lowLevelStack, NodeBuilderPtr nodeBuilder, ScopePtr rootScope) {

	this->lowLevelStack = lowLevelStack;
	this->nodeBuilder = nodeBuilder;
	this->rootScope = rootScope;

	this->Treat(this->nodeBuilder->GetRootNode()->blockList);
}

RegisterPtr VirtualAnalizer::_GetSourceRegister(VariablePtr variable) {

	if (variable->GetClassType()->IsFloat()) {
		return this->lowLevelStack->FindRegister(LowLevelStack::Float);
	}
	else if (variable->GetClassType()->IsInt()) {
		return this->lowLevelStack->FindRegister(LowLevelStack::Integer);
	}
	return this->lowLevelStack->FindRegister(LowLevelStack::Pointer);
}

Void VirtualAnalizer::_AnalizeBinary(NodePtr n) {

	VariablePtr sourceVar;
	VariablePtr leftVar;
	VariablePtr leftVar2;
	VariablePtr rightVar;
	VariablePtr rightVar2;

	this->Read(n, leftVar, rightVar);

	leftVar2 = leftVar;
	rightVar2 = rightVar;

	if (n->lex->lex->IsRight()) {

		sourceVar = leftVar->GetClassType()->GetPriority() >= rightVar->GetClassType()->GetPriority() ?
			this->_GetSourceRegister(leftVar) : this->_GetSourceRegister(rightVar);

		if (leftVar->GetClassType()->Hash() != rightVar->GetClassType()->Hash()) {
			if (leftVar->GetClassType()->Hash() != sourceVar->GetClassType()->Hash()) {
				leftVar = this->_GetSourceRegister(sourceVar);
				this->AnalizeCast(leftVar, leftVar2);
			}
			if (rightVar->GetClassType()->Hash() != sourceVar->GetClassType()->Hash()) {
				rightVar = this->_GetSourceRegister(sourceVar);
				this->AnalizeCast(rightVar, rightVar2);
			}
		}
	}
	else {
		if (leftVar->GetClassType()->Hash() != rightVar->GetClassType()->Hash()) {
			if (leftVar->GetClassType()->Hash() != leftVar->GetClassType()->Hash()) {
				leftVar = this->_GetSourceRegister(leftVar);
				this->AnalizeCast(leftVar, leftVar2);
			}
			if (rightVar->GetClassType()->Hash() != leftVar->GetClassType()->Hash()) {
				rightVar = this->_GetSourceRegister(leftVar);
				if (rightVar != rightVar2) {
					this->AnalizeCast(rightVar, rightVar2);
				}
			}
		}
		sourceVar = leftVar2;
	}

	this->AnalizeBinary(sourceVar, leftVar, rightVar);

	if (leftVar->CheckModificator(Class::Modificator::Register)) {
		this->lowLevelStack->ReleaseRegister(RegisterPtr(leftVar));
	}
	if (rightVar->CheckModificator(Class::Modificator::Register)) {
		this->lowLevelStack->ReleaseRegister(RegisterPtr(rightVar));
	}

	this->varStack.push_back(sourceVar);
	this->nameStack.push_back(sourceVar->GetName());
}

Void VirtualAnalizer::_AnalizeUnary(NodePtr n) {

	VariablePtr leftVar;
	VariablePtr leftVar2;
	VariablePtr sourceVar;

	this->Read(n, leftVar, leftVar);
	leftVar2 = leftVar;

	if (n->lex->lex->IsRight()) {
		sourceVar = this->_GetSourceRegister(leftVar);
	}
	else {
		sourceVar = leftVar;
	}

	this->AnalizeUnary(sourceVar, leftVar);

	this->varStack.push_back(sourceVar);
	this->nameStack.push_back(sourceVar->GetName());
}

Void VirtualAnalizer::_AnalizeNew(NodePtr n) {

	Vector<VariablePtr> stackBackup;
	VariablePtr leftVar;
	VariablePtr rightVar;
	VariablePtr sourceVar;

	this->Read(n, leftVar, leftVar);
	sourceVar = this->lowLevelStack->FindRegister(LowLevelStack::Pointer);

	if (leftVar->CheckModificator(Object::Modificator::Primitive)) {
		this->Read(n, rightVar, rightVar);
		if (!rightVar->CheckType(Object::Type::Variable)) {
			PostSyntaxError(n->lex->line, "Array parameter must be variable (%s)", rightVar->GetName().data());
		}
		if (rightVar->GetVarType() != Variable::Var::Integer) {
			PostSyntaxError(n->lex->line, "Array parameter must be integer value (%s)", rightVar->GetName().data());
		}
		this->AnalizeNew(sourceVar, rightVar, ClassPtr(leftVar));
	}
	else {
		if (leftVar->CheckModificator(Object::Modificator::Primitive)) {
			PostSyntaxError(n->lex->line, "Unable to apply 'new' for primitive type (%s)", leftVar->GetName().data());
		}
		this->AnalizeNew(sourceVar, NULL, ClassPtr(leftVar));
		ObjectPtr classVar = leftVar->Find(leftVar->Hash());
		ObjectPtr constructorVar = leftVar->Find(leftVar->GetName());
		stackBackup = this->varStack;
		if (classVar->GetNode()) {
			for (NodePtr n2 : classVar->GetNode()->blockList) {
				this->Treat(n2->blockList);
			}
		}
		if (constructorVar && FALSE) {
			if (constructorVar->GetMethod()->GetRootNode()) {
				this->Treat(constructorVar->GetMethod()->GetRootNode()->blockList);
			}
			else if (constructorVar->GetMethod()->GetNativeMethod()) {
				constructorVar->GetMethod()->GetNativeMethod()(constructorVar->GetMethod());
			}
			else {
				PostSyntaxError(n->lex->line, "Non-implemented class constructor (%s)", constructorVar->GetName().data());
			}
		}
		this->varStack = stackBackup;
	}

	this->varStack.push_back(sourceVar);
	this->nameStack.push_back(sourceVar->GetName());
}

Void VirtualAnalizer::_AnalizeSelection(NodePtr n) {

	VariablePtr leftVar;
	VariablePtr rightVar;
	Buffer fieldName;
	ObjectPtr fieldObject;

	this->Read(n, leftVar, rightVar);
	fieldName = this->nameStack.back();
	this->nameStack.pop_back();
	this->nameStack.pop_back();

	if (leftVar->GetVarType() != Variable::Var::Object) {
		PostSyntaxError(n->lex->line, "You can access to object's fields only (%s)", fieldName.data());
	}

	if (!leftVar->objectValue) {
		PostSyntaxError(n->lex->line, "Object not allocated (%s)", leftVar->GetName().data());
	}

	fieldObject = leftVar->objectValue->Find(fieldName);

	if (!fieldObject) {
		PostSyntaxError(n->lex->line, "Undeclared object's field (%s.%s)", leftVar->GetClass()->GetName().data(), fieldName.data());
	}
	rightVar = fieldObject->GetVariable();

	varStack.push_back(rightVar);
	nameStack.push_back(fieldName);
}

Void VirtualAnalizer::_AnalizeCondition(NodePtr n) {

}

Void VirtualAnalizer::_AnalizeCast(NodePtr n) {

	VariablePtr leftVar;
	ClassPtr rightVar;
	VariablePtr sourceVar;

	this->Read(n, leftVar, leftVar);
	rightVar = n->var->GetClass();

	__asm int 3

	this->varStack.push_back(sourceVar);
	this->nameStack.push_back(sourceVar->GetName());
}

Void VirtualAnalizer::_AnalizeInvoke(NodePtr n) {

	VariablePtr sourceVar;
	VariablePtr leftVar;
	ObjectPtr methodVar;
	Uint32 methodHash;
	Vector<ClassPtr> objectList;
	Uint32 invokeHash;
	Buffer formattedParameters;

	if (n->var->CheckModificator(Object::Modificator::Primitive)) {
		this->Read(n, leftVar, leftVar);
		if (!leftVar->CheckType(Object::Type::Variable)) {
			PostSyntaxError(n->lex->line, "Array parameter must be variable (%s)", leftVar->GetName().data());
		}
		this->varStack.push_back(leftVar->GetVariable());
		this->nameStack.push_back(leftVar->GetName());
	}
	else {
		for (Uint32 i = 0; i < n->lex->args; i++) {
			objectList.push_back(this->varStack.back()->GetClass());
			this->AnalizePush(this->varStack.back());
			formattedParameters.append(this->varStack.back()->GetClass()->GetName());
			this->varStack.pop_back();
			this->nameStack.pop_back();
			if (i != n->lex->args - 1) {
				formattedParameters.append(", ");
			}
		}

		invokeHash = Method::ComputeInvokeHash(objectList);
		methodHash = n->var->GetNode()->methodHash;
		methodVar = n->var->Find(Uint64(n->var->GetPathHash32()) << 32 | invokeHash);

		if (!methodVar) {
			PostSyntaxError(n->lex->line, "Undeclared method %s(%s)", n->word.data(), formattedParameters.data());
		}

		printf("Invoked %s(%s)\n", n->word.data(), formattedParameters.data());

		sourceVar = this->lowLevelStack->FindRegister(LowLevelStack::Pointer);
		this->varStack.push_back(sourceVar);
		this->nameStack.push_back(sourceVar->GetName());
	}
}

LAME_END2