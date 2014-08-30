#ifndef __LAME_CORE_THROWABLE_ADAPTER__
#define __LAME_CORE_THROWABLE_ADAPTER__

#include "Throwable.h"

LAME_BEGIN2(Core)

class LAME_API ThrowableAdapter : public Throwable {
public:
	inline Void Debug(PrintStreamPtr printer) override {
		printer->Print(this->GetErrorBuffer());
	}
	inline StringC What() override {
		return this->GetErrorBuffer();
	}
	inline Void Throw() override {
		throw (*this);
	}
public:
	inline Void SetErrorBuffer(StringC message) {
		memcpy(this->GetErrorBuffer(), message, strlen(message));
	}
public:
	String GetErrorBuffer(Void);
};

LAME_END2

#endif // ~__LAME_CORE_THROWABLE_ADAPTER__