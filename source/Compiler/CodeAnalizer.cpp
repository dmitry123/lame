#include "CodeAnalizer.h"
#include "LowLevelStack.h"

#include <VirtualMachine/Assembler.h>

LAME_BEGIN2(Compiler)

using namespace VirtualMachine;

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
	case OR:  source->v.intValue = left->v.intValue | right->v.intValue; break;
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

Void CodeAnalizer::OverloadInteger(ScopePtr scope) {

	ClassPtr intClass = scope->classFloat;

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
Void CodeAnalizer::OverloadFloat(ScopePtr scope) {

	ClassPtr floatClass = scope->classFloat;

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
Void CodeAnalizer::OverloadString(ScopePtr scope) {

	ClassPtr stringClass = scope->classFloat;

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
Void CodeAnalizer::OverloadObject(ScopePtr scope) {

	ClassPtr objectClass = scope->classFloat;

	objectClass->Overload(Class::Operator::Move, [](VariablePtr source, VariablePtr left, VariablePtr right) {
		//left->Clone(source->GetName());
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

RegisterPtr CodeAnalizer::Integer2Float(VariablePtr value) {
	return NULL;
}

RegisterPtr CodeAnalizer::Float2Integer(VariablePtr value) {
	return NULL;
}

RegisterPtr CodeAnalizer::Object2String(VariablePtr value) {
	return NULL;
}

RegisterPtr CodeAnalizer::String2Object(VariablePtr value) {
	return NULL;
}

RegisterPtr CodeAnalizer::Integer2String(VariablePtr value) {
	return NULL;
}

RegisterPtr CodeAnalizer::String2Integer(VariablePtr value) {
	return NULL;
}

RegisterPtr CodeAnalizer::Float2String(VariablePtr value) {
	return NULL;
}

RegisterPtr CodeAnalizer::String2Float(VariablePtr value) {
	return NULL;
}

Void CodeAnalizer::AnalizeBinary(VariablePtr source, VariablePtr left, VariablePtr right) {

}

Void CodeAnalizer::AnalizeUnary(VariablePtr source) {

}

Void CodeAnalizer::AnalizeNew(VariablePtr source, ClassPtr left) {

}

Void CodeAnalizer::AnalizeSelection(VariablePtr source, VariablePtr left, VariablePtr right) {

}

Void CodeAnalizer::AnalizeCondition(NodePtr n, VariablePtr source) {

}

Void CodeAnalizer::AnalizeCast(VariablePtr source, ClassPtr left) {

}

Void CodeAnalizer::AnalizeInvoke(NodePtr n, VariablePtr source) {

}

LAME_END2