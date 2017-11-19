#ifndef __LAME_LANG__ABSTRACT_LEX_MANAGER__
#define __LAME_LANG__ABSTRACT_LEX_MANAGER__

#include "Foundation.h"
#include "Token.h"
#include "TokenFinder.h"

LAME_LANG_BEGIN

template <class TokenType> class TokenManager : public TokenFinder {
	static_assert(std::is_enum<TokenType>::value, "token type must be enum class");
public:
	typedef Map<TokenType, Token> TokenMap;
	typedef Map<StringPtrC, Token> CacheMap;
public:
	TokenManager(const TokenMap& map):
		map(map)
	{
	}
public:
	static TokenManager* getManager() {
		return nullptr;
	}
public:
	inline TokenFinderPtr getFinder() {
		return this;
	}
public:
	TokenPtr findByWord(StringPtrC word, UInt32 length = 0) override {
		UInt32 l;
		if (this->cache.count(word) > 0) {
			return &this->cache.at(word);
		}
		if (!length) {
			length = strlen(word);
		}
		for (auto& token : this->map) {
			if ((l = strlen(token.second.word)) > length) {
				length = l;
			}
			if (!strnicmp(word, token.second.word, length)) {
				return &token.second;
			}
		}
		return nullptr;
	}
	TokenPtr findByToken(TokenType token) {
		return this->map.count(token) > 0 ? &this->map.at(token) : nullptr;
	}
public:
	inline UInt32 getTotal() const {
		return UInt32(TokenType::_TOTAL);
	}
public:
	inline TokenMap& getMap() const {
		return this->map;
	}
private:
	TokenMap map;
	CacheMap cache;
};

LAME_LANG_END

#endif // ~__LAME_LANG__ABSTRACT_LEX_MANAGER__