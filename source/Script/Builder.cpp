#include "Script.h"

#define __Inc(_i) \
	if (_i == this->parser_->lexList_.end() || ++_i == this->parser_->lexList_.end()) {\
		PostSyntaxError((*(_i-1))->line, "Not finished expression", 1); \
	}

#define __Dec(_i) \
	--_i

LAME_BEGIN2(Script)

Void ScriptBuilder::Build(ScriptParserPtr parser) {

	ScriptLexNode lexNode("${root}", 0, ScriptParser::Find(LAME_NULL, kScriptLexDefault));
	this->_Reset();
	this->parser_ = parser;
	this->rootNode_ = this->_CreateNode(&lexNode, kScriptNodeEntry);
	this->parentNode_ = this->rootNode_;
	Iterator first = parser->lexList_.begin();
	this->_BuildEntry(this->rootNode_, first);
}

ScriptBuilder::Iterator ScriptBuilder::_BuildFunction(ScriptNodePtr& parent, Iterator i) {

	ScriptNodePtr functionNode;
	Buffer functionType;
	ScriptNodePtr typeNode;
	
	functionType = (*i++)->word;
	functionNode = this->_CreateNode(*i, kScriptNodeFunction);
	functionNode->typeName = functionType;
	typeNode = parent;
	parent = functionNode;
	this->parentNode_ = parent;
	parent->modificators = typeNode->modificators;
	
	__Inc(i);

	while (LAME_TRUE) {
		if ((*i)->lex->id == kScriptLexBraceL) {
			this->_Push(&parent->blockList);
		}
		else if ((*i)->lex->id == kScriptLexBraceR) {
			if (this->nodeQueue_ != &parent->blockList) {
				PostSyntaxError((*i)->line, "Left brace lost or braces mismatched", 1);
			}
			this->_Pop();
			break;
		}
		else if ((*i)->lex->id == kScriptLexParentheseL) {
			if (this->nodeQueue_ == &parent->argList) {
				PostSyntaxError((*i)->line, "Two similar parentheses in function arguments", 1);
			}
			this->_Push(&parent->argList);
		}
		else if ((*i)->lex->id == kScriptLexParentheseR) {
			if (this->nodeQueue_ != &parent->argList) {
				PostSyntaxError((*i)->line, "Left parenthese lost", 1);
			}
			this->_Pop();
			if ((*(i + 1))->lex->id == kScriptLexSemicolon) {
				__Inc(i);
				break;
			}
		}
		else if ((*i)->lex->id == kScriptLexComa) {
			if (this->nodeQueue_ == &parent->argList) {
				++parent->args;
			}
			goto __NextNode;
		}
		else {
			if (!parent->args && this->nodeQueue_ == &parent->argList) {
				++parent->args;
			}
		__SaveNode:
			this->nodeQueue_->push_back(this->_CreateNode(*i, kScriptNodeDefault));
			i = this->_Build(this->nodeQueue_->back(), i);
			if ((*i)->lex->id == kScriptLexParentheseR && this->nodeQueue_ == &parent->argList) {
				__Dec(i);
			}
		}
	__NextNode:
		if (++i == this->parser_->lexList_.end()) {
			if (this->stackNodeQueue_.size()) {
				PostSyntaxError((*(i - 1))->line, "Braces or Parentheses mismatched", 1);
			}
			break;
		}
	}
	parent->Order();

	return i;
}

ScriptBuilder::Iterator ScriptBuilder::_BuildVariable(ScriptNodePtr& parent, Iterator i) {

	ScriptNodePtr typeNode = 0;
	ScriptNodePtr prevNode = 0;
	ScriptNodePtr varNode = 0;
	Buffer varType;

	varType = (*i++)->word;
	varNode = this->_CreateNode(*i, kScriptNodeVariable);
	varNode->typeName = varType;
	typeNode = parent;
	parent = varNode;
	parent->modificators = typeNode->modificators;

	__Inc(i);

	this->_Push(&parent->blockList);

	while (LAME_TRUE) {
		if ((*i)->lex->id == kScriptLexSemicolon ||
			(*i)->lex->id == kScriptLexComa ||
			(*i)->lex->id == kScriptLexParentheseR
		) {
			break;
		}
		else if ((*i)->lex->id == kScriptLexSet) {
			goto __NextNode;
		}
		else {
			this->nodeQueue_->push_back(this->_CreateNode(*i, kScriptNodeDefault));
			prevNode = this->nodeQueue_->back();
			i = this->_Build(this->nodeQueue_->back(), i);
		}
	__NextNode:
		if (++i == this->parser_->lexList_.end()) {
			PostSyntaxError((*(i - 1))->line, "Semicolon lost", 1);
		}
	}

	this->_Pop();
	parent->Order();

	return i;
}

ScriptBuilder::Iterator ScriptBuilder::_BuildClass(ScriptNodePtr& parent, Iterator i) {

	__Inc(i);

	if (!(*i)->lex->IsUnknown()) {
		PostSyntaxError((*i)->line, "Unable to create class without name", 1);
	}

	Buffer className = (*i)->word;
	parent->typeName = className;
	Uint32 modificators = 0;

	__Inc(i);

	while (LAME_TRUE) {
		if ((*i)->lex->id == kScriptLexBraceL) {
			this->_Push(&parent->blockList);
		}
		else if ((*i)->lex->id == kScriptLexBraceR) {
			this->_Pop();
			break;
		}
		else if ((*i)->lex->IsModificator()) {
			switch ((*i)->lex->id) {
                case kScriptLexPublic:
                    modificators |= kScriptFlagPublic;
                    break;
                case kScriptLexPrivate:
                    modificators |= kScriptFlagPrivate;
                    break;
                case kScriptLexProtected:
                    modificators |= kScriptFlagProtected;
                    break;
                case kScriptLexStatic:
                    modificators |= kScriptFlagStatic;
                    break;
                case kScriptLexNative:
                    modificators |= kScriptFlagNative;
                    break;
                case kScriptLexFinal:
                    modificators |= kScriptFlagFinal;
                    break;
                default:
                    break;
			}
		}
		else {
			this->nodeQueue_->push_back(this->_CreateNode(*i, kScriptNodeDefault));
			this->nodeQueue_->back()->modificators = modificators;
			modificators = 0;
			i = this->_Build(this->nodeQueue_->back(), i);
		}
		if (++i == this->parser_->lexList_.end()) {
			if (this->stackNodeQueue_.size()) {
				PostSyntaxError((*(i - 1))->line, "Braces mismatched", 1);
			}
			break;
		}
	}

	return i;
}

ScriptBuilder::Iterator ScriptBuilder::_BuildCondition(ScriptNodePtr& parent, Iterator i) {

	__Inc(i);

	Uint32 extraParenseses = 0;
	Uint32 modificators = 0;
	Bool isSingleExpression = 0;

	if ((*i)->lex->id != kScriptLexBraceL && (
		parent->lex->lex->id == kScriptLexElse ||
		parent->lex->lex->id == kScriptLexDo)
	) {
		goto __AvoidBraces;
	}

	while (LAME_TRUE) {
		if ((*i)->lex->id == kScriptLexBraceL) {
			this->_Push(&parent->blockList);
		}
		else if ((*i)->lex->id == kScriptLexBraceR) {
			if (isSingleExpression) {
				__Dec(i);
				break;
			}
			this->_Pop();
			break;
		}
		else if ((*i)->lex->id == kScriptLexParentheseL) {
			if (extraParenseses == 1) {
				++extraParenseses;
				goto __SaveNode;
			}
			else {
				this->_Push(&parent->argList);
				extraParenseses = 1;
			}
		}
		else if ((*i)->lex->id == kScriptLexParentheseR) {
			if (extraParenseses == 1) {
				this->_Pop();
				if ((*(i + 1))->lex->id == kScriptLexSemicolon) {
					break;
				}
				else if ((*(i + 1))->lex->id != kScriptLexBraceL) {
				__AvoidBraces:
					this->_Push(&parent->blockList);
					isSingleExpression = LAME_TRUE;
					if ((*i)->lex->id != kScriptLexParentheseR) {
						goto __SaveNode;
					}
				}
			}
			else {
				--extraParenseses;
			}
		}
		else if ((*i)->lex->id == kScriptLexComa) {
			goto __NextNode;
		}
		else if ((*i)->lex->IsModificator()) {
			switch ((*i)->lex->id) {
			case kScriptLexFinal:
				modificators |= kScriptFlagFinal;
				break;
			default:
				PostSyntaxError((*i)->line, "You can use this modificator only in classes (%s)", (*i)->word.data());
			}
		}
		else if ((*i)->lex->id == kScriptLexSemicolon) {
			if (!isSingleExpression) {
				goto __SaveNode;
			}
			isSingleExpression = LAME_FALSE;
			this->_Pop();
			break;
		}
		else {
		__SaveNode:
			this->nodeQueue_->push_back(this->_CreateNode(*i, kScriptNodeDefault));
			if (modificators) {
				this->nodeQueue_->back()->modificators = modificators;
			}
			modificators = 0;
			i = this->_Build(this->nodeQueue_->back(), i);
		}
	__NextNode:
		if (++i == this->parser_->lexList_.end()) {
			if (this->stackNodeQueue_.size()) {
				PostSyntaxError((*(i - 1))->line, "Braces or Parentheses mismatched", 1);
			}
			break;
		}
	}
	parent->Order();

	return i;
}

ScriptBuilder::Iterator ScriptBuilder::_BuildEntry(ScriptNodePtr& parent, Iterator i) {

	this->_Push(&parent->blockList);
	while (LAME_TRUE) {
		this->nodeQueue_->push_back(this->_CreateNode(*i, kScriptNodeDefault));
		i = this->_Build(this->nodeQueue_->back(), i);
		if (++i == this->parser_->lexList_.end()) {
			break;
		}
	}
	this->_Pop();
	return i;
}

ScriptBuilder::Iterator ScriptBuilder::_BuildArguments(ScriptNodePtr& parent, Iterator i) {

	ScriptNodePtr prevNode = 0;
	Uint32 extraParenseses = 0;

	__Inc(i);

	while (LAME_TRUE) {
		if ((*i)->lex->id == kScriptLexParentheseL) {
			if (extraParenseses == 1) {
				++extraParenseses;
				goto __SaveNode;
			}
			else {
				this->_Push(&parent->argList);
				extraParenseses = 1;
			}
		}
		else if ((*i)->lex->id == kScriptLexParentheseR) {
			if (extraParenseses == 1) {
				this->_Pop();
				break;
			}
			else {
				--extraParenseses;
			}
		}
		else if ((*i)->lex->id == kScriptLexComa) {
			if (this->nodeQueue_ == &parent->argList) {
				++parent->args;
			}
			goto __NextNode;
		}
		else {
		__SaveNode:
			if (!parent->args && this->nodeQueue_ == &parent->argList) {
				++parent->args;
			}
			this->nodeQueue_->push_back(this->_CreateNode(*i, kScriptNodeDefault));
			prevNode = this->nodeQueue_->back();
			i = this->_Build(this->nodeQueue_->back(), i);
		}
	__NextNode:
		if (++i == this->parser_->lexList_.end()) {
			if (this->stackNodeQueue_.size()) {
				PostSyntaxError((*(i - 1))->line, "Braces or Parentheses mismatched", 1);
			}
			break;
		}
	}
	parent->Order();

	return i;
}

ScriptBuilder::Iterator ScriptBuilder::_Build(ScriptNodePtr& node, Iterator i) {

	if (node->lex->lex->IsLanguage()) {
		if (node->lex->lex->id == kScriptLexClass) {
			i = this->_BuildClass(node, i);
		}
		else if (node->lex->lex->IsCondition()) {
			i = this->_BuildCondition(node, i);
		}
		else if (node->lex->lex->IsModificator()) {
		}
	}
	else {
		if (this->parser_->lexList_.end() - i > 2) {
			if ((*(i + 0))->lex->IsUnknown() &&
				(*(i + 1))->lex->IsUnknown() &&
				(*(i + 2))->lex->id == kScriptLexParentheseL
			) {
				i = this->_BuildFunction(node, i);
			}
			else if (
				(*(i + 0))->lex->IsUnknown() &&
				(*(i + 1))->lex->IsUnknown() &&
				((*(i + 2))->lex->id == kScriptLexSemicolon ||
				(*(i + 2))->lex->id == kScriptLexSet ||
				(*(i + 2))->lex->id == kScriptLexComa ||
				(*(i + 2))->lex->id == kScriptLexParentheseR)
			) {
				i = this->_BuildVariable(node, i);
			}
		}
		if (this->parser_->lexList_.end() - i > 1) {
			if ((*(i + 0))->lex->IsUnknown() &&
				(*(i + 1))->lex->id == kScriptLexParentheseL
			) {
				node->MakeInvoke();
				i = this->_BuildArguments(node, i);
			}
		}
	}

	if (i == this->parser_->lexList_.end()) {
		__Dec(i);
	}

	if (node->id != kScriptNodeDefault && node->id != kScriptNodeVariable) {
		if (this->parentNode_->parent) {
			this->parentNode_ = this->parentNode_->parent;
		}
	}

	return i;
}

Void ScriptBuilder::_Reset(Void) {

	for (ScriptNodePtr n : this->nodeList_) {
		if (n->var) {
			n->var = 0;
		}
		delete n;
	}

	this->stackNodeQueue_.clear();
	this->nodeList_.clear();
	this->parentNode_ = 0;
	this->prevNode_ = 0;
	this->rootNode_ = 0;
	this->nodeQueue_ = 0;
}

ScriptNodePtr ScriptBuilder::_CreateNode(ScriptLexNodePtrC lex, ScriptNodeID id) {

	if (lex->lex->id == kScriptLexClass) {
		id = kScriptNodeClass;
	}
	else if (lex->lex->IsCondition()) {
		id = kScriptNodeCondition;
	}

	// allocate script node
	ScriptNodePtr node(new ScriptNode(lex->word, id, lex, this->parentNode_, this->prevNode_));
	this->nodeList_.push_back(node);

	// initialize parent
	if (node->lex->lex->IsCondition() || (node->lex->lex->IsLanguage() && node->lex->lex->id == kScriptLexClass)) {
		this->prevNode_ = node;
		this->parentNode_ = node;
	}

	return node;
}

ScriptNodePtr ScriptBuilder::_RemoveNode(ScriptNodePtr node) {

	ScriptNodePtr parent = node->parent;

	if (node->parent) {

		Vector<ScriptNodePtr>::iterator i = node->parent->blockList.begin();

		while (i != node->parent->blockList.end()) {
			if (*i == node) {
				i = node->parent->blockList.erase(i);
				break;
			}
			++i;
		}
	}

	for (ScriptNodePtr n : node->blockList) {
		n = this->_RemoveNode(n);
	}
	node->blockList.clear();

	// return parent
	return parent;
}

Void ScriptBuilder::_Push(Vector<ScriptNodePtr>* stack) {

	// push current node stack to queue
	if (this->nodeQueue_) {
		this->stackNodeQueue_.push_back(this->nodeQueue_);
	}
	this->nodeQueue_ = stack;
}

Vector<ScriptNodePtr>* ScriptBuilder::_Pop(Void) {

	// pop vector from queue
	if (!this->stackNodeQueue_.size()) {
		this->nodeQueue_ = LAME_NULL;
	}
	else {
		this->nodeQueue_ = this->stackNodeQueue_.back();
		this->stackNodeQueue_.pop_back();
	}
	return this->nodeQueue_;
}

ScriptBuilder::ScriptBuilder() {
	this->_Reset();
}

ScriptBuilder::~ScriptBuilder() {
	this->_Reset();
}

LAME_END2
