#include "Json.h"
#include "Internal.h"

LAME_BEGIN

ScriptNativeInt ScriptJsonNode::GetInt(Void) {
	return ParseIntValue(this->fieldValue.data());
}

ScriptNativeFloat ScriptJsonNode::GetFloat(Void) {
	return ParseFloatValue(this->fieldValue.data());
}

ScriptNativeString ScriptJsonNode::GetString(Void) {
	return ParseStringValue(this->fieldValue.data());
}

ScriptNativeBool ScriptJsonNode::GetBool(Void) {
	return (ScriptNativeBool)ParseIntValue(this->fieldValue.data());
}

ScriptNativeJson ScriptJsonNode::GetJson(Void) {
	return ScriptNativeJson().Parse(this->fieldValue);
}

Void ScriptJsonNode::SetInt(ScriptNativeInt value) {
	this->fieldValue.reserve(80); sprintf((String)this->fieldValue.data(), "%d", value);
}

Void ScriptJsonNode::SetFloat(ScriptNativeFloat value) {
	this->fieldValue.reserve(80); sprintf((String)this->fieldValue.data(), "%.2f", value);
}

Void ScriptJsonNode::SetString(ScriptNativeString value) {
	this->fieldValue.reserve(80); sprintf((String)this->fieldValue.data(), "%s", value.data());
}

Void ScriptJsonNode::SetBool(ScriptNativeBool value) {
	this->fieldValue = value ? "True" : "False";
}

Void ScriptJsonNode::SetJson(ScriptNativeJson value) {
	this->fieldValue = value.GenerateString();
}

LAME_END