#include "Script.h"

LAME_BEGIN

Void ScriptVariable::Convert(
	      ScriptVariable& left,
	const ScriptVariable& right
) {
	if (left.type == kScriptTypeVoid ||
		left.type == kScriptTypeFunction ||
		left.type == kScriptTypeObject ||
		right.type == kScriptTypeVoid ||
		right.type == kScriptTypeFunction ||
		right.type == kScriptTypeObject
	) {
		goto __Error;
	}
	switch (left.type) {
		case kScriptTypeInt:
			switch (right.type) {
				case kScriptTypeInt:
					left.intValue = right.intValue;
					goto __Exit;
				case kScriptTypeFloat:
					left.intValue = (ScriptNativeInt)right.floatValue;
					goto __Warning;
				case kScriptTypeBool:
					left.intValue = (ScriptNativeInt)right.boolValue;
					goto __Warning;
				case kScriptTypeString:
					left.intValue = (ScriptNativeInt)strtol(right.stringValue.data(), NULL, 10);
					goto __Exit;
                default:
                    break;
			} break;
		case kScriptTypeFloat:
			switch (right.type) {
				case kScriptTypeInt:
					left.floatValue = (ScriptNativeFloat)right.intValue;
					goto __Warning;
				case kScriptTypeFloat:
					goto __Exit;
				case kScriptTypeBool:
					left.floatValue = (ScriptNativeFloat)right.boolValue;
					goto __Warning;
				case kScriptTypeString:
					left.floatValue = (ScriptNativeFloat)strtof(right.stringValue.data(), NULL);
					goto __Exit;
                default:
                    break;
			} break;
		case kScriptTypeBool:
			switch (right.type) {
				case kScriptTypeInt:
					left.boolValue = (ScriptNativeBool)right.intValue;
					goto __Warning;
				case kScriptTypeFloat:
					left.boolValue = (ScriptNativeBool)right.floatValue;
					goto __Warning;
				case kScriptTypeBool:
					goto __Exit;
				case kScriptTypeString:
					left.boolValue = (ScriptNativeBool)strtol(right.stringValue.data(), NULL, 10);
					goto __Exit;
                default:
                    break;
			} break;
		case kScriptTypeString:
			switch (right.type) {
				case kScriptTypeInt:
					left.stringValue.Format("%d", right.intValue);
					goto __Exit;
				case kScriptTypeFloat:
					left.stringValue.Format("%.2f", right.floatValue);
					goto __Exit;
				case kScriptTypeBool:
					left.stringValue.Format("%s", right.boolValue ? "TRUE" : "FALSE");
					goto __Exit;
				case kScriptTypeString:
					goto __Exit;
                default:
                    break;
			} break;
        default:
            break;
	}
	PostSyntaxError(left.object->line, "Invalid conversion from unknown type (%s)", left.type.GetString());
__Warning:
	PostSyntaxWarning(left.object->line, "Unsafe type conversion from (%s) to (%s)",
		left.type.GetString(),
		right.type.GetString());
	goto __Exit;
__Error:
	PostSyntaxError(left.object->line, "Invalid type conversion from (%s) to (%s)",
		left.type.GetString(),
		right.type.GetString());
	goto __Exit;
__Exit:;
	left.type = right.type;
}

ScriptVariable& ScriptVariable::Reset() {
    
    this->intValue = 0;
    this->floatValue = 0.0f;
    this->boolValue = 0;
    this->stringValue.clear();
    
    return *this;
}

Bool ScriptVariableManager::Declare(ScriptVariable variable) {

	if (!this->varMap.count(variable.object->word)) {
		return LAME_FALSE;
	}

	this->varMap[variable.object->word] = variable;

	return LAME_TRUE;
}

ScriptVariablePtr ScriptVariableManager::Find(StringC name) {

	if (!this->varMap.count(name)) {
		return LAME_NULL;
	}

	return &this->varMap[name];
}

LAME_END