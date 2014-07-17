#include "Analizer.h"
#include "LowLevelStack.h"

#include <VirtualMachine/Assembler.h>

LAME_BEGIN2(Compiler)

using namespace VirtualMachine;

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
			throw ClassInvalidCastException("");
		//case Class::Type::Array:
		//	if (left->GetClass() != right->GetClass()) {
		//		throw ClassInvalidCastException("");
		//	}
		case Class::Type::Variable:
			break;
		}

		if (left->GetVarType() != Variable::Var::Integer &&
			left->GetVarType() != Variable::Var::Integer
		) {
			if (left->GetVarType() == Variable::Var::Float) {
				source->v.intValue = (ScriptNativeInt)left->v.floatValue;
			}
			else {
				throw ClassInvalidCastException("");
			}
		}

		if (right->GetObject() && !left->GetObject() || !right->GetObject() && left->GetObject()) {
			throw ClassInvalidCastException("");
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
		//case Class::Type::Abstract:
		//case Class::Type::Array:
			throw ClassInvalidCastException("");
		case Class::Type::Variable:
			break;
		}

		if (left->GetVarType() != Variable::Var::Float &&
			left->GetVarType() != Variable::Var::Float
		) {
			if (left->GetVarType() == Variable::Var::Integer) {
				source->v.floatValue = (ScriptNativeFloat)left->v.intValue;
			}
			else {
				throw ClassInvalidCastException("");
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
		left->Clone(source->GetName());
	});

	objectClass->Overload(Class::Operator::Cast, [](VariablePtr source, VariablePtr left, VariablePtr right) {

		if (right->GetClass()->Hash() == left->GetClass()->Hash()) {
			goto __AllowCast;
		}

		if (left->GetClass()->GetExtend() == right->GetClass()) {
			goto __AllowCast;
		}

		for (ObjectPtr i : left->GetClass()->GetImplements()) {
			if (right->GetClass() == i->GetClass()) {
				goto __AllowCast;
			}
		}

		throw ClassInvalidCastException("Invalid object cast");

	__AllowCast:
		;
	});
}

Void CodeAnalizer::Analize(LowLevelStackPtr lowLevelStack, NodeBuilderPtr nodeBuilder) {
	this->lls = lowLevelStack;
	this->_AnalizeList(nodeBuilder->GetRootNode()->blockList);
}

Void CodeAnalizer::Overload(ScopePtr rootScope) {

	this->rootScope = rootScope;

	OverloadInteger(rootScope->classBoolean);
	OverloadInteger(rootScope->classByte);
	OverloadInteger(rootScope->classChar);
	OverloadInteger(rootScope->classInt);
	OverloadInteger(rootScope->classLong);
	OverloadInteger(rootScope->classShort);

	OverloadFloat(rootScope->classFloat);
	OverloadFloat(rootScope->classDouble);

	OverloadString(rootScope->classString);
	OverloadObject(rootScope->classString);
	OverloadObject(rootScope->classObject);
	OverloadObject(rootScope->classClass);
}

Void CodeAnalizer::_AnalizeList(NodeListRef nodeList) {

	for (NodePtr n : nodeList) {
		try {
			if (n->id == kScriptNodeClass || n->id == kScriptNodeInterface || n->id == kScriptNodeFunction) {
				continue;
			}
			else if (n->id == kScriptNodeInvoke || n->id == kScriptNodeAlloc) {
				if (!n->var) {
					PostSyntaxError(n->lex->line, "Undeclared class (%s)", n->word.data());
					//n->var = n->parent->GetScope()->GetClassScope()->Find(n->word);
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
					PostSyntaxError(n->lex->line, "Undeclared variable (%s)", n->word.data());
					//n->var = n->parent->GetScope()->GetClassScope()->Find(n->word);
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
					//n->var->GetNode()->templatesNew = n->templates;
				}
			}
			else if (n->lex->lex->id == kScriptLexSemicolon) {
				lls->ReleaseRegister();
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
		catch (ScriptException e) {
			throw SyntaxException(n->lex->line, e.GetErrorBuffer());
		}
	}
}

Void CodeAnalizer::_Analize0(NodePtr n) {

	VariablePtr sourceVar;
	VariablePtr leftVar;
	ObjectPtr methodVar;
	Uint32 methodHash;
	Vector<ClassPtr> objectList;
	Uint32 invokeHash;
	Buffer formattedParameters;

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

	sourceVar = lls->FindRegister(LowLevelStack::Pointer);

	this->varStack.push_back(sourceVar);
	this->nameStack.push_back(sourceVar->GetName());
}

Void CodeAnalizer::_Analize1(NodePtr n) {

	VariablePtr leftVar;
	VariablePtr rightVar;
	VariablePtr sourceVar;

	_Read(n, leftVar, rightVar);

	if (n->lex->lex->IsRight()) {
		if (leftVar->GetClassType()->IsFloat()) {
			sourceVar = lls->FindRegister(LowLevelStack::Float);
		}
		else if (leftVar->GetClassType()->IsInt()) {
			sourceVar = lls->FindRegister(LowLevelStack::Integer);
		}
		else {
			sourceVar = lls->FindRegister(LowLevelStack::Pointer);
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

Void CodeAnalizer::_Analize2(NodePtr n) {

	VariablePtr sourceVar;
	VariablePtr leftVar;
	VariablePtr rightVar;
	VariablePtr rightVar2;

	this->_Read(n, leftVar, rightVar);

	rightVar2 = rightVar;

	if (n->lex->lex->IsRight()) {
		if (leftVar->GetClassType()->Hash() != rightVar->GetClassType()->Hash()) {
			if (leftVar->GetClassType()->GetPriority() != rightVar->GetClassType()->GetPriority()) {
				if (leftVar->GetClassType()->GetPriority() > rightVar->GetClassType()->GetPriority()) {
					if (leftVar->GetClassType()->IsFloat()) {
						sourceVar = lls->FindRegister(LowLevelStack::Float);
					}
					else if (leftVar->GetClassType()->IsInt()) {
						sourceVar = lls->FindRegister(LowLevelStack::Integer);
					}
					else {
						sourceVar = lls->FindRegister(LowLevelStack::Pointer);
					}
					leftVar->GetClassType()->Evaluate(Class::Operator::Cast, sourceVar, rightVar, leftVar, n->lex->lex);
					rightVar = sourceVar;
				}
				else {
					if (rightVar->GetClassType()->IsFloat()) {
						sourceVar = lls->FindRegister(LowLevelStack::Float);
					}
					else if (rightVar->GetClassType()->IsInt()) {
						sourceVar = lls->FindRegister(LowLevelStack::Integer);
					}
					else {
						sourceVar = lls->FindRegister(LowLevelStack::Pointer);
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
						sourceVar = lls->FindRegister(LowLevelStack::Float);
					}
					else if (leftVar->GetClassType()->IsInt()) {
						sourceVar = lls->FindRegister(LowLevelStack::Integer);
					}
					else {
						sourceVar = lls->FindRegister(LowLevelStack::Pointer);
					}
				}
				else {
					if (rightVar->GetClassType()->IsFloat()) {
						sourceVar = lls->FindRegister(LowLevelStack::Float);
					}
					else if (rightVar->GetClassType()->IsInt()) {
						sourceVar = lls->FindRegister(LowLevelStack::Integer);
					}
					else {
						sourceVar = lls->FindRegister(LowLevelStack::Pointer);
					}
				}
			}
			else {
				if (leftVar->GetClassType()->IsFloat()) {
					sourceVar = lls->FindRegister(LowLevelStack::Float);
				}
				else if (leftVar->GetClassType()->IsInt()) {
					sourceVar = lls->FindRegister(LowLevelStack::Integer);
				}
				else {
					sourceVar = lls->FindRegister(LowLevelStack::Pointer);
				}
			}
		}
		else {
			sourceVar = lls->FindRegister(LowLevelStack::Integer);
		}
	}
	else {
		if (rightVar->GetVarType() != Variable::Var::Object && leftVar->GetClassType()->Hash() != rightVar->GetClassType()->Hash()) {
			if (leftVar->GetClassType()->IsFloat()) {
				sourceVar = lls->FindRegister(LowLevelStack::Float);
			}
			else if (leftVar->GetClassType()->IsInt()) {
				sourceVar = lls->FindRegister(LowLevelStack::Integer);
			}
			else {
				sourceVar = lls->FindRegister(LowLevelStack::Pointer);
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
		lls->ReleaseRegister(RegisterPtr(leftVar));
	}
	if (rightVar->CheckModificator(Class::Modificator::Register)) {
		lls->ReleaseRegister(RegisterPtr(rightVar));
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
	classVar = ClassPtr(leftVar);

	//if (!classVar->GetNode() && !classVar->CheckModificator(Class::Modificator::Primitive)) {
	//	PostSyntaxError(n->lex->line, "Object isn't class (%s)", classVar->GetName().data());
	//}

	sourcePtr = lls->FindRegister(LowLevelStack::Pointer);

	if (classVar->CheckModificator(Class::Modificator::Primitive)) {
		sourcePtr->SetObject(new Class(*classVar));
	}
	else {
		stackBackup = this->varStack;
		classVar->GetClass()->New(sourcePtr);

		//if (leftVar->GetTemplateClass()) {
		//	if (!(templateClass = ScopeBuilder::_FindClass(leftVar->GetNode()->parent, leftVar->GetNode()->templatesNew))) {
		//		PostSyntaxError(n->lex->line, "Undeclared class %s", leftVar->GetNode()->templates.data());
		//	}
		//	sourcePtr->SetTemplateClass(templateClass->GetClass());
		//	ClassPtr classT = new Class(leftVar->GetNode()->templates);
		//	templateClass->Clone(classT);
		//	sourcePtr->GetObject()->GetScopeController()->GetClassScope()
		//		->Add(classT)->SetNode(templateClass->GetNode());
		//}
		
		if (classVar->GetNode()) {
			for (NodePtr n2 : classVar->GetNode()->blockList) {
				this->_AnalizeList(n2->blockList);
			}
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

	fieldObject = leftVar->objectValue->Find(fieldName);

	if (!fieldObject) {
		PostSyntaxError(n->lex->line, "Undeclared object's field (%s.%s)", leftVar->GetClass()->GetName().data(), fieldName.data());
	}
	rightVar = fieldObject->GetVariable();

	varStack.push_back(rightVar);
	nameStack.push_back(fieldName);
}

Void CodeAnalizer::_AnalizeCast(NodePtr n) {

	VariablePtr leftVar;
	ClassPtr rightVar;
	VariablePtr sourceVar;

	_Read(n, leftVar, leftVar);
	rightVar = n->var->GetClass();

	if (leftVar->GetClass()->Hash() != rightVar->GetClass()->Hash()) {
		if (rightVar->GetClass() == Scope::classFloat ||
			rightVar->GetClass() == Scope::classDouble
		) {
			sourceVar = lls->FindRegister(LowLevelStack::Float);
		}
		else {
			sourceVar = lls->FindRegister(LowLevelStack::Integer);
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