#ifndef __LAME_SCRIPT__GLOBAL_SCOPE__
#define __LAME_SCRIPT__GLOBAL_SCOPE__

#include "ScopeController.h"

LAME_BEGIN2(Script)

class LAME_API GlobalScope {
public:
	inline ScopeControllerPtr operator -> () {
		return globalScope_;
	}
public:
    GlobalScope() {
        if (!isGlobalScopeInitialized_) {
            _InitializeGlobalScope();
        }
    }
public:
	inline static ScopeControllerPtr GetInstance() {
		if (!isGlobalScopeInitialized_) {
			_InitializeGlobalScope();
		}
		return globalScope_;
	}
	inline static SegmentPtr GetCodeSegment() {
		return codeSegment_;
	}
	inline static Void SetCodeSegment(SegmentPtr codeSegment) {
		codeSegment_ = codeSegment;
	}
public:
    static ClassPtr classChar;
    static ClassPtr classByte;
    static ClassPtr classShort;
    static ClassPtr classInt;
    static ClassPtr classLong;
    static ClassPtr classFloat;
    static ClassPtr classDouble;
    static ClassPtr classObject;
    static ClassPtr classClass;
    static ClassPtr classBoolean;
    static ClassPtr classVoid;
	static ClassPtr classString;
public:
	static VariablePtr r0;
	static VariablePtr r1;
	static VariablePtr r2;
	static VariablePtr r3;
	static VariablePtr r4;
	static VariablePtr r5;
	static VariablePtr r6;
	static VariablePtr r7;
	static VariablePtr r8;
	static VariablePtr r9;
	static VariablePtr r10;
	static VariablePtr r11;
	static VariablePtr r12;
	static VariablePtr r13;
	static VariablePtr r14;
	static VariablePtr r15;
public:
	static VariablePtr f0;
	static VariablePtr f1;
	static VariablePtr f2;
	static VariablePtr f3;
	static VariablePtr f4;
	static VariablePtr f5;
	static VariablePtr f6;
	static VariablePtr f7;
	static VariablePtr f8;
	static VariablePtr f9;
private:
    static Void _InitializeGlobalScope(Void);
    static Void _InitializeCharClass(Void);
    static Void _InitializeByteClass(Void);
    static Void _InitializeShortClass(Void);
    static Void _InitializeIntClass(Void);
    static Void _InitializeLongClass(Void);
    static Void _InitializeFloatClass(Void);
    static Void _InitializeDoubleClass(Void);
	static Void _InitializeBooleanClass(Void);
	static Void _InitializeVoidClass(Void);
	static Void _InitializeStringClass(Void);
    static Void _InitializeObjectClass(Void);
    static Void _InitializeClassClass(Void);
private:
    static Bool isGlobalScopeInitialized_;
	static ScopeControllerPtr globalScope_;
	static SegmentPtr codeSegment_;
};

LAME_END2

#endif // ~__LAME_SCRIPT__GLOBAL_SCOPE__