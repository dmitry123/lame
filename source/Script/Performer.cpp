#include "Script.h"
#include "Performer.h"
#include "Internal.h"

LAME_BEGIN

Void ScriptPerformer::Evaluate(ScriptBuilderPtr builder) {

	this->builder_ = builder;

	for (ScriptNodePtr node : builder->rootNode_->childList) {
		if (node->id == kScriptNodeClass) {
			this->_Declare(node);
		}
	}

	this->_Evaluate(&builder->rootNode_->FindChild("do")->childList);
}

Void ScriptPerformer::Trace(Void) {

	puts("\n------------");
	printf("- COMMANDS -\n");
	puts("------------");
	for (BufferRefC b : this->cmdList_) {
		printf("%s ", b.data());
	}

	this->traceDepth_ = 0;
	puts("\n---------");
	printf("- TRACE -\n");
	puts("---------");
	for (auto& vm : this->manager_.varMapQueue) {
		for (auto i = vm.begin(); i != vm.end(); i++) {
			if (i->second->typeName == i->first) {
				printf("%s\n", i->first.data());
			}
			else {
				puts("");
				printf("%s %s = ", i->second->typeName.data(), i->first.data());
				if (i->second->IsFunction()) {
					printf("Function");
				}
				else {
					switch (i->second->type) {
					case kScriptTypeBool:
						printf("%s", i->second->intValue ? "True" : "False");
						break;
					case kScriptTypeFloat:
						printf("%.2f", i->second->floatValue);
						break;
					case kScriptTypeFunction:
						printf("Function");
						break;
					case kScriptTypeInt:
						printf("%lld", i->second->intValue);
						break;
					case kScriptTypeString:
						printf("\"%s\"", i->second->stringValue.data());
						break;
					case kScriptTypeAuto:
						printf("Undeclared");
						break;
					case kScriptTypeClass:
						if (i->second->classValue) {
							printf("{\n");
							this->_Trace(&*i->second->classValue);
							printf("}\n");
						}
						else {
							printf("Null");
						}
						break;
					default:
						break;
					}
				}
			}
		}
	}
}

Void ScriptPerformer::_Trace(ScriptNodePtr node) {

	++this->traceDepth_;
	for (ScriptNodePtr n : node->childList) {
		if (n->id == kScriptNodeClass) {
			this->_Trace(n);
		}
		else {
			for (Uint32 i = 0; i < this->traceDepth_; i++) {
				printf("  ");
			}
			printf("%s %s = ", n->typeName.data(), n->var->name.data());
			if (n->var->IsFunction()) {
				printf("FUNCTION");
			}
			else {
				switch (n->var->type) {
				case kScriptTypeBool:
					printf("%s", n->var->intValue ? "True" : "False");
					break;
				case kScriptTypeFloat:
					printf("%.2f", n->var->floatValue);
					break;
				case kScriptTypeFunction:
					printf("Function");
					break;
				case kScriptTypeInt:
					printf("%lld", n->var->intValue);
					break;
				case kScriptTypeString:
					printf("\"%s\"", n->var->stringValue.data());
					break;
				case kScriptTypeAuto:
					printf("Undeclared");
					break;
				case kScriptTypeClass:
					if (n->var->classValue) {
						if (&*n->var->classValue == n) {
							printf("new %s", n->var->name.data());
						}
						else {
							printf("{\n");
							this->_Trace(&*n->var->classValue);
							for (Uint32 i = 0; i < this->traceDepth_; i++) {
								printf("  ");
							}
							printf("}");
						}
					}
					else {
						printf("Null");
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

Void ScriptPerformer::_Declare(ScriptNodePtr node) {

	ScriptVarPtr typeVar;
	Bool isVar;
	Vector<ScriptNodePtr> result;
	ScriptVarPtr resultVar;

	if (node->var) {
		PostSyntaxError(node->lex->line, "Variable redeclaration (%s)", node->var->name.data());
	}

	if (node->id == kScriptNodeClass) {
		goto __DeclareClass;
	}

__DeclareVariable:

	if (!(typeVar = this->manager_.Find(node->typeName))) {
		PostSyntaxError(node->lex->line, "Undeclared type (%s)", node->typeName.data());
	}

	node->var = new ScriptVar(*typeVar);
	node->var->name = node->word;

	isVar = node->id == kScriptNodeVariable;

	node->MakeDefault();

	if (typeVar->type == kScriptTypeClass) {
		node->var->classValue.reset();
	}

	if (isVar && node->childList.size()) {
		this->_Evaluate(&node->childList, &result);
		if (!result.size()) {
			PostSyntaxError(node->lex->line, "Result of variable's expression must be something except nothing", 1);
		}
		resultVar = result.back()->var;
		if (!resultVar) {
			PostSyntaxError(node->lex->line, "The result of variable's expression isn't variable or constant", 1);
		}
		resultVar = this->_CreateTemp(resultVar);
		resultVar->Convert(node->var);
		resultVar->Clone(node->var);
	}

	if (!this->manager_.Declare(node->var)) {
		PostSyntaxError(node->lex->line, "Variable redeclaration (%s)", node->var->name.data());
	}

	goto __ExitDeclaration;
__DeclareClass:
	
	LAME_BLOCK {

		ScriptVar classVar(kScriptTypeClass, node->typeName, node->typeName, node->lex->line);

		typeVar = this->manager_.Find(node->typeName);

		if (typeVar && typeVar->classValue) {
			PostSyntaxError(node->lex->line, "Class (%s) has already implemented", node->typeName.data());
		}
		else {
			typeVar = &classVar;
		}

		typeVar->classValue.reset(new ScriptNode(*node));

		for (ScriptNodePtr node : typeVar->classValue->childList) {

			if (!(typeVar = this->manager_.Find(node->typeName))) {
				PostSyntaxError(node->lex->line, "Undeclared type (%s)", node->typeName.data());
			}

			node->var = new ScriptVar(*typeVar);
			node->var->name = node->word;

			isVar = node->id == kScriptNodeVariable;

			node->MakeDefault();

			if (typeVar->type == kScriptTypeClass) {
				node->var->classValue.reset();
			}

			if (isVar && node->childList.size()) {
				this->_Evaluate(&node->childList, &result);
				if (!result.size()) {
					PostSyntaxError(node->lex->line, "Result of variable's expression must be something except nothing", 1);
				}
				resultVar = result.back()->var;
				if (!resultVar) {
					PostSyntaxError(node->lex->line, "The result of variable's expression isn't variable or constant", 1);
				}
				resultVar = this->_CreateTemp(resultVar);
				resultVar->Convert(node->var);
				resultVar->Clone(node->var);
			}
		}

		if (!this->manager_.Declare(&classVar)) {
			PostSyntaxError(node->lex->line, "Class redeclaration (%s)", node->typeName.data());
		}
	}

	goto __ExitDeclaration;
__ExitDeclaration:
	;
}

ScriptVarPtr ScriptPerformer::_Find(BufferRefC name, ScriptNodePtr node) {

	ScriptVarPtr var = this->manager_.Find(name);

	if (!var) {
		PostSyntaxError(node->lex->line, "Undeclared variable or type (%s)", name.data());
	}

	return var;
}

Void ScriptPerformer::_RegisterConstant(ScriptNodePtr node) {

	switch (node->lex->id) {
	case kScriptLexInt:
		node->var = new ScriptVar(kScriptTypeInt, node->word, "Int", node->lex->line);
		node->var->intValue = ParseIntValue(node->word.data());
		break;
	case kScriptLexFloat:
		node->var = new ScriptVar(kScriptTypeFloat, node->word, "Float", node->lex->line);
		node->var->floatValue = ParseFloatValue(node->word.data());
		break;
	case kScriptLexString:
		node->var = new ScriptVar(kScriptTypeString, node->word, "String", node->lex->line);
		node->var->stringValue = ParseStringValue(node->word.data());
		break;
	default:
		break;
	}
}

Void ScriptPerformer::_Evaluate(Vector<ScriptNodePtr>* list) {

	Vector<ScriptNodePtr> result;
	this->_Evaluate(list, &result);
}

Void ScriptPerformer::_Evaluate(Vector<ScriptNodePtr>* list, Vector<ScriptNodePtr>* result) {

	ScriptNodePtr expNode = 0;
	ScriptNodePtr left = 0;
	ScriptNodePtr right = 0;
	Uint32 arguments = 0;
	ScriptVarPtr typeVar = 0;
	ScriptVarPtr classVar = 0;

	this->tempList_.clear();
	result->clear();

	for (ScriptNodePtr node : *list) {

		this->cmdList_.push_back(node->word);

		if (node->word == "b") {
			//__asm int 3
		}

		if (!node->var && node->lex->IsConst()) {
			this->_RegisterConstant(node);
		}

		if ((typeVar = this->manager_.Find(node->word))) {
			if (typeVar->IsType()) {
				goto __EvaluateOperator;
			}
			typeVar = LAME_NULL;
		}

		if (node->id != kScriptNodeDefault) {
			if (node->id == kScriptNodeFunction) {
				// :D
			}
			else if (
				node->id == kScriptNodeVariable ||
				node->id == kScriptNodeClass
			) {
				this->_Declare(node);
			}
			else if (node->id == kScriptNodeCondition) {
				// :D
			}
			else if (node->id == kScriptNodeInvoke) {
				// :D
			}
		}
		else {
			if (node->lex->IsConst() ||
				node->lex->IsUnknown()
			) {
				result->push_back(node);
			}
			else {
			__EvaluateOperator:
				if (node->lex->id == kScriptLexDefault) {
					result->push_back(node); continue;
				}
				if ((arguments = node->lex->args) == -1) {
					PostSyntaxError(node->lex->line, "Lex (%s) isn't as operation", node->lex->word.data());
				}
				if (result->size() < arguments) {
					PostSyntaxError(node->lex->line, "Invalid parameters, (%s) needs (%d) arguments", node->lex->word.data(), arguments);
				}
				if (arguments == 1) {

					left = result->back();
					result->pop_back();

					if (node->lex->id == kScriptLexNew) {

						if (!(classVar = this->_Find(left->word, node))->classValue) {
							PostSyntaxError(left->lex->line, "Non-Implemented class (%s)", left->word.data());
						}

						left->var = new ScriptVar(*classVar);
						left->var->classValue.reset(new ScriptNode(*left));
						left->var->classValue->childList.clear();
						left->var->classValue->var = left->var;

						for (ScriptNodePtr n : classVar->classValue->childList) {
							ScriptNodePtr node = new ScriptNode(*n);
							node->var = new ScriptVar(*n->var);
							left->var->classValue->childList.push_back(node);
						}
					}
					else {
						if (!left->var) {
							left->var = this->_Find(left->word, node);
						}
						if (node->lex->IsUnknown()) {
							left->var->Convert(this->_Find(node->word, node));
						}
						if (!node->lex->IsUnknown()) {
							left->var->_EvaluateSingle(node->lex);
						}
						else {
							result->push_back(node);
						}
					}
				}
				else if (arguments == 2) {

					right = result->back();
					result->pop_back();
					left = result->back();
					result->pop_back();

					if (!left->var || left->lex->IsUnknown() && left->var && !left->var->IsTemp()) {
						if (left->parent->id != kScriptNodeClass) {
							left->var = this->_Find(left->word, node);
						}
					}
					if (!right->var &&
						node->lex->id != kScriptLexDirected &&
						node->lex->id != kScriptLexMediated
					) {
						right->var = this->_CreateTemp(this->_Find(right->word, node));
					}
					if (node->lex->IsRight()) {
						left->var = this->_CreateTemp(left->var);
					}
					if (left->var && left->var->type == kScriptTypeAuto) {
						left->var->typeName = right->var->typeName;
					}
					if (right->var && left->var) {
						right->var->Convert(left->var);
					}
					if ((expNode = left->var->_EvaluateDouble(right, node->lex))) {
						left = expNode;
					}
				}
				if (arguments) {
					result->push_back(left);
				}
			}
		}
	}

	if (result->size() == 1) {
		ScriptNodePtr back = result->back();
		if (!back->var) {
			back->var = this->_Find(back->word, back);
		}
		result->back()->var = back->var;
	}
}

Void ScriptPerformer::_EvaluateArgs(ScriptNodePtr node) {
	this->_Evaluate(&node->argList);
}

Void ScriptPerformer::_EvaluateBlock(ScriptNodePtr node) {
	this->_Evaluate(&node->childList);
}

ScriptVarPtr ScriptPerformer::_CreateTemp(ScriptVarPtr var) {
	this->tempList_.push_back({ ScriptNode("TEMP", kScriptNodeDefault, LAME_NULL, LAME_NULL), *var });
	this->tempList_.back().var.MakeTemp();
	return &this->tempList_.back().var;
}

ScriptNodePtr ScriptPerformer::_CreateTemp(ScriptNodePtr node) {
	this->tempList_.push_back({ *node, *node->var });
	this->tempList_.back().var.MakeTemp();
	return &this->tempList_.back().node;
}

Void ScriptPerformer::_EvaluateCondition(ScriptNodePtr node) {


}

LAME_END