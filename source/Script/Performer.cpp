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

	Vector<KeyWordPtr> stack;
	Uint32 jumpPosition = 0;

	for (KeyWordPtr self : parser.yardQueue) {
		if (self->key == kKeyWordSeparator) {
			if (self->word[0] == '{') {
				self->jump = jumpPosition;
				stack.push_back(self);
			} else if (self->word[0] == '}') {
				KeyWordPtr back = stack.back();
				back->jump = jumpPosition - back->jump + 1;
				self->jump = -back->jump;
				stack.pop_back();
			}
		}
		++jumpPosition;
	}

	return *this;
}

ScriptPerformer& ScriptPerformer::EvaluateDouble(KeyWordPtr& left, KeyWordPtr right, KeyWordPtr key) {
    
	if (left->key == kKeyWordPriority && left->priority == kPriorityVariable) {
		if (!left->registered) {
			left = (KeyWordPtr)this->FindVariable(left->word.data());
            if (!left) {
                PostSyntaxError(left->line, "Undeclared variable (%s)", left->word.data());
            }
		}
	}
    
	if (right->key == kKeyWordPriority && right->priority == kPriorityVariable) {
		if (!right->registered) {
			right = (KeyWordPtr)this->FindVariable(right->word.data());
            if (!right) {
                PostSyntaxError(right->line, "Undeclared variable (%s)", right->word.data());
            }
		}
	}

	if (key->priority.IsRightAssociated()) {
		if (right == &this->temporary) {
			std::swap(left, right);
		}
		else {
			if (this->temporary.type != left->type) {
				this->temporary = *left;
			}
			left = &this->temporary;
			left->word = "TEMP";
		}
	}
    
    switch (key->priority) {
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

	KeyWord local;
    
	if (key->priority.IsRightAssociated()) {
		if (this->temporary.type != left->type) {
			this->temporary = *left;
		}
		left = &this->temporary;
		left->word = "TEMP";
	}

	switch (key->priority) {
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
            if (kw->word[0] == '{' && this->isJump) {
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
            }
            while (--count) {
				i = parser.yardQueue.erase(i) - 1;
            }
			if (stack.size()) {
				stack.pop_back();
			}
            while (stack.size()) {
				if (*i != stack.back()) {
					i = parser.yardQueue.insert(i, stack.back());
				}
				stack.pop_back();
            }
			if (i == parser.yardQueue.end()) {
				break;
			}
			this->temporary.Reset();
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
            
            this->temporary = *argument;
            this->temporary.ToBool();
            
            if (this->temporary.boolValue) {
                this->isJump = LAME_FALSE;
            } else {
                this->isJump = LAME_TRUE;
            }
            
            break;
        case kScriptLanguageElse:
            this->isJump = !this->isJumpPrev;
            this->isJumpPrev = this->isJump;
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
//                i->second->boolValue ? printf("TRUE") : printf("FALSE"); break;
            case kScriptTypeFloat:
                printf("%.2f", i->second->floatValue); break;
            case kScriptTypeInt:
                printf("%2d", i->second->intValue); break;
            case kScriptTypeString:
                printf("%s", i->second->stringValue.data()); break;
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