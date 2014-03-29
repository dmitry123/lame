#include "Script.h"

LAME_BEGIN

Void ScriptVariable::Set(const ScriptVariable& value) {
    switch (this->type) {
        case kScriptTypeBool: this->boolValue = value.boolValue; break;
        case kScriptTypeFloat: this->floatValue = value.floatValue; break;
        case kScriptTypeInt: this->intValue = value.intValue; break;
        case kScriptTypeString: this->stringValue = value.stringValue; break;
        default: goto __Error;
    }
    return;
__Error:
    PostSyntaxError(this->line, "Unable to apply = operation to this type", this->type.String());
}

Void ScriptVariable::Add(const ScriptVariable& value) {
    switch (this->type) {
        case kScriptTypeBool: this->boolValue += value.boolValue; break;
        case kScriptTypeFloat: this->floatValue += value.floatValue; break;
        case kScriptTypeInt: this->intValue += value.intValue; break;
        case kScriptTypeString: this->stringValue += value.stringValue; break;
        default: goto __Error;
    }
    return;
__Error:
    PostSyntaxError(this->line, "Unable to apply + operation to this type", this->type.String());
}

Void ScriptVariable::Sub(const ScriptVariable& value) {
    switch (this->type) {
        case kScriptTypeBool: this->boolValue -= value.boolValue; break;
        case kScriptTypeFloat: this->floatValue -= value.floatValue; break;
        case kScriptTypeInt: this->intValue -= value.intValue; break;
        default: goto __Error;
    }
    return;
__Error:
    PostSyntaxError(this->line, "Unable to apply - operation to this type", this->type.String());
}

Void ScriptVariable::Mul(const ScriptVariable& value) {
    switch (this->type) {
        case kScriptTypeBool: this->boolValue *= value.boolValue; break;
        case kScriptTypeFloat: this->floatValue *= value.floatValue; break;
        case kScriptTypeInt: this->intValue *= value.intValue; break;
        default: goto __Error;
    }
    return;
__Error:
    PostSyntaxError(this->line, "Unable to apply * operation to this type", this->type.String());
}

Void ScriptVariable::Div(const ScriptVariable& value) {
    switch (this->type) {
        case kScriptTypeBool: this->boolValue /= value.boolValue; break;
        case kScriptTypeFloat: this->floatValue /= value.floatValue; break;
        case kScriptTypeInt: this->intValue /= value.intValue; break;
        default: goto __Error;
    }
    return;
__Error:
    PostSyntaxError(this->line, "Unable to apply / operation to this type", this->type.String());
}

Void ScriptVariable::Mod(const ScriptVariable& value) {
    switch (this->type) {
        case kScriptTypeBool: this->boolValue %= value.boolValue; break;
        case kScriptTypeInt: this->intValue %= value.intValue; break;
        default: goto __Error;
    }
    return;
__Error:
    PostSyntaxError(this->line, "Unable to apply % operation to this type", this->type.String());
}

Void ScriptVariable::BitAnd(const ScriptVariable& value) {
    switch (this->type) {
        case kScriptTypeBool: this->boolValue &= value.boolValue; break;
        case kScriptTypeInt: this->intValue &= value.intValue; break;
        default: goto __Error;
    }
    return;
__Error:
    PostSyntaxError(this->line, "Unable to apply & operation to this type", this->type.String());
}

Void ScriptVariable::BitOr(const ScriptVariable& value) {
    switch (this->type) {
        case kScriptTypeBool: this->boolValue |= value.boolValue; break;
        case kScriptTypeInt: this->intValue |= value.intValue; break;
        default: goto __Error;
    }
    return;
__Error:
    PostSyntaxError(this->line, "Unable to apply | operation to this type", this->type.String());
}

Void ScriptVariable::BitXor(const ScriptVariable& value) {
    switch (this->type) {
        case kScriptTypeBool: this->boolValue ^= value.boolValue; break;
        case kScriptTypeInt: this->intValue ^= value.intValue; break;
        default: goto __Error;
    }
    return;
__Error:
    PostSyntaxError(this->line, "Unable to apply ^ operation to this type", this->type.String());
}

Void ScriptVariable::BitShiftL(const ScriptVariable& value) {
    switch (this->type) {
        case kScriptTypeBool: this->boolValue <<= value.boolValue; break;
        case kScriptTypeInt: this->intValue <<= value.intValue; break;
        default: goto __Error;
    }
    return;
__Error:
    PostSyntaxError(this->line, "Unable to apply << operation for (%s) type", this->type.String());
}

Void ScriptVariable::BitShiftR(const ScriptVariable& value) {
    switch (this->type) {
        case kScriptTypeBool: this->boolValue >>= value.boolValue; break;
        case kScriptTypeInt: this->intValue >>= value.intValue; break;
        default: goto __Error;
    }
    return;
__Error:
    PostSyntaxError(this->line, "Unable to apply >> operation to this type", this->type.String());
}

Void ScriptVariable::Above(const ScriptVariable& value) {
    switch (this->type) {
        case kScriptTypeBool: this->boolValue = this->boolValue > value.boolValue; break;
        case kScriptTypeFloat: this->boolValue = this->floatValue > value.floatValue; break;
        case kScriptTypeInt: this->boolValue = this->intValue > value.intValue; break;
        case kScriptTypeString: this->boolValue = this->stringValue > value.stringValue; break;
        default: goto __Error;
    }
    this->type = kScriptTypeBool;
    return;
__Error:
    PostSyntaxError(this->line, "Unable to apply > operation to this type", this->type.String());
}

Void ScriptVariable::Bellow(const ScriptVariable& value) {
    switch (this->type) {
        case kScriptTypeBool: this->boolValue = this->boolValue < value.boolValue; break;
        case kScriptTypeFloat: this->boolValue = this->floatValue < value.floatValue; break;
        case kScriptTypeInt: this->boolValue = this->intValue < value.intValue; break;
        case kScriptTypeString: this->boolValue = this->stringValue < value.stringValue; break;
        default: goto __Error;
    }
    this->type = kScriptTypeBool;
    return;
__Error:
    PostSyntaxError(this->line, "Unable to apply < operation to this type", this->type.String());
}

Void ScriptVariable::AboveEqual(const ScriptVariable& value) {
    switch (this->type) {
        case kScriptTypeBool: this->boolValue = this->boolValue >= value.boolValue; break;
        case kScriptTypeFloat: this->boolValue = this->floatValue >= value.floatValue; break;
        case kScriptTypeInt: this->boolValue = this->intValue >= value.intValue; break;
        case kScriptTypeString: this->boolValue = this->stringValue >= value.stringValue; break;
        default: goto __Error;
    }
    this->type = kScriptTypeBool;
    return;
__Error:
    PostSyntaxError(this->line, "Unable to apply >= operation to this type", this->type.String());
}

Void ScriptVariable::BellowEqual(const ScriptVariable& value) {
    switch (this->type) {
        case kScriptTypeBool: this->boolValue = this->boolValue <= value.boolValue; break;
        case kScriptTypeFloat: this->boolValue = this->floatValue <= value.floatValue; break;
        case kScriptTypeInt: this->boolValue = this->intValue <= value.intValue; break;
        case kScriptTypeString: this->boolValue = this->stringValue <= value.stringValue; break;
        default: goto __Error;
    }
    this->type = kScriptTypeBool;
    return;
__Error:
    PostSyntaxError(this->line, "Unable to apply <= operation to this type", this->type.String());
}

Void ScriptVariable::Equal(const ScriptVariable& value) {
    switch (this->type) {
        case kScriptTypeBool: this->boolValue = this->boolValue == value.boolValue; break;
        case kScriptTypeFloat: this->boolValue = this->floatValue == value.floatValue; break;
        case kScriptTypeInt: this->boolValue = this->intValue == value.intValue; break;
        case kScriptTypeString: this->boolValue = this->stringValue == value.stringValue; break;
        default: goto __Error;
    }
    this->type = kScriptTypeBool;
    return;
__Error:
    PostSyntaxError(this->line, "Unable to apply == operation to this type", this->type.String());
}

Void ScriptVariable::NotEqual(const ScriptVariable& value) {
    switch (this->type) {
        case kScriptTypeBool: this->boolValue = this->boolValue != value.boolValue; break;
        case kScriptTypeFloat: this->boolValue = this->floatValue != value.floatValue; break;
        case kScriptTypeInt: this->boolValue = this->intValue != value.intValue; break;
        case kScriptTypeString: this->boolValue = this->stringValue != value.stringValue; break;
        default: goto __Error;
    }
    this->type = kScriptTypeBool;
    return;
__Error:
    PostSyntaxError(this->line, "Unable to apply != operation to this type", this->type.String());
}

Void ScriptVariable::BitNot() {
    switch (this->type) {
        case kScriptTypeBool: this->boolValue = ~this->boolValue; break;
        case kScriptTypeInt: this->intValue = ~this->intValue; break;
        default: goto __Error;
    }
    this->type = kScriptTypeBool;
    return;
__Error:
    PostSyntaxError(this->line, "Unable to apply ~ operation to this type", this->type.String());
}

Void ScriptVariable::Not() {
    switch (this->type) {
        case kScriptTypeBool: this->boolValue = !this->boolValue; break;
        case kScriptTypeFloat: this->floatValue = !this->floatValue; break;
        case kScriptTypeInt: this->intValue = !this->intValue; break;
        default: goto __Error;
    }
    this->type = kScriptTypeBool;
    return;
__Error:
    PostSyntaxError(this->line, "Unable to apply ! operation to this type", this->type.String());
}

Void ScriptVariable::Inc() {
    switch (this->type) {
        case kScriptTypeBool: ++this->boolValue; break;
        case kScriptTypeFloat: ++this->floatValue; break;
        case kScriptTypeInt: ++this->intValue; break;
        default: goto __Error;
    }
    this->type = kScriptTypeBool;
    return;
__Error:
    PostSyntaxError(this->line, "Unable to apply ++ operation to this type", this->type.String());
}

Void ScriptVariable::Dec() {
    switch (this->type) {
        case kScriptTypeBool: --this->boolValue; break;
        case kScriptTypeFloat: --this->floatValue; break;
        case kScriptTypeInt: --this->intValue; break;
        default: goto __Error;
    }
    this->type = kScriptTypeBool;
    return;
__Error:
    PostSyntaxError(this->line, "Unable to apply -- operation to this type", this->type.String());
}

Void ScriptVariable::ToBool() {
    
    switch (this->type) {
        case kScriptTypeBool:
            break;
        case kScriptTypeFloat:
            this->boolValue = this->floatValue != 0; break;
        case kScriptTypeInt:
            this->boolValue = this->intValue != 0; break;
        case kScriptTypeString:
            this->boolValue = this->stringValue.length() != 0; break;
        default:
            this->boolValue = 0; break;
    }
}

Bool ScriptVariable::Check(
	const ScriptVariable& left,
	const ScriptVariable& right
) {
	if (left.type == kScriptTypeVoid ||
		left.type == kScriptTypeVoid
	) {
		PostSyntaxError(left.line, "Type 'Void' isn't left associated and can't store anything", 1);
	}

	switch (left.type) {
		case kScriptTypeBool:
			switch (right.type) {
				case kScriptTypeBool: goto __Ok;
				case kScriptTypeFloat: goto __Warning;
				case kScriptTypeInt: goto __Warning;
				case kScriptTypeString: goto __Error;
			} break;
		case kScriptTypeFloat:
			switch (right.type) {
				case kScriptTypeBool: goto __Warning;
				case kScriptTypeFloat: goto __Ok;
				case kScriptTypeInt: goto __Warning;
				case kScriptTypeString: goto __Error;
			} break;
		case kScriptTypeInt:
			switch (right.type) {
				case kScriptTypeBool: goto __Warning;
				case kScriptTypeFloat: goto __Warning;
				case kScriptTypeInt: goto __Ok;
				case kScriptTypeString: goto __Error;
			} break;
		case kScriptTypeString:
			switch (right.type) {
				case kScriptTypeBool: goto __Error;
				case kScriptTypeFloat: goto __Error;
				case kScriptTypeInt: goto __Error;
				case kScriptTypeString: goto __Ok;
			} break;
	}

__Ok:
	return LAME_TRUE;
__Warning:
	PostSyntaxWarning(left.line, "Unsave conversion from (%s) to (%s)",
		ScriptType(right.type).String(),
		ScriptType(left.type).String());
	return LAME_TRUE;
__Error:
	PostSyntaxError(left.line, "Invalid conversion from (%s) to (%s)",
		ScriptType(right.type).String(),
		ScriptType(left.type).String());
	return LAME_FALSE;
}

Void ScriptVariable::Convert(
	      ScriptVariable& left,
	const ScriptVariable& right
) {
	if (left.type == kScriptTypeDefault) {
		left.boolValue = right.boolValue;
		left.floatValue = right.floatValue;
		left.intValue = right.intValue;
		left.stringValue = right.stringValue;
		left.type = right.type;
	}
	switch (right.type) {
		case kScriptTypeBool:
			switch (left.type) {
				case kScriptTypeFloat:
					left.boolValue = (ScriptNativeBool)right.floatValue; break;
				case kScriptTypeInt:
					left.boolValue = (ScriptNativeBool)right.intValue; break;
			} break;
		case kScriptTypeFloat:
			switch (left.type) {
				case kScriptTypeBool:
					left.floatValue = (ScriptNativeFloat)right.boolValue; break;
				case kScriptTypeInt:
					left.floatValue = (ScriptNativeFloat)right.intValue; break;
			} break;
		case kScriptTypeInt:
			switch (left.type) {
				case kScriptTypeBool:
					left.intValue = (ScriptNativeInt)right.boolValue; break;
				case kScriptTypeFloat:
					left.intValue = (ScriptNativeInt)right.floatValue; break;
			} break;
	}
	left.type = right.type;
}

LAME_END