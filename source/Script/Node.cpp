#include "Script.h"

LAME_BEGIN

Void ScriptNode::Order(Void) {

	if (this->condition.size()) {
		this->Order(&this->condition);
	}
	if (this->block.size()) {
		this->Order(&this->block);
	}
}

Void ScriptNode::Evaluate(ScriptPerformerPtr performer) {

	Vector<ScriptNodePtr> result;

	if (this->object->IsCondition()) {

		if (!this->condition.size() && this->object->IsBinary()) {
			PostSyntaxError(this->object->line, "Condition block hasn't expression", 1);
		}

		switch (this->object->flag) {
			case kScriptIf:

				performer->Evaluate(&this->condition, &result);

				if (result.size() != 1) {
					PostSyntaxError(this->object->line,
						"Invalid condition expression, the result of condition isn't 1 argument", 1);
				}
				performer->AsBool(*result.back()->object->var);

				this->result = result.back()->object->var->boolValue;

				if (this->result) {
					if (this->block.size()) {
						performer->Evaluate(&this->block, &result);
					}
				}
				goto __ExitExpression;

				break;
			case kScriptElse:

				if (!this->parent) {
					PostSyntaxError(this->object->line, "Else construction must have 'if' node", 1);
				}
				if (!this->parent->result) {
					if (this->block.size()) {
						performer->Evaluate(&this->block, &result);
					}
				}
				goto __ExitExpression;

				break;
			case kScriptWhile:

				performer->Evaluate(&this->condition, &result);

				if (result.size() != 1) {
					PostSyntaxError(this->object->line,
						"Invalid condition expression, the result of condition isn't 1 argument", 1);
				}
				performer->AsBool(*result.back()->object->var);

				this->result = result.back()->object->var->boolValue;

				if (!this->result) {
					goto __ExitExpression;
				}

				if (this->parent && this->parent->object->flag == kScriptDo) {
					do {
						if (this->parent->block.size()) {
							performer->Evaluate(&this->parent->block, &result);
						}
						performer->Evaluate(&this->condition, &result);
						this->result = result.back()->object->var->boolValue;
					} while (this->result);
				}
				else {
					while (this->result) {
						performer->Evaluate(&this->condition, &result);
						this->result = result.back()->object->var->boolValue;
						if (this->block.size()) {
							performer->Evaluate(&this->block, &result);
						}
					}
				}
				goto __ExitExpression;

				break;
			case kScriptDo:

				if (this->block.size()) {
					performer->Evaluate(&this->block, &result);
				}
				goto __ExitExpression;

				break;
			case kScriptFor:

				PostSyntaxError(this->object->line,
					"'for' construction support later", 1);

				performer->Evaluate(&this->condition, &result);

				if (result.size() < 3) {
					PostSyntaxError(this->object->line,
						"Invalid condition expression, 'for' construction must have 3 parameters", 1);
				}

				break;
		default:
			break;
		}
	}

__ExitExpression:
	return;
}

Void ScriptNode::Order(Vector<ScriptNodePtr>* list) {

	List<ScriptNodePtr> stack;
	List<ScriptNodePtr> result;

	Bool found = 0;
	ScriptNodePtr back = 0;

	for (ScriptNodePtr node : *list) {

		if (!node->object) {
			continue;
		}

		if (node->object->flag == kScriptDeclare) {
			continue;
		}

		if ( // If object constant or variable
			node->object->flag == kScriptInt  ||
			node->object->flag == kScriptFloat ||
			node->object->flag == kScriptString ||
			node->object->flag == kScriptDefault ||
			// If object condition
			node->object->IsCondition()
		) {
			result.push_back(node);
		}
		else if (node->object->flag == kScriptParentheseL) {
			stack.push_back(node);
		}
		else if (node->object->flag == kScriptParentheseR) {
			found = 0;
			while (stack.size()) {
				back = stack.back();
				if (back->object->flag == kScriptParentheseL) {
					found = 1; break;
				}
				else {
					result.push_back(back);
					stack.pop_back();
				}
			}
			if (!found) {
				PostSyntaxError(node->object->line, "Parentheses mismatched", 1);
			}
			else {
				stack.pop_back();
				if (stack.size()) {
					back = stack.back();
					if (back->object->IsCondition() ||
						back->object->flag == kScriptDefault
					) {
						result.push_back(back);
						stack.pop_back();
					}
				}
			}
		}
		else if (node->object->flag == kScriptSemicolon) {
			while (stack.size()) {
				back = stack.back();
				if (back->object->flag == kScriptParentheseL ||
					back->object->flag == kScriptParentheseR
				) {
					PostSyntaxError(back->object->line, "Parentheses mismatched", 1);
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
				if (((node->object->IsLeftAssociated() && node->object->priority >= back->object->priority)) ||
					(!node->object->IsLeftAssociated() && node->object->priority > back->object->priority))
				{
					result.push_back(back);
					stack.pop_back();
				}
				else {
					break;
				}
			}
			stack.push_back(node);
		}
	}

	while (stack.size()) {
		back = stack.back();
		if (back->object->flag == kScriptParentheseL ||
			back->object->flag == kScriptParentheseR
		) {
			PostSyntaxError(back->object->line, "Parentheses mismatched", 1);
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

LAME_END