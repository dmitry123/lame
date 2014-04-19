#include "Script.h"

LAME_BEGIN

Void ScriptNode::Order(Void) {

	if (this->childList.size()) {
		this->_Order(&this->childList);
	}
	if (this->argList.size()) {
		this->_Order(&this->argList);
	}
}

ScriptNodePtr ScriptNode::FindChild(BufferRefC name) {

	for (ScriptNodePtr node : this->childList) {
		if (node->word == name) {
			return node;
		}
	}
	return LAME_NULL;
}

Void ScriptNode::_Order(Vector<ScriptNodePtr>* list) {

	List<ScriptNodePtr> stack;
	List<ScriptNodePtr> result;

	Bool isFound = 0;
	ScriptNodePtr back = 0;
	Bool isSelectionLast = 0;
	ScriptNodePtr prev = 0;

	for (ScriptNodePtr node : *list) {

		if (isSelectionLast) {
			result.push_back(node);
			if (!stack.size()) {
				PostSyntaxError(node->lex->line, "Very strange field selection", 1);
			}
			result.push_back(stack.back());
			stack.pop_back();
			isSelectionLast = LAME_FALSE;
			goto __Continue;
		}
		isSelectionLast = LAME_FALSE;

		if (
			node->lex->IsConst() ||
			node->lex->IsUnknown()
		) {
			result.push_back(node);
		}
		else if (node->lex->id == kScriptLexParentheseL) {
			if (prev && prev->lex->IsUnknown()) {
				stack.push_back(prev);
				prev->MakeInvoke();
				result.pop_back();
			}
			stack.push_back(node);
		}
		else if (node->lex->id == kScriptLexParentheseR) {
			isFound = 0;
			while (stack.size()) {
				back = stack.back();
				if (back->lex->id == kScriptLexParentheseL) {
					isFound = 1; break;
				}
				else {
					result.push_back(back);
					stack.pop_back();
				}
			}
			if (!isFound) {
				PostSyntaxError(node->lex->line, "Parentheses mismatched", 1);
			}
			else {
				stack.pop_back();
				if (stack.size()) {
					back = stack.back();
					if (back->lex->IsCondition() ||
						back->lex->IsUnknown()
					) {
						result.push_back(back);
						stack.pop_back();
					}
				}
			}
		}
		else if (node->lex->id == kScriptLexSemicolon) {
			while (stack.size()) {
				back = stack.back();
				if (back->lex->id == kScriptLexParentheseL ||
					back->lex->id == kScriptLexParentheseR
				) {
					PostSyntaxError(back->lex->line, "Parentheses mismatched", 1);
				}
				else {
					result.push_back(back);
					stack.pop_back();
				}
			}
			result.push_back(node);
		}
		else {
			while (stack.size()) {
				back = stack.back();
				if (((node->lex->IsLeft() && node->lex->priority >= back->lex->priority)) ||
					(!node->lex->IsLeft() && node->lex->priority > back->lex->priority))
				{
					result.push_back(back);
					stack.pop_back();
				}
				else {
					break;
				}
			}
			if (node->lex->id == kScriptLexMediated ||
				node->lex->id == kScriptLexDirected
			) {
				isSelectionLast = LAME_TRUE;
			}
			stack.push_back(node);
		}
	__Continue:
		prev = node;
	}

	while (stack.size()) {
		back = stack.back();
		if (back->lex->id == kScriptLexParentheseL ||
			back->lex->id == kScriptLexParentheseR
		) {
			PostSyntaxError(back->lex->line, "Parentheses mismatched", 1);
		}
		else if (back->id != kScriptNodeDefault) {
			result.push_front(back);
		}
		else {
			result.push_back(back);
		}
		stack.pop_back();
	}

	list->clear();
	while (result.size()) {
		list->push_back(result.front());
		result.pop_front();
	}
}

LAME_END