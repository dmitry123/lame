#include "Node.h"
#include "Exception.h"

#include <stack>

LAME_BEGIN2(Script)

class ShuntingYard {
public:
	Core::Stack<NodePtr>* Convert(Deque<NodePtr>* list) {

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
			if (this->operandsLeft == -1) {
				if (node->lex->lex->id == kScriptLexPostfixIncrement) {
					node->lex->ChangeLex(Lex::Find(kScriptLexPrefixIncrement));
				}
				if (node->lex->lex->id == kScriptLexPostfixDecrement) {
					node->lex->ChangeLex(Lex::Find(kScriptLexPrefixDecrement));
				}
			}
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
	Core::Stack<NodePtr> resultStack;
	Core::Stack<NodePtr> opStack;
    Sint32 operandsLeft;
};

Void _Order(Deque<NodePtr>* list) {

	Core::Stack<NodePtr>* result;
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
	this->elseNode = NULL;
	this->methodInfo.invokeHash = 0;
	this->classInfo.extendNode = NULL;
	this->templateNode = NULL;
	this->typeNode = NULL;
	this->var = NULL;
	this->flags = 0;
	this->next = NULL;
	this->depth = 0;
}

Void Node::ShuntingYard(Void) {

	if (this->blockList.size() > 0) {
		_Order(&this->blockList);
	}
	if (this->argList.size() > 0) {
		_Order(&this->argList);
	}
	if (this->elseList.size() > 0) {
		_Order(&this->argList);
	}
	if (this->forInfo.beginList.size() > 0) {
		_Order(&this->forInfo.beginList);
	}
	if (this->forInfo.conditionList.size() > 0) {
		_Order(&this->forInfo.conditionList);
	}
	if (this->forInfo.nextList.size() > 0) {
		_Order(&this->forInfo.nextList);
	}
}

Void Node::Extend(NodePtr node) {
	this->classInfo.extendNode = node;
}

Void Node::Implement(NodePtr node) {
	this->classInfo.implementNode.push_back(node);
}

Void Node::Template(NodePtr node) {
	this->templateNode = node;
}

Void Node::Type(NodePtr node) {
	this->typeNode = node;
}

LAME_END2
