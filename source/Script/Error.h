#ifndef __LAME_SCRIPT__ERROR__
#define __LAME_SCRIPT__ERROR__

#include "Define.h"

LAME_BEGIN2(Script)

class LAME_API Error {
public:
	typedef Void(*ErrorHandler)(Error error);
public:
	enum {
		UnknownError,
		NoError,
		Class_AlreadyExtended,
		Class_AlreadyImplemented,
		Class_CanImplementOnlyInterfaces,
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
		AbstractClass_ObjectMustImplementAbstractMethod,
		Array_ObjectNotArray
	};
public:
	Error() :
		errorCode(NoError)
	{
	}
	Error(Uint32 errorCode) :
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
    inline Uint32 GetCode() {
        return errorCode;
    }
public:
	static Error GetLastError(Void);
	static StringC GetErrorDescription(Error error);
	static Void SetErrorHandler(ErrorHandler handler);
private:
	static Void SetLastError(ErrorPtr code);
private:
	static const StringC description[];
private:
	Uint32 errorCode;
};

LAME_END2

#endif // ~__LAME_SCRIPT__ERROR__