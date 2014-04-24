#include "Script.h"
#include "Internal.h"

LAME_BEGIN

#define __EvalError(_operation) \
	PostSyntaxError(0, "Unable to apply %s operation to this type", #_operation, left->name.data());

static Void Set(ScriptVarPtr left, ScriptVarPtr right) {
	Uint32 flags = left->flags;
	switch (left->type) {
	case kScriptTypeBool:
	case kScriptTypeInt:
		left->intValue = right->intValue;
		break;
	case kScriptTypeFloat:
		left->floatValue = right->floatValue;
		break;
	case kScriptTypeString:
		left->stringValue = right->stringValue;
		break;
	case kScriptTypeClass:
		if (left->typeName != right->typeName) {
			PostSyntaxError(left->line, "Unable to copy class with another type", 1);
		}
		left->classValue = right->classValue;
		break;
	default:
		__EvalError(=);
	}
	left->flags = flags;
}

static Void Add(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case kScriptTypeBool:
	case kScriptTypeInt:
		left->intValue += right->intValue;
		break;
	case kScriptTypeFloat:
		left->floatValue += right->floatValue;
		break;
	case kScriptTypeString:
		left->stringValue += right->stringValue;
		break;
	default:
		__EvalError(+);
	}
}

static Void Sub(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case kScriptTypeBool:
	case kScriptTypeInt:
		left->intValue -= right->intValue;
		break;
	case kScriptTypeFloat:
		left->floatValue -= right->floatValue;
		break;
	default:
		__EvalError(-);
	}
}

static Void Mul(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case kScriptTypeBool:
	case kScriptTypeInt:
		left->intValue *= right->intValue;
		break;
	case kScriptTypeFloat:
		left->floatValue *= right->floatValue;
		break;
	default:
		__EvalError(*);
	}
}

static Void Div(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case kScriptTypeBool:
	case kScriptTypeInt:
		left->intValue /= right->intValue;
		break;
	case kScriptTypeFloat:
		left->floatValue /= right->floatValue;
		break;
	default:
		__EvalError(/);
	}
}

static Void Mod(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case kScriptTypeBool:
	case kScriptTypeInt:
		left->intValue %= right->intValue;
		break;
	default:
		__EvalError(%);
	}
}

static Void BitAnd(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case kScriptTypeBool:
	case kScriptTypeInt:
		left->intValue &= right->intValue;
		break;
	default:
		__EvalError(&);
	}
}

static Void BitOr(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case kScriptTypeBool:
	case kScriptTypeInt:
		left->intValue |= right->intValue;
		break;
	default:
		__EvalError(|);
	}
}

static Void BitXor(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case kScriptTypeBool:
	case kScriptTypeInt:
		left->intValue ^= right->intValue;
		break;
	default:
		__EvalError(^);
	}
}

static Void BitShiftR(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case kScriptTypeBool:
	case kScriptTypeInt:
		left->intValue >>= right->intValue;
		break;
	default:
		__EvalError(>>);
	}
}

static Void BitShiftL(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case kScriptTypeBool:
	case kScriptTypeInt:
		left->intValue <<= right->intValue;
		break;
	default:
		__EvalError(<<);
	}
}

static Void And(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case kScriptTypeBool:
	case kScriptTypeInt:
		left->intValue = left->intValue && right->intValue;
		break;
	case kScriptTypeFloat:
		left->intValue = left->floatValue && right->floatValue;
		break;
	case kScriptTypeString:
		left->intValue = left->stringValue.length() && right->stringValue.length();
		break;
	default:
		__EvalError(&&);
	}
}

static Void Or(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case kScriptTypeBool:
	case kScriptTypeInt:
		left->intValue = left->intValue || right->intValue;
		break;
	case kScriptTypeFloat:
		left->intValue = left->floatValue || right->floatValue;
		break;
	case kScriptTypeString:
		left->intValue = left->stringValue.length() || right->stringValue.length();
		break;
	default:
		__EvalError(||);
	}
}

static Void Above(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case kScriptTypeBool:
	case kScriptTypeInt:
		left->intValue = left->intValue > right->intValue;
		break;
	case kScriptTypeFloat:
		left->intValue = left->floatValue > right->floatValue;
		break;
	case kScriptTypeString:
		left->intValue = left->stringValue > right->stringValue;
		break;
	default:
		__EvalError(>);
	}
}

static Void Bellow(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case kScriptTypeBool:
	case kScriptTypeInt:
		left->intValue = left->intValue < right->intValue;
		break;
	case kScriptTypeFloat:
		left->intValue = left->floatValue < right->floatValue;
		break;
	case kScriptTypeString:
		left->intValue = left->stringValue < right->stringValue;
		break;
	default:
		__EvalError(<);
	}
}

static Void AboveEqual(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case kScriptTypeBool:
	case kScriptTypeInt:
		left->intValue = left->intValue >= right->intValue;
		break;
	case kScriptTypeFloat:
		left->intValue = left->floatValue >= right->floatValue;
		break;
	case kScriptTypeString:
		left->intValue = left->stringValue >= right->stringValue;
		break;
	default:
		__EvalError(>=);
	}
}

static Void BellowEqual(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case kScriptTypeBool:
	case kScriptTypeInt:
		left->intValue = left->intValue <= right->intValue;
		break;
	case kScriptTypeFloat:
		left->intValue = left->floatValue <= right->floatValue;
		break;
	case kScriptTypeString:
		left->intValue = left->stringValue <= right->stringValue;
		break;
	default:
		__EvalError(<=);
	}
}

static Void Equal(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case kScriptTypeBool:
	case kScriptTypeInt:
		left->intValue = left->intValue == right->intValue;
		break;
	case kScriptTypeFloat:
		left->intValue = left->floatValue == right->floatValue;
		break;
	case kScriptTypeString:
		left->intValue = left->stringValue == right->stringValue;
		break;
	case kScriptTypeAuto:
		left->intValue = left->type == right->type;
		break;
	default:
		__EvalError(==);
	}
}

static Void NotEqual(ScriptVarPtr left, ScriptVarPtr right) {
	switch (left->type) {
	case kScriptTypeBool:
	case kScriptTypeInt:
		left->intValue = left->intValue != right->intValue;
		break;
	case kScriptTypeFloat:
		left->intValue = left->floatValue != right->floatValue;
		break;
	case kScriptTypeString:
		left->intValue = left->stringValue != right->stringValue;
		break;
	default:
		__EvalError(!=);
	}
}

static Void BitNot(ScriptVarPtr left) {
	switch (left->type) {
	case kScriptTypeBool:
	case kScriptTypeInt:
		left->intValue = ~left->intValue;
		break;
	default:
		__EvalError(~);
	}
}

static Void Not(ScriptVarPtr left) {
	switch (left->type) {
	case kScriptTypeBool:
	case kScriptTypeInt:
		left->intValue = !left->intValue;
		break;
	default:
		__EvalError(!);
	}
}

static Void Inc(ScriptVarPtr left) {
	switch (left->type) {
	case kScriptTypeBool:
    case kScriptTypeInt:
		++left->intValue;
		break;
	default:
		__EvalError(++);
	}
}

static Void Dec(ScriptVarPtr left) {
	switch (left->type) {
	case kScriptTypeBool:
	case kScriptTypeInt:
		--left->intValue;
		break;
	default:
		__EvalError(--);
	}
}

Void ScriptVar::Reset(Void) {

	this->intValue = 0;
	this->floatValue = 0.0f;
	this->stringValue.clear();
	this->classValue.reset();
	this->flags = 0;
	this->callback = 0;
}

Void ScriptVar::Convert(ScriptVarPtr var) {
	if (var->type == this->type) {
		return;
	}
	if (var->type == kScriptTypeVoid || var->type == kScriptTypeFunction ||
		this->type == kScriptTypeVoid || this->type == kScriptTypeFunction
	) {
		goto __Error;
	}
	if (var->type == kScriptTypeAuto) {
		var->type = this->type;
		var->typeName = this->typeName;
		return;
	}
	switch (var->type) {
	case kScriptTypeInt:
		switch (this->type) {
		case kScriptTypeFloat:
			this->intValue = (ScriptNativeInt)this->floatValue;
			goto __Warning;
		case kScriptTypeBool:
            goto __Exit;
		case kScriptTypeString:
			this->intValue = ParseIntValue(this->stringValue.data());
			goto __Exit;
		case kScriptTypeAuto:
			this->intValue = LAME_FALSE;
			goto __Exit;
		default:
			break;
		} break;
	case kScriptTypeFloat:
		switch (this->type) {
		case kScriptTypeInt:
		case kScriptTypeBool:
			this->floatValue = (ScriptNativeFloat)this->intValue;
			goto __Warning;
		case kScriptTypeString:
			this->floatValue = (ScriptNativeFloat)strtof(this->stringValue.data(), NULL);
			goto __Exit;
		case kScriptTypeAuto:
			this->intValue = LAME_FALSE;
			goto __Exit;
		default:
			break;
		} break;
	case kScriptTypeBool:
		switch (this->type) {
		case kScriptTypeInt:
            goto __Exit;
		case kScriptTypeFloat:
			this->intValue = (ScriptNativeBool)this->floatValue;
			goto __Warning;
		case kScriptTypeString:
			this->intValue = (ScriptNativeBool)strtol(this->stringValue.data(), NULL, 10);
			goto __Exit;
		case kScriptTypeAuto:
			this->intValue = LAME_FALSE;
			goto __Exit;
		default:
			break;
		} break;
	case kScriptTypeString:
		switch (this->type) {
		case kScriptTypeInt:
			this->stringValue.Format("%lld", this->intValue);
			goto __Exit;
		case kScriptTypeFloat:
			this->stringValue.Format("%.2f", this->floatValue);
			goto __Exit;
		case kScriptTypeBool:
			this->stringValue.Format("%s", this->intValue ? "True" : "False");
			goto __Exit;
		default:
			break;
		} break;
	default:
		break;
	}
	PostSyntaxError(this->line, "Invalid conversion from unknown type (%s)",
		this->typeName.data());
__Warning:
	PostSyntaxWarning(this->line, "Unsafe type conversion from (%s) to (%s)",
		this->typeName.data(),
		var->typeName.data());
	goto __Exit;
__Error:
	PostSyntaxError(this->line, "Invalid type conversion from (%s) to (%s)",
		this->typeName.data(),
		var->typeName.data());
	goto __Exit;
__Exit:
	this->type = var->type;
	this->typeName = var->typeName;
}

Void ScriptVar::Clone(ScriptVarPtr result) {

	result->intValue = this->intValue;
	result->floatValue = this->floatValue;
	result->stringValue = this->stringValue;
	result->classValue = this->classValue;
	//result->flags = this->flags;
	result->type = this->type;
	result->callback = this->callback;
}

Void ScriptVar::_EvaluateMath(ScriptVarPtr right, ScriptLexID id) {
    
	switch (id) {
	case kScriptLexSet:
		Set(this, right);
		break;
	case kScriptLexMul:
	case kScriptLexMulSet:
		Mul(this, right);
		break;
	case kScriptLexDiv:
	case kScriptLexDivSet:
		Div(this, right);
		break;
	case kScriptLexMod:
	case kScriptLexModSet:
		Mod(this, right);
		break;
	case kScriptLexAdd:
	case kScriptLexAddSet:
		Add(this, right);
		break;
	case kScriptLexSub:
	case kScriptLexSubSet:
		Sub(this, right);
		break;
	case kScriptLexBitShiftL:
	case kScriptLexBitShiftSetL:
		BitShiftL(this, right);
		break;
	case kScriptLexBitShiftR:
	case kScriptLexBitShiftSetR:
		BitShiftR(this, right);
		break;
	case kScriptLexBitAnd:
	case kScriptLexBitAndSet:
		BitAnd(this, right);
		break;
	case kScriptLexBitXor:
	case kScriptLexBitXorSet:
		BitXor(this, right);
		break;
	case kScriptLexBitOr:
	case kScriptLexBitOrSet:
		BitOr(this, right);
		break;
	default:
        __debugbreak();
	}
}

Void ScriptVar::_EvaluateBool(ScriptVarPtr right, ScriptLexID id) {

	switch (id) {
	case kScriptLexAnd:
		And(this, right);
		break;
	case kScriptLexOr:
		Or(this, right);
		break;
	case kScriptLexAbove:
		Above(this, right);
		break;
	case kScriptLexAboveEqual:
		AboveEqual(this, right);
		break;
	case kScriptLexBellow:
		Bellow(this, right);
		break;
	case kScriptLexBellowEqual:
		BellowEqual(this, right);
		break;
	case kScriptLexEqual:
		Equal(this, right);
		break;
	case kScriptLexNotEqual:
		NotEqual(this, right);
		break;
	default:
        __debugbreak();
	}

	this->type = kScriptTypeBool;
}

ScriptNodePtr ScriptVar::_EvaluateDouble(ScriptNodePtr right, ScriptLexPtr lex) {

	ScriptNodePtr fieldNode = LAME_NULL;

	if (lex->id == kScriptLexMediated ||
		lex->id == kScriptLexDirected
	) {
		if (!this->classValue) {
			PostSyntaxError(lex->line, "Non-Initialized class object (%s)", this->name.data());
		}

		fieldNode = this->classValue->FindChild(right->word);

		if (!fieldNode) {
			PostSyntaxError(lex->line, "Unable to find class field (%s)", right->word.data());
		}

		if (this->IsType() && !fieldNode->var->IsStatic()) {
			PostSyntaxError(lex->line, "You can't address to non-static members with type (%s)", this->typeName.data());
		}

		fieldNode->var->MakeTemp();

		if (this->IsFinal()) {
			fieldNode->var->MakeFinal();
		}

		return fieldNode;
	}
	else {
		if (lex->IsMath()) {
			this->_EvaluateMath(right->var, lex->id);
		}
		else if (lex->IsBool()) {
			this->_EvaluateBool(right->var, lex->id);
		}
		else {
			PostSyntaxError(lex->line, "Non-Implemented operation with 2 arguments (%s)", lex->word.data());
		}
	}

	return fieldNode;
}

ScriptVarPtr ScriptVar::_EvaluateSingle(ScriptLexPtr lex) {
    
    switch (lex->id) {
    case kScriptLexIncrement:
        Inc(this);
        break;
	case kScriptLexDecrement:
        Dec(this);
        break;
	case kScriptLexSizeof:
        break;
	case kScriptLexBitNot:
        BitNot(this);
        break;
	case kScriptLexNot:
        Not(this);
        break;
	case kScriptLexUnaryMinus:
        break;
	case kScriptLexUnaryPlus:
        break;
	case kScriptLexNew:
    case kScriptLexReturn:
        break;
    default:
        __debugbreak();
    }

	return this;
}

Bool ScriptVar::Boolean(Void) {

	switch (this->type) {
	case kScriptTypeBool:
	case kScriptTypeInt:
		return this->intValue != 0;
	case kScriptTypeFloat:
		return this->floatValue != 0;
	case kScriptTypeString:
		return this->stringValue.length() > 0;
	case kScriptTypeClass:
		return (Bool)this->classValue;
	case kScriptTypeFunction:
		return LAME_FALSE;
	default:
		break;
	}
	return LAME_FALSE;
}

LAME_END