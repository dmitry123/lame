#include "Script.h"
#include "Performer.h"
#include "Internal.h"

LAME_BEGIN

Void ScriptPerformer::Evaluate(ScriptBuilderPtr builder) {

	this->builder_ = builder;

	for (ScriptNodePtr node : builder->rootNode_->blockList) {
		if (node->id == kScriptNodeClass) {
			this->_Declare(node);
		}
	}

	this->_Evaluate(&builder->rootNode_->FindBlock("do")->blockList);
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
			if (i->second->IsType()) {
				printf("%s\n", i->first.data());
			}
		}
	}

	puts("");
	for (auto& vm : this->manager_.varMapQueue) {
		for (auto i = vm.begin(); i != vm.end(); i++) {
			if (i->second->IsFinal()) {
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
				puts("");
			}
		}
	}

	puts("");
	for (auto& vm : this->manager_.varMapQueue) {
		for (auto i = vm.begin(); i != vm.end(); i++) {
			if (!i->second->IsType() && !i->second->IsFinal()) {
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
				puts("");
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
				printf("Function");
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
					printf("Function");
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

	if (node->id == kScriptNodeClass) {
		goto __DeclareClass;
	}

__DeclareVariable:

	if (!(typeVar = this->manager_.Find(node->typeName))) {
		PostSyntaxError(node->lex->line, "Undeclared type (%s)", node->typeName.data());
	}

	node->var = new ScriptVar(*typeVar);
	node->var->name = node->word;
	node->var->flags &= ~kScriptFlagType & ~kScriptFlagTemp;
	node->var->flags |= node->modificators;
	isVar = node->id == kScriptNodeVariable;
	this->varList_.push_back(node->var);

	node->MakeDefault();

	if (typeVar->type == kScriptTypeClass) {
		node->var->classValue.reset();
	}

	if (isVar && node->blockList.size()) {
		this->_Evaluate(&node->blockList, &result);
		if (!result.size()) {
			PostSyntaxError(node->lex->line, "Result of variable's expression must be something except nothing", 1);
		}
		resultVar = result.back()->var;
		resultVar->flags |= node->modificators;
		if (!resultVar) {
			PostSyntaxError(node->lex->line, "The result of variable's expression isn't variable or constant", 1);
		}
		resultVar = this->_CreateTemp(resultVar)->var;
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
		ScriptVarPtr refVar;

		refVar = this->manager_.Find(node->typeName);

		if (refVar && refVar->classValue) {
			PostSyntaxError(node->lex->line, "Class (%s) has already implemented", node->typeName.data());
		}
		else {
			refVar = &classVar;
		}

		refVar->classValue.reset(new ScriptNode(*node));

		for (ScriptNodePtr node : refVar->classValue->blockList) {

			ScriptNodePtr newNode(new ScriptNode(*node));

			if (!(typeVar = this->manager_.Find(node->typeName))) {
				PostSyntaxError(newNode->lex->line, "Undeclared type (%s)", newNode->typeName.data());
			}

			refVar->classValue->childList.push_back(newNode);

			newNode->var = new ScriptVar(*typeVar);
			newNode->var->name = newNode->word;
			newNode->var->flags &= ~kScriptFlagType & ~kScriptFlagTemp;
			newNode->var->flags |= newNode->modificators;
			isVar = newNode->id == kScriptNodeVariable;

			newNode->MakeDefault();

			if (typeVar->type == kScriptTypeClass) {
				newNode->var->classValue.reset();
			}

			if (isVar && newNode->blockList.size()) {
				this->_Evaluate(&newNode->blockList, &result);
				if (!result.size()) {
					PostSyntaxError(newNode->lex->line, "The result of variable's expression isn't variable or constant", 1);
				}
				resultVar = result.back()->var;
				resultVar->flags |= newNode->modificators;
				if (!resultVar) {
					PostSyntaxError(newNode->lex->line, "The result of variable's expression isn't variable or constant", 1);
				}
				resultVar = this->_CreateTemp(resultVar)->var;
				resultVar->Convert(newNode->var);
				resultVar->Clone(newNode->var);
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

	if (node->var) {
		this->varList_.push_back(node->var);
	}
}

Void ScriptPerformer::_Evaluate(Vector<ScriptNodePtr>* list) {

	Vector<ScriptNodePtr> result;
	this->_Evaluate(list, &result);
	result.clear();
}

Void ScriptPerformer::_Evaluate(Vector<ScriptNodePtr>* list, Vector<ScriptNodePtr>* result) {

	ScriptNodePtr expNode = 0;
	ScriptNodePtr left = 0;
	ScriptNodePtr right = 0;
	Uint32 arguments = 0;
	ScriptVarPtr typeVar = 0;
	ScriptVarPtr classVar = 0;
	ScriptNodePtr invokeNode = 0;

	// clear stack
	result->clear();

	for (ScriptNodePtr node : *list) {

		// skip semicolons
		if (node->lex->id == kScriptLexSemicolon) {
			//this->tempList_.clear();
		}

		// save current command in stack with commands (for trace)
		//this->cmdList_.push_back(node->word);

		// if out node hasn't variable (every node hasn't variable) and
		// was defined as const then register it's variable
		if (node->lex->IsConst() && !node->var) {
			this->_RegisterConstant(node);
		}

		// if our node is type or variable. it look like
		// function invoke but like as cast for examepl
		// int("0x7b") we'll get integer with value 123
		if ((typeVar = this->manager_.Find(node->word))) {

			// if found variable - type
			if (typeVar->IsType()) {

				// if it has variable so ... it might be variable with operator? (strange condition)
				if (node->var) {
					PostSyntaxError(node->lex->line, "You can't use variable (%s) as operator", node->word.data());
				}

				// save found type's var
				node->var = typeVar;

				// look though all fields in node's list
				// to declare it's constants
				for (ScriptNodePtr n : node->argList) {
					if (!n->var && n->lex->IsConst()) {
						this->_RegisterConstant(n);
					}
				}

				// now we have to evaluate this list
				// and write changes in our variable
				// last object in result stack will be
				// result of our expression
				this->_Evaluate(&node->argList, result);

				// now we can evaluate current node's lex
				// if count of arguments was above then 0
				if (node->argList.size()) {
					goto __EvaluateOperator;
				}
			}

			// if our found type - class then save its var
			if (typeVar->IsClass()) {
				node->var = typeVar;
			}

			typeVar = LAME_NULL;
		}

		// if node's id not equal with default
		// then it might be something more then normal
		// variable or type, for example declaration
		// of variable, function, class or condition block
		// likes while, for, if, else etc
		if (node->id != kScriptNodeDefault) {
			if (node->id == kScriptNodeFunction) {
				// if our node - function, so fuck it
			}
			else if (
				node->id == kScriptNodeVariable ||
				node->id == kScriptNodeClass
			) {
				this->_Declare(node);
			}
			else if (node->id == kScriptNodeCondition) {
				this->_EvaluateCondition(node);
			}
			else if (node->id == kScriptNodeInvoke) {

				// save object in stack and copy node's pointer
				// to invokeNode, cuz now we havn't arguments
				// in stack and we'll invoke it later
				result->push_back(node);
				invokeNode = node;
			}
		}
		else {

			// if our lex const or unknown then push in stack
			if (node->lex->IsConst() ||
				node->lex->IsUnknown()
			) {
				result->push_back(node);
			}
			else {
			__EvaluateOperator:

				// if out lex has unknown id and isn't variable
				if (node->lex->id == kScriptLexDefault && !typeVar) {
					result->push_back(node); continue;
				}

				// if we have unknown count of operands in lex then we have unknown lex, so have to throw an exception
				if ((arguments = node->lex->args) == -1) {
					PostSyntaxError(node->lex->line, "Lex (%s) isn't as operation", node->lex->word.data());
				}

				// if count of stack's arguments bellow then lex's arguments throw an exception
				if (result->size() < arguments) {
					PostSyntaxError(node->lex->line, "Invalid parameters, (%s) needs (%d) arguments", node->lex->word.data(), arguments);
				}

				// one argument for single expression
				if (arguments == 1) {

					// get left variable from stack
					left = result->back();
					result->pop_back();

					// if we havn't left variable or have, but its not temporary and has unknown lex
					if (left->lex && left->lex->IsUnknown() && left->var && !left->var->IsTemp()) {
						// if parent's id isn't class, its mean that variable isn't class's field
						if (left->parent->id != kScriptNodeClass) {
							left->var = this->_Find(left->word, node);
						}
					}

					// if left variable type then throw an exception
					if ((left->var->IsType() || left->var->IsFinal()) && node->lex->IsLeft()) {
						PostSyntaxError(node->lex->line, "You can't change final variables", 1);
					}

					// if left variable has private/protected
					// modificators then we must throw an exception, but
					if (left->var->IsPrivate() || left->var->IsProtected()) {

						// only if we right's parent's parent not equal
						// with left's parent, its mean that right object
						// is local and has method parent, and method has
						// same parent as field's parent - its a class
						if (right->parent->parent != left->parent) {
							PostSyntaxError(node->lex->line, "You can't change private/protected variables from non-class scope", 1);
						}
					}

					// if node's lex is new, then evaluate new lex
					if (node->lex->id == kScriptLexNew) {
						this->_EvaluateNew(node, left, classVar);
					}
					// if node's lex is return, then evaluate return lex
					else if (node->lex->id == kScriptLexReturn) {
						this->_EvaluateReturn(node, left);
					}
					// else evaluate single expression
					else {
						this->_EvaluateSingle(node, left);
					}
				}

				// two arguments for double expression
				else if (arguments == 2) {

					// get right and left variables from stack
					right = result->back();
					result->pop_back();
					left = result->back();
					result->pop_back();

					// i have no idea what it is
					if (right->var && right->parent && right->parent->args) {
						right->var = 0;
					}

					// if we havn't left variable or have, but its not temporary and has unknown lex
					if (!left->var || left->lex && left->lex->IsUnknown() && left->var && !left->var->IsTemp()) {
						// if parent's id isn't class, its mean that variable isn't class's field
						if (left->parent->id != kScriptNodeClass) {
							left->var = this->_Find(left->word, node);
						}
					}

					// if we havn't right variable and lex is selection then create temp
					if (!right->var || right->var && !right->var->IsTemp() && !right->lex->IsConst()) {
						if (node->lex->id != kScriptLexDirected && node->lex->id != kScriptLexMediated) {
							right->var = this->_CreateTemp(this->_Find(right->word, node))->var;
						}
					}

					// if operator right associated then create temporary var
					if (node->lex->IsRight()) {
						left->var = this->_CreateTemp(left->var)->var;
					}

					// if left variable has auto type, then copy right's var name
					if (left->var && left->var->type == kScriptTypeAuto) {
						left->var->typeName = right->var->typeName;
					}

					// if we have left and right variables then convert right to left
					if (right->var && left->var) {
						right->var->Convert(&*left->var);
					}

					// if left variable type then throw an exception
					if ((left->var->IsType() || left->var->IsFinal()) && node->lex->IsLeft()) {
						PostSyntaxError(node->lex->line, "You can't change final variables", 1);
					}

					// if we have result from double expression then swap left with result
					if ((expNode = left->var->_EvaluateDouble(right, node->lex))) {
						std::swap(left, expNode);
					}

					if (right->parent && right->parent->parent == left->parent) {
						if ((right->parent->modificators & kScriptFlagStatic) != 0) {
							PostSyntaxError(node->lex->line, "You can't address to this in static method", 1);
						}
					}

					// if left variable has private/protected
					// modificators then we must throw an exception, but
					if (left->var->IsPrivate() || left->var->IsProtected() ||
						right->var && (
						right->var->IsPrivate() ||
						right->var->IsProtected())
					) {

						// only if we right's parent's parent not equal
						// with left's parent, its mean that right object
						// is local and has method parent, and method has
						// same parent as field's parent - its a class
						if (right->parent && right->parent->parent != left->parent) {
							PostSyntaxError(node->lex->line, "You can't change private/protected variables from non-class scope", 1);
						}
					}
				}

				// if count of arguments above then 0
				// its mean that we poppped arguments from
				// stack and have to push result
				if (arguments) {

					// but, if we have invokeNode then we have all
					// nessesary information to invoke method
					if (invokeNode) {
						left = this->_InvokeMethod(node, expNode, invokeNode, left);
					}

					// remove invoke node and save left variable
					invokeNode = LAME_NULL;
					result->push_back(left);
				}
			}
		}
	}
}

Void ScriptPerformer::_EvaluateNew(ScriptNodePtr node, ScriptNodePtr left, ScriptVarPtr var) {

	// if found type non-implemented then throw an exception
	if (!(var = this->_Find(left->word, node))->classValue) {
		PostSyntaxError(left->lex->line, "Non-Implemented class (%s)", left->word.data());
	}

	// creating copy of current variable
	// from class type to temporary variable
	left->var = new ScriptVar(*var);
	this->newList_.push_back(left->var);

	// reset class's value with new node
	left->var->classValue.reset(new ScriptNode(*left));
	left->var->classValue->childList.clear();
	left->var->classValue->var = left->var;

	// create copy of all fields and move to new temp class object
	// we'll move all variables and nodes from type object to new
	// and save it to new object's child list
	for (ScriptNodePtr n : var->classValue->childList) {
		if (!n->var->IsStatic()) {
			ScriptNodePtr node = new ScriptNode(*n);
			node->var = new ScriptVar(*n->var);
			left->var->classValue->childList.push_back(node);
		}
		else {
			left->var->classValue->childList.push_back(n);
		}
	}
}

Void ScriptPerformer::_EvaluateSingle(ScriptNodePtr node, ScriptNodePtr left) {

	// if we havn't left variable we have to find it
	if (!left->var) {
		left->var = this->_Find(left->word, node);
	}

	// if our lex has unknown token ... we have to
	// convert left variable to found token's word? Wut?
	// anyway, else we can evaluate single expression
	if (node->lex->IsUnknown()) {
		left->var->Convert(this->_Find(node->word, node));
	}
	else {
		left->var->_EvaluateSingle(node->lex);
	}
}

Void ScriptPerformer::_EvaluateReturn(ScriptNodePtr node, ScriptNodePtr left) {

	// if we havn't variable, we'll find it
	// in current scope
	if (!left->var) {
		left->var = this->_Find(left->word, node);
	}

	// find this var in scope and set return var
	this->_Find("this", node)->classValue->returnVar = left->var;
}

ScriptNodePtr ScriptPerformer::_InvokeMethod(ScriptNodePtr node, ScriptNodePtr expNode, ScriptNodePtr invokeNode, ScriptNodePtr left) {

	// if left variable has private/protected
	// modificators then we must throw an exception, but
	if (left->var->IsPrivate() || left->var->IsProtected()) {

		// ...
		if (expNode->word != "this") {
			PostSyntaxError(node->lex->line, "You can't invoke private/protected methods from non-class scope", 1);
		}
	}

	// in left variable we have our method
	// with all information (name, arguments, types)
	// so we must set function modificator flag
	if (left->var->type != kScriptTypeClass) {
		left->var->MakeFunction();
	}

	// throw an exception if we have different
	// count of arguments
	if (invokeNode->args != left->args) {
		PostSyntaxError(node->lex->line, "Method (%s.%s) requires (%d) arguments", left->parent->typeName.data(), left->word.data(), left->args);
	}

	// result of node's expression
	Vector<ScriptNodePtr> result;

	// we gonna to evaluate list with arguments in expression 
	if (invokeNode->argList.size()) {
		this->_Evaluate(&invokeNode->argList, &result);
	}

	// if we have different arguments count in
	// expression's result and method
	if (result.size() != left->args) {
		PostSyntaxError(node->lex->line, "Invalid expression, method (%s.%s) requires (%d) arguments", left->parent->typeName.data(), left->word.data(), left->args);
	}

	// if our method (in class) has body
	if (left->blockList.size() || left->var->IsNative()) {

		// save current scope
		this->manager_.Push();

		// get first argument in invoke list
		auto i = result.begin();

		// copy all arguments with converted variables
		for (ScriptNodePtr n : left->argList) {

			ScriptVarPtr left;

			// now we have to declare new variable
			// in this scope, get it, convert to
			// right's variable and clone
			this->_Declare(n);
			left = this->_Find(n->word, node);
			left->Convert((*i)->var);
			left->_EvaluateMath((*i)->var, kScriptLexSet);
			++i;
		}

		// create 'this' node
		ScriptNode thisNode("this", kScriptNodeVariable, ScriptParser::Find(node->lex, kScriptLexDefault), left->parent, LAME_NULL);

		// if it has same names, that its constructor
		// and save left's word
		if (invokeNode->word == left->word) {
			thisNode.typeName = expNode->word;
		}
		// else get type's name from class value
		else {
			thisNode.typeName = expNode->var->classValue->word;
		}

		ScriptVarPtr thisVar;

		// deaclare 'this' node in current scope,
		// find it and copy class value from left node
		// and we can succesfully evaluate our method
		this->_Declare(&thisNode);
		(thisVar = this->_Find("this", node))->classValue = expNode->var->classValue;

		if (left->var->IsNative()) {
			left->var->callback = [](ScriptNodePtr var, ScriptManagerPtr manager, ScriptVarPtr self) {
				self->classValue->returnVar = new ScriptVar(kScriptTypeString, "TEMP", "String", var->lex->line);
				self->classValue->returnVar->stringValue = "Hello from C++!";
			};
			if (!left->var->callback) {
				PostSyntaxError(node->lex->line, "Undeclared native method (%s.%s)", expNode->word.data(), left->word.data());
			}
			left->var->callback(left, &this->manager_, thisVar);
		}
		else {
			this->_Evaluate(&left->blockList);
		}

		// if method has final modificator then change this
		// to final
		if (left->var->IsFinal()) {
			thisVar->MakeFinal();
		}

		// if method's return type - void
		if (left->var->type == kScriptTypeVoid) {
			// but we have returned variable .. its a mistake
			if (thisVar->classValue->returnVar) {
				PostSyntaxError(node->lex->line, "You can return variable only in non-void methods", 1);
			}
		}
		else {
			if (!thisVar->classValue->returnVar) {
				PostSyntaxError(node->lex->line, "Non-void methods must return something (%s.%s)", thisVar->typeName.data(), left->var->name.data());
			}

			// now we'll try to convert returned variable to method's return type
			thisVar->classValue->returnVar->Convert(left->var);

			// if everything is ok, we wont throw an exception, but
			// now we have to create its copy and push to temporary stack
			// cuz we'll pop current scope and all local variables
			// we'll be removed
			left = this->_CreateTemp(thisVar->classValue->returnVar);

			// when we make a copy of our return variable
			// we'll have function flags there, so we have to
			// remove it from temporary result
			left->var->flags &= ~kScriptFlagFunction;

			// reset return var to avoid mistakes in future
			// with void methods
			thisVar->classValue->returnVar = LAME_NULL;
		}

		// finally pop current scope from stack
		this->manager_.Pop();
	}

	return left;
}

ScriptNodePtr ScriptPerformer::_CreateTemp(ScriptVarPtr var) {
	this->tempList_.push_back({ ScriptNode("TEMP", kScriptNodeDefault, LAME_NULL, LAME_NULL, LAME_NULL), *var });
	this->tempList_.back().var.MakeTemp();
	this->tempList_.back().node.var = &this->tempList_.back().var;
	return &this->tempList_.back().node;
}

ScriptNodePtr ScriptPerformer::_CreateTemp(ScriptNodePtr node) {
	return this->_CreateTemp(node->var);
}

Void ScriptPerformer::_EvaluateCondition(ScriptNodePtr node) {

	Vector<ScriptNodePtr> result;

	this->manager_.Push();

	switch (node->lex->id) {
	case kScriptLexIf:

		this->_Evaluate(&node->argList, &result);

		if (result.size() != 1) {
			PostSyntaxError(node->lex->line, "Invalid condition expression, the result of condition isn't 1 argument", 1);
		}
		node->result = result.back()->var->Boolean();

		if (node->result) {
			this->_Evaluate(&node->blockList, &result);
		}

		break;
	case kScriptLexElse:

		if (!node->prev || node->prev->lex->id != kScriptLexIf) {
			PostSyntaxError(node->lex->line, "Else construction must have 'if' node", 1);
		}

		if (!node->prev->result) {
			if (node->blockList.size()) {
				this->_Evaluate(&node->blockList, &result);
			}
		}

		break;
	case kScriptLexWhile:

		this->_Evaluate(&node->argList, &result);

		if (result.size() != 1) {
			PostSyntaxError(node->lex->line, "Invalid condition expression, the result of condition isn't 1 argument", 1);
		}

		node->result = result.back()->var->Boolean();
		result.clear();

		if (node->prev->lex->id == kScriptLexDo && node->prev->parent->parent) {
			if (node->prev->blockList.size()) {
				this->_Evaluate(&node->prev->blockList, &result);
				result.clear();
			}
		}

		if (!node->result) {
			break;
		}

		if (node->prev && node->prev->lex->id == kScriptLexDo && node->prev->parent->parent) {
			if (!node->prev->blockList.size()) {
				break;
			}
			do {
				result.clear();
				if (node->prev->blockList.size()) {
					this->_Evaluate(&node->prev->blockList, &result);
				}
				this->_Evaluate(&node->argList, &result);
				node->result = result.back()->var->Boolean();
			} while (node->result);
		}
		else {
			if (!node->blockList.size() && node->argList.size() == 1 && node->argList.back()->lex->IsConst() && node->argList.back()->var->Boolean()) {
				PostSyntaxError(node->lex->line, "Infinite loop, non-zero constant expression in while without body", 1);
			}
			while (node->result) {
				if (node->blockList.size()) {
					this->_Evaluate(&node->blockList, &result);
					result.clear();
				}
				this->_Evaluate(&node->argList, &result);
				node->result = result.back()->var->Boolean();
				result.clear();
			}
		}

		break;
	default:
		break;
	}

	this->manager_.Pop();
}

ScriptPerformer::~ScriptPerformer() {

	//for (ScriptVarPtr v : this->newList_) {
	//	if (v->classValue) {
	//		v->classValue.reset();
	//	}
	//	delete v;
	//}

	//for (ScriptVarPtr v : this->varList_) {
	//	if (v->classValue) {
	//		v->classValue.reset();
	//	}
	//	delete v;
	//}

	this->varList_.clear();
	this->cmdList_.clear();
	this->tempList_.clear();
}

LAME_END