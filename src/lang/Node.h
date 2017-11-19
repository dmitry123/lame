#ifndef __LAME_LANG__NODE__
#define __LAME_LANG__NODE__

#include "Token.h"

LAME_LANG_BEGIN

typedef class Node {
	typedef Node *NodePtr;
public:
	typedef enum {
		FLAG_INVALID = 0x0000,
		FLAG_CONDITION = 0x0001,
		FLAG_ELSE = 0x0002,
		FLAG_FOR = 0x0004,
		FLAG_WHILE = 0x0008,
		FLAG_DO = 0x0010,
		FLAG_CLASS = 0x0020,
		FLAG_INTERFACE = 0x0040,
		FLAG_ANNOTATION = 0x0080,
		FLAG_METHOD = 0x0100,
		FLAG_TEMP = 0x2000
	} Flag;
public:
	Node(TokenPtr token, StringPtrC word, UInt32 line, NodePtr parent = nullptr) :
		token(token),
		word(word),
		parent(parent),
		flags(0),
		identifier(nullptr),
		line(line)
	{
		if (parent != nullptr) {
			parent->insert(this);
		}
	}
	virtual ~Node();
public:
	void insert(NodePtr child);
	void erase(NodePtr child);
public:
	inline void erase() {
		if (this->parent != nullptr) {
			this->parent->erase(this);
		}
	}
public:
	inline NodePtr require(TokenPtr token) {
		auto node = this->findByToken(token);
		if (!node) {
			node = new Node(token, strdup(token->word), 0, this);
		}
		return node;
	}
public:
	NodePtr findByToken(TokenPtr token, Logic depth = false);
	NodePtr findByWord(StringPtrC word, Logic depth = false);
public:
	template <class TokenType> inline bool is(TokenType id) const {
		return this->token->id == UInt32(id);
	}
	inline Logic is(UInt32 flags) const {
		return (this->flags & flags) != 0;
	}
	inline Logic is(Flag flag) const {
		return this->is(UInt32(flag));
	}
	inline Logic isConstruction() {
		return this->is(FLAG_CONDITION) ||
			this->is(FLAG_ELSE) ||
			this->is(FLAG_FOR) ||
			this->is(FLAG_WHILE) ||
			this->is(FLAG_DO);
	}
public:
	inline NodePtr setFlags(UInt32 flags) {
		this->flags |= flags; return this;
	}
	inline NodePtr setIdentifier(NodePtr identifier) {
		this->identifier = identifier; return this;
	}
	inline NodePtr setWord(StringPtrC word) {
		delete this->word;
		this->word = strdup(word);
		return this;
	}
public:
	inline Vector<NodePtr>& getChildren() {
		return this->children;
	}
	inline Vector<NodePtr>& getBody() {
		return this->body;
	}
	inline Vector<NodePtr>& getArgs() {
		return this->args;
	}
	inline Vector<NodePtr>& getImplements() {
		return this->implements;
	}
	inline Vector<NodePtr>& getExtends() {
		return this->extends;
	}
	inline NodePtr getParent() {
		if (this->parent->is(FLAG_TEMP)) {
			return this->parent->getParent();
		}
		return this->parent;
	}
	inline TokenPtr getToken() {
		return this->token;
	}
	inline StringPtrC getWord() {
		return this->word;
	}
	inline NodePtr getLast() {
		if (!this->children.empty()) {
			return this->children.back();
		}
		else {
			return nullptr;
		}
	}
	inline UInt32 getFlags() {
		return this->flags;
	}
	inline NodePtr getIdentifier() {
		return this->identifier;
	}
	inline UInt32 getLine() const {
		return this->line;
	}
	inline Vector<NodePtr>& getThrows() {
		return this->throws;
	}
private:
	StringPtrC word;
	TokenPtr token;
	Vector<NodePtr> children;
	NodePtr parent;
	Vector<NodePtr> body;
	Vector<NodePtr> args;
	UInt32 flags;
	Vector<NodePtr> implements;
	Vector<NodePtr> extends;
	NodePtr identifier;
	UInt32 line;
	Vector<NodePtr> throws;
} *NodePtr;

LAME_LANG_END

#endif // ~__LAME_LANG__NODE__