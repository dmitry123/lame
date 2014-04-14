#include "Script.h"

#define __Inc(_i) \
	if (++i == this->parser->objectList.end()) {\
		PostSyntaxError((*(i-1))->line, "Not finished expression", 1); \
		}
#define __Dec(_i) \
	--i

LAME_BEGIN

ScriptBuilder& ScriptBuilder::Build(ScriptParserPtr parser, ScriptPerformerPtr performer) {

	this->parser = parser;
	this->performer = performer;

	// add braces to parser
	this->parser->objectList.push_front(
		new ScriptObject(*ScriptObject::FindScriptObjectByFlag(kScriptObjectBraceL)));
	this->parser->objectList.push_back(
		new ScriptObject(*ScriptObject::FindScriptObjectByFlag(kScriptObjectBraceR)));

	Iterator first = this->parser->objectList.begin();

	if (!performer->root) {
		performer->root = this->_AppendNode(*first);
	}

	// save root's block as node stack
	this->_PushStack(&performer->root->block);

	// build block with root
	this->_BuildBlock(performer->root, first);

	// order root node
	this->_Order(performer->root);

	return *this;
}

Void ScriptBuilder::_Order(ScriptNodePtr node) {

	node->Order(this->performer);

	for (ScriptNodePtr n : node->block) {
		if (n->type == kScriptNodeDefault) {
			continue;
		}
		if (n->type == kScriptNodeDeclare) {
			n->Order(this->performer);
		}
		else {
			this->_Order(n);
		}
	}
}

Void ScriptBuilder::_Build(ScriptNodePtr node, IteratorRef i) {
	while (i != this->parser->objectList.end()) {
		this->_BuildRecursive(node, i);
	}
}

Void ScriptBuilder::_BuildCondition(ScriptNodePtr parent, IteratorRef i) {

	Uint32 extraParentheses = 0;

	parent->type = kScriptNodeCondition;

	// skip parent node
	++i;

	// if condition is 'else' then we have to skip condition block
	if (parent->object->object == kScriptObjectElse ||
		parent->object->object == kScriptObjectDo
	) {
		goto __SkipParentheses;
	}

	while (LAME_TRUE) {

		if ((*i)->object == kScriptObjectParentheseR) {
			if (extraParentheses == 1) {
				this->_PopStack();
				++i;
				break;
			}
			--extraParentheses;
			goto __SaveNode;
		}

		if ((*i)->object == kScriptObjectParentheseL) {
			if (extraParentheses >= 1) {
				++extraParentheses;
				goto __SaveNode;
			}
			else {
				this->_PushStack(&parent->condition);
				extraParentheses = 1;
			}
		}
		else {
			if (!this->nodeQueue_) {
				PostSyntaxError((*i)->line, "Parenthese in unknown place without parent", 1);
			}

		__SaveNode:
			ScriptNodePtr node = this->_AppendNode(*i);
			this->nodeQueue_->push_back(node);

			if (this->_BuildRecursive(node, i)) {
				--i;
			}
		}

		if (++i == this->parser->objectList.end()) {
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
			this->_PopStack();
			++i;
			break;
		}

		if ((*i)->object == kScriptObjectBraceL) {
			this->_PushStack(&parent->block);
		}
		else {
			if (!this->nodeQueue_) {
				PostSyntaxError((*i)->line, "Braces in unknown place without parent", 1);
			}

			ScriptNodePtr node = this->_AppendNode(*i);
			this->nodeQueue_->push_back(node);

			if (this->_BuildRecursive(node, i)) {
				--i;
			}
		}

		if (++i == this->parser->objectList.end()) {
			PostSyntaxError((*(i - 1))->line, "Braces mismatched", 1);
		}
	}
}

Void ScriptBuilder::_BuildClass(ScriptNodePtr parent, IteratorRef i) {

	Uint32 extraParentheses = 0;

	parent->type = kScriptNodeClass;

	// skip parent node
	__Inc(i);

	// save type's name
	(*i)->type.name = (*i)->word;

	// save parent's type
	parent->object->type = (*i)->type;
	parent->object->type.type = kScriptTypeClass;
	parent->object->type.object = parent;

	// declare type
	if (!this->performer->vtManager_.DeclareType(&parent->object->type)) {
		PostSyntaxError((*i)->line, "Type redeclaration (%s)", parent->object->type.GetString());
	}

	// skip class name
	__Inc(i);

	if ((*i)->object == kScriptObjectBraceL) {
		this->_BuildBlock(parent, i);
	}
}

Void ScriptBuilder::_BuildField(ScriptNodePtr parent, IteratorRef i) {

	ScriptTypePtr type;
	ScriptNodePtr node;
	ScriptNodePtr typeNode;
	Buffer variableName;

	parent->type = kScriptNodeField;
	parent->object->object = kScriptObjectVariable;

	try {
		type = this->performer->FindType((*i)->word);
	}
	catch (...) {
		__Inc(i); return;
	}

	// get object's type node
	typeNode = this->_AppendNode(*i);
	__Inc(i);

	// get variable
	node = this->_AppendNode(*i);
	__Inc(i);

	// swap parent's objects with variable
	std::swap(parent->object, node->object);
	std::swap(parent->var, node->var);

	// find type object
	*typeNode->object = *ScriptObject::FindScriptObjectByFlag(kScriptObjectType);
	typeNode->object->type = *type;

	// find variable object
	variableName = parent->object->word;
	*parent->object = *ScriptObject::FindScriptObjectByFlag(kScriptObjectVariable);
	parent->object->word = variableName;
	parent->type = kScriptNodeDeclare;
	parent->object->type = typeNode->object->type;

	if ((*i)->object == kScriptObjectSemicolon) {

		// push parent in block
		parent->block.push_back(parent);
		// push var's type in block
		parent->block.push_back(typeNode);
		// skip semicolon
		++i;
	}
	else if ((*i)->object == kScriptObjectSet) {

		// push parent in block
		parent->block.push_back(parent);
		// push var's type in block
		parent->block.push_back(typeNode);

		while (LAME_TRUE) {
			if ((*i)->object == kScriptObjectSemicolon) {
				break;
			}
			else {
				parent->block.push_back(this->_AppendNode(*i));
			}
			if (++i == this->parser->objectList.end()) {
				PostSyntaxError((*(i - 1))->line, "Expression not closed with semicolon", 1);
			}
		}
		++i;
	}
	else if ((*i)->object == kScriptObjectParentheseL) {
		parent->var->modificators = kScriptModificatorFunction;
		__Dec(i);
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

	parent->type = kScriptNodeThread;

	// skip parent node
	++i;

	while (LAME_TRUE) {

		if ((*i)->object == kScriptObjectParentheseR) {
			this->_PopStack();
			__Inc(i);
			break;
		}

		if ((*i)->object == kScriptObjectParentheseL) {
			this->_PushStack(&parent->condition);
		}
		else if ((*i)->object == kScriptObjectString) {
			if (!this->nodeQueue_) {
				PostSyntaxError((*i)->line, "Brace in unknown place without parent", 1);
			}

			ScriptNodePtr node = this->_AppendNode(*i);
			this->nodeQueue_->push_back(node);

			if (this->_BuildRecursive(node, i)) {
				__Dec(i);
			}
		}
		else {
			PostSyntaxError((*i)->line, "Thread's name must be string, but not (%s)", (*i)->word.data());
		}

		if (++i == this->parser->objectList.end()) {
			PostSyntaxError((*(i - 1))->line, "Brace mismatched", 1);
		}
	}

	if ((*i)->object == kScriptObjectBraceL) {
		this->_BuildBlock(parent, i);
	}
}

Bool ScriptBuilder::_BuildRecursive(ScriptNodePtr parent, IteratorRef i) {

	if ((*i)->IsClass()) {
		this->_BuildClass(parent, i);
	}
	else if ((*i)->IsCondition()) {
		this->_BuildCondition(parent, i);
	}
	else if ((*i)->IsVariable()) {
		this->_BuildField(parent, i);
	}
	else if ((*i)->IsThread()) {
		this->_BuildThread(parent, i);
	}
	else {
		return LAME_FALSE;
	}

	return LAME_TRUE;
}

ScriptNodePtr ScriptBuilder::_AppendNode(ScriptObjectPtr object) {

	// allocate script node
	ScriptNodePtr node (new ScriptNode());

	// allocate script variable
	this->performer->varList_.push_back(new ScriptVariable(object));

	// initialize node
	node->var = this->performer->varList_.back();
	node->object = object;
	node->var->object = object;
	node->prev = this->prevNode_;
	node->parent = this->parentNode_;

	// create doubly linked list
	if (this->prevNode_) {
		this->prevNode_->next = node;
	}

	this->prevNode_ = node;

	// initialize parent
	if (node->object->IsCondition() ||
		node->object->IsClass() ||
		node->object->IsThread()
	) {
		this->parentNode_ = node;
	}

	// check for constant and register
	this->performer->RegisterConstant(node);

	return node;
}

ScriptNodePtr ScriptBuilder::_RemoveNode(ScriptNodePtr node) {

	if (!node) {
		return LAME_NULL;
	}

	// save node's parent
	ScriptNodePtr parent = node->parent;

	// remove nodes from condition block
	if (node->condition.size()) {
		for (ScriptNodePtr n : node->condition) {
			this->_RemoveNode(n);
		}
	}

	// remove nodes from executable block
	if (node->block.size()) {
		for (ScriptNodePtr n : node->block) {
			this->_RemoveNode(n);
		}
	}

	ScriptNodePtr next = node->next;
	ScriptNodePtr prev = node->prev;

	// remove links from node
	if (prev) {
		prev->next = next;
	}
	if (next) {
		next->prev = prev;
	}

	// deallocate memory
	delete node->var;
	delete node->object;
	delete node;

	// return parent
	return parent;
}

Void ScriptBuilder::_PushStack(Vector<ScriptNodePtr>* stack) {

	// push current node stack to queue
	if (this->nodeQueue_) {
		this->stackNodeQueue_.push_back(this->nodeQueue_);
	}

	// save active queue
	this->nodeQueue_ = stack;
}

Vector<ScriptNodePtr>* ScriptBuilder::_PopStack(Void) {

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