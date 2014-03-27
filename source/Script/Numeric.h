#ifndef __LAME_SCRIPT__NUMERIC__	
#define __LAME_SCRIPT__NUMERIC__

#include "Define.h"
#include "Virtual.h"

LAME_BEGIN

typedef class ScriptNumeric ScriptNumeric, *ScriptNumericPtr;

#define __LAME_APPLY_OPERATION_NUMERIC_BASIC(_operation) \
	inline Void operator _operation (const ScriptNativeInt& value) { \
		if (this->type_ == kScriptTypeFloat) { \
			this->float_ _operation= (ScriptNativeFloat)value; goto __Warning; \
		} else if (this->type_ == kScriptTypeBool) { \
			this->bool_ _operation= (ScriptNativeBool)value; goto __Warning; \
		} else { \
			this->int_ _operation= value; goto __Exit; \
		} \
	__Warning: \
		PostSyntaxWarning(0, "Unsafe conversion from (Int) to (%s)", StringType(this->type_)); \
	__Exit: \
		this->type_ = kScriptTypeInt; \
	} \
	inline Void operator _operation (const ScriptNativeFloat& value) { \
		if (this->type_ == kScriptTypeInt) { \
			this->int_ _operation= (ScriptNativeInt)value; goto __Warning; \
		} else if (this->type_ == kScriptTypeBool) { \
			this->bool_ _operation= (ScriptNativeBool)value; goto __Warning; \
		} else { \
			this->float_ _operation= value; goto __Exit; \
		} \
	__Warning: \
		PostSyntaxWarning(0, "Unsafe conversion from (Float) to (%s)", StringType(this->type_)); \
	__Exit: \
		this->type_ = kScriptTypeFloat; \
	} \
	inline Void operator _operation (const ScriptNativeBool& value) { \
		if (this->type_ == kScriptTypeInt) { \
			this->int_ _operation= (ScriptNativeInt)value; goto __Warning; \
		} else if (this->type_ == kScriptTypeFloat) { \
			this->float_ _operation= (ScriptNativeFloat)value; goto __Warning; \
		} else { \
			this->bool_ _operation= value; goto __Exit; \
		} \
	__Warning: \
		PostSyntaxWarning(0, "Unsafe conversion from (Bool) to (%s)", StringType(this->type_)); \
	__Exit: \
		this->type_ = kScriptTypeBool; \
	}

#define __LAME_APPLY_OPERATION_NUMERIC_LOGIC(_operation) \
	inline Void operator _operation (const ScriptNativeInt& value) { \
		if (this->type_ == kScriptTypeFloat) { \
			PostSyntaxError(0, "Unable to apply %s for (Float)", StringType(this->type_)); \
		} else if (this->type_ == kScriptTypeBool) { \
			this->bool_ _operation= (ScriptNativeBool)value; goto __Warning; \
		} else { \
			this->int_ _operation= value; goto __Exit; \
		} \
	__Warning: \
		PostSyntaxWarning(0, "Unsafe conversion from (Int) to (%s)", StringType(this->type_)); \
	__Exit: \
		this->type_ = kScriptTypeInt; \
	} \
	inline Void operator _operation (const ScriptNativeFloat& value) { \
		PostSyntaxError(0, "Unable to apply %s for (Float)", StringType(this->type_)); \
	} \
	inline Void operator _operation (const ScriptNativeBool& value) { \
		if (this->type_ == kScriptTypeInt) { \
			this->int_ _operation= (ScriptNativeInt)value; goto __Warning; \
		} else if (this->type_ == kScriptTypeFloat) { \
			PostSyntaxError(0, "Unable to apply %s for (Float)", StringType(this->type_)); \
		} else { \
			this->bool_ _operation= value; goto __Exit; \
		} \
	__Warning: \
		PostSyntaxWarning(0, "Unsafe conversion from (Bool) to (%s)", StringType(this->type_)); \
	__Exit: \
		this->type_ = kScriptTypeBool; \
	}

#define __LAME_APPLY_OPERATION_NUMERIC_COMPARE(_operation) \
	inline Void operator _operation (const ScriptNativeInt& value) { \
		if (this->type_ == kScriptTypeFloat) { \
			this->bool_ = this->float_ _operation (ScriptNativeFloat)value; \
		} else if (this->type_ == kScriptTypeBool) { \
			this->bool_ = this->bool_ _operation (ScriptNativeBool)value; \
		} else { \
			this->bool_ = this->int_ _operation value; \
		} \
		this->type_ = kScriptTypeBool; \
	} \
	inline Void operator _operation (const ScriptNativeFloat& value) { \
		if (this->type_ == kScriptTypeInt) { \
			this->bool_ = this->int_ _operation (ScriptNativeInt)value; \
		} else if (this->type_ == kScriptTypeBool) { \
			this->bool_ = this->bool_ _operation (ScriptNativeBool)value; \
		} else { \
			this->bool_ = this->float_ _operation value; \
		} \
		this->type_ = kScriptTypeBool; \
	} \
	inline Void operator _operation (const ScriptNativeBool& value) { \
		if (this->type_ == kScriptTypeInt) { \
			this->bool_ = this->int_ _operation (ScriptNativeInt)value; \
		} else if (this->type_ == kScriptTypeFloat) { \
			this->bool_ = this->float_ _operation (ScriptNativeFloat)value; \
		} else { \
			this->bool_ = this->bool_ _operation value; \
		} \
		this->type_ = kScriptTypeBool; \
	}

class ScriptNumeric : public IScriptType {
public:
	inline Void operator = (const ScriptNativeInt& value) {
		if (this->type_ == kScriptTypeFloat) {
			this->float_ = (ScriptNativeFloat)value; goto __Warning;
		} else if (this->type_ == kScriptTypeBool) {
			this->bool_ = (ScriptNativeBool)value; goto __Warning;
		} else {
			this->int_ = value; goto __Exit;
		}
	__Warning:
		PostSyntaxWarning(0, "Unsafe conversion from (Int) to (%s)", StringType(this->type_));
	__Exit:
		this->type_ = kScriptTypeInt;
	}
	inline Void operator = (const ScriptNativeFloat& value) {
		if (this->type_ == kScriptTypeInt) {
			this->int_ = (ScriptNativeInt)value; goto __Warning;
		} else if (this->type_ == kScriptTypeBool) {
			this->bool_ = (ScriptNativeBool)value; goto __Warning;
		} else {
			this->float_ = value; goto __Exit;
		}
	__Warning:
		PostSyntaxWarning(0, "Unsafe conversion from (Float) to (%s)", StringType(this->type_));
	__Exit:
		this->type_ = kScriptTypeFloat;
	}
	inline Void operator = (const ScriptNativeBool& value) {
		if (this->type_ == kScriptTypeInt) {
			this->int_ = (ScriptNativeInt)value; goto __Warning;
		} else if (this->type_ == kScriptTypeFloat) {
			this->float_ = (ScriptNativeFloat)value; goto __Warning;
		} else {
			this->bool_ = value; goto __Exit;
		}
	__Warning:
		PostSyntaxWarning(0, "Unsafe conversion from (Bool) to (%s)", StringType(this->type_));
	__Exit:
		this->type_ = kScriptTypeBool;
	}
public:
	__LAME_APPLY_OPERATION_NUMERIC_BASIC(+);
	__LAME_APPLY_OPERATION_NUMERIC_BASIC(-);
	__LAME_APPLY_OPERATION_NUMERIC_BASIC(*);
	__LAME_APPLY_OPERATION_NUMERIC_BASIC(/);
public:
	__LAME_APPLY_OPERATION_NUMERIC_LOGIC(%);
	__LAME_APPLY_OPERATION_NUMERIC_LOGIC(&);
	__LAME_APPLY_OPERATION_NUMERIC_LOGIC(|);
	__LAME_APPLY_OPERATION_NUMERIC_LOGIC(^);
	__LAME_APPLY_OPERATION_NUMERIC_LOGIC(>>);
	__LAME_APPLY_OPERATION_NUMERIC_LOGIC(<<);
public:
	__LAME_APPLY_OPERATION_NUMERIC_COMPARE(<);
	__LAME_APPLY_OPERATION_NUMERIC_COMPARE(>);
	__LAME_APPLY_OPERATION_NUMERIC_COMPARE(<=);
	__LAME_APPLY_OPERATION_NUMERIC_COMPARE(>=);
	__LAME_APPLY_OPERATION_NUMERIC_COMPARE(==);
	__LAME_APPLY_OPERATION_NUMERIC_COMPARE(!=);
public:
	ScriptNumeric() {
		this->type_ = kScriptTypeInt;
	}
};

LAME_END

#endif // ~__LAME_SCRIPT__NUMERIC__