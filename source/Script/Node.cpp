#include "Node.h"
#include "Exception.h"

#include <stack>

LAME_BEGIN2(Script)

class ShuntingYard {
public:
	Stack<NodePtr>* Convert(Deque<NodePtr>* list) {

		NodePtr top = NULL;
        
        this->operandsLeft = 0;

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
                this->SaveResult(node);
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
							PostSyntaxError(node->lex->line, "Parentheses mismatched", 0);
						}
                        this->SaveResult(top);
						this->opStack.pop();
					}
                    this->SaveResult(node);
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
				_HandleOperator:
					this->HandleOperator(node);
				}
			}
		}
		while (!this->opStack.empty()) {
            this->SaveResult(this->opStack.top());
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
            this->SaveResult(this->opStack.top());
			this->opStack.pop();
		}
		if (this->opStack.empty() || (!this->opStack.empty() && (
			this->opStack.top()->lex->lex->id != kScriptLexParenthesisL &&
			this->opStack.top()->lex->lex->id != kScriptLexBracketL))
		) {
			PostSyntaxError(node->lex->line, "Parentheses mismatched", 0);
		}
		this->opStack.pop();
	}
	inline void HandleOperator(NodePtr node) {
        if (node->lex->lex->args > 0) {
            this->operandsLeft -= node->lex->lex->args;
            if (this->operandsLeft == -2) {
                if (
                    node->lex->lex->id == kScriptLexAdd ||
                    node->lex->lex->id == kScriptLexSub
                ) {
                    LexPtr lex = Lex::Find(kScriptLexUnaryMinus);
                    if (node->lex->lex->id == kScriptLexAdd) {
                        lex = Lex::Find(kScriptLexUnaryPlus);
                    }
                    node->lex->priority = lex->priority;
                    node->lex->args = lex->args;
                }
            }
            this->operandsLeft = 0;
        }
		while (!this->opStack.empty() && this->GetPrecedence(node) <= this->StackPrecedence()) {
            this->SaveResult(this->opStack.top());
			this->opStack.pop();
		}
		this->opStack.push(node);
	}
private:
    inline void SaveResult(NodePtr node) {
        if (node->lex->lex->IsUnknown() ||
            node->lex->lex->IsConst()
        ) {
            ++this->operandsLeft;
        }
        this->resultStack.push(node);
    }
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
    Sint32 operandsLeft;
};

Void _Order(Deque<NodePtr>* list) {

	Stack<NodePtr>* result;
	ShuntingYard shuntingYard;
    
	result = shuntingYard.Convert(list);

	list->clear();
	while (!result->empty()) {
		if (result->top()->lex->lex->id == kScriptLexBraceL ||
			result->top()->lex->lex->id == kScriptLexBraceR
		) {
			PostSyntaxError(result->top()->lex->line, "Braces mismatched", 0);
		}
		if (result->top()->lex->lex->id == kScriptLexParenthesisL ||
			result->top()->lex->lex->id == kScriptLexParenthesisR
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
	if (this->extendNode) {
		delete this->extendNode;
		this->extendNode = NULL;
	}
	this->extendNode = new Node(*node);
}

Void Node::Implement(NodePtr node) {
	this->implementNode.push_back(new Node(*node));
}

Void Node::Template(NodePtr node) {
	if (this->templateNode) {
		delete this->templateNode;
		this->templateNode = NULL;
	}
	this->templateNode = new Node(*node);
}

Void Node::Type(NodePtr node) {
	if (this->typeNode) {
		delete this->typeNode;
		this->typeNode = NULL;
	}
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
