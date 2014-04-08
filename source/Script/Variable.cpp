#include "Script.h"
#include "Internal.h"

LAME_BEGIN

Void ScriptVariable::Convert(
	      ScriptVariable& left,
	const ScriptVariable& right
) {
	if (left.type == right.type) {
		return;
	}
	if (left.type == kScriptTypeVoid ||
		left.type == kScriptTypeFunction ||
		left.type == kScriptTypeObject ||
		right.type == kScriptTypeVoid ||
		right.type == kScriptTypeFunction ||
		right.type == kScriptTypeObject
	) {
		goto __Error;
	}
	switch (right.type) {
		case kScriptTypeInt:
			switch (left.type) {
				case kScriptTypeInt:
					left.intValue = left.intValue;
					goto __Exit;
				case kScriptTypeFloat:
					left.intValue = (ScriptNativeInt)left.floatValue;
					goto __Warning;
				case kScriptTypeBool:
					left.intValue = (ScriptNativeInt)left.boolValue;
					goto __Warning;
				case kScriptTypeString:
					left.intValue = ParseIntValue(left.stringValue.data());
					goto __Exit;
                default:
                    break;
			} break;
		case kScriptTypeFloat:
			switch (left.type) {
				case kScriptTypeInt:
					left.floatValue = (ScriptNativeFloat)left.intValue;
					goto __Warning;
				case kScriptTypeFloat:
					goto __Exit;
				case kScriptTypeBool:
					left.floatValue = (ScriptNativeFloat)left.boolValue;
					goto __Warning;
				case kScriptTypeString:
					left.floatValue = (ScriptNativeFloat)strtof(left.stringValue.data(), NULL);
					goto __Exit;
                default:
                    break;
			} break;
		case kScriptTypeBool:
			switch (left.type) {
				case kScriptTypeInt:
					left.boolValue = (ScriptNativeBool)left.intValue;
					goto __Warning;
				case kScriptTypeFloat:
					left.boolValue = (ScriptNativeBool)left.floatValue;
					goto __Warning;
				case kScriptTypeBool:
					goto __Exit;
				case kScriptTypeString:
					left.boolValue = (ScriptNativeBool)strtol(left.stringValue.data(), NULL, 10);
					goto __Exit;
                default:
                    break;
			} break;
		case kScriptTypeString:
			switch (left.type) {
				case kScriptTypeInt:
					left.stringValue.Format("%d", left.intValue);
					goto __Exit;
				case kScriptTypeFloat:
					left.stringValue.Format("%.2f", left.floatValue);
					goto __Exit;
				case kScriptTypeBool:
					left.stringValue.Format("%s", left.boolValue ? "TRUE" : "FALSE");
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

Bool ScriptVariableManager::Declare(ScriptObjectPtr variable) {

	if (this->varMap.count(variable->word)) {
		return LAME_FALSE;
	}

	variable->var = new ScriptVariable();
	variable->var->Reset();

	this->varMap[variable->word] = variable;

	return LAME_TRUE;
}

ScriptObjectPtr ScriptVariableManager::Find(StringC name) {

	if (!this->varMap.count(name)) {
		return LAME_NULL;
	}

	return this->varMap[name];
}

LAME_END