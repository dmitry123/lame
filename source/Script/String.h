#ifndef __LAME_SCRIPT__STRING__
#define __LAME_SCRIPT__STRING__

#include "Define.h"
#include "Virtual.h"

LAME_BEGIN

typedef class ScriptString ScriptString, *ScriptStringPtr;

#define __LAME_APPLY_OPERATION_STRING_COMPARE(_operation) \
	inline Void operator _operation (const ScriptNativeString& value) { \
		if (this->type_ == kScriptTypeString) { \
			this->bool_ = this->string_ _operation value; goto __Exit; \
		} \
	__Exit: \
		this->type_ = kScriptTypeInt; \
	} \

#define __LAME_DENY_OPERATION_STRING(_operation) \
	inline Void operator _operation (const ScriptNativeString& value) { \
		PostSyntaxError(0, "Unable to apply %s for (String)", #_operation); \
	}

class ScriptString : public IScriptType {
public:
	inline Void operator = (const ScriptNativeString& value) {
		if (this->type_ == kScriptTypeString) {
			this->string_ = value; goto __Exit;
		}
	__Warning:
		PostSyntaxWarning(0, "Unsafe conversion from (String) to (%s)", StringType(this->type_));
	__Exit:
		this->type_ = kScriptTypeString;
	}
public:
	inline Void operator + (const ScriptNativeString& value) {
		if (this->type_ == kScriptTypeString) {
			this->string_ += value; goto __Exit;
		}
	__Warning:
		PostSyntaxWarning(0, "Unsafe conversion from (String) to (%s)", StringType(this->type_));
	__Exit:
		this->type_ = kScriptTypeString;
	}
public:
	__LAME_DENY_OPERATION_STRING(-);
	__LAME_DENY_OPERATION_STRING(*);
	__LAME_DENY_OPERATION_STRING(/);
	__LAME_DENY_OPERATION_STRING(%);
	__LAME_DENY_OPERATION_STRING(&);
	__LAME_DENY_OPERATION_STRING(|);
	__LAME_DENY_OPERATION_STRING(^);
	__LAME_DENY_OPERATION_STRING(>>);
	__LAME_DENY_OPERATION_STRING(<<);
public:
	__LAME_APPLY_OPERATION_STRING_COMPARE(<);
	__LAME_APPLY_OPERATION_STRING_COMPARE(>);
	__LAME_APPLY_OPERATION_STRING_COMPARE(<=);
	__LAME_APPLY_OPERATION_STRING_COMPARE(>=);
	__LAME_APPLY_OPERATION_STRING_COMPARE(==);
	__LAME_APPLY_OPERATION_STRING_COMPARE(!=);
public:
	ScriptString() {
		this->type_ = kScriptTypeString;
	}
};

LAME_END

#endif // ~__LAME_SCRIPT__STRING__