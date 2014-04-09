#include "Script.h"
#include "Internal.h"

LAME_BEGIN

Void ScriptVariable::Convert(
	      ScriptVariable& left,
	const ScriptVariable& right
) {
	if (left.object->type == right.object->type) {
		return;
	}
	if (left.object->type == kScriptTypeVoid ||
		left.object->type == kScriptTypeFunction ||
		left.object->type == kScriptTypeClass ||
		right.object->type == kScriptTypeVoid ||
		right.object->type == kScriptTypeFunction ||
		right.object->type == kScriptTypeClass
	) {
		goto __Error;
	}
	switch (right.object->type) {
		case kScriptTypeInt:
			switch (left.object->type) {
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
			switch (left.object->type) {
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
			switch (left.object->type) {
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
			switch (left.object->type) {
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
	PostSyntaxError(left.object->line, "Invalid conversion from unknown type (%s)",
		left.object->type.GetString());
__Warning:
	PostSyntaxWarning(left.object->line, "Unsafe type conversion from (%s) to (%s)",
		left.object->type.GetString(),
		right.object->type.GetString());
	goto __Exit;
__Error:
	PostSyntaxError(left.object->line, "Invalid type conversion from (%s) to (%s)",
		left.object->type.GetString(),
		right.object->type.GetString());
	goto __Exit;
__Exit:;
	left.object->type = right.object->type;
}

Void ScriptVariable::Reset() {
	this->boolValue = 0;
	this->intValue = 0;
	this->floatValue = 0;
	this->next = 0;
	this->modificators = 0;
	this->stringValue.clear();
}

LAME_END