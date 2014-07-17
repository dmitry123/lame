#ifndef __TEST__
#define __TEST__

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include <Script/Script.h>

static wchar_t assertBuffer[1024];

static const wchar_t* _StrToWstr(Lame::StringC message) {

	size_t length = strlen(message);

	for (size_t i = 0; i <= length; i++) {
		assertBuffer[i] = message[i];
	}

	return assertBuffer;
}

static const wchar_t* _StrToWstr(Lame::Core::BufferRefC message) {
	return _StrToWstr(message.data());
}

#endif // ~__TEST__