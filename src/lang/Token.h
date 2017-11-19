#ifndef __LAME_LANG__TOKEN__
#define __LAME_LANG__TOKEN__

#include "Foundation.h"

LAME_LANG_BEGIN

typedef class Token {
public:
	typedef enum {
		FLAG_CONSTANT = 0x0001,
		FLAG_OPERATOR = 0x0002,
		FLAG_MODIFIER = 0x0004,
		FLAG_RIGHT = 0x0008,
		FLAG_LEFT = 0x0010,
		FLAG_BOOLEAN = 0x0020,
		FLAG_MATH = 0x0040,
		FLAG_ASSIGNMENT = 0x0080,
		FLAG_KEYWORD = 0x0100
	} Flag;
public:
	UInt32 id;
	StringPtrC word;
	UInt32 precedence;
	UInt32 flags;
	UInt32 args;
public:
	inline Logic is(UInt32 flags) const {
		return (this->flags & flags) != 0;
	}
	inline Logic is(StringPtrC word) const {
		return !strcmp(word, this->word);
	}
	inline Logic is(SInt8 character) const {
		return !strncmp(this->word, &character, 1);
	}
public:
	inline Logic isLeft() const {
		return (this->flags & FLAG_LEFT) != 0;
	}
	inline Logic isRight() const {
		return (this->flags & FLAG_RIGHT) != 0;
	}
	inline Logic isIdentifier() {
		return this->flags == 0;
	}
	inline Logic isOperator() {
		return (this->flags & FLAG_OPERATOR) != 0;
	}
	inline Logic isModifier() {
		return (this->flags & FLAG_MODIFIER) != 0;
	}
} *TokenPtr;

LAME_LANG_END

#endif // ~__LAME_LANG__TOKEN__