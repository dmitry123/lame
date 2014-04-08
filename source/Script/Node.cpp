#include "Script.h"

LAME_BEGIN

Void ScriptNode::Order(ScriptPerformerPtr performer) {

	if (this->condition.size()) {
		this->Order(performer, &this->condition);
	}
	if (this->block.size()) {
		this->Order(performer, &this->block);
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
                
                if (!this->block.size()) {
                    break;
                }

				performer->Evaluate(&this->condition, &result);

				if (result.size() != 1) {
					PostSyntaxError(this->object->line,
						"Invalid condition expression, the result of condition isn't 1 argument", 1);
				}
				performer->AsBool(*result.back()->object->var);

				this->result = result.back()->object->var->boolValue;
				result.clear();

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
                
                if (!this->block.size()) {
                    break;
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
				result.clear();
                
                if (this->parent->object->flag == kScriptDo) {
                    if (this->parent->block.size()) {
                        performer->Evaluate(&this->parent->block, &result);
                        result.clear();
                    } 
                }
                
				if (!this->result) {
					goto __ExitExpression;
				}

				if (this->parent && this->parent->object->flag == kScriptDo) {
                    if (!this->parent->block.size()) {
                        break;
                    }
					do {
						result.clear();
						if (this->parent->block.size()) {
							performer->Evaluate(&this->parent->block, &result);
						}
						performer->Evaluate(&this->condition, &result);
						performer->AsBool(*result.back()->object->var);
						this->result = result.back()->object->var->boolValue;
					} while (this->result);
				}
				else {
                    if (!this->block.size()) {
                        break;
                    }
					while (this->result) {
						performer->Evaluate(&this->condition, &result);
						performer->AsBool(*result.back()->object->var);
						this->result = result.back()->object->var->boolValue;
						if (this->block.size()) {
							performer->Evaluate(&this->block, &result);
						}
						result.clear();
					}
				}
				goto __ExitExpression;

				break;
			case kScriptDo:
				goto __ExitExpression;
			case kScriptFor: {
                
                if (!this->block.size()) {
                    break;
                }
                
                Vector<ScriptNodePtr> expInit;
                Vector<ScriptNodePtr> expCondition;
                Vector<ScriptNodePtr> expInc;
                
                Vector<ScriptNodePtr>::iterator i = this->condition.begin();
                
                if (i == this->condition.end()) {
                    goto __ForArgError;
                }
                
                while ((*i)->object->flag != kScriptSemicolon) {
                    if (i == this->condition.end()) {
                        goto __ForArgError;
                    }
                    expInit.push_back(*i); ++i;
                }
                if (i == this->condition.end()) {
                    goto __ForArgError;
                } ++i;
                while ((*i)->object->flag != kScriptSemicolon) {
                    if (i == this->condition.end()) {
                        goto __ForArgError;
                    }
                    expCondition.push_back(*i); ++i;
                }
                if (i == this->condition.end()) {
                    goto __ForArgError;
                } ++i;
                while (i != this->condition.end() && (*i)->object->flag != kScriptSemicolon) {
                    if (i == this->condition.end()) {
                        goto __ForArgError;
                    }
                    expInc.push_back(*i); ++i;
                }
                
                goto __EvalCondition;
            __ForArgError:
					PostSyntaxError(this->object->line, "Invalid condition expression, 'for' construction must have 3 parameters", 1);
            __EvalCondition:
                
                if (expInit.size()) {
                    performer->Evaluate(&expInit, &result);
                }
                result.clear();
                
                while (LAME_TRUE) {
                    
                    if (expCondition.size()) {
                        performer->Evaluate(&expCondition, &result);
                        
                        if (result.size() > 1) {
                            PostSyntaxError(this->object->line, "Invalid condition expression in 'for' construction", 1);
                        }
						performer->AsBool(*result.back()->object->var);
						this->result = result.back()->object->var->boolValue;
                        result.clear();
                        
                        if (!this->result) {
                            break;
                        }
                    }
                    
                    if (this->block.size()) {
                        performer->Evaluate(&this->block, &result);
                    }
                    result.clear();
                    
                    if (expInc.size()) {
                        performer->Evaluate(&expInc, &result);
                    }
                    result.clear();
                }
            } break;
		case kScriptFunction: {

			if (!this->condition.size()) {
				PostSyntaxError(this->object->line, "Invalid function's arguments", 1);
			}

			const Buffer& functionName = this->condition.back()->object->word;
			this->condition.pop_back();

			performer->varManager.Declare(this->object);

			} break;
		default:
			break;
		}
	}

__ExitExpression:
	return;
}

Void ScriptNode::Order(ScriptPerformerPtr performer, Vector<ScriptNodePtr>* list) {

	List<ScriptNodePtr> stack;
	List<ScriptNodePtr> result;

	Bool found = 0;
	ScriptNodePtr back = 0;
	ScriptTypePtr type = 0;
	ScriptObjectPtr object = 0;

	for (ScriptNodePtr node : *list) {

		if (!node->object) {
			continue;
		}

		if (node->object->flag == kScriptDefault) {
			type = performer->typeManager.Find(node->object->word.data());
			if (type) {
				node->object->args = type->object->args;
				node->object->associativity = type->object->associativity;
				node->object->flag = type->object->flag;
				node->object->priority = type->object->priority;
				node->object->type = type;
			}
			else {
				object = object->FindScriptObjectByFlag(kScriptVariable);
				node->object->args = object->args;
				node->object->associativity = object->associativity;
				node->object->flag = object->flag;
				node->object->priority = object->priority;
				node->object->type = object->type;
			}
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