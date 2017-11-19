#ifndef __LAME_LANG__SCRIPT_PARSER__
#define __LAME_LANG__SCRIPT_PARSER__

#include "Node.h"
#include "AbstractLoader.h"
#include "TokenManager.h"
#include "SyntaxError.h"

LAME_LANG_BEGIN

typedef class ScriptParser {
public:
	ScriptParser(AbstractLoaderPtr loader, TokenFinderPtr finder):
		loader(loader), code(nullptr), finder(finder)
	{
	}
	ScriptParser(const StringPtr code, TokenFinderPtr finder):
		code(code), loader(nullptr), finder(finder)
	{
	}
public:
	inline Logic parse() {
		return this->parse(this->loader != nullptr ? this->loader->load() : this->code);
	}
protected:
	virtual Logic parse(StringPtrC code);
protected:
	virtual UInt32 parseWord(StringPtrC& ptr, TokenPtr& token);
	virtual UInt32 parseNumber(StringPtrC& ptr, TokenPtr& token);
	virtual UInt32 parseToken(StringPtrC& ptr, TokenPtr& token);
	virtual UInt32 parseString(StringPtrC& ptr, TokenPtr& token);
public:
	inline Logic isLetter(Char token) const {
		return token >= 'a' && token <= 'z' || token >= 'A' && token <= 'Z' ||
			strchr(this->letters, token);
	}
	inline Logic isDigit(Char token) const {
		return token >= '0' && token <= '9';
	}
	inline Logic isWhitespace(Char token) const {
		return !!strchr(this->whitespaces, token);
	}
	inline Logic isToken(Char token) const {
		return !!strchr(this->tokens, token);
	}
public:
	inline const Vector<NodePtr>& getCollection() const {
		return this->collection;
	}
	inline AbstractLoaderPtr getLoader() const {
		return this->loader;
	}
	inline TokenFinderPtr getFinder() const {
		return this->finder;
	}
private:
	Vector<NodePtr> collection;
	AbstractLoaderPtr loader;
	StringPtrC code;
	TokenFinderPtr finder;
	UInt32 line;
private:
	static StringPtrC whitespaces;
	static StringPtrC letters;
	static StringPtrC tokens;
} *ScriptParserPtr;

LAME_LANG_END

#endif // ~__LAME_LANG__SCRIPT_PARSER__