#ifndef __LAME_SCRIPT__ERROR__
#define __LAME_SCRIPT__ERROR__

#include "Define.h"

LAME_BEGIN2(Script)

class LAME_API ScriptError {
public:
	typedef Void(*ErrorHandler)(ScriptError error);
public:
	enum {
		UnknownError,
		NoError,
		Class_AlreadyExtended,
		Class_AlreadyImplemented,
		Class_WrongOperator,
		Class_ObjectNotVariable,
		Class_ObjectsHaveDifferentHash,
		Class_OperatorNotOverloaded,
		Method_InvalidArguments,
		Method_CantOverrideNativeMethod,
		Class_NotStaticObjectWithoutThis,
		Object_UnableToClone,
		Object_UnableToCast,
		Interface_MethodsMustBeImplemented,
		Interface_UnableToImplementClass,
		Interface_ClassMustImplementMethods,
		AbstractClass_CantStoreImplementedAbstractMethods,
		AbstractClass_ObjectMustImplementAbstractMethod
	};
public:
	ScriptError() :
		errorCode(NoError)
	{
	}
	ScriptError(Uint32 errorCode) :
		errorCode(errorCode)
	{
		if (errorCode != NoError) {
			SetLastError(this);
		}
	}
public:
	inline operator Bool () const {
		return errorCode == NoError;
	}
public:
	inline StringC GetDescription() {
		return description[errorCode];
	}
public:
	static ScriptError GetLastError(Void);
	static StringC GetErrorDescription(ScriptError error);
	static Void SetErrorHandler(ErrorHandler handler);
private:
	static Void SetLastError(ScriptErrorPtr code);
private:
	static const StringC description[];
private:
	Uint32 errorCode;
};

LAME_END2

#endif // ~__LAME_SCRIPT__ERROR__