#include "NodeBuilder.h"
#include "Exception.h"
#include "FileParser.h"

#define __Inc(_i) \
	if (_i == this->parser_->GetLexList().end() || ++_i == this->parser_->GetLexList().end()) {\
		PostSyntaxError((*(_i-1))->line, "Not finished expression", 1); \
	}

#define __Dec(_i) \
	--_i

LAME_BEGIN2(Script)

Void _AllowModificators(NodePtr node, Uint32 modificators) {

	if ((node->modificators & kScriptFlagFinal) != 0 && !(modificators & kScriptFlagFinal) ||
		(node->modificators & kScriptFlagPublic) != 0 && !(modificators & kScriptFlagPublic) ||
		(node->modificators & kScriptFlagPrivate) != 0 && !(modificators & kScriptFlagPrivate) ||
		(node->modificators & kScriptFlagProtected) != 0 && !(modificators & kScriptFlagProtected) ||
		(node->modificators & kScriptFlagStatic) != 0 && !(modificators & kScriptFlagStatic) ||
		(node->modificators & kScriptFlagNative) != 0 && !(modificators & kScriptFlagNative) ||
		(node->modificators & kScriptFlagAbstract) != 0 && !(modificators & kScriptFlagAbstract)
	) {
		PostSyntaxError(node->lex->line, "Modificator in illegal place");
	}
}

Void NodeBuilder::Build(FileParserPtr parser) {

	LexNodePtr lexNode = new LexNode("#", 0, Lex::Find(kScriptLexDefault));

	this->_Reset();

	this->parser_ = parser;
	this->parser_->GetLexList().push_front(lexNode);

	this->rootNode_ = this->_CreateNode(lexNode, kScriptNodeEntry);
	this->parentNode_ = this->rootNode_;

	Iterator first = parser->GetLexList().begin() + 1;

	if (first != parser->GetLexList().end()) {
		this->_BuildEntry(this->rootNode_, first);
	}
}

NodeBuilder::Iterator NodeBuilder::_BuildFunction(NodePtr& parent, Iterator i) {

	NodePtr functionNode;
	Buffer functionType;
	NodePtr typeNode;

	functionType = (*i++)->word;
	functionNode = this->_CreateNode(*i, kScriptNodeFunction);
	functionNode->typeName = functionType;
	typeNode = parent;
	parent = functionNode;
	this->parentNode_ = parent;
	parent->modificators = typeNode->modificators;
	parent->isMethodImpl = TRUE;
	this->modificators_ = 0;

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
			if (this->nodeQueue_ == &parent->blockList) {
				goto __SaveNode;
			}
			if (this->nodeQueue_ == &parent->argList) {
				PostSyntaxError((*i)->line, "Two similar parentheses in method arguments", 1);
			}
			this->_Push(&parent->argList);
			__Inc(i);
			while ((*i)->lex->id != kScriptLexParentheseR) {
				Bool isArray = FALSE;
				if ((*(i + 1))->lex->id == kScriptLexArray) {
					i = this->parser_->GetLexList().erase(i + 1);
					__Dec(i);
					isArray = TRUE;
				}
				if (!((*(i + 0))->lex->IsUnknown() &&
					  (*(i + 1))->lex->IsUnknown())
				) {
					PostSyntaxError((*i)->line, "Illegal token in method's arguments");
				}
				NodePtr var = _CreateNode(*(i + 1), kScriptNodeVariable);
				var->typeName = (*i)->word;
				var->isArray = isArray;
				__Inc(i);
				__Inc(i);
				this->nodeQueue_->push_back(var);
				if ((*i)->lex->id == kScriptLexComma) {
					__Inc(i);
				}
				//++parent->arguments;
			}
			__Dec(i);
		}
		else if ((*i)->lex->id == kScriptLexParentheseR) {
			if (this->nodeQueue_ == &parent->blockList) {
				goto __SaveNode;
			}
			if (this->nodeQueue_ != &parent->argList) {
				PostSyntaxError((*i)->line, "Left parenthese lost", 1);
			}
			this->_Pop();
			if ((*(i + 1))->lex->id == kScriptLexSemicolon) {
				__Inc(i);
				parent->isMethodImpl = FALSE;
				break;
			}
		}
		else if ((*i)->lex->id == kScriptLexComma) {
			if (this->nodeQueue_ == &parent->argList) {
				//++parent->arguments;
			}
			goto __NextNode;
		}
		else {
			//if (!parent->arguments && this->nodeQueue_ == &parent->argList) {
			//	++parent->arguments;
			//}
		__SaveNode:
			this->nodeQueue_->push_back(this->_CreateNode(*i, kScriptNodeDefault));
			i = this->_Build(this->nodeQueue_->back(), i);
			if ((*i)->lex->id == kScriptLexParentheseR && this->nodeQueue_ == &parent->argList) {
				__Dec(i);
			}
		}
	__NextNode:
		if (++i == this->parser_->GetLexList().end()) {
			if (this->stackNodeQueue_.size()) {
				PostSyntaxError((*(i - 1))->line, "Braces or Parentheses mismatched", 1);
			}
			break;
		}
	}
	parent->ShuntingYard();

	return i;
}

NodeBuilder::Iterator NodeBuilder::_BuildVariable(NodePtr& parent, Iterator i) {

	NodePtr typeNode = 0;
	NodePtr prevNode = 0;
	NodePtr varNode = 0;
	Buffer varType;

	varType = (*i++)->word;
	varNode = this->_CreateNode(*i, kScriptNodeVariable);
	varNode->typeName = varType;
	typeNode = parent;
	varNode->isArray = parent->isArray;
	parent = varNode;
	parent->modificators = this->modificators_;

	if (parent->parent->id == kScriptNodeClass) {
		_AllowModificators(parent, -1);
	} else {
		_AllowModificators(parent, kScriptFlagStatic | kScriptFlagFinal);
	}

	this->modificators_ = 0;

	__Inc(i);

	if (parent->parent != NULL && parent->parent->id == kScriptNodeClass) {
		this->_Push(&parent->blockList);
		parent->blockList.push_back(this->_CreateNode(parent->lex, parent->id));
	} else {
		this->_Push(&parent->argList);
	}

	while (LAME_TRUE) {
		if ((*i)->lex->id == kScriptLexSemicolon ||
			(*i)->lex->id == kScriptLexComma
		) {
			if ((*i)->lex->id == kScriptLexSemicolon) {
				this->nodeQueue_->push_back(this->_CreateNode(*i, kScriptNodeDefault));
			}
			if (parent->blockList.size() == 1) {
				parent->blockList.clear();
			}
			break;
		}
		else if ((*i)->lex->id == kScriptLexParentheseR) {
			goto __SaveNode;
		}
		else if ((*i)->lex->id == kScriptLexSet) {
			goto __SaveNode;
		}
		else {
		__SaveNode:
			this->nodeQueue_->push_back(this->_CreateNode(*i, kScriptNodeDefault));
			prevNode = this->nodeQueue_->back();
			i = this->_Build(this->nodeQueue_->back(), i);
		}
	__NextNode:
		if (++i == this->parser_->GetLexList().end()) {
			PostSyntaxError((*(i - 1))->line, "Semicolon lost", 1);
		}
	}

	this->_Pop();
	parent->ShuntingYard();

	return i;
}

NodeBuilder::Iterator NodeBuilder::_BuildClass(NodePtr& parent, Iterator i) {

	__Inc(i);

	if (!(*i)->lex->IsUnknown()) {
		PostSyntaxError((*i)->line, "Unable to create class without name", 1);
	}

	Buffer className = (*i)->word;
	parent->typeName = className;
	Uint32 modificators = 0;
	parent->modificators = this->modificators_;
	_AllowModificators(parent, -1);
	this->modificators_ = 0;

	__Inc(i);

	while (LAME_TRUE) {
		if ((*i)->lex->id == kScriptLexBraceL) {
			this->_Push(&parent->blockList);
		}
		else if ((*i)->lex->id == kScriptLexBraceR) {
			this->_Pop();
			break;
		}
		else if ((*i)->lex->id == kScriptLexExtends) {
			__Inc(i);
			parent->extends = (*i)->word;
			if ((*(i + 1))->lex->id != kScriptLexBraceL &&
				(*(i + 1))->lex->id != kScriptLexImplements
			) {
				PostSyntaxError((*(i + 1))->line, "Illegal token (%s)", (*(i + 1))->word.data());
			}
		}
		else if ((*i)->lex->id == kScriptLexImplements) {
			__Inc(i);
			while (TRUE) {
				parent->implements.push_back((*i)->word);
				__Inc(i);
				if ((*i)->lex->id != kScriptLexComma) {
					break;
				}
				__Inc(i);
			}
			if ((*i)->lex->id == kScriptLexBraceL) {
				__Dec(i);
			}
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
			case kScriptLexAbstract:
				modificators |= kScriptFlagAbstract;
				break;
			default:
				break;
			}
		}
		else {
			this->nodeQueue_->push_back(this->_CreateNode(*i, kScriptNodeDefault));
			i = this->_Build(this->nodeQueue_->back(), i);
			this->nodeQueue_->back()->modificators = modificators;
			modificators = 0;
		}
		if (++i == this->parser_->GetLexList().end()) {
			if (this->stackNodeQueue_.size()) {
				PostSyntaxError((*(i - 1))->line, "Braces mismatched", 1);
			}
			break;
		}
	}

	return i;
}

NodeBuilder::Iterator NodeBuilder::_BuildCondition(NodePtr& parent, Iterator i) {

	__Inc(i);

	Uint32 extraParenseses = 0;
	Uint32 modificators = 0;
	Bool isSingleExpression = 0;

	parent->modificators = this->modificators_;
	_AllowModificators(parent, 0);
	this->modificators_ = 0;

	if ((*i)->lex->id != kScriptLexBraceL && (
		parent->lex->lex->id == kScriptLexElse ||
		parent->lex->lex->id == kScriptLexDo ||
		parent->lex->lex->id == kScriptLexTry ||
		parent->lex->lex->id == kScriptLexFinally)
	) {
		goto __AvoidBraces;
	}

	if ((parent->lex->lex->flags & kScriptLexFlagWoParenthese) == 0) {
		if ((*i)->lex->id != kScriptLexParentheseL) {
			PostSyntaxError((*i)->line, "Left parenthesis has been lost");
		}
	}

	while (LAME_TRUE) {
		if ((*i)->lex->id == kScriptLexBraceL) {
			if (this->nodeQueue_ == &parent->argList) {
				PostSyntaxError((*i)->line, "Right parenthesis has been lost");
			}
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
			if (extraParenseses > 0) {
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
				goto __SaveNode;
			}
		}
		else if ((*i)->lex->id == kScriptLexComma) {
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
		if (++i == this->parser_->GetLexList().end()) {
			if (this->stackNodeQueue_.size()) {
				PostSyntaxError((*(i - 1))->line, "Braces or Parentheses mismatched", 1);
			}
			break;
		}
	}
	parent->ShuntingYard();

	return i;
}

NodeBuilder::Iterator NodeBuilder::_BuildEntry(NodePtr& parent, Iterator i) {

	this->_Push(&parent->blockList);
	while (LAME_TRUE) {
		this->nodeQueue_->push_back(this->_CreateNode(*i, kScriptNodeDefault));
		i = this->_Build(this->nodeQueue_->back(), i);
		if (++i == this->parser_->GetLexList().end()) {
			break;
		}
	}
	this->_Pop();
	parent->ShuntingYard();
	return i;
}

NodeBuilder::Iterator NodeBuilder::_BuildArguments(NodePtr& parent, Iterator i) {

	NodePtr prevNode = 0;
	Uint32 extraParenseses = 0;

	parent->modificators = this->modificators_;
	_AllowModificators(parent, 0);
	this->modificators_ = 0;

	__Inc(i);

	while (LAME_TRUE) {
		if ((*i)->lex->id == kScriptLexParentheseL ||
			(*i)->lex->id == kScriptLexBracketL
		) {
			if (extraParenseses == 1) {
				++extraParenseses;
				goto __SaveNode;
			}
			else {
				this->_Push(&parent->argList);
				extraParenseses = 1;
			}
		}
		else if (
			(*i)->lex->id == kScriptLexParentheseR ||
			(*i)->lex->id == kScriptLexBracketR
		) {
			if (extraParenseses == 1) {
				this->_Pop();
				break;
			}
			else {
				--extraParenseses;
			}
		}
		else if ((*i)->lex->id == kScriptLexComma) {
			if (this->nodeQueue_ == &parent->argList) {
				//++parent->arguments;
			}
			goto __NextNode;
		}
		else {
		__SaveNode:
			//if (!parent->arguments && this->nodeQueue_ == &parent->argList) {
			//	++parent->arguments;
			//}
			this->nodeQueue_->push_back(this->_CreateNode(*i, kScriptNodeDefault));
			prevNode = this->nodeQueue_->back();
			i = this->_Build(this->nodeQueue_->back(), i);
		}
	__NextNode:
		if (++i == this->parser_->GetLexList().end()) {
			if (this->stackNodeQueue_.size()) {
				PostSyntaxError((*(i - 1))->line, "Braces, Parentheses or Brackets mismatched", 1);
			}
			break;
		}
	}

	parent->ShuntingYard();

	/* Get last element in stack. Now we're going to swap
	arguments parent with argument expression, it will help us
	to prevent different unsuitable situations in future, when we'll
	perform code. For example, if we have expression 'new int [5]', then
	we have to cut it into 2 expressions and apply shunting yard for every,
	whereupon we'll swap our expressions and push in one stack. Final
	view should be : '5 int new'. We've saved count of arguments in node 'new' */

	/* Get and pop last pushed element from parent's stack */
	NodePtr beforeArguments
		= parent->parent->blockList.back();
	parent->parent->blockList.pop_back();

	/* Push argument list in parent's block stack */
	parent->parent->blockList.insert(parent->parent->blockList.end(),
		parent->argList.begin(), parent->argList.end());

	/* Return poped element to stack */
	parent->parent->blockList.push_back(beforeArguments);

	/* Clear list with arguments */
	parent->argList.clear();

	return i;
}

Uint32 NodeBuilder::_GetCountOfArguments(Iterator i) {
	
	Uint32 argumentsCount = 0;
	Uint32 extraParenseses = 0;

	__Inc(i);

	while (LAME_TRUE) {
		if ((*i)->lex->id == kScriptLexParentheseL ||
			(*i)->lex->id == kScriptLexBracketL
		) {
			if (extraParenseses > 0) {
				++extraParenseses; goto __NextNode;
			} else {
				extraParenseses = 1;
			}
		}
		else if (
			(*i)->lex->id == kScriptLexParentheseR ||
			(*i)->lex->id == kScriptLexBracketR
		) {
			if (extraParenseses == 1) {
				break;
			} else {
				--extraParenseses;
			}
		}
		else if ((*i)->lex->id == kScriptLexComma) {
			if (extraParenseses < 2) {
				++argumentsCount;
			}
			goto __NextNode;
		}
		else {
			if (!argumentsCount && extraParenseses < 2) {
				++argumentsCount;
			}
		}
	__NextNode:
		if (++i == this->parser_->GetLexList().end()) {
			if (this->stackNodeQueue_.size()) {
				PostSyntaxError((*(i - 1))->line, "Braces, Parentheses or Brackets mismatched", 1);
			}
			break;
		}
	}

	return argumentsCount;
}

NodeBuilder::Iterator NodeBuilder::_Build(NodePtr& node, Iterator i) {

	if (this->parser_->GetLexList().end() - i > 1 && (*(i + 1))->lex->id == kScriptLexArray) {
		if (!node->lex->lex->IsUnknown()) {
			PostSyntaxError(node->lex->line, "Illegal token before brackets (%s)", node->lex->word.data());
		}
		node->isArray = TRUE;
		i = this->parser_->GetLexList().erase(i + 1);
		__Dec(i);
	}

	if ((*i)->lex->id == kScriptLexIncrement ||
		(*i)->lex->id == kScriptLexDecrement
	) {
		if ((*(i + 1))->lex->IsUnknown()) {
			(*i)->priority = Lex::Find(kScriptLexPrefixIncrement)->priority;
		} else {
			LexNodePtr lexNode = new LexNode((*(i - 1))->word, (*(i - 1))->line, Lex::Find(kScriptLexDefault));
			i = this->parser_->GetLexList().insert(i, lexNode) + 1;
			this->nodeQueue_->push_back(this->_CreateNode(lexNode, kScriptNodeDefault));
		}
	}

	if (node->lex->lex->IsLanguage()) {
		if (node->lex->lex->id == kScriptLexClass ||
			node->lex->lex->id == kScriptLexInterface
		) {
			i = this->_BuildClass(node, i);
		}
		else if (node->lex->lex->IsCondition()) {
			i = this->_BuildCondition(node, i);
		}
		else if (node->lex->lex->IsModificator()) {
			switch ((*i)->lex->id) {
			case kScriptLexPublic:
				this->modificators_ |= kScriptFlagPublic;
				break;
			case kScriptLexPrivate:
				this->modificators_ |= kScriptFlagPrivate;
				break;
			case kScriptLexProtected:
				this->modificators_ |= kScriptFlagProtected;
				break;
			case kScriptLexStatic:
				this->modificators_ |= kScriptFlagStatic;
				break;
			case kScriptLexNative:
				this->modificators_ |= kScriptFlagNative;
				break;
			case kScriptLexFinal:
				this->modificators_ |= kScriptFlagFinal;
				break;
			case kScriptLexAbstract:
				this->modificators_ |= kScriptFlagAbstract;
				break;
			default:
				break;
			}
		}
	}
	else {
		if (this->parser_->GetLexList().end() - i > 2) {
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
				(*(i + 2))->lex->id == kScriptLexComma ||
				(*(i + 2))->lex->id == kScriptLexParentheseR)
			) {
				i = this->_BuildVariable(node, i);
			}
		}
		if (this->parser_->GetLexList().end() - i > 1) {
			if ((*(i + 0))->lex->IsUnknown() &&
				((*(i + 1))->lex->id == kScriptLexParentheseL ||
				 (*(i + 1))->lex->id == kScriptLexBracketL)
			) {
				if ((*(i + 1))->lex->id == kScriptLexParentheseL) {
					node->MakeInvoke();
				}
				else {
					node->MakeAlloc();
				}
				node->lex->args = this->_GetCountOfArguments(i);
				//i = this->_BuildArguments(node, i);
			}
		}
	}

	if (i == this->parser_->GetLexList().end()) {
		__Dec(i);
	}

	if (node->id != kScriptNodeDefault &&
		node->id != kScriptNodeVariable &&
		node->id != kScriptNodeInvoke &&
		node->id != kScriptNodeAlloc
	) {
		if (this->parentNode_->parent) {
			this->parentNode_ = this->parentNode_->parent;
		}
	}

	return i;
}

Void NodeBuilder::_Reset(Void) {

	for (NodePtr n : this->nodeList_) {
		delete n;
	}

	this->stackNodeQueue_.clear();
	this->nodeList_.clear();
	this->parentNode_ = 0;
	this->prevNode_ = 0;
	this->rootNode_ = 0;
	this->nodeQueue_ = 0;
	this->modificators_ = 0;
}

NodePtr NodeBuilder::_CreateNode(LexNodePtrC lex, NodeID id) {

	if (lex->lex->id == kScriptLexClass) {
		id = kScriptNodeClass;
	}
	else if (lex->lex->id == kScriptLexInterface) {
		id = kScriptNodeInterface;
	}
	else if (lex->lex->IsCondition()) {
		id = kScriptNodeCondition;
	}

	NodePtr node(new Node(lex->word, id, lex, this->parentNode_, this->prevNode_));
	this->nodeList_.push_back(node);

	if (node->lex->lex->IsCondition() || (node->lex->lex->IsLanguage() && node->lex->lex->id == kScriptLexClass)) {
		this->prevNode_ = node;
		this->parentNode_ = node;
	}

	return node;
}

NodePtr NodeBuilder::_RemoveNode(NodePtr node) {

	NodePtr parent = node->parent;

	if (node->parent) {

		Vector<NodePtr>::iterator i = node->parent->blockList.begin();

		while (i != node->parent->blockList.end()) {
			if (*i == node) {
				i = node->parent->blockList.erase(i);
				break;
			}
			++i;
		}
	}

	for (NodePtr n : node->blockList) {
		n = this->_RemoveNode(n);
	}
	node->blockList.clear();

	return parent;
}

Void NodeBuilder::_Push(Vector<NodePtr>* stack) {

	if (this->nodeQueue_) {
		this->stackNodeQueue_.push_back(this->nodeQueue_);
	}
	this->nodeQueue_ = stack;
}

Vector<NodePtr>* NodeBuilder::_Pop(Void) {

	if (!this->stackNodeQueue_.size()) {
		this->nodeQueue_ = LAME_NULL;
	}
	else {
		this->nodeQueue_ = this->stackNodeQueue_.back();
		this->stackNodeQueue_.pop_back();
	}
	return this->nodeQueue_;
}

NodeBuilder::NodeBuilder() {
	this->_Reset();
}

NodeBuilder::~NodeBuilder() {
	this->_Reset();
}

LAME_END2
