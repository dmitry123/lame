#ifndef __LAME_LANG__SYNTAX_BUILDER__
#define __LAME_LANG__SYNTAX_BUILDER__

#include "TokenManager.h"
#include "ScriptParser.h"
#include "Node.h"
#include "ExpMatcher.h"
#include "SyntaxError.h"

LAME_LANG_BEGIN

template <class TokenType, class This> class SyntaxBuilder {
public:
	typedef TokenManager<TokenType> Factory, *FactoryPtr;
	typedef Vector<NodePtr>::const_iterator Iterator, IteratorPtr;
	typedef ExpMatcher<TokenType, Iterator> Matcher, *MatcherPtr;
	typedef Stack<NodePtr> NodeStack, *NodeStackPtr;
public:
	SyntaxBuilder(FactoryPtr factory, ScriptParserPtr parser):
		factory(factory), parser(parser), matcher(nullptr)
	{
		this->root = this->active = new Node(
			nullptr, nullptr, 0, nullptr
		);
	}
	virtual ~SyntaxBuilder() {
		if (this->matcher != nullptr) {
			delete this->matcher;
		}
		delete this->root;
	}
protected:
	virtual void matchBuilder() = 0;
public:
	inline void build() {
		this->build(this->getParser()->getCollection());
	}
	void build(const Vector<NodePtr>& collection);
public:
	inline void error(SyntaxError::Code code) {
		this->error(code, ErrorMessage());
	}
	inline void error(SyntaxError::Code code, ErrorMessage message) {
		throw new SyntaxError(code, message, this->active->getLine());
	}
	inline This* require(TokenType id, Logic skip = false) {
		if (!(*this->begin)->is(id)) {
			this->error(SyntaxError::INVALID_TOKEN, ErrorMessage("missed token \"%s\"", this->getFactory()->findByToken(id)->word));
		}
		else if (skip) {
			this->next();
		}
		return dynamic_cast<This*>(this);
	}
	inline This* require(Vector<TokenType> tokens, Logic skip = false) {
		for (auto t : tokens) {
			if (!(*this->begin)->is(t)) {
				continue;
			}
			if (skip) {
				this->next();
			}
			return dynamic_cast<This*>(this);
		}
		this->error(SyntaxError::INVALID_TOKEN);
		return dynamic_cast<This*>(this);
	}
	inline Logic finish(const Iterator& i) {
		return !(i == this->end - 1 || i == this->end);
	}
	inline This* next(UInt32 count = 1) {
		while (this->finish(this->begin) && count--) {
			this->begin++;
		}
		return dynamic_cast<This*>(this);
	}
	inline This* prev(UInt32 count = 1) {
		this->begin -= count;
		return dynamic_cast<This*>(this);
	}
public:
	inline NodePtr push(NodePtr node = nullptr) {
		assert(this->active);
		if (node == nullptr) {
			node = *(this->begin - this->getMatcher()->getOffset());
		}
		this->active->insert(node);
		this->stack.push(this->active);
		this->active = node;
		return node;
	}
	inline This* pop(NodePtr node) {
		assert(!this->stack.empty());
		auto prev = this->stack.top();
		if (node != nullptr) {
			assert(this->active == node);
		}
		this->stack.pop();
		this->active = prev;
		return dynamic_cast<This*>(this);
	}
public:
	inline FactoryPtr getFactory() {
		return this->factory;
	}
	inline ScriptParserPtr getParser() {
		return this->parser;
	}
	inline NodePtr getRoot() {
		return this->root;
	}
	inline MatcherPtr getMatcher() {
		return this->matcher;
	}
	inline const NodeStack& getStack() {
		return this->stack;
	}
	inline NodePtr getActive() {
		return this->active;
	}
protected:
	Iterator begin;
	Iterator end;
private:
	FactoryPtr factory;
	ScriptParserPtr parser;
	NodePtr root;
	MatcherPtr matcher;
	NodeStack stack;
	NodePtr active;
};

template <class TokenType, class This>
void SyntaxBuilder<TokenType, This>::build(const Vector<NodePtr>& collection) {

	this->begin = collection.begin();
	this->end = collection.end();

	this->matcher = new Matcher(
		&this->begin, &this->end
	);

	for (; this->begin != this->end; this->begin++) {
		this->matchBuilder();
	}
}

LAME_LANG_END

#endif // ~__LAME_LANG__SYNTAX_BUILDER__