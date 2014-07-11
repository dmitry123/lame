#include "Node.h"
#include "Exception.h"
#include "GlobalScope.h"
#include "Variable.h"

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
						if (top->lex->lex->id == kScriptLexParentheseL ||
							top->lex->lex->id == kScriptLexParentheseR ||
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
					node->lex->lex->id == kScriptLexParentheseL ||
					node->lex->lex->id == kScriptLexBracketL
				) {
					this->HandleLeftParenthesis(node);
				}
				else if (
					node->lex->lex->id == kScriptLexParentheseR ||
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
			this->opStack.top()->lex->lex->id != kScriptLexParentheseL &&
			this->opStack.top()->lex->lex->id != kScriptLexBracketL)
		) {
			this->resultStack.push(this->opStack.top());
			this->opStack.pop();
		}
		if (this->opStack.empty() || !this->opStack.empty() && (
			this->opStack.top()->lex->lex->id != kScriptLexParentheseL &&
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
	this->modificators = 0;
	this->var = 0;
	this->isMethodImpl = 0;
	this->isArray = 0;
	this->thisVar = 0;
}

Node::~Node() {

}

ScopeControllerPtr Node::GetScope() {

	if (this->id == kScriptNodeEntry) {
		return GlobalScope::GetInstance();
	}
	if (this->thisVar) {
		return this->thisVar->GetScopeController();
	}
	else {
		return this->var->GetScopeController();
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

LAME_END2
