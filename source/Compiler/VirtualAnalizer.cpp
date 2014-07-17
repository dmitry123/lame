#include "VirtualAnalizer.h"

LAME_BEGIN2(Compiler)

using namespace Core;
using namespace Script;

static Map<LexID, Class::Operator> operatorMap =
{
	{ kScriptLexDefault, Class::Operator::Unkown },
	{ kScriptLexAdd, Class::Operator::Add },
	{ kScriptLexAddSet, Class::Operator::Add },
	{ kScriptLexSub, Class::Operator::Sub },
	{ kScriptLexSubSet, Class::Operator::Sub },
	{ kScriptLexMul, Class::Operator::Mul },
	{ kScriptLexMulSet, Class::Operator::Mul },
	{ kScriptLexDiv, Class::Operator::Div },
	{ kScriptLexDivSet, Class::Operator::Div },
	{ kScriptLexMod, Class::Operator::Mod },
	{ kScriptLexModSet, Class::Operator::Mod },
	{ kScriptLexAbove, Class::Operator::Above },
	{ kScriptLexBellow, Class::Operator::Bellow },
	{ kScriptLexEqual, Class::Operator::Equal },
	{ kScriptLexNotEqual, Class::Operator::NotEqual },
	{ kScriptLexSet, Class::Operator::Move },
	{ kScriptLexBitShiftR, Class::Operator::ShiftR },
	{ kScriptLexBitShiftSetR, Class::Operator::ShiftR },
	{ kScriptLexBitShiftL, Class::Operator::ShiftL },
	{ kScriptLexBitShiftSetL, Class::Operator::ShiftL },
	{ kScriptLexBitAnd, Class::Operator::BitAnd },
	{ kScriptLexBitAndSet, Class::Operator::BitAnd },
	{ kScriptLexBitOr, Class::Operator::BitOr },
	{ kScriptLexBitOrSet, Class::Operator::BitOr },
	{ kScriptLexBitXor, Class::Operator::BitXor },
	{ kScriptLexBitXorSet, Class::Operator::BitXor },
	{ kScriptLexBellowEqual, Class::Operator::BellowE },
	{ kScriptLexAboveEqual, Class::Operator::AboveE },
	{ kScriptLexAnd, Class::Operator::And },
	{ kScriptLexOr, Class::Operator::Or },
	{ kScriptLexIncrement, Class::Operator::Inc },
	{ kScriptLexDecrement, Class::Operator::Dec },
	{ kScriptLexSizeof, Class::Operator::Sizeof },
	{ kScriptLexBitNot, Class::Operator::BitNot },
	{ kScriptLexNot, Class::Operator::Not },
	{ kScriptLexCast, Class::Operator::Cast }
};

Void VirtualAnalizer::Treat(NodeListRef nodeList) {

	for (NodePtr n : nodeList) {
		this->currentNode = n;
		try {
			if (n->id == kScriptNodeClass || n->id == kScriptNodeInterface || n->id == kScriptNodeFunction) {
				continue;
			}
			else if (n->id == kScriptNodeInvoke || n->id == kScriptNodeAlloc) {
				if (!n->var) {
					PostSyntaxError(n->lex->line, "Undeclared class (%s)", n->word.data());
				}
				if (n->lex->args == 1 && n->var->CheckType(Class::Type::Class)) {
					this->_AnalizeCast(n);
				}
				else {
					this->_AnalizeInvoke(n);
					if (n->parent->id != kScriptNodeClass) {
						goto _SaveNode;
					}
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

Void VirtualAnalizer::Analize(LowLevelStackPtr lowLevelStack, NodeBuilderPtr nodeBuilder, ScopePtr rootScope) {

	this->lowLevelStack = lowLevelStack;
	this->nodeBuilder = nodeBuilder;
	this->rootScope = rootScope;

	this->OverloadInteger(rootScope->classBoolean);
	this->OverloadInteger(rootScope->classByte);
	this->OverloadInteger(rootScope->classChar);
	this->OverloadInteger(rootScope->classInt);
	this->OverloadInteger(rootScope->classLong);
	this->OverloadInteger(rootScope->classShort);
	this->OverloadFloat(rootScope->classFloat);
	this->OverloadFloat(rootScope->classDouble);
	this->OverloadString(rootScope->classString);
	this->OverloadObject(rootScope->classString);
	this->OverloadObject(rootScope->classObject);
	this->OverloadObject(rootScope->classClass);

	this->Treat(this->nodeBuilder->GetRootNode()->blockList);
}

Void VirtualAnalizer::_AnalizeBinary(NodePtr n) {

	VariablePtr sourceVar;
	VariablePtr leftVar;
	VariablePtr rightVar;
	VariablePtr rightVar2;

	this->Read(n, leftVar, rightVar);

	rightVar2 = rightVar;

	if (n->lex->lex->IsRight()) {
		if (leftVar->GetClassType()->Hash() != rightVar->GetClassType()->Hash()) {
			if (leftVar->GetClassType()->GetPriority() != rightVar->GetClassType()->GetPriority()) {
				if (leftVar->GetClassType()->GetPriority() > rightVar->GetClassType()->GetPriority()) {
					if (leftVar->GetClassType()->IsFloat()) {
						sourceVar = this->lowLevelStack->FindRegister(LowLevelStack::Float);
					}
					else if (leftVar->GetClassType()->IsInt()) {
						sourceVar = this->lowLevelStack->FindRegister(LowLevelStack::Integer);
					}
					else {
						sourceVar = this->lowLevelStack->FindRegister(LowLevelStack::Pointer);
					}
					leftVar->GetClassType()->Evaluate(Class::Operator::Cast, sourceVar, rightVar, leftVar, n->lex->lex);
					rightVar = sourceVar;
				}
				else {
					if (rightVar->GetClassType()->IsFloat()) {
						sourceVar = this->lowLevelStack->FindRegister(LowLevelStack::Float);
					}
					else if (rightVar->GetClassType()->IsInt()) {
						sourceVar = this->lowLevelStack->FindRegister(LowLevelStack::Integer);
					}
					else {
						sourceVar = this->lowLevelStack->FindRegister(LowLevelStack::Pointer);
					}
					rightVar->GetClassType()->Evaluate(Class::Operator::Cast, sourceVar, leftVar, rightVar, n->lex->lex);
					leftVar = sourceVar;
				}
			}
		}
		if (n->lex->lex->IsMath()) {
			if (leftVar->GetClassType()->GetPriority() != rightVar2->GetClassType()->GetPriority()) {
				if (leftVar->GetClassType()->GetPriority() > rightVar2->GetClassType()->GetPriority()) {
					if (leftVar->GetClassType()->IsFloat()) {
						sourceVar = this->lowLevelStack->FindRegister(LowLevelStack::Float);
					}
					else if (leftVar->GetClassType()->IsInt()) {
						sourceVar = this->lowLevelStack->FindRegister(LowLevelStack::Integer);
					}
					else {
						sourceVar = this->lowLevelStack->FindRegister(LowLevelStack::Pointer);
					}
				}
				else {
					if (rightVar->GetClassType()->IsFloat()) {
						sourceVar = this->lowLevelStack->FindRegister(LowLevelStack::Float);
					}
					else if (rightVar->GetClassType()->IsInt()) {
						sourceVar = this->lowLevelStack->FindRegister(LowLevelStack::Integer);
					}
					else {
						sourceVar = this->lowLevelStack->FindRegister(LowLevelStack::Pointer);
					}
				}
			}
			else {
				if (leftVar->GetClassType()->IsFloat()) {
					sourceVar = this->lowLevelStack->FindRegister(LowLevelStack::Float);
				}
				else if (leftVar->GetClassType()->IsInt()) {
					sourceVar = this->lowLevelStack->FindRegister(LowLevelStack::Integer);
				}
				else {
					sourceVar = this->lowLevelStack->FindRegister(LowLevelStack::Pointer);
				}
			}
		}
		else {
			sourceVar = this->lowLevelStack->FindRegister(LowLevelStack::Integer);
		}
	}
	else {
		if (rightVar->GetVarType() != Variable::Var::Object && leftVar->GetClassType()->Hash() != rightVar->GetClassType()->Hash()) {
			if (leftVar->GetClassType()->IsFloat()) {
				sourceVar = this->lowLevelStack->FindRegister(LowLevelStack::Float);
			}
			else if (leftVar->GetClassType()->IsInt()) {
				sourceVar = this->lowLevelStack->FindRegister(LowLevelStack::Integer);
			}
			else {
				sourceVar = this->lowLevelStack->FindRegister(LowLevelStack::Pointer);
			}
			leftVar->GetClass()->Evaluate(Class::Operator::Cast, sourceVar, rightVar, leftVar, n->lex->lex);
			rightVar = sourceVar;
		}
		sourceVar = leftVar;
	}

	printf("%s = %s %s %s\n", sourceVar->GetName().data(), leftVar->GetName().data(), n->word.data(), rightVar->GetName().data());

	sourceVar->GetClass()->Evaluate(operatorMap[n->lex->lex->id],
		sourceVar, leftVar, rightVar, n->lex->lex);

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
	VariablePtr rightVar;
	VariablePtr sourceVar;

	this->Read(n, leftVar, rightVar);

	if (n->lex->lex->IsRight()) {
		if (leftVar->GetClassType()->IsFloat()) {
			sourceVar = this->lowLevelStack->FindRegister(LowLevelStack::Float);
		}
		else if (leftVar->GetClassType()->IsInt()) {
			sourceVar = this->lowLevelStack->FindRegister(LowLevelStack::Integer);
		}
		else {
			sourceVar = this->lowLevelStack->FindRegister(LowLevelStack::Pointer);
		}
		leftVar->GetClassType()->Evaluate(Class::Operator::Move, sourceVar, leftVar, leftVar, n->lex->lex);
	}
	else {
		sourceVar = leftVar;
	}

	sourceVar->GetClass()->Evaluate(operatorMap[n->lex->lex->id],
		sourceVar, sourceVar, sourceVar, n->lex->lex);

	this->varStack.push_back(sourceVar);
	this->nameStack.push_back(sourceVar->GetName());
}

Void VirtualAnalizer::_AnalizeNew(NodePtr n) {

	Vector<VariablePtr> stackBackup;
	VariablePtr leftVar;
	ClassPtr classVar;
	VariablePtr sourcePtr;
	ObjectPtr templateClass;

	this->Read(n, leftVar, leftVar);
	classVar = ClassPtr(leftVar);
	sourcePtr = this->lowLevelStack->FindRegister(LowLevelStack::Pointer);

	if (classVar->CheckModificator(Class::Modificator::Primitive)) {
		sourcePtr->SetObject(new Class(*classVar));
	}
	else {
		stackBackup = this->varStack;
		classVar->GetClass()->New(sourcePtr);
		if (classVar->GetNode()) {
			for (NodePtr n2 : classVar->GetNode()->blockList) {
				this->Treat(n2->blockList);
			}
		}
		this->varStack = stackBackup;
	}

	this->varStack.push_back(sourcePtr);
	this->nameStack.push_back(sourcePtr->GetName());

	if (classVar->GetNewNode()) {
		this->_AnalizeInvoke(classVar->GetNewNode());
	}
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

	if (leftVar->GetClass()->Hash() != rightVar->GetClass()->Hash()) {
		if (rightVar->GetClass() == Scope::classFloat ||
			rightVar->GetClass() == Scope::classDouble
			) {
			sourceVar = this->lowLevelStack->FindRegister(LowLevelStack::Float);
		}
		else {
			sourceVar = this->lowLevelStack->FindRegister(LowLevelStack::Integer);
		}
		rightVar->GetClass()->GetOperators()[Uint32(Class::Operator::Cast)](
			sourceVar, leftVar, sourceVar);
		rightVar->GetClass()->Evaluate(operatorMap[n->lex->lex->id],
			sourceVar, sourceVar, sourceVar, n->lex->lex);
	}
	else {
		sourceVar = leftVar;
	}

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

	if (n->var && n->var->CheckType(Object::Type::Class)) {
		if (n->var->GetNewNode() != n) {
			n->var->SetNewNode(n);
			return;
		}
	}

	for (Uint32 i = 0; i < n->lex->args; i++) {
		objectList.push_back(this->varStack.back()->GetClass());
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

LAME_END2