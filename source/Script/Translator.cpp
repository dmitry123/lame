#include "Translator.h"

#include <VirtualMachine/VirtualMachine.h>

LAME_BEGIN2(Compiler)

using namespace VirtualMachine;

#define OVERLOAD(_operator, _class, _code) \
	_class->Overload(Class::Operator::_operator, [] (VariablePtr left, VariablePtr right) _code); \

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
	{ kScriptLexNot, Class::Operator::Not }
};

static VirtualMachine::ByteCodePrinterPtr bcp
	= VirtualMachine::ByteCodePrinter::GetInstance();

#define REGISTERS_COUNT 10

static Vector<VariablePtr> registerStack;
static NodePtr lastNode = 0;
static Bool isLastCmp = 0;

typedef struct {
	VariablePtr registerVar;
	Bool isAvailable;
} AvailableRegister;

static AvailableRegister isRegisterAvailableL[REGISTERS_COUNT] = {
		{ GlobalScope::r0, TRUE },
		{ GlobalScope::r1, TRUE },
		{ GlobalScope::r2, TRUE },
		{ GlobalScope::r3, TRUE },
		{ GlobalScope::r4, TRUE },
		{ GlobalScope::r5, TRUE },
		{ GlobalScope::r6, TRUE },
		{ GlobalScope::r7, TRUE },
		{ GlobalScope::r8, TRUE },
		{ GlobalScope::r9, TRUE }
};

static AvailableRegister isRegisterAvailableF[REGISTERS_COUNT] = {
		{ GlobalScope::f0, TRUE },
		{ GlobalScope::f1, TRUE },
		{ GlobalScope::f2, TRUE },
		{ GlobalScope::f3, TRUE },
		{ GlobalScope::f4, TRUE },
		{ GlobalScope::f5, TRUE },
		{ GlobalScope::f6, TRUE },
		{ GlobalScope::f7, TRUE },
		{ GlobalScope::f8, TRUE },
		{ GlobalScope::f9, TRUE }
};

static Void _ClearAvailableRegisters() {
	
	for (Uint32 i = 0; i < REGISTERS_COUNT; i++) {
		isRegisterAvailableL[i].isAvailable = TRUE;
		isRegisterAvailableF[i].isAvailable = TRUE;
	}
}

static VariablePtr _FindAvailableRegister() {

	for (Uint32 i = 0; i < REGISTERS_COUNT; i++) {
		if (isRegisterAvailableL[i].isAvailable) {
			isRegisterAvailableL[i].isAvailable = FALSE;
			return isRegisterAvailableL[i].registerVar;
		}
	}

	__asm int 3
}

static VariablePtr _FindAvailableRegisterF() {

	for (Uint32 i = 0; i < REGISTERS_COUNT; i++) {
		if (isRegisterAvailableF[i].isAvailable) {
			isRegisterAvailableF[i].isAvailable = FALSE;
			return isRegisterAvailableF[i].registerVar;
		}
	}

	__asm int 3
}

static Void _ReleaseRegister(VariablePtr var) {

	if (var->CheckModificator(Class::Modificator::Register)) {

		if (var->GetName()[0] == 'r') {
			isRegisterAvailableL[var->GetName()[1] - '0'].isAvailable = TRUE;
		} else if (var->GetName()[0] == 'f') {
			isRegisterAvailableF[var->GetName()[1] - '0'].isAvailable = TRUE;
		}
	}
}

static Void _Print1(VariablePtr left) {

	if (!left->CheckModificator(Class::Modificator::Register)) {
		registerStack.push_back(left);
	}
}

static Void _Print2(VariablePtr left, VariablePtr right) {
	
	_Print1(right);
	_Print1(left);
}

static Void _Read2(VariablePtr* left, VariablePtr* right) {

	if (registerStack.size() <= 1) {
		return;
	}

	*left = registerStack.back();
	registerStack.pop_back();

	*right = registerStack.back();
	registerStack.pop_back();
}

static Void _Read1(VariablePtr* left) {

	if (registerStack.size() <= 0) {
		return;
	}

	*left = registerStack.back();
	registerStack.pop_back();
}

static Void _New(Uint32 command) {
	bcp->New(VirtualMachine::Asm(command));
}

static Void _Write(VariablePtr var) {

	if (var->CheckModificator(Class::Modificator::Register)) {
		bcp->Write(var->GetName());
	} else {
		bcp->Write(var->GetAddress());
	}

	if (!bcp->IsSource()) {
		_ReleaseRegister(var);
	}
}

static Void _WriteJump(Uint32 address) {
	bcp->Write(address, VirtualMachine::ByteCodePrinter::LineType::Jump);
}

static Void _WriteConst(Uint32 value) {
	bcp->Write(value, VirtualMachine::ByteCodePrinter::LineType::Const);
}

static Void _Return(VariablePtr var) {
	registerStack.push_back(var);
}

static Void _TranslateMov(Uint32 command, VariablePtr source, VariablePtr left, VariablePtr right, Bool isFloat = FALSE) {

	VariablePtr r;

	if (isFloat) {
		r = _FindAvailableRegisterF();
	} else {
		r = _FindAvailableRegister();
	}

	switch (command) {
		case MOV: r->v.intValue = right->v.intValue; break;
		case FMOV: r->v.floatValue = right->v.floatValue; break;
	}

	_Read2(&left, &right);
	_New(command);
	_Write(left);
	_Write(right);
	_Return(r);
}

static Void _TranslateMath(Uint32 command, VariablePtr source, VariablePtr left, VariablePtr right, Bool isFloat = FALSE) {

	VariablePtr r;

	if (isFloat) {
		r = _FindAvailableRegisterF();
	} else {
		r = _FindAvailableRegister();
	}

	_Read2(&left, &right);
	_New(command);
	_Write(r);
	_Write(left);
	_Write(right);
	_Return(r);

	switch (command) {
		case ADD: r->v.intValue = left->v.intValue + right->v.intValue; break;
		case SUB: r->v.intValue = left->v.intValue - right->v.intValue; break;
		case MUL: r->v.intValue = left->v.intValue * right->v.intValue; break;
		case DIV: r->v.intValue = left->v.intValue / right->v.intValue; break;
		case FADD: r->v.floatValue = left->v.floatValue + right->v.floatValue; break;
		case FSUB: r->v.floatValue = left->v.floatValue - right->v.floatValue; break;
		case FMUL: r->v.floatValue = left->v.floatValue * right->v.floatValue; break;
		case FDIV: r->v.floatValue = left->v.floatValue / right->v.floatValue; break;
	}
}

static Void _TranslateCondition(Uint32 cmp, Uint32 command, VariablePtr source, VariablePtr left, VariablePtr right) {

	VariablePtr r;

	r = _FindAvailableRegister();

	_Read2(&left, &right);
	_New(cmp);
	_Write(left);
	_Write(right);
	_New(command);
	_WriteJump(bcp->GetPosition() + 19);
	_New(MOV);
	_Write(r);
	_WriteConst(1);
	_New(JMP);
	_WriteJump(bcp->GetPosition() + 14);
	_New(MOV);
	_Write(r);
	_WriteConst(0);
	_Return(r);
}

static Void _TranslateSingle(Uint32 command, VariablePtr source) {

	_Read1(&source);
	_New(command);
	_Write(source);
	_Return(source);
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

		VariablePtr r;

		_Read1(&right);

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

			r = _FindAvailableRegister();
			registerStack.pop_back();

			if (left->GetVarType() == Variable::Var::Float) {

				_New(FTI);
				_Write(r);
				_Write(left);
				_Return(r);
				_Return(right);

			} else {
				throw ClassInvalidCastException();
			}
		} else {
			_Return(left);
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

		VariablePtr r;

		_Read1(&right);

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
			r = _FindAvailableRegisterF();
			registerStack.pop_back();
			if (left->GetVarType() == Variable::Var::Integer) {
				_New(ITF);
				_Write(r);
				_Write(left);
				_Return(r);
				_Return(right);
			} else {
				throw ClassInvalidCastException();
			}
		} else {
			_Return(left);
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

		_Read2(&left, &right);
		_New(MOV);
		_Write(left);
		_Write(right);
		_Return(left);
	});

	objectClass->Overload(Class::Operator::Cast, [](VariablePtr source, VariablePtr left, VariablePtr right) {

		if (right->GetClass() == left->GetClass()) {
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

Void CodeTranslator::Overload(Void) {

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

Void CodeTranslator::Translate(NodeBuilderPtr builder) {

	this->builder = builder;
	
	this->Evaluate(
		builder->GetRootNode()->blockList);
}

Bool CodeTranslator::Evaluate(NodeListRef nodeList) {

	return TRUE;
}

Void CodeTranslator::_TranslateNew(NodePtr node) {

	ObjectPtr type;
	NodePtr left;
	VariablePtr countVar;

	_ReadArguments(node);
	left = this->argsArray[0].node;

	type = ScopeBuilder::_FindClass(left->parent, left->word);

	printf("  // %s %s\n", node->word.data(), left->word.data());

	if (type->CheckModificator(Class::Modificator::Primitive)) {

		_ReadArguments(node);

		if (!(countVar = this->argsArray[0].var->GetVariable())) {
			countVar = this->argsArray[0].node->var->GetVariable();
		}

		VariablePtr r = _FindAvailableRegister();
		Uint32 classSize = type->GetSizeOf();

		_New(VirtualMachine::NEW);
		_Write(r);
		_WriteJump(classSize * countVar->GetInteger());
		_Return(r);

		r->registerType = node->parent->GetScope()->GetTempScope()
			->Add(new Array("", node, type->GetClass(), countVar->GetInteger()))->GetArray();
	}
	else {

		VariablePtr r = _FindAvailableRegister();
		Uint32 classSize = type->GetSizeOf();

		_New(VirtualMachine::NEW);
		_Write(r);
		_WriteJump(classSize);
		_Return(r);

		r->registerType = type->GetClass();

		//result = node->parent->GetScope()->GetTempScope()
		//	->Add(new Variable("", type));

		//type->GetClass()->New(result);

		//NodePtr n;

		//if ((n = result->GetClass()->GetNode()) != NULL) {

		//	n->var = result->GetVariable()->GetObject();

		//	for (NodePtr n : n->blockList) {
		//		if (n->id == kScriptNodeVariable && n->blockList.size()) {
		//			this->Evaluate(n->blockList);
		//		}
		//	}
		//}
	}

	varStack.push_back({ left, registerStack.back() });
}

Void CodeTranslator::_TranslateCondition(NodePtr node) {

}

Void CodeTranslator::_Translate0(NodePtr n) {

}

Void CodeTranslator::_Translate1(NodePtr n) {

	VarNodePtr l;

	this->_ReadArguments(n);
	l = &this->argsArray[0];

	NodePtr left = l->node;

	if (!left->var) {
		if (!(left->var = left->parent->GetScope()->GetVarScope()->Find(left->word))) {
			PostSyntaxError(left->lex->line, "Undeclared variable (%s)", left->word.data());
		}
	}

	VariablePtr result = left->var->GetVariable();

	Error e = left->var->GetClass()->Evaluate(
		operatorMap[n->lex->lex->id], result, result, result, n->lex->lex);

	if (e != Error::NoError) {
		PostSyntaxError(n->lex->line, "%s (%s)", e.GetDescription(), left->var->GetClass()->GetName().data());
	}

	varStack.push_back({ left, registerStack.back() });
}

Void CodeTranslator::_Translate2(NodePtr n) {

//	VarNodePtr r;
//	VarNodePtr l;
//
//	this->_ReadArguments(n);
//
//	r = &this->argsArray[0];
//	l = &this->argsArray[1];
//
//	NodePtr right = r->node;
//	NodePtr left = l->node;
//
//	if (n->lex->lex->id == kScriptLexDirected ||
//		n->lex->lex->id == kScriptLexMediated
//	) {
//		if (!left->var) {
//			if (!(left->var = left->parent->GetScope()->GetVarScope()->Find(left->word))) {
//				if (!(left->var = left->parent->GetScope()->GetClassScope()->Find(left->word))) {
//					PostSyntaxError(left->lex->line, "Undeclared variable or class (%s)", left->word.data());
//				}
//			}
//		}
//		if (!left->var->CheckType(Class::Type::Variable)) {
//			PostSyntaxError(left->lex->line, "Unable to apply directed selection to non-variable object (%s)", left->word.data());
//		}
//	}
//
//	ObjectPtr result = left->var;
//	ObjectPtr with = right->var;
//
//	if (n->lex->lex->id == kScriptLexDirected ||
//		n->lex->lex->id == kScriptLexMediated
//	) {
//		Uint32 methodHash = 0;
//		ClassPtr varClass;
//
//		varClass = left->var->GetVariable()->GetObject();
//
//		if (right->id == kScriptNodeInvoke ||
//			right->id == kScriptNodeAlloc
//		) {
//			Vector<VariablePtr> varList;
//			for (NodePtr n : right->argList) {
//				varList.push_back(n->var->GetVariable());
//			}
//			methodHash = Method::ComputeInvokeHash(varList);
//			result = varClass->FindMethod(right->word, methodHash);
//			if (!result) {
//				PostSyntaxError(n->lex->line, "Undeclared method (%s)", right->word.data());
//			}
//			result->GetMethod()->Invoke(varList);
//		}
//		else {
//			if (!varClass) {
//				PostSyntaxError(n->lex->line, "Null pointer (%s)", result->GetName().data());
//			}
//			result = varClass->FindVariable(right->word);
//			if (!result) {
//				PostSyntaxError(n->lex->line, "Undeclared field (%s)", right->word.data());
//			}
//		}
//
//		if (result->CheckModificator(Class::Modificator::Private) ||
//			result->CheckModificator(Class::Modificator::Protected)
//		) {
//			PostSyntaxError(n->lex->line, "You can't get access to private/protected objects from this scope (%s)", right->word.data());
//		}
//
//		left->var = result;
//
//		goto _SaveNode;
//	}
//
//	printf("  // %s %s %s\n", result->GetName().data(), n->word.data(), with->GetName().data());
//
//	if (with->CheckModificator(Class::Modificator::Register)) {
//		with->GetVariable()->registerType->SetSegment(result->GetSegment(), result->GetAddressPtr());
//	}
//
//	_Print2(result->GetVariable(), with->GetVariable());
//
//	try {
//		Error e = left->var->GetClass()->Evaluate(
//			operatorMap[n->lex->lex->id], result, with, n->lex->lex);
//
//		if (e != Error::NoError) {
//			PostSyntaxError(n->lex->line, "%s (%s)", e.GetDescription(), left->var->GetClass()->GetName().data());
//		}
//	}
//	catch (ClassInvalidCastException) {
//
//		if (with->CheckModificator(Class::Modificator::Register)) {
//			PostSyntaxError(n->lex->line, "Unable to cast from (%s) to (%s)", result->GetClass()->GetName().data(), with->GetVariable()->registerType->GetClass()->GetName().data());
//		} else {
//			PostSyntaxError(n->lex->line, "Unable to cast from (%s) to (%s)", result->GetClass()->GetName().data(), with->GetClass()->GetName().data());
//		}
//	}
//
//	if (n->lex->lex->IsLeft() && n->lex->lex->id != kScriptLexSet) {
//		left->var->GetClass()->Evaluate(
//			operatorMap[kScriptLexSet], result, GlobalScope::r1, Lex::Find(kScriptLexSet));
//	}
//
//_SaveNode:
//	varStack.push_back({ left, registerStack.back() });
}

Void CodeTranslator::_ReadArguments(NodePtr node) {

	if (varStack.size() < node->lex->args) {
		PostSyntaxError(node->lex->line, "Operator \"%s\" requires %d arguments", node->word.data(), node->lex->args);
	}

	this->argsArray.clear();

	for (Uint32 i = 0; i < node->lex->args; i++) {
		this->argsArray.push_back(varStack.back());
		varStack.pop_back();
	}
}

LAME_END2
