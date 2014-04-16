#include "Script.h"

#define __Inc(_i) \
	if (++i == this->parser->lexList_.end()) {\
		PostSyntaxError((*(i-1))->line, "Not finished expression", 1); \
		}
#define __Dec(_i) \
	--i

LAME_BEGIN

Void ScriptBuilder::Build(ScriptParserPtr parser, ScriptVirtualMachinePtr machine) {

	this->parser = parser;
	this->machine = machine;

	// add braces to parser
	this->parser->lexList_.push_front(
		new ScriptLex(*ScriptLex::Find(kScriptObjectBraceL)));
	this->parser->lexList_.push_back(
		new ScriptLex(*ScriptLex::Find(kScriptObjectBraceR)));

	Iterator first = this->parser->lexList_.begin();

	if (!machine->root) {
		machine->root = this->_CreateNode(*first);
	}

	// save root's block as node stack
	this->_Push(&machine->root->blockList);

	// build block with root
	this->_BuildBlock(machine->root, first);

	// order root node
	this->_Order(machine->root);
}

Void ScriptBuilder::_Order(ScriptNodePtr node) {

	node->Order();

	for (ScriptNodePtr n : node->blockList) {
		if (n->node == kScriptNodeField ||
			n->node == kScriptNodeDeclare
		) {
			n->Order();
		}
		else {
			this->_Order(n);
		}
	}
	for (ScriptNodePtr n : node->conditionList) {
		if (n->node == kScriptNodeField ||
			n->node == kScriptNodeDeclare
		) {
			n->Order();
		}
		else {
			this->_Order(n);
		}
	}
	for (ScriptNodePtr n : node->fieldList) {
        if (n->node == kScriptNodeField ||
			n->node == kScriptNodeDeclare
		) {
            n->Order();
        } else {
            this->_Order(n);
        }
	}
}

Void ScriptBuilder::_BuildCondition(ScriptNodePtr parent, IteratorRef i) {

	Uint32 extraParentheses = 0;

	parent->node = kScriptNodeCondition;

	// skip parent node
	++i;

	// if condition is 'else' then we have to skip condition block
	if (parent->var->lex->object == kScriptObjectElse ||
		parent->var->lex->object == kScriptObjectDo
	) {
		goto __SkipParentheses;
	}
    
    parent->var->condition = parent;

	while (LAME_TRUE) {

		if ((*i)->object == kScriptObjectParentheseR) {
			if (extraParentheses == 1) {
				this->_Pop(); ++i; break;
			}
			--extraParentheses; goto __SaveNode;
		}

		if ((*i)->object == kScriptObjectParentheseL) {
			if (extraParentheses >= 1) {
				++extraParentheses; goto __SaveNode;
			} else {
				this->_Push(&parent->conditionList); extraParentheses = 1;
			}
		}
		else {
			if (!this->nodeQueue_) {
				PostSyntaxError((*i)->line, "Parenthese in unknown place without parent", 1);
			}
		__SaveNode:
			ScriptNodePtr node = this->_CreateNode(*i);
			this->nodeQueue_->push_back(node);
			if (this->_RecursiveBuild(node, i)) {
				--i;
			}
		}

		if (++i == this->parser->lexList_.end()) {
			PostSyntaxError((*(i - 1))->line, "Parenthese mismatched", 1);
		}
	}
__SkipParentheses:
	if ((*i)->object == kScriptObjectBraceL) {
		this->_BuildBlock(parent, i);
	}
}

Void ScriptBuilder::_BuildBlock(ScriptNodePtr parent, IteratorRef i) {

	while (LAME_TRUE) {

		if ((*i)->object == kScriptObjectBraceR) {
			this->_Pop(); ++i; break;
		}

		if ((*i)->object == kScriptObjectBraceL) {
			this->_Push(&parent->blockList);
		}
		else {
			if (!this->nodeQueue_) {
				PostSyntaxError((*i)->line, "Brace in unknown place without parent", 1);
			}
			ScriptNodePtr node = this->_CreateNode(*i);
			this->nodeQueue_->push_back(node);
			if (this->_RecursiveBuild(node, i)) {
				--i;
			}
		}

		if (++i == this->parser->lexList_.end()) {
			PostSyntaxError((*(i - 1))->line, "Braces mismatched", 1);
		}
	}
}

Void ScriptBuilder::_BuildClass(ScriptNodePtr parent, IteratorRef i) {
    
    // vector with results (for fields evaluating)
    Vector<ScriptNodePtr> result;
    
	parent->node = kScriptNodeClass;

	// skip parent node
	__Inc(i);

	// create node's var as class
	parent->var->name = (*i)->word;
	parent->var->MakeClass();

	// skip class name
	__Inc(i);

	// looking for semicolon or left brace
	if ((*i)->object == kScriptObjectSemicolon) {
		++i; goto __SkipBlock;
	}
	else if ((*i)->object != kScriptObjectBraceL) {
		PostSyntaxError((*i)->line, "Invalid token after class declaration (%s)", (*i)->word.data());
	}

	// generating class's body
	while (LAME_TRUE) {

		if ((*i)->object == kScriptObjectBraceR) {
			this->_Pop(); ++i; break;
		}

		if ((*i)->object == kScriptObjectBraceL) {
			this->_Push(&parent->fieldList);
		}
		else {
			if (!this->nodeQueue_) {
				PostSyntaxError((*i)->line, "Brace in unknown place without parent", 1);
			}

			ScriptNodePtr node = this->_CreateNode(*i);
			this->nodeQueue_->push_back(node);
			
			if (node->var->lex->object != kScriptObjectType &&
				node->var->lex->object != kScriptObjectDefault
			) {
				PostSyntaxError((*i)->line, "Invalid object in class field (%s)", (*i)->word.data());
			}

			if (this->_RecursiveBuild(node, i)) {
				--i;
			}
		}

		if (++i == this->parser->lexList_.end()) {
			PostSyntaxError((*(i - 1))->line, "Braces mismatched", 1);
		}
	}

	// order class fields
	for (ScriptNodePtr n : parent->fieldList) {
		n->Order();
	}
    // evaluate class fields
    this->machine->_Evaluate(&parent->fieldList, &result);
    // create new class value for current node
	parent->var->classValue.reset(new ScriptNode(*parent));
    
__SkipBlock:
    
	// declare type
	if (!this->machine->manager.DeclareType(parent->var)) {
		PostSyntaxError((*i)->line, "Type redeclaration (%s)", parent->var->lex->word.data());
	}
}

Void ScriptBuilder::_BuildAttribute(ScriptNodePtr parent, IteratorRef i) {

	ScriptVarPtr type;
	ScriptNodePtr node;
	ScriptNodePtr typeNode;
	Buffer variableName;

	parent->node = kScriptNodeField;
	parent->var->lex->object = kScriptObjectVariable;

	type = this->machine->manager.FindType((*i)->word);

	if (!type) {
		__Inc(i); return;
	}

	if (!(*(i + 1))->IsVariable()) {
		__Inc(i); return;
	}

	// get object's type node
	typeNode = this->_CreateNode(*i);
	__Inc(i);

	// get variable
	node = this->_CreateNode(*i);
	__Inc(i);

	// initialize variable with type's var
	std::swap(parent->var, node->var);
	type->Clone(parent->var);
	parent->var->name = parent->var->lex->word;
	parent->var->className = node->var->name;

	// find type object
	typeNode->var->lex = ScriptLex::Find(kScriptObjectType);
	typeNode->var->type = type->type;

	// find variable object
	variableName = parent->var->lex->word;
	parent->var->lex = ScriptLex::Find(kScriptObjectVariable);
	parent->node = kScriptNodeDeclare;
	parent->var->type = typeNode->var->type;

	if ((*i)->object == kScriptObjectSemicolon) {

		// skip semicolon
		++i;
	}
	else if ((*i)->object == kScriptObjectSet) {

		__Inc(i);

		while (LAME_TRUE) {
			if ((*i)->object == kScriptObjectSemicolon) {
				break;
			}
			else {
				parent->RegisterBlock(this->_CreateNode(*i));
			}
			if (++i == this->parser->lexList_.end()) {
				PostSyntaxError((*(i - 1))->line, "Expression not closed with semicolon", 1);
			}
		}
		++i;
	}
	else if ((*i)->object == kScriptObjectParentheseL) {

		// get function's lex
		parent->var->lex = ScriptLex::Find(kScriptObjectFunction);
		parent->var->MakeFunction();
        
		__Dec(i);
        
        // build method's condition block
		this->_BuildCondition(parent, i);
	}
	else if ((*i)->object == kScriptObjectParentheseR) {
		// :D
	}
	else if ((*i)->object == kScriptObjectComa) {
		++i;
	}
	else {
		PostSyntaxError((*i)->line, "Invalid expression after variable (%s)", (*i)->word.data());
	}
}

Void ScriptBuilder::_BuildThread(ScriptNodePtr parent, IteratorRef i) {

	parent->node = kScriptNodeThread;

	// skip parent node
	++i;

	while (LAME_TRUE) {

		if ((*i)->object == kScriptObjectParentheseR) {
			this->_Pop();
			__Inc(i);
			break;
		}

		if ((*i)->object == kScriptObjectParentheseL) {
			this->_Push(&parent->conditionList);
		}
		else if ((*i)->object == kScriptObjectString) {
			if (!this->nodeQueue_) {
				PostSyntaxError((*i)->line, "Brace in unknown place without parent", 1);
			}

			ScriptNodePtr node = this->_CreateNode(*i);
			this->nodeQueue_->push_back(node);
		}
		else {
			PostSyntaxError((*i)->line, "Thread's name must be string, but not (%s)", (*i)->word.data());
		}

		if (++i == this->parser->lexList_.end()) {
			PostSyntaxError((*(i - 1))->line, "Brace mismatched", 1);
		}
	}

	if ((*i)->object == kScriptObjectBraceL) {
		this->_BuildBlock(parent, i);
	}
}

Bool ScriptBuilder::_RecursiveBuild(ScriptNodePtr parent, IteratorRef i) {

	if ((*i)->IsClass()) {
		this->_BuildClass(parent, i);
	}
	else if ((*i)->IsCondition()) {
		this->_BuildCondition(parent, i);
	}
	else if ((*i)->IsVariable()) {
		this->_BuildAttribute(parent, i);
	}
	else if ((*i)->IsThread()) {
		this->_BuildThread(parent, i);
	}
    else if ((*i)->IsModificator()) {
        __debugbreak();
    }
	else {
		return LAME_FALSE;
	}

	return LAME_TRUE;
}

ScriptNodePtr ScriptBuilder::_CreateNode(ScriptLexPtr object) {

	// allocate script node
	ScriptNodePtr node (new ScriptNode());

	// save node to list
	this->nodeList_.push_back(node);

	// for debug
	node->__lex = object;

	// allocate script variable
	this->machine->varList_.push_back(new ScriptVar(object));

	// initialize node
	node->var = this->machine->varList_.back();
	node->var->lex = object;
    node->var->name = node->var->lex->word;

#if 0
	node->prev = this->prevNode_;
#endif

	node->parent = this->parentNode_;

	// create doubly linked list
#if 0
	if (this->prevNode_) {
		this->prevNode_->next = node;
	}
	this->prevNode_ = node;
#endif

	// initialize parent
	if (node->var->lex->IsCondition() ||
		node->var->lex->IsClass() ||
		node->var->lex->IsThread()
	) {
		this->parentNode_ = node;
	}

	// check for constant and register
	this->machine->_RegisterConstant(node);

	return node;
}

ScriptNodePtr ScriptBuilder::_RemoveNode(ScriptNodePtr node) {

    __debugbreak();
    
	// return parent
	return LAME_NULL;
}

Void ScriptBuilder::_Push(Vector<ScriptNodePtr>* stack) {

	// push current node stack to queue
	if (this->nodeQueue_) {
		this->stackNodeQueue_.push_back(this->nodeQueue_);
	}

	// save active queue
	this->nodeQueue_ = stack;
}

Vector<ScriptNodePtr>* ScriptBuilder::_Pop(Void) {

	// if we queue is empty
	if (!this->stackNodeQueue_.size()) {
		// set last queue as NULL
		this->nodeQueue_ = LAME_NULL;
	}
	else {
		// remove nodeQueue from tail
		this->nodeQueue_ = this->stackNodeQueue_.back();
		this->stackNodeQueue_.pop_back();
	}

	// return tail
	return this->nodeQueue_;
}

LAME_END