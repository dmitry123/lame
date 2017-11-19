#ifndef __LAME_LANG__TOKEN_FINDER__
#define __LAME_LANG__TOKEN_FINDER__

#include "Foundation.h"
#include "Token.h"

LAME_LANG_BEGIN

typedef class TokenFinder {
public:
	virtual TokenPtr findByWord(StringPtrC word, UInt32 length = 0) = 0;
} *TokenFinderPtr;

LAME_LANG_END

#endif // ~__LAME_LANG__TOKEN_MANAGER_PROTOCOL__