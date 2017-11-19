#ifndef __LAME_LANG__SHUNTING_YARD__
#define __LAME_LANG__SHUNTING_YARD__

#include "Node.h"
#include "SyntaxError.h"

LAME_LANG_BEGIN

typedef class ShuntingYard {
public:
	ShuntingYard() : left(0)
	{
	}
public:

	const Stack<NodePtr>& convert(Vector<NodePtr>& list) {

		if (list.empty()) {
			return this->result;
		}

		while (!this->result.empty()) {
			this->result.pop();
		}

		Vector<NodePtr> items;

		for (auto n : list) {
			if (n->getToken()->is(';')) {
				this->perform(items);
				items.clear();
			}
			else {
				items.push_back(n);
			}
		}

		if (!items.empty()) {
			this->perform(items);
		}

		Stack<NodePtr> reverse;

		while (!this->result.empty()) {
			reverse.push(this->result.top()); this->result.pop();
		}

		list.clear();

		while (!reverse.empty()) {
			list.push_back(reverse.top()); reverse.pop();
		}

		return this->result;
	}

	const Stack<NodePtr>& perform(const Vector<NodePtr>& list) {

		for (auto n : list) {
			if (n->getToken()->isOperator()) {
				this->handleOperator(n);
			}
			else if (n->getToken()->is('(')) {
				this->handleLeftParentheses(n);
			}
			else if (n->getToken()->is(')')) {
				this->handleRightParentheses(n);
			}
			else {
				this->result.push(n);
			}
		}

		while (!this->stack.empty()) {
			this->result.push(this->stack.top()); this->stack.pop();
		}

		return this->result;
	}

protected:

	void handleOperator(NodePtr n) {

		while (!this->stack.empty() && !this->stack.top()->getToken()->isOperator()) {

			if ((n->getToken()->isLeft() && n->getToken()->precedence >= this->stack.top()->getToken()->precedence) ||
				(n->getToken()->isRight() && n->getToken()->precedence > this->stack.top()->getToken()->precedence)
			) {
				this->result.push(this->stack.top()); this->stack.pop(); continue;
			}

			break;
		}

		this->stack.push(n);
	}

	void handleLeftParentheses(NodePtr n) {
		this->stack.push(n);
	}

	void handleRightParentheses(NodePtr n) {

		while (!this->stack.empty() && !this->stack.top()->getToken()->is('(')) {
			this->result.push(this->stack.top()); this->stack.pop();
		}

		this->stack.pop();
	}
public:
	Stack<NodePtr> result;
	UInt32 left;
	Stack<NodePtr> stack;
} *ShuntingYardPtr;

LAME_LANG_END

#endif // ~__LAME_LANG__SHUNTING_YARD__