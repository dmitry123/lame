#include "Node.h"
#include "Exception.h"

#include <stack>

LAME_BEGIN2(Script)

class ShuntingYard {
public:
	Stack<NodePtr>* Convert(Vector<NodePtr>* list) {

		NodePtr top = NULL;

		for (NodePtr node : *list) {
			if (node->id == kScriptNodeInvoke ||
				node->id == kScriptNodeAlloc
			) {
				goto _HandleOperator;
			}
			else if (
				node->lex->lex->IsConst() ||
				node->lex->lex->IsUnknown() ||
				node->lex->lex->IsCondition() ||
				node->lex->lex->IsClass() ||
				node->lex->lex->IsInterface()
			) {
				this->resultStack.push(node);
			}
			else {
				if (node->lex->lex->id == kScriptLexSemicolon) {
					while (!this->opStack.empty()) {
						top = this->opStack.top();
						if (top->lex->lex->id == kScriptLexParenthesisL ||
							top->lex->lex->id == kScriptLexParenthesisR ||
							top->lex->lex->id == kScriptLexBracketL ||
							top->lex->lex->id == kScriptLexBracketR
						) {
							PostSyntaxError(node->lex->line, "Parenthese mismatched", 0);
						}
						this->resultStack.push(top);
						this->opStack.pop();
					}
					this->resultStack.push(node);
				}
				else if (
					node->lex->lex->id == kScriptLexParenthesisL ||
					node->lex->lex->id == kScriptLexBracketL
				) {
					this->HandleLeftParenthesis(node);
				}
				else if (
					node->lex->lex->id == kScriptLexParenthesisR ||
					node->lex->lex->id == kScriptLexBracketR
				) {
					this->HandleRightParenthesis(node);
				}
				else if (node->lex->lex->id == kScriptLexComma) {
					continue;
				}
				else {
					if (
						this->opStack.empty() &&
						this->resultStack.empty() && (
							node->lex->lex->id == kScriptLexAdd ||
							node->lex->lex->id == kScriptLexSub
						)
					) {
						LexPtr lex = Lex::Find(kScriptLexUnaryPlus);
						if (node->lex->lex->id == kScriptLexAdd) {
							lex = Lex::Find(kScriptLexUnaryPlus);
						} else if (node->lex->lex->id == kScriptLexSub) {
							lex = Lex::Find(kScriptLexUnaryMinus);
						}
						node->lex->priority = lex->priority;
						node->lex->args = lex->args;
					}
				_HandleOperator:
					this->HandleOperator(node);
				}
			}
		}
		while (!this->opStack.empty()) {
			this->resultStack.push(this->opStack.top());
			this->opStack.pop();
		}
		return &this->resultStack;
	}
private:
	inline void HandleLeftParenthesis(NodePtr node) {
		this->opStack.push(node);
	}
	inline void HandleRightParenthesis(NodePtr node) {
		while (!this->opStack.empty() && (
			this->opStack.top()->lex->lex->id != kScriptLexParenthesisL &&
			this->opStack.top()->lex->lex->id != kScriptLexBracketL)
		) {
			this->resultStack.push(this->opStack.top());
			this->opStack.pop();
		}
		if (this->opStack.empty() || !this->opStack.empty() && (
			this->opStack.top()->lex->lex->id != kScriptLexParenthesisL &&
			this->opStack.top()->lex->lex->id != kScriptLexBracketL)
		) {
			PostSyntaxError(node->lex->line, "Parenthese mismatched", 0);
		}
		this->opStack.pop();
	}
	inline void HandleOperator(NodePtr node) {
		while (!this->opStack.empty() && this->GetPrecedence(node) <= this->StackPrecedence()) {
			this->resultStack.push(this->opStack.top());
			this->opStack.pop();
		}
		this->opStack.push(node);
	}
private:
	inline int GetPrecedence(NodePtr node) const {
		return node->lex->priority;
	}
	inline int StackPrecedence() const {
		if (this->opStack.empty()) {
			return -1;
		}
		return this->GetPrecedence(this->opStack.top());
	}
private:
	Stack<NodePtr> resultStack;
	Stack<NodePtr> opStack;
};

Void _Order(Vector<NodePtr>* list) {

	Stack<NodePtr>* result;
	ShuntingYard shuntingYard;

	result = shuntingYard.Convert(list);

	list->clear();
	while (!result->empty()) {
		if (result->top()->id == kScriptLexBraceL ||
			result->top()->id == kScriptLexBraceR
		) {
			PostSyntaxError(result->top()->lex->line, "Braces mismatched", 0);
		}
		if (result->top()->id == kScriptLexParenthesisL ||
			result->top()->id == kScriptLexParenthesisR
		) {
			PostSyntaxError(result->top()->lex->line, "Parentheses mismatched", 0);
		}
		list->push_front(result->top());
		result->pop();
	}
}

Node::Node(Buffer word, NodeID id, LexNodePtr lex, NodePtr parent, NodePtr prev) :
	word(word),
	id(id),
	lex(lex),
	parent(parent),
	previous(prev)
{
	this->flags = 0;
	this->var = 0;
	this->typeNode = 0;
	this->templateNode = 0;
	this->extendNode = 0;
	this->methodHash = 0;
}

Node::~Node() {

	if (this->typeNode) {
		delete this->typeNode;
	}
	
	if (this->templateNode) {
		delete this->templateNode;
	}
	
	if (this->extendNode) {
		delete this->extendNode;
	}

	for (NodePtr n : this->implementNode) {
		delete n;
	}
}

Void Node::ShuntingYard(Void) {

	if (this->blockList.size() > 0) {
		_Order(&this->blockList);
	}
	if (this->argList.size() > 0) {
		_Order(&this->argList);
	}
}

Void Node::Extend(NodePtr node) {
	this->extendNode = new Node(*node);
}

Void Node::Implement(NodePtr node) {
	this->implementNode.push_back(new Node(*node));
}

Void Node::Template(NodePtr node) {
	this->templateNode = new Node(*node);
}

Void Node::Type(NodePtr node) {
	this->typeNode = new Node(*node);
}

Node::Node(const Node& node) :
	Node(node.word, node.id, node.lex, node.parent, node.previous)
{
	if (node.extendNode) {
		this->Extend(node.extendNode);
	}

	if (node.templateNode) {
		this->Template(node.templateNode);
	}

	if (node.typeNode) {
		this->Type(node.typeNode);
	}

	for (NodePtr n : node.implementNode) {
		this->Implement(n);
	}

	this->flags = node.flags;
}

Node::Node() :
	word(""),
	parent(NULL),
	previous(NULL),
	lex(NULL),
	id(kScriptNodeDefault)
{
	this->flags = 0;
	this->var = 0;
	this->typeNode = 0;
	this->templateNode = 0;
	this->extendNode = 0;
}

LAME_END2
