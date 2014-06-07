#include "Json.h"

LAME_BEGIN

Buffer ScriptJson::GenerateString() {

	std::string result = "{";


	result += "}";

	return result;
}

ScriptJson& ScriptJson::Parse(Buffer string) {



	return *this;
}

LAME_END