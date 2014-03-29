#include "Script.h"

LAME_BEGIN

ScriptPerformer& ScriptPerformer::EvaluateConstants(ScriptParser& parser) {

	typedef List<KeyWordPtr>::iterator Iterator;

	List <KeyWordPtr> stack;
	Uint32 arguments;
	KeyWordPtr left;
	KeyWordPtr right;

	for (KeyWordPtr kw: parser.yardQueue) {

		if (kw->key == kKeyWordPriority) {
			if (kw->priority == kPriorityVariable) {
				goto __PushEvaluatable;
			}

			arguments = kw->priority.Arguments();

			if (stack.size() < arguments) {
				PostSyntaxError(kw->line, "Invalid parameters, (%s) needs (%d) arguments", kw->word.data(), arguments);
			}

			if (arguments == 1) {
				goto __PushEvaluatable;
			} else if (arguments == 2) {

				right = stack.back();
				stack.pop_back();
				left = stack.back();
				stack.pop_back();

				if ((left->key == kKeyWordPriority && left->priority == kPriorityVariable) ||
					(right->key == kKeyWordPriority && right->priority == kPriorityVariable)
				) {
					stack.push_back(left);
					stack.push_back(right);
					goto __PushEvaluatable;
				} else {
					this->EvaluateDouble(left, right, kw);
					stack.push_back(left);
				}
			}
		}
		else {
		__PushEvaluatable:
			stack.push_back(kw);
		}
	}

	parser.yardQueue.clear();

	while (stack.size()) {
		parser.yardQueue.push_front(stack.back()); stack.pop_back();
	}

	return *this;
}

ScriptPerformer& ScriptPerformer::FixBraces(ScriptParser& parser) {

	typedef Vector<KeyWordPtr>::iterator Iterator;

	KeyWordPtr self;
	KeyWordPtr next = 0;

	Sint32 rightBraces = 0;

	for (Iterator i = parser.yardQueue.begin(); i != parser.yardQueue.end(); i++) {

		self = *i;
        
		if (i != parser.yardQueue.end() - 1) { next = *(i + 1); }

		if (self->key == kKeyWordPriority && self->priority == kPriorityLanguage) {
			if (next && next->word[0] != '{') {
				parser.keyQueue.push_back(KeyWord().Parse(self->line, "{"));
				i = parser.yardQueue.insert(i + 1, &parser.keyQueue.back());
				++rightBraces;
			}
		}

		if (self->key == kKeyWordSeparator && self->word[0] != '{') {
			while (rightBraces && rightBraces--) {
				parser.keyQueue.push_back(KeyWord().Parse(self->line, "}"));
				i = parser.yardQueue.insert(i + 1, &parser.keyQueue.back());
			}
		}
	}

	return *this;
}

ScriptPerformer& ScriptPerformer::ComputeJumps(ScriptParser& parser) {

	typedef struct {
		KeyWordPtr kw;
		Uint32 distance;
	} JumpKeyWord;

	Vector<JumpKeyWord> stack;
	Uint32 jumpPosition = 0;
	KeyWordPtr lastLang = 0;
	Uint32 distance = 0;

	for (KeyWordPtr self : parser.yardQueue) {
		if (self->key == kKeyWordSeparator) {
			if (self->word[0] == '{') {
				self->jump = jumpPosition;
				if (lastLang) {
					self->language = lastLang->language;
				}
				stack.push_back({ self, distance });
			} else if (self->word[0] == '}') {
				JumpKeyWord back = stack.back();
				back.kw->jump = jumpPosition - back.kw->jump + 1;
				self->jump = -back.kw->jump - back.distance + 3;
				if (lastLang) {
					self->language = lastLang->language;
				}
				stack.pop_back();
				distance = 0;
			} else if (self->word[0] == ';') {
				distance = 1;
			}
		} else if (self->priority == kPriorityLanguage) {
			lastLang = self;
		}
		if (distance) {
			++distance;
		}
		++jumpPosition;
	}

	return *this;
}

ScriptPerformer& ScriptPerformer::EvaluateDouble(KeyWordPtr& left, KeyWordPtr right, KeyWordPtr key) {
    
	StringC savedName = LAME_NULL;

	if (left->key == kKeyWordPriority && left->priority == kPriorityVariable) {
		if (!left->registered) {
			savedName = left->word.data();
			left = (KeyWordPtr)this->FindVariable(left->word.data());
            if (!left) {
				PostSyntaxError(key->line, "Undeclared variable (%s)", savedName);
            }
		}
	}
    
	if (right->key == kKeyWordPriority && right->priority == kPriorityVariable) {
		if (!right->registered) {
			savedName = right->word.data();
			right = (KeyWordPtr)this->FindVariable(right->word.data());
            if (!right) {
				PostSyntaxError(key->line, "Undeclared variable (%s)", savedName);
            }
		}
	}

	if (key->priority.IsRightAssociated()) {
		this->tempList.push_back(*left);
		left = &this->tempList.back();
		left->word = "${temp}";
	}
    
    switch (key->priority) {
		case kPriorityAnd:
			left->BitAnd(*right); break;
		case kPriorityOr:
			left->BitOr(*right); break;
        case kPriorityAbove:
            left->Above(*right); break;
        case kPriorityAboveEqual:
            left->AboveEqual(*right); break;
        case kPriorityBellow:
            left->Bellow(*right); break;
        case kPriorityBellowEqual:
            left->BellowEqual(*right); break;
        case kPriorityEqual:
            left->Equal(*right); break;
        case kPriorityNotEqual:
            left->NotEqual(*right); break;
        default:
            goto __NotCompare;
    }
    
    left->type = kScriptTypeBool;
    goto __Return;
    
__NotCompare:
    if (ScriptVariable::Check(*left, *right)) {
        ScriptVariable::Convert(*left, *right);
    }

	switch (key->priority) {
        case kPrioritySet:
            left->Set(*right); break;
        case kPriorityMul:
        case kPriorityMulSet:
            left->Mul(*right); break;
        case kPriorityDiv:
        case kPriorityDivSet:
            left->Div(*right); break;
        case kPriorityMod:
        case kPriorityModSet:
            left->Mod(*right); break;
        case kPriorityAdd:
        case kPriorityAddSet:
            left->Add(*right); break;
        case kPrioritySub:
        case kPrioritySubSet:
            left->Sub(*right); break;
        case kPriorityShiftL:
        case kPriorityShiftSetL:
            left->BitShiftL(*right); break;
        case kPriorityShiftR:
        case kPriorityShiftSetR:
            left->BitShiftR(*right); break;
        case kPriorityBitAnd:
        case kPriorityBitAndSet:
            left->BitAnd(*right); break;
        case kPriorityBitXor:
        case kPriorityBitXorSet:
            left->BitXor(*right); break;
        case kPriorityBitOr:
        case kPriorityBitOrSet:
            left->BitOr(*right); break;
        default:
            goto __Return;
	}
    
__Return:
	return *this;
}

ScriptPerformer& ScriptPerformer::EvaluateSingle(KeyWordPtr& left, KeyWordPtr key) {

	StringC savedName = LAME_NULL;
	KeyWord local;

	if (left->key == kKeyWordPriority && left->priority == kPriorityVariable) {
		if (!left->registered) {
			savedName = left->word.data();
			left = (KeyWordPtr)this->FindVariable(left->word.data());
			if (!left) {
				PostSyntaxError(key->line, "Undeclared variable (%s)", savedName);
			}
		}
	}

	if (key->priority.IsRightAssociated()) {
		this->tempList.push_back(*left);
		left = &this->tempList.back();
		left->word = "${temp}";
	}

	switch (key->priority) {
		case kPriorityIncrement:
			left->Inc(); break;
		case kPriorityDecrement:
			left->Dec(); break;
        case kPriorityType:
            local = *left; local.type = key->type;
            left->Convert(*left, local);
            break;
        case kPriorityLanguage:
            break;
        case kPriorityDeclare:
            break;
        default:
            break;
	}

	return *this;
}

ScriptPerformer& ScriptPerformer::EvaluateScript(ScriptParser& parser) {

    typedef Vector<KeyWordPtr>::iterator Iterator;
    
	List <KeyWordPtr> stack;
	Uint32 arguments;
	KeyWordPtr left;
	KeyWordPtr right;
    Uint32 count;
    KeyWordPtr kw;
	Uint32 backJump = 0;
	Uint32 removedNodes = 0;

    count = 0;
    
#ifdef LAME_DEBUG
    puts("\n------------------------");
#endif
    
    for (Iterator i = parser.yardQueue.begin(); i != parser.yardQueue.end(); i++) {
        
        kw = *i; ++count;
        
#ifdef LAME_DEBUG
        if (kw->word.length()) {
            if (kw->key == kKeyWordPriority) {
                printf("%s ", kw->word.data());
            } else if (kw->jump != 0) {
                printf("[%d] ", kw->jump);
            } else if (kw->key == kKeyWordSeparator) {
                printf("# ");
            } else {
                switch (kw->type) {
                    case kScriptTypeInt:
                        printf("%d ", kw->intValue);
                        break;
                    case kScriptTypeFloat:
                        printf("%.2f ", kw->floatValue);
                        break;
                    case kScriptTypeString:
                        printf("\"%s\" ", kw->stringValue.data());
                        break;
                    default:
                        printf("%s ", kw->word.data());
                        break;
                }
            }
        }
#endif
        
		if (kw->key == kKeyWordDefault) {
			continue;
		}

		if (kw->key == kKeyWordPriority) {
			if (kw->priority == kPriorityVariable) {
				goto __PushEvaluatable;
			} else if (kw->priority == kPriorityLanguage) {
                this->EvaluateConstruction(&stack, kw);
            } else {
                arguments = kw->priority.Arguments();
                
                if (stack.size() < arguments) {
                    PostSyntaxError(kw->line, "Invalid parameters, (%s) needs (%d) arguments", kw->word.data(), arguments);
                }
                
                if (arguments == 1) {
                    
                    left = stack.back();
                    stack.pop_back();
                    this->EvaluateSingle(left, kw);
                }
                else if (arguments == 2) {
                    
                    right = stack.back();
                    stack.pop_back();
                    left = stack.back();
                    stack.pop_back();
                    this->EvaluateDouble(left, right, kw);
                }
                if (arguments > 0) {
					stack.push_back(left);
                }
            }
		} else if (kw->key == kKeyWordSeparator) {
			while (this->tempList.size()) {
				this->tempList.pop_back();
			}
			if (kw->language == kScriptLanguageWhile) {
				if (kw->word[0] == '}') {
					this->isJump = LAME_TRUE;
				}
			}
			if ((kw->word[0] == '{' || kw->word[0] == '}') && this->isJump) {
				if (this->isJumpPrev) {
					this->isJumpPrev = !this->isJump;
					this->isJump = LAME_TRUE;
				} else {
					this->isJumpPrev = this->isJump;
					this->isJump = LAME_FALSE;
				}
                i += kw->jump - 1;
				if (i + 1 == parser.yardQueue.end()) {
					break;
				}
				if (kw->language == kScriptLanguageWhile) {
					continue;
				}
			}
            count = 0;
        } else {
		__PushEvaluatable:
			stack.push_back(kw);
		}
	}
    
    if (stack.size() > 0) {
        while (count--) {
            parser.yardQueue.pop_front();
        }
        while (stack.size()) {
            parser.yardQueue.push_front(stack.back()); stack.pop_back();
        }
    }

	return *this;
}

ScriptPerformer& ScriptPerformer::RegisterVariables(ScriptParser& parser) {
    
    KeyWordPtr prevKey = LAME_NULL;
    Bool isWaitingComa = LAME_FALSE;
    
    for (KeyWord& kw : parser.keyQueue) {
        if (prevKey && kw.priority == kPriorityVariable && kw.key == kKeyWordPriority) {
            if (isWaitingComa) {
                continue;
            } else {
                this->RegisterVariable(&kw);
                kw.type = prevKey->type;
                prevKey->priority = kPriorityType;
            }
            isWaitingComa = LAME_TRUE;
        } else if (kw.priority == kPriorityDeclare) {
            prevKey = &kw;
        } else if (kw.priority == kPriorityComa) {
            isWaitingComa = LAME_FALSE;
        } else if (kw.key == kKeyWordSeparator && kw.word[0] != ',') {
            if (prevKey) {
                prevKey->Reset();
                prevKey->key = kKeyWordDefault;
            }
            isWaitingComa = LAME_FALSE;
            prevKey = LAME_NULL;
        }
    }
    
    return *this;
}

ScriptPerformer& ScriptPerformer::EvaluateConstruction(List <KeyWordPtr>* stack, KeyWordPtr kw) {
    
    KeyWordPtr argument = 0;

    switch (kw->language) {
        case kScriptLanguageIf:
            
            argument = stack->back();
            stack->pop_back();
            
            if (argument->key == kKeyWordPriority && argument->priority == kPriorityVariable) {
                if (!argument->registered) {
                    argument = (KeyWordPtr)this->FindVariable(argument->word.data());
                    if (!argument) {
                        PostSyntaxError(argument->line, "Undeclared variable (%s)", argument->word.data());
                    }
                }
            }

			this->tempList.push_back(*argument);
			argument = &this->tempList.back();
			argument->ToBool();
			argument->language = kScriptLanguageIf;
            
			if (argument->boolValue) {
                this->isJump = LAME_FALSE;
            } else {
                this->isJump = LAME_TRUE;
            }
            
            break;
        case kScriptLanguageElse:

            this->isJump = !this->isJumpPrev;
            this->isJumpPrev = this->isJump;

            break;
		case kScriptLanguageWhile:

			argument = stack->back();
			stack->pop_back();

			if (argument->key == kKeyWordPriority && argument->priority == kPriorityVariable) {
				if (!argument->registered) {
					argument = (KeyWordPtr)this->FindVariable(argument->word.data());
					if (!argument) {
						PostSyntaxError(argument->line, "Undeclared variable (%s)", argument->word.data());
					}
				}
			}

			this->tempList.push_back(*argument);
			argument = &this->tempList.back();
			argument->ToBool();
			argument->language = kScriptLanguageWhile;

			if (!argument->boolValue) {
				this->isJump = LAME_TRUE;
			} else {
				this->isJump = LAME_FALSE;
			}

			break;
        default:
            PostSyntaxError(kw->line, "Unsupported syntax construction (%s)", kw->word.data());
            break;
    }
    
    return *this;
}

ScriptPerformer& ScriptPerformer::Trace(Void) {

	puts("\n------------------------");
	for (auto i = this->varMap.begin(); i != this->varMap.end(); i++) {
		printf("[%s, %s] : ", i->first.data(), i->second->type.String());
		switch (i->second->type) {
            case kScriptTypeBool:
                printf("%2d", i->second->intValue); break;
                //i->second->boolValue ? printf("TRUE") : printf("FALSE"); break;
            case kScriptTypeFloat:
                printf("%.2f", i->second->floatValue); break;
            case kScriptTypeInt:
                printf("%2d", i->second->intValue); break;
            case kScriptTypeString:
                printf("\"%s\"", i->second->stringValue.data()); break;
            default: break;
		}
		printf("\n");
	}

	return *this;
}

Void ScriptPerformer::RegisterVariable(KeyWordPtr kw) {

	Map<Buffer, ScriptVariablePtr>::iterator where = this->varMap.find(kw->word);

	if (where != this->varMap.end()) {
		PostSyntaxError(kw->line, "Variable redeclaration (%s)", kw->word.data());
	}
	kw->registered = LAME_TRUE;

	this->varMap[kw->word] = kw;
}

ScriptVariablePtr ScriptPerformer::FindVariable(StringC name) {

	Map<Buffer, ScriptVariablePtr>::iterator where = this->varMap.find(name);

	if (where == this->varMap.end()) {
		return LAME_NULL;
	}

	return where->second;
}

LAME_END