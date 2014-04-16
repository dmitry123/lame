#include "Script.h"
#include "Internal.h"

LAME_BEGIN

#define __EvalError(_operation) \
	PostSyntaxError(left->lex->line, "Unable to apply %s operation to this type", #_operation, left->name.data());

static Void Set(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case ScriptVar::kScriptTypeBool:
	case ScriptVar::kScriptTypeInt:
		left->intValue = right->intValue;
		break;
	case ScriptVar::kScriptTypeFloat:
		left->floatValue = right->floatValue;
		break;
	case ScriptVar::kScriptTypeString:
		left->stringValue = right->stringValue;
		break;
	case ScriptVar::kScriptTypeClass:
		if (left->className != right->name) {
			PostSyntaxError(left->lex->line, "Unable to copy class with another type", 1);
		}
		left->classValue = right->classValue;
		break;
	default:
		__EvalError(=);
	}
}

static Void Add(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case ScriptVar::kScriptTypeBool:
	case ScriptVar::kScriptTypeInt:
		left->intValue += right->intValue;
		break;
	case ScriptVar::kScriptTypeFloat:
		left->floatValue += right->floatValue;
		break;
	case ScriptVar::kScriptTypeString:
		left->stringValue += right->stringValue;
		break;
	default:
		__EvalError(+);
	}
}

static Void Sub(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case ScriptVar::kScriptTypeBool:
	case ScriptVar::kScriptTypeInt:
		left->intValue -= right->intValue;
		break;
	case ScriptVar::kScriptTypeFloat:
		left->floatValue -= right->floatValue;
		break;
	default:
		__EvalError(-);
	}
}

static Void Mul(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case ScriptVar::kScriptTypeBool:
	case ScriptVar::kScriptTypeInt:
		left->intValue *= right->intValue;
		break;
	case ScriptVar::kScriptTypeFloat:
		left->floatValue *= right->floatValue;
		break;
	default:
		__EvalError(*);
	}
}

static Void Div(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case ScriptVar::kScriptTypeBool:
	case ScriptVar::kScriptTypeInt:
		left->intValue /= right->intValue;
		break;
	case ScriptVar::kScriptTypeFloat:
		left->floatValue /= right->floatValue;
		break;
	default:
		__EvalError(/);
	}
}

static Void Mod(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case ScriptVar::kScriptTypeBool:
	case ScriptVar::kScriptTypeInt:
		left->intValue %= right->intValue;
		break;
	default:
		__EvalError(%);
	}
}

static Void BitAnd(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case ScriptVar::kScriptTypeBool:
	case ScriptVar::kScriptTypeInt:
		left->intValue &= right->intValue;
		break;
	default:
		__EvalError(&);
	}
}

static Void BitOr(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case ScriptVar::kScriptTypeBool:
	case ScriptVar::kScriptTypeInt:
		left->intValue |= right->intValue;
		break;
	default:
		__EvalError(|);
	}
}

static Void BitXor(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case ScriptVar::kScriptTypeBool:
	case ScriptVar::kScriptTypeInt:
		left->intValue ^= right->intValue;
		break;
	default:
		__EvalError(^);
	}
}

static Void BitShiftR(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case ScriptVar::kScriptTypeBool:
	case ScriptVar::kScriptTypeInt:
		left->intValue >>= right->intValue;
		break;
	default:
		__EvalError(>>);
	}
}

static Void BitShiftL(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case ScriptVar::kScriptTypeBool:
	case ScriptVar::kScriptTypeInt:
		left->intValue <<= right->intValue;
		break;
	default:
		__EvalError(<<);
	}
}

static Void And(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case ScriptVar::kScriptTypeBool:
	case ScriptVar::kScriptTypeInt:
		left->intValue = left->intValue && right->intValue;
		break;
	case ScriptVar::kScriptTypeFloat:
		left->intValue = left->floatValue && right->floatValue;
		break;
	case ScriptVar::kScriptTypeString:
		left->intValue = left->stringValue.length() && right->stringValue.length();
		break;
	default:
		__EvalError(&&);
	}
}

static Void Or(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case ScriptVar::kScriptTypeBool:
	case ScriptVar::kScriptTypeInt:
		left->intValue = left->intValue || right->intValue;
		break;
	case ScriptVar::kScriptTypeFloat:
		left->intValue = left->floatValue || right->floatValue;
		break;
	case ScriptVar::kScriptTypeString:
		left->intValue = left->stringValue.length() || right->stringValue.length();
		break;
	default:
		__EvalError(||);
	}
}

static Void Above(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case ScriptVar::kScriptTypeBool:
	case ScriptVar::kScriptTypeInt:
		left->intValue = left->intValue > right->intValue;
		break;
	case ScriptVar::kScriptTypeFloat:
		left->intValue = left->floatValue > right->floatValue;
		break;
	case ScriptVar::kScriptTypeString:
		left->intValue = left->stringValue > right->stringValue;
		break;
	default:
		__EvalError(>);
	}
}

static Void Bellow(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case ScriptVar::kScriptTypeBool:
	case ScriptVar::kScriptTypeInt:
		left->intValue = left->intValue < right->intValue;
		break;
	case ScriptVar::kScriptTypeFloat:
		left->intValue = left->floatValue < right->floatValue;
		break;
	case ScriptVar::kScriptTypeString:
		left->intValue = left->stringValue < right->stringValue;
		break;
	default:
		__EvalError(<);
	}
}

static Void AboveEqual(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case ScriptVar::kScriptTypeBool:
	case ScriptVar::kScriptTypeInt:
		left->intValue = left->intValue >= right->intValue;
		break;
	case ScriptVar::kScriptTypeFloat:
		left->intValue = left->floatValue >= right->floatValue;
		break;
	case ScriptVar::kScriptTypeString:
		left->intValue = left->stringValue >= right->stringValue;
		break;
	default:
		__EvalError(>=);
	}
}

static Void BellowEqual(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case ScriptVar::kScriptTypeBool:
	case ScriptVar::kScriptTypeInt:
		left->intValue = left->intValue <= right->intValue;
		break;
	case ScriptVar::kScriptTypeFloat:
		left->intValue = left->floatValue <= right->floatValue;
		break;
	case ScriptVar::kScriptTypeString:
		left->intValue = left->stringValue <= right->stringValue;
		break;
	default:
		__EvalError(<=);
	}
}

static Void Equal(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case ScriptVar::kScriptTypeBool:
	case ScriptVar::kScriptTypeInt:
		left->intValue = left->intValue == right->intValue;
		break;
	case ScriptVar::kScriptTypeFloat:
		left->intValue = left->floatValue == right->floatValue;
		break;
	case ScriptVar::kScriptTypeString:
		left->intValue = left->stringValue == right->stringValue;
		break;
	default:
		__EvalError(==);
	}
}

static Void NotEqual(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case ScriptVar::kScriptTypeBool:
	case ScriptVar::kScriptTypeInt:
		left->intValue = left->intValue != right->intValue;
		break;
	case ScriptVar::kScriptTypeFloat:
		left->intValue = left->floatValue != right->floatValue;
		break;
	case ScriptVar::kScriptTypeString:
		left->intValue = left->stringValue != right->stringValue;
		break;
	default:
		__EvalError(!=);
	}
}

static Void BitNot(ScriptVarPtr left) {
	switch (left->type) {
	case ScriptVar::kScriptTypeBool:
	case ScriptVar::kScriptTypeInt:
		left->intValue = ~left->intValue;
		break;
	default:
		__EvalError(~);
	}
}

static Void Not(ScriptVarPtr left) {
	switch (left->type) {
	case ScriptVar::kScriptTypeBool:
	case ScriptVar::kScriptTypeInt:
		left->intValue = !left->intValue;
		break;
	default:
		__EvalError(!);
	}
}

static Void Inc(ScriptVarPtr left) {
	switch (left->type) {
	case ScriptVar::kScriptTypeBool:
    case ScriptVar::kScriptTypeInt:
		++left->intValue;
		break;
	default:
		__EvalError(++);
	}
}

static Void Dec(ScriptVarPtr left) {
	switch (left->type) {
	case ScriptVar::kScriptTypeBool:
	case ScriptVar::kScriptTypeInt:
		--left->intValue;
		break;
	default:
		__EvalError(--);
	}
}

ScriptVar::ScriptVar(Void) {
	this->Reset();
}

ScriptVar::ScriptVar(ScriptNativeInt intValue) {
	this->intValue = intValue;
	this->flags |= kScriptFlagInt;
}

ScriptVar::ScriptVar(ScriptNativeFloat floatValue) {
	this->floatValue = floatValue;
	this->flags |= kScriptFlagFloat;
}

ScriptVar::ScriptVar(ScriptNativeBool intValue) {
	this->intValue = intValue;
	this->flags |= kScriptFlagInt;
	this->flags |= kScriptFlagBool;
}

ScriptVar::ScriptVar(ScriptNativeString stringValue) {
	this->stringValue = stringValue;
	this->flags |= kScriptFlagString;
}

ScriptVar::ScriptVar(ScriptNativeClass classValue) {
	this->classValue = classValue;
	this->flags |= kScriptFlagClass;
}

Void ScriptVar::Reset(Void) {
	this->intValue = 0;
	this->floatValue = 0.0f;
	this->stringValue.clear();
	this->classValue.reset();
	this->flags = 0;
	this->name.clear();
	this->callback = 0;
	this->type = kScriptTypeDefault;
    this->condition = 0;
    this->result = 0;
}

Void ScriptVar::Convert(EType& type) {
	if (type == this->type) {
		return;
	}
	if (type == kScriptTypeVoid || type == kScriptTypeFunction ||
		this->type == kScriptTypeVoid || this->type == kScriptTypeFunction
	) {
		goto __Error;
	}
	if (type == kScriptTypeVar) {
		type = this->type; return;
	}
	switch (type) {
	case ScriptVar::kScriptTypeInt:
		switch (this->type) {
		case ScriptVar::kScriptTypeFloat:
			this->intValue = (ScriptNativeInt)this->floatValue;
			goto __Warning;
		case ScriptVar::kScriptTypeBool:
            goto __Exit;
		case ScriptVar::kScriptTypeString:
			this->intValue = ParseIntValue(this->stringValue.data());
			goto __Exit;
		default:
			break;
		} break;
	case ScriptVar::kScriptTypeFloat:
		switch (this->type) {
		case ScriptVar::kScriptTypeInt:
			this->floatValue = (ScriptNativeFloat)this->intValue;
			goto __Warning;
		case ScriptVar::kScriptTypeBool:
			this->floatValue = (ScriptNativeFloat)this->intValue;
			goto __Warning;
		case ScriptVar::kScriptTypeString:
			this->floatValue = (ScriptNativeFloat)strtof(this->stringValue.data(), NULL);
			goto __Exit;
		default:
			break;
		} break;
	case ScriptVar::kScriptTypeBool:
		switch (this->type) {
		case ScriptVar::kScriptTypeInt:
            goto __Exit;
		case ScriptVar::kScriptTypeFloat:
			this->intValue = (ScriptNativeBool)this->floatValue;
			goto __Warning;
		case ScriptVar::kScriptTypeString:
			this->intValue = (ScriptNativeBool)strtol(this->stringValue.data(), NULL, 10);
			goto __Exit;
		default:
			break;
		} break;
	case ScriptVar::kScriptTypeString:
		switch (this->type) {
		case ScriptVar::kScriptTypeInt:
			this->stringValue.Format("%d", this->intValue);
			goto __Exit;
		case ScriptVar::kScriptTypeFloat:
			this->stringValue.Format("%.2f", this->floatValue);
			goto __Exit;
		case ScriptVar::kScriptTypeBool:
			this->stringValue.Format("%s", this->intValue ? "TRUE" : "FALSE");
			goto __Exit;
		default:
			break;
		} break;
	default:
		break;
	}
	PostSyntaxError(this->lex->line, "Invalid conversion from unknown type (%s)",
		this->name.data());
__Warning:
	PostSyntaxWarning(this->lex->line, "Unsafe type conversion from (%s) to (%s)",
		this->name.data(),
		this->name.data());
	goto __Exit;
__Error:
	PostSyntaxError(this->lex->line, "Invalid type conversion from (%s) to (%s)",
		this->name.data(),
		this->name.data());
	goto __Exit;
__Exit:
    this->type = type;
}

Void ScriptVar::Clone(ScriptVarPtr result) {

	result->intValue = this->intValue;
	result->floatValue = this->floatValue;
	result->stringValue = this->stringValue;
	result->classValue = this->classValue;
	result->flags = this->flags;
	result->type = this->type;
	result->callback = this->callback;
}

Void ScriptVar::Invoke(Vector<ScriptNodePtr>& parameters) {
	__asm int 3
}

Void ScriptVar::_EvaluateMath(ScriptVarPtr right, ScriptLexPtr lex) {
    
	switch (lex->object) {
	case kScriptObjectSet:
		Set(this, right);
		break;
	case kScriptObjectMul:
	case kScriptObjectMulSet:
		Mul(this, right);
		break;
	case kScriptObjectDiv:
	case kScriptObjectDivSet:
		Div(this, right);
		break;
	case kScriptObjectMod:
	case kScriptObjectModSet:
		Mod(this, right);
		break;
	case kScriptObjectAdd:
	case kScriptObjectAddSet:
		Add(this, right);
		break;
	case kScriptObjectSub:
	case kScriptObjectSubSet:
		Sub(this, right);
		break;
	case kScriptObjectBitShiftL:
	case kScriptObjectBitShiftSetL:
		BitShiftL(this, right);
		break;
	case kScriptObjectBitShiftR:
	case kScriptObjectBitShiftSetR:
		BitShiftR(this, right);
		break;
	case kScriptObjectBitAnd:
	case kScriptObjectBitAndSet:
		BitAnd(this, right);
		break;
	case kScriptObjectBitXor:
	case kScriptObjectBitXorSet:
		BitXor(this, right);
		break;
	case kScriptObjectBitOr:
	case kScriptObjectBitOrSet:
		BitOr(this, right);
		break;
	default:
        __debugbreak();
	}
}

Void ScriptVar::_EvaluateBool(ScriptVarPtr right, ScriptLexPtr lex) {

	switch (lex->object) {
	case kScriptObjectAnd:
		And(this, right);
		break;
	case kScriptObjectOr:
		Or(this, right);
		break;
	case kScriptObjectAbove:
		Above(this, right);
		break;
	case kScriptObjectAboveEqual:
		AboveEqual(this, right);
		break;
	case kScriptObjectBellow:
		Bellow(this, right);
		break;
	case kScriptObjectBellowEqual:
		BellowEqual(this, right);
		break;
	case kScriptObjectEqual:
		Equal(this, right);
		break;
	case kScriptObjectNotEqual:
		NotEqual(this, right);
		break;
	default:
        __debugbreak();
	}
}

ScriptVarPtr ScriptVar::_EvaluateDouble(ScriptNodePtr right, ScriptLexPtr lex) {

	ScriptNodePtr fieldNode;

	if (lex->object == kScriptObjectMediated ||
		lex->object == kScriptObjectDirect
	) {
		if (!this->classValue) {
			PostSyntaxError(lex->line, "Non-Initialized class object (%s)", this->name.data());
		}

		fieldNode = this->classValue->FindField(right->var->name);

		if (!fieldNode) {
			if (this->type == kScriptTypeClass) {
				fieldNode = right;
				fieldNode->var->MakeVar();
				this->classValue->RegisterField(fieldNode);
			}
			else {
				PostSyntaxError(right->var->lex->line, "Unable to find class field (%s)", right->var->name.data());
			}
		}

		fieldNode->var->MakeTemp();

		return fieldNode->var;
	}
	else {
		right->var->Convert(this->type);

		if (lex->IsMath()) {
			this->_EvaluateMath(right->var, lex);
		}
		else if (lex->IsBool()) {
			this->_EvaluateBool(right->var, lex);
		}
		else {
			PostSyntaxError(lex->line, "Non-Implemented operation with 2 arguments (%s)", lex->word.data());
		}
	}

	return this;
}

ScriptVarPtr ScriptVar::_EvaluateSingle(ScriptLexPtr lex) {
    
    switch (lex->object) {
        case kScriptObjectIncrement:
            Inc(this);
            break;
        case kScriptObjectDecrement:
            Dec(this);
            break;
        case kScriptObjectSizeof:
            break;
        case kScriptObjectBitNot:
            BitNot(this);
            break;
        case kScriptObjectNot:
            Not(this);
            break;
        case kScriptObjectUnaryMinus:
            break;
        case kScriptObjectUnaryPlus:
            break;
        case kScriptObjectNew:
            break;
        default:
            __debugbreak();
    }

	return this;
}

Bool ScriptVar::Boolean(Void) {

	switch (this->type) {
	case ScriptVar::kScriptTypeBool:
	case ScriptVar::kScriptTypeInt:
		return this->intValue != 0;
	case ScriptVar::kScriptTypeFloat:
		return this->floatValue != 0;
	case ScriptVar::kScriptTypeString:
		return this->stringValue.length() > 0;
	case ScriptVar::kScriptTypeClass:
		return (bool)this->classValue;
	case ScriptVar::kScriptTypeFunction:
		return LAME_FALSE;
	default:
		break;
	}
	return LAME_FALSE;
}

LAME_END