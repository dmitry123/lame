#include "Script.h"

LAME_BEGIN

Void ScriptNode::Reset(Void) {

	this->next = 0;
	this->prev = 0;
	this->conditionList.clear();
	this->blockList.clear();
	this->fieldList.clear();
	this->parent = 0;
	this->var = 0;
	this->node = kScriptNodeDefault;
}

Void ScriptNode::Order(Void) {

	if (this->conditionList.size()) {
		this->_Order(&this->conditionList);
	}
	if (this->blockList.size()) {
		this->_Order(&this->blockList);
	}
	if (this->fieldList.size()) {
		this->_Order(&this->fieldList);
	}
}

ScriptNodePtr ScriptNode::_Find(NodeListPtr list, BufferRefC name) {

	for (ScriptNodePtr n : *list) {
		if (n->var->name == name) {
			return n;
		}
	}

	return LAME_NULL;
}

Void ScriptNode::_Order(NodeListPtr list) {

	List<ScriptNodePtr> stack;
	List<ScriptNodePtr> result;

	Bool found = 0;
	ScriptNodePtr back = 0;
	Bool isSelectionLast = 0;

	for (ScriptNodePtr node : *list) {

		if (!node->var->lex) {
			continue;
		}

		if (isSelectionLast) {
			result.push_back(node);
			if (!stack.size()) {
				PostSyntaxError(node->var->lex->line, "Very strange field selection", 1);
			}
			result.push_back(stack.back());
			stack.pop_back();
			isSelectionLast = LAME_FALSE;
			continue;
		}
		isSelectionLast = LAME_FALSE;

		if ( // If object constant or variable
			node->var->lex->object == kScriptObjectInt ||
			node->var->lex->object == kScriptObjectFloat ||
			node->var->lex->object == kScriptObjectString ||
			node->var->lex->object == kScriptObjectDefault ||
			node->var->lex->object == kScriptObjectVariable ||
			// If object condition
			node->var->lex->IsCondition()
		) {
			result.push_back(node);
		}
		else if (node->var->lex->object == kScriptObjectParentheseL) {
			stack.push_back(node);
		}
		else if (node->var->lex->object == kScriptObjectParentheseR) {
			found = 0;
			while (stack.size()) {
				back = stack.back();
				if (back->var->lex->object == kScriptObjectParentheseL) {
					found = 1; break;
				}
				else {
					result.push_back(back);
					stack.pop_back();
				}
			}
			if (!found) {
				PostSyntaxError(node->var->lex->line, "Parentheses mismatched", 1);
			}
			else {
				stack.pop_back();
				if (stack.size()) {
					back = stack.back();
					if (back->var->lex->IsCondition() ||
						back->var->lex->object == kScriptObjectDefault
						) {
						result.push_back(back);
						stack.pop_back();
					}
				}
			}
		}
		else if (node->var->lex->object == kScriptObjectSemicolon) {
			while (stack.size()) {
				back = stack.back();
				if (back->var->lex->object == kScriptObjectParentheseL ||
					back->var->lex->object == kScriptObjectParentheseR
					) {
					PostSyntaxError(back->var->lex->line, "Parentheses mismatched", 1);
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
				if (((node->var->lex->IsLeftAssociated() && node->var->lex->priority >= back->var->lex->priority)) ||
					(!node->var->lex->IsLeftAssociated() && node->var->lex->priority > back->var->lex->priority))
				{
					result.push_back(back);
					stack.pop_back();
				}
				else {
					break;
				}
			}
			if (node->var->lex->object == kScriptObjectMediated ||
				node->var->lex->object == kScriptObjectDirect
			) {
				isSelectionLast = LAME_TRUE;
			}
			stack.push_back(node);
		}
	}

	while (stack.size()) {
		back = stack.back();
		if (back->var->lex->object == kScriptObjectParentheseL ||
			back->var->lex->object == kScriptObjectParentheseR
		) {
			PostSyntaxError(back->var->lex->line, "Parentheses mismatched", 1);
		}
		else {
			result.push_back(back);
			stack.pop_back();
		}
	}

	list->clear();
	while (result.size()) {
		list->push_back(result.front());
		result.pop_front();
	}
}

ScriptNode::~ScriptNode() {


}

LAME_END