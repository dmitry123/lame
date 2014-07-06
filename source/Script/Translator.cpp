#include "Translator.h"
#include "Exception.h"
#include "Class.h"
#include "Method.h"
#include "Variable.h"
#include "Interface.h"
#include "GlobalScope.h"
#include "Internal.h"
#include "Array.h"
#include "ScopeBuilder.h"
#include "Segment.h"

#include <VirtualMachine/VirtualMachine.h>

LAME_BEGIN2(Script)

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

static AvailableRegister isRegisterAvailable[REGISTERS_COUNT] = {
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

static Void _ClearAvailableRegisters() {
	
	for (Uint32 i = 1; i < REGISTERS_COUNT; i++) {
		isRegisterAvailable[i].isAvailable = TRUE;
	}

	isRegisterAvailable[0].isAvailable = FALSE;
}

static VariablePtr _FindAvailableRegister() {

	for (Uint32 i = 1; i < REGISTERS_COUNT; i++) {
		if (isRegisterAvailable[i].isAvailable) {
			isRegisterAvailable[i].isAvailable = FALSE;
			return isRegisterAvailable[i].registerVar;
		}
	}

	__asm int 3
}

static Void _ReleaseRegister(VariablePtr var) {

	if (var->CheckModificator(Class::Modificator::Register)) {
		isRegisterAvailable[var->GetName()[1] - '0'].isAvailable = TRUE;
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

static Void _TranslateMov(Uint32 command, VariablePtr left, VariablePtr right) {

	switch (command) {
	case MOV: 
		break;
	}

	_Read2(&left, &right);
	_New(command);
	_Write(left);
	_Write(right);
	_Return(GlobalScope::r1);
}

static Void _TranslateMath(Uint32 command, VariablePtr left, VariablePtr right) {

	VariablePtr r = _FindAvailableRegister();

	switch (command) {
	case ADD:
		break;
	case SUB:
		break;
	case MUL:
		break;
	case DIV:
		break;
	}

	_Read2(&left, &right);
	_New(command);
	_Write(r);
	_Write(left);
	_Write(right);
	_Return(r);
}

static Void _TranslateCondition(Uint32 command, VariablePtr left, VariablePtr right) {

	_Read2(&left, &right);
	_New(CMP);
	_Write(left);
	_Write(right);
	_New(command);
	_WriteJump(bcp->GetPosition() + 19);
	_New(MOV);
	_Write(GlobalScope::r1);
	_WriteConst(1);
	_New(JMP);
	_WriteJump(bcp->GetPosition() + 14);
	_New(XOR);
	_Write(GlobalScope::r1);
	_Write(GlobalScope::r1);
	if (!isLastCmp) {
		_New(MOV);
		_Write(GlobalScope::r4);
		_Write(GlobalScope::r1);
	}
	_Return(GlobalScope::r4);
	isLastCmp = TRUE;
}

static Void _TranslateLogic(Uint32 command, VariablePtr left, VariablePtr right) {

	if (isLastCmp) {
		registerStack.pop_back();
	}
	_Read2(&left, &right);
	_New(command);
	_Write(left);
	_Write(right);
	_Return(GlobalScope::r1);
}

static Void _TranslateSingle(Uint32 command, VariablePtr left, VariablePtr right) {

	_Read1(&left);
	_New(command);
	_Write(left);
	_Return(GlobalScope::r1);
}

Void OverloadInteger(ClassPtr intClass) {

	intClass->Overload(Class::Operator::Move, [](VariablePtr left, VariablePtr right) {
		_TranslateMov(MOV, left, right);
	});
	intClass->Overload(Class::Operator::Add, [](VariablePtr left, VariablePtr right) {
		_TranslateMath(ADD, left, right);
	});
	intClass->Overload(Class::Operator::Sub, [](VariablePtr left, VariablePtr right) {
		_TranslateMath(SUB, left, right);
	});
	intClass->Overload(Class::Operator::Mul, [](VariablePtr left, VariablePtr right) {
		_TranslateMath(MUL, left, right);
	});
	intClass->Overload(Class::Operator::Div, [](VariablePtr left, VariablePtr right) {
		_TranslateMath(DIV, left, right);
	});
	intClass->Overload(Class::Operator::Mod, [](VariablePtr left, VariablePtr right) {
		_TranslateMath(MOD, left, right);
	});
	intClass->Overload(Class::Operator::ShiftL, [](VariablePtr left, VariablePtr right) {
		_TranslateMath(SHL, left, right);
	});
	intClass->Overload(Class::Operator::ShiftR, [](VariablePtr left, VariablePtr right) {
		_TranslateMath(SHR, left, right);
	});
	intClass->Overload(Class::Operator::Above, [](VariablePtr left, VariablePtr right) {
		_TranslateCondition(JNA, left, right);
	});
	intClass->Overload(Class::Operator::Bellow, [](VariablePtr left, VariablePtr right) {
		_TranslateCondition(JNB, left, right);
	});
	intClass->Overload(Class::Operator::AboveE, [](VariablePtr left, VariablePtr right) {
		_TranslateCondition(JNAE, left, right);
	});
	intClass->Overload(Class::Operator::BellowE, [](VariablePtr left, VariablePtr right) {
		_TranslateCondition(JNBE, left, right);
	});
	intClass->Overload(Class::Operator::Equal, [](VariablePtr left, VariablePtr right) {
		_TranslateCondition(JNE, left, right);
	});
	intClass->Overload(Class::Operator::NotEqual, [](VariablePtr left, VariablePtr right) {
		_TranslateCondition(JE, left, right);
	});
	intClass->Overload(Class::Operator::And, [](VariablePtr left, VariablePtr right) {
		_TranslateLogic(AND, left, right);
	});
	intClass->Overload(Class::Operator::Or, [](VariablePtr left, VariablePtr right) {
		_TranslateLogic(OR, left, right);
	});
	intClass->Overload(Class::Operator::BitAnd, [](VariablePtr left, VariablePtr right) {
		_TranslateLogic(AND, left, right);
	});
	intClass->Overload(Class::Operator::BitOr, [](VariablePtr left, VariablePtr right) {
		_TranslateLogic(OR, left, right);
	});
	intClass->Overload(Class::Operator::BitXor, [](VariablePtr left, VariablePtr right) {
		_TranslateLogic(XOR, left, right);
	});
	intClass->Overload(Class::Operator::Inc, [](VariablePtr left, VariablePtr right) {
		_TranslateSingle(INC, left, right);
	});
	intClass->Overload(Class::Operator::Dec, [](VariablePtr left, VariablePtr right) {
		_TranslateSingle(INC, left, right);
	});
	intClass->Overload(Class::Operator::BitNot, [](VariablePtr left, VariablePtr right) {
		_TranslateSingle(LNOT, left, right);
	});
	intClass->Overload(Class::Operator::Not, [](VariablePtr left, VariablePtr right) {
		_TranslateSingle(NOT, left, right);
	});

	intClass->Overload(Class::Operator::Cast, [](VariablePtr left, VariablePtr right) {

		_Read1(&left);

		if (left->GetVarType() != Variable::Var::Integer) {

			_New(FTI);
			_Write(GlobalScope::r1);
			_Write(left);
			_Return(GlobalScope::r1);
			_Return(right);
		} else {
			_Return(left);
		}
	});
}

Void OverloadFloat(ClassPtr floatClass) {

	floatClass->Overload(Class::Operator::Move, [](VariablePtr left, VariablePtr right) {

		if (left == right) {
			return;
		}

		bcp->New(VirtualMachine::FMOV);

		if (left->CheckModificator(Class::Modificator::Register)) {
			bcp->Write(left->GetName());
		} else {
			bcp->Write(left->GetAddress());
		}

		if (right->CheckModificator(Class::Modificator::Register)) {
			bcp->Write(right->GetName());
		} else {
			bcp->Write(right->GetAddress());
		}
	});

	floatClass->Overload(Class::Operator::Add, [](VariablePtr left, VariablePtr right) {

		_Read2(&left, &right);

		bcp->New(VirtualMachine::FADD)
			->Write(GlobalScope::f1->GetName())
			->Write(left->GetName())
			->Write(right->GetName());

		registerStack.push_back(GlobalScope::f1);
	});

	floatClass->Overload(Class::Operator::Sub, [](VariablePtr left, VariablePtr right) {

		_Read2(&left, &right);

		bcp->New(VirtualMachine::FSUB)
			->Write(GlobalScope::f1->GetName())
			->Write(left->GetName())
			->Write(right->GetName());

		registerStack.push_back(GlobalScope::f1);
	});

	floatClass->Overload(Class::Operator::Mul, [](VariablePtr left, VariablePtr right) {

		_Read2(&left, &right);

		bcp->New(VirtualMachine::FMUL)
			->Write(GlobalScope::f1->GetName())
			->Write(left->GetName())
			->Write(right->GetName());

		registerStack.push_back(GlobalScope::f1);
	});

	floatClass->Overload(Class::Operator::Div, [](VariablePtr left, VariablePtr right) {

		_Read2(&left, &right);

		bcp->New(VirtualMachine::FDIV)
			->Write(GlobalScope::f1->GetName())
			->Write(left->GetName())
			->Write(right->GetName());

		registerStack.push_back(GlobalScope::f1);
	});

	OVERLOAD(Above, floatClass, {
		left->v.intValue = left->v.floatValue > right->v.floatValue;
	});

	OVERLOAD(Bellow, floatClass, {
		left->v.intValue = left->v.floatValue < right->v.floatValue;
	});

	OVERLOAD(AboveE, floatClass, {
		left->v.intValue = left->v.floatValue >= right->v.floatValue;
	});

	OVERLOAD(BellowE, floatClass, {
		left->v.intValue = left->v.floatValue <= right->v.floatValue;
	});

	OVERLOAD(Equal, floatClass, {
		left->v.intValue = left->v.floatValue == right->v.floatValue;
	});

	OVERLOAD(NotEqual, floatClass, {
		left->v.intValue = left->v.floatValue != right->v.floatValue;
	});

	OVERLOAD(And, floatClass, {
		left->v.intValue = left->v.floatValue && right->v.floatValue;
	});

	OVERLOAD(Or, floatClass, {
		left->v.intValue = left->v.floatValue || right->v.floatValue;
	});

	OVERLOAD(Or, floatClass, {
		left->v.intValue = left->GetClass()->GetSizeOf();
	});

	OVERLOAD(Or, floatClass, {
		left->v.floatValue = !left->v.floatValue;
	});

	floatClass->Overload(Class::Operator::Cast, [](VariablePtr left, VariablePtr right) {
		__asm int 3
	});
}

Void NodePerformer::Perform(NodeBuilderPtr builder) {

	this->builder = builder;

	OverloadInteger(GlobalScope::classBoolean);
	OverloadInteger(GlobalScope::classByte);
	OverloadInteger(GlobalScope::classChar);
	OverloadInteger(GlobalScope::classInt);
	OverloadInteger(GlobalScope::classLong);
	OverloadInteger(GlobalScope::classShort);

	OverloadFloat(GlobalScope::classFloat);
	OverloadFloat(GlobalScope::classDouble);
	
	this->Evaluate(builder->GetRootNode()->blockList);
}

Bool NodePerformer::Evaluate(NodeListRef nodeList) {

	VarList varStack;

	if (!this->builder) {
		return FALSE;
	}

	NodeList::iterator i = nodeList.begin();

	while (i != nodeList.end()) {

		NodePtr n = *i;

		if (n->id == kScriptNodeClass ||
			n->id == kScriptNodeInterface
		) {
			goto _Continue;
		}

		if (n->lex->lex->id == kScriptLexSemicolon) {
			registerStack.clear();
			nodeStack.clear();
			_ClearAvailableRegisters();
			goto _Continue;
		}

		lastNode = n;

		if (n->id == kScriptNodeInvoke ||
			n->id == kScriptNodeAlloc
		) {
			this->methodHash = 0;
			for (Uint32 i = 0; i < n->lex->args; i++) {
				n->argList.push_back(this->nodeStack.back().node);
				this->nodeStack.pop_back();
			}
		}
		
		if (n->lex->lex->IsUnknown() ||
			n->lex->lex->IsConst()
		) {
			nodeStack.push_back({ n, n->var });
		}
		else if (n->lex->lex->id == kScriptLexNew) {
			this->_EvaluateNew(n);
		}
		else if (
			n->lex->lex->IsMath() ||
			n->lex->lex->IsBool() ||
			n->lex->lex->id == kScriptLexDirected ||
			n->lex->lex->id == kScriptLexMediated
		) {
			if (n->lex->args == 1) {
				this->_EvaluateExpression1(n);
			} else if (n->lex->args == 2) {
				this->_EvaluateExpression2(n);
			} else {
				this->_EvaluateExpression0(n);
			}
		}

	_Continue:
		++i;
	}

	if (nodeStack.size() > 1) {
		//PostSyntaxError(nodeStack.back().node->lex->line, "Unexcepted expression (%s)", nodeStack.back().node->word.data());
	}

	return TRUE;
}

Void NodePerformer::_EvaluateNew(NodePtr node) {

	ObjectPtr type;
	ObjectPtr result;

	this->_ReadArguments(node);

	type = ScopeBuilder::_FindClass(
		this->argsArray[0].node->parent,
		this->argsArray[0].node->word);

	if (type->CheckModificator(Class::Modificator::Primitive)) {

		VariablePtr var = this->argsArray[0].var->GetVariable();

		result = node->parent->GetScope()->GetTempScope()
			->Add(new Array("", node, type->GetClass(), var->GetInteger()));
	}
	else {
		result = node->parent->GetScope()->GetTempScope()
			->Add(new Variable("", type));

		type->GetClass()->New(result);

		NodePtr n;

		if ((n = result->GetClass()->GetNode()) != NULL) {

			n->var = result->GetVariable()->GetObject();

			for (NodePtr n : n->blockList) {
				if (n->id == kScriptNodeVariable && n->blockList.size()) {
					this->Evaluate(n->blockList);
				}
			}
		}
	}

	nodeStack.push_back({ node, result });
}

Void NodePerformer::_EvaluateExpression0(NodePtr n) {

}

Void NodePerformer::_EvaluateExpression1(NodePtr n) {

	VarNodePtr l;

	this->_ReadArguments(n);
	l = &this->argsArray[0];

	NodePtr left = l->node;

	if (!left->var) {
		if (!(left->var = left->parent->GetScope()->GetVarScope()->Find(left->word))) {
			PostSyntaxError(left->lex->line, "Undeclared variable (%s)", left->word.data());
		}
	}

	ObjectPtr result = left->var;

	//_Print1(result->GetVariable());

	Error e = left->var->GetClass()->Evaluate(
		operatorMap[n->lex->lex->id], result, result);

	if (e != Error::NoError) {
		PostSyntaxError(n->lex->line, "%s (%s)", e.GetDescription(), left->var->GetClass()->GetName().data());
	}

	if (n->lex->lex->IsLeft()) {
		if (left->var->GetVariable()->GetVarType() == Variable::Var::Integer) {
			left->var->GetClass()->Evaluate(
				operatorMap[kScriptLexSet], result, GlobalScope::r1);
		} else {
			left->var->GetClass()->Evaluate(
				operatorMap[kScriptLexSet], result, GlobalScope::f1);
		}
	}

	if (left->var->GetVariable()->GetVarType() == Variable::Var::Integer) {
		nodeStack.push_back({ left, GlobalScope::r1 });
	} else {
		nodeStack.push_back({ left, GlobalScope::f1 });
	}
}

Void NodePerformer::_EvaluateExpression2(NodePtr n) {

	VarNodePtr r;
	VarNodePtr l;

	this->_ReadArguments(n);

	r = &this->argsArray[0];
	l = &this->argsArray[1];

	NodePtr right = r->node;
	NodePtr left = l->node;

	if (r->var) {
		right->var = r->var;
	}
	if (l->var) {
		left->var = l->var;
	}

	if (n->lex->lex->id == kScriptLexDirected ||
		n->lex->lex->id == kScriptLexMediated
	) {
		if (!left->var) {
			if (!(left->var = left->parent->GetScope()->GetVarScope()->Find(left->word))) {
				if (!(left->var = left->parent->GetScope()->GetClassScope()->Find(left->word))) {
					PostSyntaxError(left->lex->line, "Undeclared variable or class (%s)", left->word.data());
				}
			}
		}
		if (!left->var->CheckType(Class::Type::Variable)) {
			PostSyntaxError(left->lex->line, "Unable to apply directed selection to non-variable object (%s)", left->word.data());
		}
	}
	else {
		if (!left->var) {
			if (!(left->var = left->parent->GetScope()->GetVarScope()->Find(left->word))) {
				PostSyntaxError(left->lex->line, "Undeclared variable (%s)", left->word.data());
			}
		}
		if (!right->var) {
			if (!(right->var = right->parent->GetScope()->GetVarScope()->Find(right->word))) {
				PostSyntaxError(right->lex->line, "Undeclared variable (%s)", right->word.data());
			}
		}
	}

	ObjectPtr result = left->var;
	ObjectPtr with = right->var;

	if (n->lex->lex->id == kScriptLexDirected ||
		n->lex->lex->id == kScriptLexMediated
	) {
		Uint32 methodHash = 0;
		ClassPtr varClass;

		varClass = left->var->GetVariable()->GetObject();

		if (right->id == kScriptNodeInvoke ||
			right->id == kScriptNodeAlloc
		) {
			Vector<VariablePtr> varList;
			for (NodePtr n : right->argList) {
				varList.push_back(n->var->GetVariable());
			}
			methodHash = Method::ComputeInvokeHash(varList);
			result = varClass->FindMethod(right->word, methodHash);
			if (!result) {
				PostSyntaxError(n->lex->line, "Undeclared method (%s)", right->word.data());
			}
			result->GetMethod()->Invoke(this, varList);
		}
		else {
			if (!varClass) {
				PostSyntaxError(n->lex->line, "Null pointer (%s)", result->GetName().data());
			}
			result = varClass->FindVariable(right->word);
			if (!result) {
				PostSyntaxError(n->lex->line, "Undeclared field (%s)", right->word.data());
			}
		}

		if (result->CheckModificator(Class::Modificator::Private) ||
			result->CheckModificator(Class::Modificator::Protected)
		) {
			PostSyntaxError(n->lex->line, "You can't get access to private/protected objects from this scope (%s)", right->word.data());
		}

		left->var = result;

		goto _SaveNode;
	}

	try {
		if (n->lex->lex->IsBool()) {
			//_Print2(result->GetVariable(), with->GetVariable());
		} else if (n->lex->lex->IsMath()) {
			if (n->lex->lex->id != kScriptLexSet) {
				//_Print2(result->GetVariable(), with->GetVariable());
			} else {
			}
		}

		_Print2(result->GetVariable(), with->GetVariable());

		if (!n->lex->lex->IsBool()) {
			isLastCmp = FALSE;
		}

		Error e = left->var->GetClass()->Evaluate(
			operatorMap[n->lex->lex->id], result, with);

		if (e != Error::NoError) {
			PostSyntaxError(n->lex->line, "%s (%s)", e.GetDescription(), left->var->GetClass()->GetName().data());
		}
	}
	catch (ClassInvalidCastException) {

		PostSyntaxError(n->lex->line, "Unable to cast from (%s) to (%s)",
			result->GetClass()->GetName().data(),
			with->GetClass()->GetName().data());
	}

	if (n->lex->lex->IsLeft() && n->lex->lex->id != kScriptLexSet) {
		left->var->GetClass()->Evaluate(
			operatorMap[kScriptLexSet], result, GlobalScope::r1);
	}

_SaveNode:
	if (left->var->GetVariable()->GetVarType() == Variable::Var::Integer) {
		nodeStack.push_back({ left, GlobalScope::r1 });
	} else {
		nodeStack.push_back({ left, GlobalScope::f1 });
	}
}

Void NodePerformer::_ReadArguments(NodePtr node) {

	if (nodeStack.size() < node->lex->args) {
		PostSyntaxError(node->lex->line, "Operator \"%s\" requires %d arguments", node->word.data(), node->lex->args);
	}

	this->argsArray.clear();

	for (Uint32 i = 0; i < node->lex->args; i++) {
		this->argsArray.push_back(nodeStack.back());
		nodeStack.pop_back();
	}
}

LAME_END2
