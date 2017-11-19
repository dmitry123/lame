#ifndef __LAME_LANG__EXP_MATCHER__
#define __LAME_LANG__EXP_MATCHER__

#include "Foundation.h"
#include "Token.h"

LAME_LANG_BEGIN

template <class TokenType, class Iterator>
class ExpMatcher {
public:
	ExpMatcher(Iterator* begin, Iterator* end) :
		begin(begin), end(end), offset(0)
	{
	}
public:
	Logic match(TokenType token) {
		return this->match(Deque<TokenType>{ token });
	}
	Logic match(Deque<TokenType> tokens) {
		auto& i = *this->begin;
		this->offset = 0;
		for (; tokens.size() > 0 && i != *this->end - 1; i++) {
			if (UInt32(tokens.front()) == (*i)->getToken()->id) {
				tokens.pop_front();
				this->offset++;
			}
			else {
				i -= this->offset;
				return false;
			}
		}
		return true;
	}
public:
	inline UInt32 getOffset() const {
		return this->offset;
	}
private:
	Iterator* begin;
	Iterator* end;
	UInt32 offset;
};

LAME_LANG_END

#endif // ~__LAME_LANG__EXP_MATCHER__