#include "Analizer.h"
#include "Internal.h"

LAME_BEGIN2(Compiler)

static Void _TranslateMov(Uint32 command, VariablePtr source, VariablePtr left, VariablePtr right, Bool isFloat = FALSE) {
	source->v = left->v;
}

static Void _TranslateMath(Uint32 command, VariablePtr source, VariablePtr left, VariablePtr right, Bool isFloat = FALSE) {

	switch (command) {
		/* Integer Operators */
	case ADD: source->v.intValue = left->v.intValue + right->v.intValue; break;
	case SUB: source->v.intValue = left->v.intValue - right->v.intValue; break;
	case MUL: source->v.intValue = left->v.intValue * right->v.intValue; break;
	case DIV: source->v.intValue = left->v.intValue / right->v.intValue; break;
	case AND: source->v.intValue = left->v.intValue & right->v.intValue; break;
	case OR: source->v.intValue = left->v.intValue | right->v.intValue; break;
	case XOR: source->v.intValue = left->v.intValue ^ right->v.intValue; break;
		/* Float Operators */
	case FADD: source->v.floatValue = left->v.floatValue + right->v.floatValue; break;
	case FSUB: source->v.floatValue = left->v.floatValue - right->v.floatValue; break;
	case FMUL: source->v.floatValue = left->v.floatValue * right->v.floatValue; break;
	case FDIV: source->v.floatValue = left->v.floatValue / right->v.floatValue; break;
	}
}

static Void _TranslateCondition(Uint32 cmp, Uint32 command, VariablePtr source, VariablePtr left, VariablePtr right) {

#define _MAKEJUMP(_operator) \
	source->v.intValue = cmp == CMP ? \
		left->v.intValue _operator right->v.intValue : \
		left->v.floatValue _operator right->v.floatValue;

	switch (command) {
	case JA: _MAKEJUMP(<= ); break;
	case JNBE: _MAKEJUMP(<= ); break;
	case JAE: _MAKEJUMP(< ); break;
	case JNB: _MAKEJUMP(< ); break;
	case JB: _MAKEJUMP(>= ); break;
	case JNAE: _MAKEJUMP(>= ); break;
	case JBE: _MAKEJUMP(> ); break;
	case JNA: _MAKEJUMP(> ); break;
	case JE: _MAKEJUMP(!= ); break;
	case JNE: _MAKEJUMP(== ); break;
	}
}

static Void _TranslateSingle(Uint32 command, VariablePtr source) {

	switch (command) {
	case INC: source->v.intValue++; break;
	case DEC: source->v.intValue--; break;
	case LNOT: source->v.intValue = ~source->v.intValue; break;
	case NOT: source->v.intValue = !source->v.intValue; break;
	}
}

static Void OverloadInteger(ClassPtr intClass) {

	intClass->Overload(Class::Operator::Move, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateMov(MOV, source, left, right);
	});
	intClass->Overload(Class::Operator::Add, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateMath(ADD, source, left, right);
	});
	intClass->Overload(Class::Operator::Sub, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateMath(SUB, source, left, right);
	});
	intClass->Overload(Class::Operator::Mul, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateMath(MUL, source, left, right);
	});
	intClass->Overload(Class::Operator::Div, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateMath(DIV, source, left, right);
	});
	intClass->Overload(Class::Operator::Mod, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateMath(MOD, source, left, right);
	});
	intClass->Overload(Class::Operator::ShiftL, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateMath(SHL, source, left, right);
	});
	intClass->Overload(Class::Operator::ShiftR, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateMath(SHR, source, left, right);
	});
	intClass->Overload(Class::Operator::Above, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateCondition(CMP, JNA, source, left, right);
	});
	intClass->Overload(Class::Operator::Bellow, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateCondition(CMP, JNB, source, left, right);
	});
	intClass->Overload(Class::Operator::AboveE, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateCondition(CMP, JNAE, source, left, right);
	});
	intClass->Overload(Class::Operator::BellowE, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateCondition(CMP, JNBE, source, left, right);
	});
	intClass->Overload(Class::Operator::Equal, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateCondition(CMP, JNE, source, left, right);
	});
	intClass->Overload(Class::Operator::NotEqual, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateCondition(CMP, JE, source, left, right);
	});
	intClass->Overload(Class::Operator::And, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateMath(AND, source, left, right);
	});
	intClass->Overload(Class::Operator::Or, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateMath(OR, source, left, right);
	});
	intClass->Overload(Class::Operator::BitAnd, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateMath(AND, source, left, right);
	});
	intClass->Overload(Class::Operator::BitOr, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateMath(OR, source, left, right);
	});
	intClass->Overload(Class::Operator::BitXor, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateMath(XOR, source, left, right);
	});
	intClass->Overload(Class::Operator::Inc, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateSingle(INC, source);
	});
	intClass->Overload(Class::Operator::Dec, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateSingle(INC, source);
	});
	intClass->Overload(Class::Operator::BitNot, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateSingle(LNOT, source);
	});
	intClass->Overload(Class::Operator::Not, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateSingle(NOT, source);
	});

	intClass->Overload(Class::Operator::Cast, [](VariablePtr source, VariablePtr left, VariablePtr right) {

		switch (left->GetType()) {
		case Class::Type::Class:
		case Class::Type::Method:
		case Class::Type::Interface:
		case Class::Type::Abstract:
			throw ClassInvalidCastException();
		case Class::Type::Array:
			if (left->GetClass() != right->GetClass()) {
				throw ClassInvalidCastException();
			}
		case Class::Type::Variable:
			break;
		}

		if (left->GetVarType() != Variable::Var::Integer &&
			left->GetVarType() != Variable::Var::IntegerPtr
			) {
			if (left->GetVarType() == Variable::Var::Float) {
				source->v.intValue = (ScriptNativeInt)left->v.floatValue;
			}
			else {
				throw ClassInvalidCastException();
			}
		}

		if (right->GetObject() && !left->GetObject() || !right->GetObject() && left->GetObject()) {
			throw ClassInvalidCastException();
		}
	});
}

static Void OverloadFloat(ClassPtr floatClass) {

	floatClass->Overload(Class::Operator::Move, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateMov(FMOV, source, left, right, TRUE);
	});
	floatClass->Overload(Class::Operator::Add, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateMath(FADD, source, left, right, TRUE);
	});
	floatClass->Overload(Class::Operator::Sub, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateMath(FSUB, source, left, right, TRUE);
	});
	floatClass->Overload(Class::Operator::Mul, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateMath(FMUL, source, left, right, TRUE);
	});
	floatClass->Overload(Class::Operator::Div, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateMath(FDIV, source, left, right, TRUE);
	});
	floatClass->Overload(Class::Operator::Above, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateCondition(JCMP, JNA, source, left, right);
	});
	floatClass->Overload(Class::Operator::Bellow, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateCondition(JCMP, JNB, source, left, right);
	});
	floatClass->Overload(Class::Operator::AboveE, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateCondition(JCMP, JNAE, source, left, right);
	});
	floatClass->Overload(Class::Operator::BellowE, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateCondition(JCMP, JNBE, source, left, right);
	});
	floatClass->Overload(Class::Operator::Equal, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateCondition(JCMP, JNE, source, left, right);
	});
	floatClass->Overload(Class::Operator::NotEqual, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		_TranslateCondition(JCMP, JE, source, left, right);
	});

	floatClass->Overload(Class::Operator::Cast, [](VariablePtr source, VariablePtr left, VariablePtr right) {

		switch (left->GetType()) {
		case Class::Type::Class:
		case Class::Type::Method:
		case Class::Type::Interface:
		case Class::Type::Abstract:
		case Class::Type::Array:
			throw ClassInvalidCastException();
		case Class::Type::Variable:
			break;
		}

		if (left->GetVarType() != Variable::Var::Float &&
			left->GetVarType() != Variable::Var::FloatPtr
			) {
			if (left->GetVarType() == Variable::Var::Integer) {
				source->v.floatValue = (ScriptNativeFloat)left->v.intValue;
			}
			else {
				throw ClassInvalidCastException();
			}
		}
	});
}

static Void OverloadString(ClassPtr stringClass) {

	stringClass->Overload(Class::Operator::Move, [](VariablePtr source, VariablePtr left, VariablePtr right) {
	});
	stringClass->Overload(Class::Operator::Add, [](VariablePtr source, VariablePtr left, VariablePtr right) {
	});
	stringClass->Overload(Class::Operator::Above, [](VariablePtr source, VariablePtr left, VariablePtr right) {
	});
	stringClass->Overload(Class::Operator::Bellow, [](VariablePtr source, VariablePtr left, VariablePtr right) {
	});
	stringClass->Overload(Class::Operator::AboveE, [](VariablePtr source, VariablePtr left, VariablePtr right) {
	});
	stringClass->Overload(Class::Operator::BellowE, [](VariablePtr source, VariablePtr left, VariablePtr right) {
	});
	stringClass->Overload(Class::Operator::Equal, [](VariablePtr source, VariablePtr left, VariablePtr right) {
	});
	stringClass->Overload(Class::Operator::NotEqual, [](VariablePtr source, VariablePtr left, VariablePtr right) {
	});
	stringClass->Overload(Class::Operator::Cast, [](VariablePtr source, VariablePtr left, VariablePtr right) {
	});
}

static Void OverloadObject(ClassPtr objectClass) {

	objectClass->Overload(Class::Operator::Move, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		left->Clone(source);
	});

	objectClass->Overload(Class::Operator::Cast, [](VariablePtr source, VariablePtr left, VariablePtr right) {

		if (right->GetClass()->GetNameHash() == left->GetClass()->GetNameHash()) {
			goto __AllowCast;
		}

		if (left->GetClass()->GetExtended() == right->GetClass()) {
			goto __AllowCast;
		}

		for (InterfacePtr i : left->GetClass()->GetImplements()) {
			if (right->GetClass() == i->GetClass()) {
				goto __AllowCast;
			}
		}

		throw ClassInvalidCastException();

	__AllowCast:
		;
	});
}

Void CodeAnalizer::Analize(NodeBuilderPtr nodeBuilder) {
	this->_AnalizeList(nodeBuilder->GetRootNode()->blockList);
}

Void CodeAnalizer::Overload(Void) {

	OverloadInteger(GlobalScope::classBoolean);
	OverloadInteger(GlobalScope::classByte);
	OverloadInteger(GlobalScope::classChar);
	OverloadInteger(GlobalScope::classInt);
	OverloadInteger(GlobalScope::classLong);
	OverloadInteger(GlobalScope::classShort);

	OverloadFloat(GlobalScope::classFloat);
	OverloadFloat(GlobalScope::classDouble);

	OverloadString(GlobalScope::classString);
	OverloadObject(GlobalScope::classString);
	OverloadObject(GlobalScope::classObject);
	OverloadObject(GlobalScope::classClass);
}

Void CodeAnalizer::_AnalizeList(NodeListRef nodeList) {

	for (NodePtr n : nodeList) {
		try {
			if (n->id == kScriptNodeClass || n->id == kScriptNodeInterface) {
				continue;
			}
			else if (n->id == kScriptNodeInvoke || n->id == kScriptNodeAlloc) {
				if (!n->var) {
					n->var = n->parent->GetScope()->GetClassScope()->Find(n->word);
				}
				if (n->lex->args == 1 && n->var->CheckType(Class::Type::Class)) {
					this->_AnalizeCast(n);
				}
				else {
					this->_Analize0(n);
					if (n->parent->id != kScriptNodeClass) {
						goto _SaveNode;
					}
				}
			}
			else if (n->lex->lex->IsUnknown() || n->lex->lex->IsConst()) {
			_SaveNode:
				if (!n->var) {
					n->var = n->parent->GetScope()->GetClassScope()->Find(n->word);
					if (n->var) {
						varStack.push_back(VariablePtr(n->var));
					}
					else {
						varStack.push_back(NULL);
					}
				}
				else {
					varStack.push_back(VariablePtr(n->var));
				}
				nameStack.push_back(n->word);
				if (n->var) {
					n->var->GetNode()->templatesNew = n->templates;
				}
			}
			else if (n->lex->lex->id == kScriptLexSemicolon) {
				_ReleaseAllRegisters();
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
					this->_Analize1(n);
				}
				else if (n->lex->args == 2) {
					this->_Analize2(n);
				}
				else {
					this->_Analize0(n);
				}
			}
		}
		catch (ClassInvalidCastException) {
			PostSyntaxError(n->lex->line, "Invalid type cast", 0);
		}
	}
}

Void CodeAnalizer::_Analize0(NodePtr n) {

	VariablePtr sourceVar;
	VariablePtr leftVar;

	if (!n->lex->args) {
		return;
	}

	__asm int 3

	this->varStack.push_back(sourceVar);
	this->nameStack.push_back(sourceVar->GetName());
}

Void CodeAnalizer::_Analize1(NodePtr n) {

	VariablePtr leftVar;
	VariablePtr rightVar;
	VariablePtr sourceVar;

	_Read(n, leftVar, rightVar);

	if (n->lex->lex->IsRight()) {
		sourceVar = _FindAvailableRegister(leftVar->IsFloat());
		leftVar->GetClassType()->Evaluate(Class::Operator::Move, sourceVar, leftVar, leftVar, n->lex->lex);
	}

	sourceVar->GetClass()->Evaluate(operatorMap[n->lex->lex->id],
		sourceVar, sourceVar, sourceVar, n->lex->lex);

	this->varStack.push_back(sourceVar);
	this->nameStack.push_back(sourceVar->GetName());
}

Void CodeAnalizer::_Analize2(NodePtr n) {

	VariablePtr sourceVar;
	VariablePtr leftVar;
	VariablePtr rightVar;
	VariablePtr rightVar2;

	this->_Read(n, leftVar, rightVar);

	rightVar2 = rightVar;

	if (n->lex->lex->IsRight()) {
		if (leftVar->GetClassType()->GetNameHash() != rightVar->GetClassType()->GetNameHash()) {
			if (leftVar->GetClassType()->GetPriority() != rightVar->GetClassType()->GetPriority()) {
				if (leftVar->GetClassType()->GetPriority() > rightVar->GetClassType()->GetPriority()) {
					sourceVar = _FindAvailableRegister(leftVar->IsFloat());
					leftVar->GetClassType()->Evaluate(Class::Operator::Cast, sourceVar, rightVar, leftVar, n->lex->lex);
					rightVar = sourceVar;
				}
				else {
					sourceVar = _FindAvailableRegister(rightVar->IsFloat());
					rightVar->GetClassType()->Evaluate(Class::Operator::Cast, sourceVar, leftVar, rightVar, n->lex->lex);
					leftVar = sourceVar;
				}
			}
		}
		if (n->lex->lex->IsMath()) {
			if (leftVar->GetClassType()->GetPriority() != rightVar2->GetClassType()->GetPriority()) {
				if (leftVar->GetClassType()->GetPriority() > rightVar2->GetClassType()->GetPriority()) {
					sourceVar = _FindAvailableRegister(leftVar->IsFloat());
				}
				else {
					sourceVar = _FindAvailableRegister(rightVar->IsFloat());
				}
			}
			else {
				sourceVar = _FindAvailableRegister();
			}
		}
		else {
			sourceVar = _FindAvailableRegister();
		}
	}
	else {
		if (rightVar->GetVarType() != Variable::Var::Object && leftVar->GetClass() != rightVar->GetClass()) {
			sourceVar = _FindAvailableRegister(leftVar->IsFloat());
			leftVar->GetClass()->Evaluate(Class::Operator::Cast, sourceVar, rightVar, leftVar, n->lex->lex);
			rightVar = sourceVar;
		}
		sourceVar = leftVar;
	}

	sourceVar->GetClass()->Evaluate(operatorMap[n->lex->lex->id],
		sourceVar, leftVar, rightVar, n->lex->lex);

	if (leftVar->CheckModificator(Class::Modificator::Register)) {
		_ReleaseRegister(leftVar);
	}
	if (rightVar->CheckModificator(Class::Modificator::Register)) {
		_ReleaseRegister(rightVar);
	}

	this->varStack.push_back(sourceVar);
	this->nameStack.push_back(sourceVar->GetName());
}

Void CodeAnalizer::_AnalizeCondition(NodePtr n) {

}

Void CodeAnalizer::_AnalizeNew(NodePtr n) {

	Vector<VariablePtr> stackBackup;
	VariablePtr leftVar;
	ClassPtr classVar;
	VariablePtr sourcePtr;
	ObjectPtr templateClass;

	this->_Read(n, leftVar, leftVar);
	classVar = leftVar->GetClass();

	if (!classVar->GetNode() && !classVar->CheckModificator(Class::Modificator::Primitive)) {
		PostSyntaxError(n->lex->line, "Object isn't class (%s)", classVar->GetName().data());
	}

	sourcePtr = _FindAvailableRegister();

	if (classVar->CheckModificator(Class::Modificator::Primitive)) {
		sourcePtr->SetObject(new Class(*classVar));
	}
	else {
		stackBackup = this->varStack;
		classVar->GetClass()->New(sourcePtr);
		if (leftVar->GetNode()->templates.length() > 0) {
			if (!(templateClass = ScopeBuilder::_FindClass(leftVar->GetNode()->parent, leftVar->GetNode()->templatesNew))) {
				PostSyntaxError(n->lex->line, "Undeclared class %s", leftVar->GetNode()->templates.data());
			}
			sourcePtr->SetTemplateClass(templateClass->GetClass());
			ClassPtr classT = new Class(leftVar->GetNode()->templates);
			templateClass->Clone(classT);
			sourcePtr->GetObject()->GetScopeController()->GetClassScope()
				->Add(classT)->SetNode(templateClass->GetNode());
		}
		for (NodePtr n2 : classVar->GetNode()->blockList) {
			for (NodePtr n3 : n2->blockList) {
				if (n3->lex->lex->IsUnknown()) {
					n3->var = sourcePtr->GetObject()->GetScopeController()
						->GetVarScope()->Find(n3->word);
					if (!n3->var) {
						n3->var = sourcePtr->GetObject()->GetScopeController()
							->GetClassScope()->Find(n3->word);
						if (!n3->var) {
							PostSyntaxError(n3->lex->line, "Undeclared variable or class (%s)", n3->word.data());
						}
					}
				}
			}
			this->_AnalizeList(n2->blockList);
		}
		this->varStack = stackBackup;
	}

	this->varStack.push_back(sourcePtr);
	this->nameStack.push_back(sourcePtr->GetName());
}

Void CodeAnalizer::_AnalizeSelection(NodePtr n) {

	VariablePtr leftVar;
	VariablePtr rightVar;
	Buffer fieldName;
	ObjectPtr fieldObject;

	_Read(n, leftVar, rightVar);
	fieldName = this->nameStack.back();
	this->nameStack.pop_back();
	this->nameStack.pop_back();

	if (leftVar->GetVarType() != Variable::Var::Object) {
		PostSyntaxError(n->lex->line, "You can access to object's fields only (%s)", fieldName.data());
	}

	if (!leftVar->objectValue) {
		PostSyntaxError(n->lex->line, "Object not allocated (%s)", leftVar->GetName().data());
	}

	fieldObject = leftVar->objectValue->GetScopeController()
		->GetVarScope()->Find(fieldName);

	if (!fieldObject) {
		PostSyntaxError(n->lex->line, "Undeclared object's field (%s.%s)", leftVar->GetClass()->GetName().data(), fieldName.data());
	}
	rightVar = fieldObject->GetVariable();

	varStack.push_back(rightVar);
	nameStack.push_back(fieldName);
}

Void CodeAnalizer::_AnalizeCast(NodePtr n) {

	VariablePtr leftVar;
	VariablePtr rightVar;
	VariablePtr sourceVar;

	_Read(n, leftVar, leftVar);
	rightVar = VariablePtr(n->var);

	if (leftVar->GetClass()->GetNameHash() != rightVar->GetClass()->GetNameHash()) {
		if (rightVar->GetClass() == GlobalScope::classFloat ||
			rightVar->GetClass() == GlobalScope::classDouble
			) {
			sourceVar = _FindAvailableRegister(TRUE);
		}
		else {
			sourceVar = _FindAvailableRegister(FALSE);
		}
		sourceVar->GetClass()->GetOperators().at(Uint32(Class::Operator::Cast))(
			sourceVar, leftVar, sourceVar);
		sourceVar->GetClass()->Evaluate(operatorMap[n->lex->lex->id],
			sourceVar, sourceVar, sourceVar, n->lex->lex);
	}
	else {
		sourceVar = leftVar;
	}

	this->varStack.push_back(sourceVar);
	this->nameStack.push_back(sourceVar->GetName());
}

Void CodeAnalizer::_Read(NodePtr n, VariablePtr& left, VariablePtr& right) {

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

LAME_END2