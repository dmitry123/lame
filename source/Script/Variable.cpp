#include "Variable.h"

LAME_BEGIN

#define StringType(_type) \
	ScriptType(_type).String()

#define __APPLY_ADD_2(_operator) \
	Void ScriptVariable::operator _operator (const ScriptVariable& value) { \
		Check(*this, value); Convert(*this, value); \
		switch (this->type) { \
			case kScriptTypeBool: this->boolValue _operator= value.boolValue; break; \
			case kScriptTypeFloat: this->floatValue _operator= value.floatValue; break; \
			case kScriptTypeInt: this->intValue _operator= value.intValue; break; \
			case kScriptTypeString: this->stringValue _operator= value.stringValue; break; \
		} \
	}

#define __APPLY_MATH_2(_operator) \
	Void ScriptVariable::operator _operator (const ScriptVariable& value) { \
		Check(*this, value); Convert(*this, value); \
		switch (this->type) { \
			case kScriptTypeBool: this->boolValue _operator= value.boolValue; break; \
			case kScriptTypeFloat: this->floatValue _operator= value.floatValue; break; \
			case kScriptTypeInt: this->intValue _operator= value.intValue; break; \
		} \
	}

#define __APPLY_MATH_1(_operator) \
	Void ScriptVariable::operator _operator (Void) { \
		switch (this->type) { \
			case kScriptTypeBool: this->boolValue =_operator this->boolValue; break; \
			case kScriptTypeFloat: this->floatValue =_operator this->floatValue; break; \
			case kScriptTypeInt: this->intValue =_operator this->intValue; break; \
			default: PostSyntaxError(this->line, "Unable to evaluate (%s) with (%s) type", #_operator, StringType(this->type)); \
		} \
	}

#define __APPLY_LOGIC_1(_operator) \
	Void ScriptVariable::operator _operator (Void) { \
		switch (this->type) { \
			case kScriptTypeBool: this->boolValue =_operator this->boolValue; break; \
			case kScriptTypeInt: this->intValue =_operator this->intValue; break; \
			default: PostSyntaxError(this->line, "Unable to evaluate (%s) with (%s) type", #_operator, StringType(this->type)); \
		} \
	}

#define __APPLY_LOGIC_2(_operator) \
	Void ScriptVariable::operator _operator (const ScriptVariable& value) { \
		Check(*this, value); Convert(*this, value); \
		switch (this->type) { \
			case kScriptTypeBool: this->boolValue _operator= value.boolValue; break; \
			case kScriptTypeInt: this->intValue _operator= value.intValue; break; \
			case kScriptTypeFloat: \
			case kScriptTypeString: \
				PostSyntaxError(this->line, "Unable to evaluate (%s) with (%s) and (%s) types", #_operator, \
					StringType(this->type), \
					StringType(value.type)); \
				break; \
		} \
	}

#define __APPLY_COMPARE_2(_operator) \
	Void ScriptVariable::operator _operator (const ScriptVariable& value) { \
		Check(*this, value); Convert(*this, value); \
		switch (this->type) { \
			case kScriptTypeBool: this->boolValue = this->boolValue _operator value.boolValue; break; \
			case kScriptTypeFloat: this->boolValue = this->floatValue _operator value.floatValue; break; \
			case kScriptTypeInt: this->boolValue = this->intValue _operator value.intValue; break; \
			case kScriptTypeString: this->boolValue = this->stringValue _operator value.stringValue; break; \
		} \
		this->type = kScriptTypeBool; \
	}

inline Void ScriptVariable::operator = (const ScriptVariable& value) {
	Check(*this, value); Convert(*this, value);
}

// Operator '+' for Bool/Int/Float/String
__APPLY_ADD_2(+);

// Basic Math Operators for Bool/Int/Float
__APPLY_MATH_2(-);
__APPLY_MATH_2(*);
__APPLY_MATH_2(/);

// Operator '!' for Bool/Int/Float
__APPLY_MATH_1(!);

// Operator '~' for Bool/Int
__APPLY_LOGIC_1(~);

// Basic Logic Operators for Bool/Int
__APPLY_LOGIC_2(%);
__APPLY_LOGIC_2(&);
__APPLY_LOGIC_2(|);
__APPLY_LOGIC_2(^);
__APPLY_LOGIC_2(<<);
__APPLY_LOGIC_2(>>);

// Basic Compare Operators for Bool/Int/Float/String
__APPLY_COMPARE_2(<);
__APPLY_COMPARE_2(>);
__APPLY_COMPARE_2(<=);
__APPLY_COMPARE_2(>=);
__APPLY_COMPARE_2(==);
__APPLY_COMPARE_2(!=);

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
	switch (left.type) {
		case kScriptTypeBool:
			switch (right.type) {
				case kScriptTypeFloat:
					left.boolValue = (ScriptNativeBool)right.floatValue; break;
				case kScriptTypeInt:
					left.boolValue = (ScriptNativeBool)right.intValue; break;
			} break;
		case kScriptTypeFloat:
			switch (right.type) {
				case kScriptTypeBool:
					left.floatValue = (ScriptNativeFloat)right.boolValue; break;
				case kScriptTypeInt:
					left.floatValue = (ScriptNativeFloat)right.intValue; break;
			} break;
		case kScriptTypeInt:
			switch (right.type) {
				case kScriptTypeBool:
					left.intValue = (ScriptNativeInt)right.boolValue; break;
				case kScriptTypeFloat:
					left.intValue = (ScriptNativeInt)right.floatValue; break;
			} break;
	}
}

LAME_END