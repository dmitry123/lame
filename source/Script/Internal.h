#ifndef __LAME_SCRIPT__INTERNAL__
#define __LAME_SCRIPT__INTERNAL__

#include "Define.h"
#include "Exception.h"

LAME_BEGIN2(Script)

Bool LAME_API IsDigit(Sint8 symbol);
Bool LAME_API IsLetter(Sint8 symbol);
Bool LAME_API IsIntValue(StringC string);
Bool LAME_API IsHexValue(StringC string);
Bool LAME_API IsStringValue(StringC string);
Bool LAME_API IsIntValue(const Core::Buffer& string);
Bool LAME_API IsFloatValue(StringC string);
Bool LAME_API IsFloatValue(const Core::Buffer& string);

ScriptNativeInt    LAME_API ParseIntValue(StringC string);
ScriptNativeFloat  LAME_API ParseFloatValue(StringC string);
ScriptNativeString LAME_API ParseStringValue(StringC string);

LAME_END2

#endif // ~__LAME_SCRIPT__INTERNAL__