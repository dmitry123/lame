#include "Node.h"
#include "Exception.h"
#include "GlobalScope.h"

#include <stack>

LAME_BEGIN2(Script)

class ShuntingYard {
public:
	inline Stack<NodePtr>* Convert(Vector<NodePtr>* list) {

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
							top->lex->lex->id == kScriptLexParentheseR
						) {
							PostSyntaxError(node->lex->line, "Parenthese mismatched", 0);
						}
						this->resultStack.push(top);
						this->opStack.pop();
					}
				}
				else if (node->lex->lex->id == kScriptLexParentheseL) {
					this->HandleLeftParenthesis(node);
				}
				else if (node->lex->lex->id == kScriptLexParentheseR) {
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
		while (!this->opStack.empty() && this->opStack.top()->lex->lex->id != kScriptLexParentheseL) {
			this->resultStack.push(this->opStack.top());
			this->opStack.pop();
		}
		if (this->opStack.empty() || !this->opStack.empty() && this->opStack.top()->lex->lex->id != kScriptLexParentheseL) {
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

	return;

	//List<NodePtr> stack;
	//List<NodePtr> result;

	//Bool isFound = 0;
	//NodePtr back = 0;
	//Bool isSelectionLast = 0;
	//NodePtr prev = 0;

	//for (NodePtr node : *list) {
	//	break;
	//	if (isSelectionLast) {
	//		result.push_back(node);
	//		if (!stack.size()) {
	//			PostSyntaxError(node->lex->line, "Very strange field selection", 1);
	//		}
	//		result.push_back(stack.back());
	//		stack.pop_back();
	//		isSelectionLast = LAME_FALSE;
	//		goto __Continue;
	//	}
	//	isSelectionLast = LAME_FALSE;

	//	if (
	//		node->lex->lex->IsConst() ||
	//		node->lex->lex->IsUnknown() ||
	//		node->lex->lex->IsCondition() ||
	//		node->lex->lex->IsClass()
	//	) {
	//		result.push_back(node);
	//	}
	//	else if (
	//		node->lex->lex->id == kScriptLexParentheseL ||
	//		node->lex->lex->id == kScriptLexBracketL
	//	) {
	//		if (prev && prev->lex->lex->IsUnknown()) {
	//			stack.push_back(prev);
	//			prev->MakeInvoke();
	//			result.pop_back();
	//		}
	//		stack.push_back(node);
	//	}
	//	else if (
	//		node->lex->lex->id == kScriptLexParentheseR ||
	//		node->lex->lex->id == kScriptLexBracketR
	//	) {
	//		isFound = 0;
	//		while (stack.size()) {
	//			back = stack.back();
	//			if (back->lex->lex->id == kScriptLexParentheseL ||
	//				back->lex->lex->id == kScriptLexBracketL
	//			) {
	//				isFound = 1; break;
	//			}
	//			else {
	//				result.push_back(back);
	//				stack.pop_back();
	//			}
	//		}
	//		if (!isFound) {
	//			PostSyntaxError(node->lex->line, "Parentheses mismatched", 1);
	//		}
	//		else {
	//			stack.pop_back();
	//			if (stack.size()) {
	//				back = stack.back();
	//				if (back->lex->lex->IsCondition() ||
	//					back->lex->lex->IsUnknown()
	//				) {
	//					result.push_back(back);
	//					stack.pop_back();
	//				}
	//			}
	//		}
	//	}
	//	else if (node->lex->lex->id == kScriptLexSemicolon) {
	//		while (stack.size()) {
	//			back = stack.back();
	//			if (back->lex->lex->id == kScriptLexParentheseL ||
	//				back->lex->lex->id == kScriptLexParentheseR ||
	//				back->lex->lex->id == kScriptLexBracketL ||
	//				back->lex->lex->id == kScriptLexBracketR
	//			) {
	//				PostSyntaxError(back->lex->line, "Parentheses mismatched", 1);
	//			}
	//			else {
	//				result.push_back(back);
	//				stack.pop_back();
	//			}
	//		}
	//	}
	//	else if (node->lex->lex->id == kScriptLexComma) {
	//		continue;
	//	}
	//	else {
	//		while (stack.size()) {
	//			back = stack.back();
	//			if (((node->lex->lex->IsLeft() && node->lex->lex->priority >= back->lex->lex->priority)) ||
	//				(!node->lex->lex->IsLeft() && node->lex->lex->priority > back->lex->lex->priority))
	//			{
	//				result.push_back(back);
	//				stack.pop_back();
	//			}
	//			else {
	//				break;
	//			}
	//		}
	//		if (node->lex->lex->id == kScriptLexMediated ||
	//			node->lex->lex->id == kScriptLexDirected
	//		) {
	//			isSelectionLast = LAME_TRUE;
	//		}
	//		stack.push_back(node);
	//	}
	//__Continue:
	//	prev = node;
	//}

	//while (stack.size()) {
	//	back = stack.back();
	//	if (back->lex->lex->id == kScriptLexParentheseL ||
	//		back->lex->lex->id == kScriptLexParentheseR ||
	//		back->lex->lex->id == kScriptLexBracketL ||
	//		back->lex->lex->id == kScriptLexBracketR
	//	) {
	//		PostSyntaxError(back->lex->line, "Parentheses mismatched", 1);
	//	}
	//	else if (back->id != kScriptNodeDefault) {
	//		result.push_back(back);
	//	}
	//	else {
	//		result.push_back(back);
	//	}
	//	stack.pop_back();
	//}

	//list->clear();
	//while (result.size()) {
	//	list->push_back(result.front());
	//	result.pop_front();
	//}
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
}

Node::~Node() {
}

ScopeControllerPtr Node::GetScope() {

	if (this->id == kScriptNodeEntry) {
		return GlobalScope::GetInstance();
	}
	return this->var->GetScopeController();
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
