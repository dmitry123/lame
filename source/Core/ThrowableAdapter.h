#ifndef __LAME_CORE_THROWABLE_ADAPTER__
#define __LAME_CORE_THROWABLE_ADAPTER__

#include "Throwable.h"

LAME_BEGIN2(Core)

class ThrowableAdapter : public Throwable {
public:
	inline Void Debug(PrintStreamPtr printer) override {
		printer->Print(this->errorBuffer);
	}
	inline StringC What() override {
		return &errorBuffer[0];
	}
	inline Void Throw() override {
		throw (*this);
	}
public:
	inline Void SetErrorBuffer(StringC message) {
		memcpy(this->errorBuffer, message, strlen(message));
	}
	inline String GetErrorBuffer() {
		return this->errorBuffer;
	}
private:
	ErrorBuffer errorBuffer;
};

LAME_END2

#endif // ~__LAME_CORE_THROWABLE_ADAPTER__