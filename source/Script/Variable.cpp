#include "Script.h"

LAME_BEGIN

Void ScriptVariable::Convert(ScriptType& type) {
	if (type.type == this->object->type.type) {
        return;
	}
	if (type == kScriptTypeVoid ||
		type == kScriptTypeFunction ||
		type == kScriptTypeClass ||
		this->object->type == kScriptTypeVoid ||
		this->object->type == kScriptTypeFunction ||
		this->object->type == kScriptTypeClass
	) {
		goto __Error;
	}
    if (type == kScriptTypeVar) {
        type = this->object->type; return;
    }
	switch (type) {
		case kScriptTypeInt:
			switch (this->object->type) {
				case kScriptTypeFloat:
					this->intValue = (ScriptNativeInt)this->floatValue;
					goto __Warning;
				case kScriptTypeBool:
					this->intValue = (ScriptNativeInt)this->boolValue;
					goto __Warning;
				case kScriptTypeString:
					this->intValue = ScriptObject::ParseIntValue(this->stringValue.data());
					goto __Exit;
                default:
                    break;
			} break;
		case kScriptTypeFloat:
			switch (this->object->type) {
				case kScriptTypeInt:
					this->floatValue = (ScriptNativeFloat)this->intValue;
					goto __Warning;
				case kScriptTypeBool:
					this->floatValue = (ScriptNativeFloat)this->boolValue;
					goto __Warning;
				case kScriptTypeString:
					this->floatValue = (ScriptNativeFloat)strtof(this->stringValue.data(), NULL);
					goto __Exit;
                default:
                    break;
			} break;
		case kScriptTypeBool:
			switch (this->object->type) {
				case kScriptTypeInt:
					this->boolValue = (ScriptNativeBool)this->intValue;
					goto __Warning;
				case kScriptTypeFloat:
					this->boolValue = (ScriptNativeBool)this->floatValue;
					goto __Warning;
				case kScriptTypeString:
					this->boolValue = (ScriptNativeBool)strtol(this->stringValue.data(), NULL, 10);
					goto __Exit;
                default:
                    break;
			} break;
		case kScriptTypeString:
			switch (this->object->type) {
				case kScriptTypeInt:
					this->stringValue.Format("%d", this->intValue);
					goto __Exit;
				case kScriptTypeFloat:
					this->stringValue.Format("%.2f", this->floatValue);
					goto __Exit;
				case kScriptTypeBool:
					this->stringValue.Format("%s", this->boolValue ? "TRUE" : "FALSE");
					goto __Exit;
                default:
                    break;
			} break;
        default:
            break;
	}
	PostSyntaxError(this->object->line, "Invalid conversion from unknown type (%s)",
		this->object->type.GetString());
__Warning:
	PostSyntaxWarning(this->object->line, "Unsafe type conversion from (%s) to (%s)",
		this->object->type.GetString(),
		type.GetString());
	goto __Exit;
__Error:
	PostSyntaxError(this->object->line, "Invalid type conversion from (%s) to (%s)",
		this->object->type.name.data(),
		type.GetString());
	goto __Exit;
__Exit:;
}

Void ScriptVariable::Reset() {
	this->boolValue = 0;
	this->intValue = 0;
	this->floatValue = 0;
	this->modificators = 0;
	this->declared = 0;
	this->stringValue.clear();
}

LAME_END