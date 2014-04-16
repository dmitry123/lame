#include "Script.h"
#include "Internal.h"

LAME_BEGIN

Void ScriptVirtualMachine::Execute(Void) {
    
    Vector<ScriptNodePtr> result;
    ScriptNodePtr main;
    
    // find main thread
    main = this->root->FindBlock("thread");
    
    // check for existance
    if (!main) {
        PostSyntaxError(0, "Unable to find entry point", 1);
    }
    
    // evaluate main block
	this->_Evaluate(&main->blockList, &result);
}

Void ScriptVirtualMachine::Trace(Void) {
    
    // set trace depth to zero
    this->traceDepth_ = 0;
    
	puts("\n\n----------");
	printf("- TYPES -\n");
	puts("----------\n");
	for (auto& vm : this->manager.typeMapQueue) {
		for (auto i = vm.begin(); i != vm.end(); i++) {
			if (*i->first.data() != '$') {
				printf("%s\n", i->first.data());
			}
		}
	}
	puts("");
	puts("-------------");
	printf("- VARIABLES -\n");
	puts("-------------\n");
	for (auto& vm : this->manager.varMapQueue) {
		for (auto i = vm.begin(); i != vm.end(); i++) {
            printf("%s %s = ", this->_GetTypeName(i->second->type), i->first.data());
            if (i->second->IsFunction()) {
                printf("FUNCTION");
            } else {
                switch (i->second->type) {
                    case ScriptVar::kScriptTypeBool:
                        printf("%s", i->second->intValue ? "TRUE" : "FALSE");
                        break;
                    case ScriptVar::kScriptTypeFloat:
                        printf("%.2f", i->second->floatValue);
                        break;
                    case ScriptVar::kScriptTypeFunction:
                        printf("FUNCTION");
                        break;
                    case ScriptVar::kScriptTypeInt:
                        printf("%lld", i->second->intValue);
                        break;
                    case ScriptVar::kScriptTypeString:
                        printf("\"%s\"", i->second->stringValue.data());
                        break;
                    case ScriptVar::kScriptTypeVar:
                        printf("UNDECLARED");
                        break;
                    case ScriptVar::kScriptTypeClass:
                        if (i->second->classValue) {
							printf("{\n");
                            this->_Trace(&*i->second->classValue);
							printf("}\n");
                        } else {
                            printf("NULL");
                        }
                        break;
                    default:
                        break;
                }
            }
            puts("");
		}
	}
    puts("");
}

Void ScriptVirtualMachine::_DeclareType(ScriptNodePtr node) {
    
    if (!this->manager.DeclareType(node->var)) {
        PostSyntaxError(node->var->lex->line, "Type redeclaration (%s)", node->var->name.data());
    }
}

Void ScriptVirtualMachine::_DeclareVar(ScriptNodePtr node) {

    if (!this->manager.DeclareVar(node->var)) {
        PostSyntaxError(node->var->lex->line, "Variable redeclaration (%s)", node->var->name.data());
    }
}

ScriptVarPtr ScriptVirtualMachine::_FindType(const Buffer& name) {

    ScriptVarPtr var = this->manager.FindType(name);
    
    if (!var) {
        PostSyntaxError(0, "Undeclared type (%s)", name.data());
    }
    
    return var;
}

ScriptVarPtr ScriptVirtualMachine::_FindVar(const Buffer& name) {

    ScriptVarPtr var = this->manager.FindVar(name);
    
    if (!var) {
        PostSyntaxError(0, "Undeclared variable (%s)", name.data());
    }
    
    return var;
}

Void ScriptVirtualMachine::_RegisterConstant(ScriptNodePtr node) {
    
	if (node->var->lex->object == kScriptObjectInt) {
		node->var->intValue = ParseIntValue(node->var->lex->word.data());
		node->var->lex = ScriptLex::Find(kScriptObjectInt);
		node->var->MakeInt();
	}
	else if (node->var->lex->object == kScriptObjectFloat) {
		node->var->floatValue = ParseFloatValue(node->var->lex->word.data());
		*node->var->lex = *ScriptLex::Find(kScriptObjectFloat);
		node->var->MakeFloat();
	}
	else if (node->var->lex->object == kScriptObjectBool) {
		node->var->intValue = (ScriptNativeBool)ParseIntValue(node->var->lex->word.data());
		node->var->lex = ScriptLex::Find(kScriptObjectBool);
		node->var->MakeBool();
	}
	else if (node->var->lex->object == kScriptObjectString) {
		node->var->stringValue = ParseStringValue(node->var->lex->word.data());
		node->var->lex = ScriptLex::Find(kScriptObjectString);
		node->var->MakeString();
	}
}

Void ScriptVirtualMachine::_Evaluate(Vector<ScriptNodePtr>* list, Vector<ScriptNodePtr>* result) {
    
	ScriptNodePtr left = 0;
	ScriptNodePtr right = 0;
    Uint32 arguments = 0;
	ScriptNodePtr prevNode = 0;
    ScriptVarPtr typeVar = 0;
	ScriptVarPtr classVar = 0;
    
    this->tempList_.clear();

	if (list->size() == 6) {
		//__asm int 3
	}

    for (ScriptNodePtr node : *list) {
        
		printf("%s ", node->var->name.data());

		if (node->var->name == "functionValue") {
			int a = 0;
		}

		//if (node->var->name == "0xff") __asm int 3

        if (node->var->lex->IsVariable()) {
            if ((typeVar = this->manager.FindType(node->var->name))) {
                node->var = typeVar;
                if (typeVar->lex->object == kScriptObjectClass) {
                    result->push_back(node);
                }
                continue;
            }
        }
        
		if (node->node == kScriptNodeDeclare) {
            if (node->var->IsClass()) {
                node->var->classValue.reset();
            }
			node->node = kScriptNodeField;
			if (node->parent->node != kScriptNodeClass) {
				this->_DeclareVar(node);
				node->var = this->_FindVar(node->var->name);
			}
			if (node->blockList.size()) {
				this->_Evaluate(&node->blockList, result);
                if (node->var->IsVar()) {
                    node->var->type = result->back()->var->type;
                }
				result->back()->var->Convert(node->var->type);
				node->var->_EvaluateMath(result->back()->var, ScriptLex::Find(kScriptObjectSet));
			}
			result->clear();
		}
        
		if (node->var->lex->object == kScriptObjectInt ||
			node->var->lex->object == kScriptObjectFloat ||
			node->var->lex->object == kScriptObjectString ||
			node->var->lex->object == kScriptObjectDefault ||
			node->var->lex->object == kScriptObjectVariable ||
            node->var->lex->object == kScriptObjectSemicolon
        ) {
            if (typeVar) {
				node = this->_CreateTemp(node);
                node->var->Convert(typeVar->type);
            }
			result->push_back(node);
            typeVar = 0;
		}
		else if (node->var->lex->object == kScriptObjectComa) {
			continue;
		}
		else if (node->var->lex->IsCondition()) {
            this->_EvaluateCondition(node);
		}
		else {
			if ((arguments = node->var->lex->arguments) == -1) {
				PostSyntaxError(node->var->lex->line, "Lex (%s) isn't as operation", node->var->lex->word.data());
			}
			if (result->size() < arguments) {
				PostSyntaxError(node->var->lex->line, "Invalid parameters, (%s) needs (%d) arguments", node->var->lex->word.data(), arguments);
			}
			if (arguments == 1) {
				left = result->back();
				result->pop_back();
				if (node->var->lex->object == kScriptObjectNew) {
					classVar = this->_FindType(left->var->name);
					if (!classVar->classValue) {
						PostSyntaxError(left->var->lex->line, "Non-Implemented class (%s)", left->var->name.data());
					}
					left = this->_CreateTemp(left);
					left->var->classValue.reset(new ScriptNode());
					left->var->classValue->var = left->var;
					left->var->MakeClass();
					for (ScriptNodePtr n : classVar->classValue->fieldList) {
						ScriptNodePtr node = new ScriptNode(*n);
						node->var = new ScriptVar(*n->var);
						left->var->classValue->RegisterField(node);
					}
				}
				else {
					if (left->var->lex->IsVariable() && !left->var->IsTemp()) {
						left->var = this->_FindVar(left->var->name);
					}
					if (left->var->IsClass()) {
						left->var = this->_FindType(left->var->name);
					}
					if (node->var->lex->IsRightAssociated()) {
						left = this->_CreateTemp(left);
					}
					left->var->_EvaluateSingle(node->var->lex);
				}
			}
			else if (arguments == 2) {
				right = result->back();
				result->pop_back();
				left = result->back();
				result->pop_back();
                if (left->var->lex->IsVariable() && !left->var->IsTemp()) {
                    left->var = this->_FindVar(left->var->name);
                }
                if (right->var->lex->IsVariable() && !right->var->IsTemp() &&
                        node->var->lex->object != kScriptObjectDirect &&
                        node->var->lex->object != kScriptObjectMediated
                ) {
                    right->var = this->_FindVar(right->var->name);
				}
                if (node->var->lex->IsRightAssociated()) {
                    left = this->_CreateTemp(left);
                }
				left->var = left->var->_EvaluateDouble(right, node->var->lex);
			}
			if (arguments) {
				result->push_back(left);
			}
		}
	}
}

Void ScriptVirtualMachine::_EvaluateCondition(ScriptNodePtr node) {
    
    Vector<ScriptNodePtr> result;
    
    switch (node->var->lex->object) {
        case kScriptObjectIf:
            
            this->_Evaluate(&node->conditionList, &result);
            
            if (result.size() != 1) {
                PostSyntaxError(node->var->lex->line, "Invalid condition expression, the result of condition isn't 1 argument", 1);
            }
            node->var->result = result.back()->var->Boolean();
            
            if (node->var->result) {
                this->_Evaluate(&node->blockList, &result);
            }
            
            break;
        case kScriptObjectElse:
            
            if (!node->parent || node->parent->var->lex->object != kScriptObjectIf) {
                PostSyntaxError(node->var->lex->line, "Else construction must have 'if' node", 1);
            }
            
            if (!node->parent->var->result) {
                if (node->blockList.size()) {
                    this->_Evaluate(&node->blockList, &result);
                }
            }
            
            break;
        case kScriptObjectWhile:
            
            this->_Evaluate(&node->conditionList, &result);
            
            if (result.size() != 1) {
                PostSyntaxError(node->var->lex->line,"Invalid condition expression, the result of condition isn't 1 argument", 1);
            }

            node->var->result = result.back()->var->Boolean();
            result.clear();
            
            if (node->parent->var->lex->object == kScriptObjectDo) {
                if (node->parent->blockList.size()) {
                    this->_Evaluate(&node->parent->blockList, &result);
                    result.clear();
                }
            }
            
            if (!node->var->result) {
                break;
            }
            
            if (node->parent && node->parent->var->lex->object == kScriptObjectDo) {
                if (!node->parent->blockList.size()) {
                    break;
                }
                do {
                    result.clear();
                    if (node->parent->blockList.size()) {
                        this->_Evaluate(&node->parent->blockList, &result);
                    }
                    this->_Evaluate(&node->conditionList, &result);
                    node->var->result = result.back()->var->Boolean();
                } while (node->var->result);
            }
            else {
                if (!node->blockList.size()) {
                    break;
                }
                while (node->var->result) {
                    if (node->blockList.size()) {
                        this->_Evaluate(&node->blockList, &result);
                        result.clear();
                    }
                    this->_Evaluate(&node->conditionList, &result);
                    node->var->result = result.back()->var->Boolean();
                    result.clear();
                }
            }
            
            break;
        default:
            break;
    }
}

Void ScriptVirtualMachine::_Trace(ScriptNodePtr node) {
    
	++this->traceDepth_;
	for (ScriptNodePtr n : node->fieldList) {
		if (n->node == kScriptNodeClass) {
			this->_Trace(n);
		}
		else {
			for (Uint32 i = 0; i < this->traceDepth_; i++) {
				printf("  ");
			}
            printf("%s %s = ", this->_GetTypeName(n->var->type), n->var->name.data());
			if (n->var->IsFunction()) {
				printf("FUNCTION");
			} else {
				switch (n->var->type) {
                    case ScriptVar::kScriptTypeBool:
                        printf("%s", n->var->intValue ? "TRUE" : "FALSE");
                        break;
                    case ScriptVar::kScriptTypeFloat:
                        printf("%.2f", n->var->floatValue);
                        break;
                    case ScriptVar::kScriptTypeFunction:
                        printf("FUNCTION");
                        break;
                    case ScriptVar::kScriptTypeInt:
                        printf("%lld", n->var->intValue);
                        break;
                    case ScriptVar::kScriptTypeString:
                        printf("\"%s\"", n->var->stringValue.data());
                        break;
                    case ScriptVar::kScriptTypeVar:
                        printf("UNDECLARED");
                        break;
                    case ScriptVar::kScriptTypeClass:
                        if (n->var->classValue) {
                            if (&*n->var->classValue == n) {
                                printf("new %s", n->var->name.data());
                            } else {
								printf("{\n");
                                this->_Trace(&*n->var->classValue);
								for (Uint32 i = 0; i < this->traceDepth_; i++) {
									printf("  ");
								}
								printf("}");
                            }
                        } else {
                            printf("NULL");
                        }
                        break;
                    default:
                        break;
				}
			}
		}
		puts("");
	}
	--this->traceDepth_;
}

StringC ScriptVirtualMachine::_GetTypeName(Uint32 type) {
    
    for (auto i = this->manager.typeMap->begin(); i != this->manager.typeMap->end(); i++) {
        if (i->second->type == type) {
            return i->second->name.data();
        }
    }
    
    return LAME_NULL;
}

ScriptNodePtr ScriptVirtualMachine::_CreateTemp(ScriptNodePtr node) {
    
    Temporary* result;
    
    this->tempList_.push_back({ *node, *node->var });
    result = &this->tempList_.back();
    result->var.MakeTemp();
    result->node.var = &result->var;
    
    return &result->node;
}

LAME_END