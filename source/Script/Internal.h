#ifndef __LAME_SCRIPT__INTERNAL__
#define __LAME_SCRIPT__INTERNAL__

#include "Define.h"
#include "Exception.h"

LAME_BEGIN2(Script)

Bool IsDigit(Sint8 symbol);
Bool IsLetter(Sint8 symbol);
Bool IsIntValue(StringC string);
Bool IsHexValue(StringC string);
Bool IsStringValue(StringC string);
Bool IsIntValue(const Core::Buffer& string);
Bool IsFloatValue(StringC string);
Bool IsFloatValue(const Core::Buffer& string);

ScriptNativeInt    ParseIntValue(StringC string);
ScriptNativeFloat  ParseFloatValue(StringC string);
ScriptNativeString ParseStringValue(StringC string);

LAME_END2

#endif // ~__LAME_SCRIPT__INTERNAL__