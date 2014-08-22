#include "NodeBuilder.h"
#include "Exception.h"

/*	Safe increment for parser's list iterator, it will
	check for end of list and throws an exception if we can't
	finish our lex building. */

#define __Inc(_i) \
	if (_i == this->parser_->GetLexList().end() || ++_i == this->parser_->GetLexList().end()) { \
		PostSyntaxError((*(_i-1))->line, "Not finished expression", 1); \
		}
#define __Dec(_i) \
	--_i

LAME_BEGIN2(Script)

static Void _AllowModificators(NodePtr node, Uint32 modificators) {

	/*	After building some script lex, we have to check its modificators,
		for example - we can't write such code : static if (true) { ... }, so
		before begin node build we must check out it's modificators.
		Node parameter - is a parent of current building lex, for condition block
		it will be 'if'/'else'/'for' etc, for class lex it will be 'class' etc.
		Modificators parameter - is allowed modificators for current lex.
		If something got wrong we will throw an exception with syntax error. */

	if (((node->flags & kScriptFlagFinal) != 0 && !(modificators & kScriptFlagFinal)) ||
		((node->flags & kScriptFlagPublic) != 0 && !(modificators & kScriptFlagPublic)) ||
		((node->flags & kScriptFlagPrivate) != 0 && !(modificators & kScriptFlagPrivate)) ||
		((node->flags & kScriptFlagProtected) != 0 && !(modificators & kScriptFlagProtected)) ||
		((node->flags & kScriptFlagStatic) != 0 && !(modificators & kScriptFlagStatic)) ||
		((node->flags & kScriptFlagNative) != 0 && !(modificators & kScriptFlagNative)) ||
		((node->flags & kScriptFlagAbstract) != 0 && !(modificators & kScriptFlagAbstract)) ||
		((node->flags & kScriptFlagOverride) != 0 && !(modificators & kScriptFlagOverride)) ||
		((node->flags & kScriptFlagDeprecated) != 0 && !(modificators & kScriptFlagDeprecated))
	) {
		PostSyntaxError(node->lex->line, "Modificator in illegal place", 1);
	}
}

Void NodeBuilder::Build(FileParserPtr parser) {

	/*	We have to create default root node with
		main initialize code. It will be abstract without
		any braces but will have own scope. Programmer will
		have to write code in root node's block likes JavaScript
		and it will be succesfully translated into LameByteCode. */

	LexNodePtr lexNode = new LexNode("#", 0, Lex::Find(kScriptLexDefault));

	/*	Reset current node builder. It will release all allocated
		memory for nodes and clear node tree. */

	this->_Reset();

	/*	Initializing script parser and insert root lex node
		into the top of the list. */

	this->parser_ = parser;
	this->parser_->GetLexList().push_front(lexNode);

	/*	Creating root node, which will be the entry point
		of our script code. */

	this->rootNode_ = this->_CreateNode(lexNode, kScriptNodeEntry);
	this->parentNode_ = this->rootNode_;

	/*	Find first object in parser's list with lexes and
		get it's iterator, _BuildEntry function will build
		entry node and invoke recursive _Build method, which
		will identity all script lexes and generate all nodes. */

	Iterator first = parser->GetLexList().begin() + 1;

	if (first != parser->GetLexList().end()) {
		this->_BuildEntry(this->rootNode_, first);
	}
}

NodeBuilder::Iterator NodeBuilder::_BuildFunction(NodePtr& parent, Iterator i) {

	__Inc(i);

	/*	In this lex we will build class's method. We have to seperate
		code into two blocks : block with arguments and block with
		body. In argument block node will generate list with arguments
		nodes (only one node for one argument), also it will have nessesary
		information about it's type or template and have different flags. */

	NodePtr functionNode;
	NodePtr typeNode;

	/*	Our method lex has next syntax : default default (default...) { default... }, it
		mean that parent node will be 'default' - its a type (void, int, float etc), its wrong
		and we must swap with method's name (first and second node). First we have to create
		main method's node instead of parent. */

	functionNode = this->_CreateNode(*i, kScriptNodeFunction);
	typeNode = parent;
	functionNode->Type(typeNode);
	parent = functionNode;
	this->parentNode_ = parent;
	parent->flags |= kScriptFlagImplemented;
	this->modificators_ = 0;

	__Inc(i);

	while (LAME_TRUE) {

		/*	If we will meet left/right brace, then
			we have to build it's block list, so
			we push it to builder's queue and waiting
			for right brace, if something got wrong
			we will throw an exception with error, else
			we will pop stack from queue and finish
			our journey. */

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

		/*	But we can meet left/right parenthesis. That
			means that we met method's arguments and now
			we must read all arguments and push it in
			method's list with arguments, if we have
			some queue offset, then something wrong
			had happened with braces or parentheses -
			its a mistake, throw syntax error. */

		else if ((*i)->lex->id == kScriptLexParenthesisL) {
			if (this->nodeQueue_ == &parent->blockList) {
				goto __SaveNode;
			}
			if (this->nodeQueue_ == &parent->argList) {
				PostSyntaxError((*i)->line, "Two similar parenthesis in method arguments", 1);
			}
			this->_Push(&parent->argList);
			__Inc(i);
			while ((*i)->lex->id != kScriptLexParenthesisR) {
				if (!((*(i + 0))->lex->IsUnknown() &&
					(*(i + 1))->lex->IsUnknown())
				) {
					PostSyntaxError((*i)->line, "Illegal token in method's arguments", 1);
				}
				NodePtr var = this->_CreateNode(*(i + 1), kScriptNodeVariable);
				var->Type(this->_CreateNode(*i, kScriptNodeDefault));
				__Inc(i);
				__Inc(i);
				this->nodeQueue_->push_back(var);
				if ((*i)->lex->id == kScriptLexComma) {
					__Inc(i);
				}
			}
			__Dec(i);
		}
		else if ((*i)->lex->id == kScriptLexParenthesisR) {
			if (this->nodeQueue_ == &parent->blockList) {
				goto __SaveNode;
			}
			if (this->nodeQueue_ != &parent->argList) {
				PostSyntaxError((*i)->line, "Left parenthese lost", 1);
			}
			this->_Pop();
			if ((*(i + 1))->lex->id == kScriptLexSemicolon) {
				__Inc(i);
				parent->flags &= ~kScriptFlagImplemented;
				break;
			}
		}

		/*	Ignore command, that symbols we've
			just used to calculate count of
			method's arguments */

		else if ((*i)->lex->id == kScriptLexComma) {
			goto __NextNode;
		}

		/*	Else we've met something unknown with default id,
			it may be operator, variable or condition, which
			we've met in method's block, so we have to invoke
			recursive _Build method to continue building. */

		else {
		__SaveNode:
			NodePtr node = this->_CreateNode(*i, kScriptNodeDefault);
			if (node->lex->lex->id == kScriptLexReturn) {
				node->lex->args = 0;
			}
			if (node->previous->lex->lex->id == kScriptLexReturn && node->lex->lex->IsUnknown()) {
				node->previous->lex->args = 1;
			}
			i = this->_Build(node, i);
			this->nodeQueue_->push_back(node);
			if ((*i)->lex->id == kScriptLexParenthesisR && this->nodeQueue_ == &parent->argList) {
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

	/*	This lex is made for variables, but only variable
		declaration, likes in methods or classes, for example :
		'int value = 100;' or 'class A { public int value = 100; }' */

	NodePtr typeNode = 0;
	NodePtr prevNode = 0;
	NodePtr varNode = 0;

	__Inc(i);

	/*	Our method lex has next syntax : default default; etc. It mean that parent
		node will be 'default' - its a type (void, int, float etc), its wrong
		and we must swap with variable's name (first and second node). First we have to create
		main variables's node instead of parent (variable type). */

	varNode = this->_CreateNode(*i, kScriptNodeVariable);
	varNode->flags = parent->flags;
	typeNode = parent;
	varNode->Type(typeNode);
	parent = varNode;
	parent->flags = this->modificators_;

	/*	We can create declare variable with limited modificators
		if it separates not in class scope (Only final modificator).
		If our parent is class, then allow all modificators. */

	if (parent->parent->id == kScriptNodeClass) {
		_AllowModificators(parent, -1 & ~kScriptFlagOverride);
	}
	else {
		_AllowModificators(parent, kScriptFlagFinal);
	}

	this->modificators_ = 0;

	__Inc(i);

	if ((*i)->lex->id != kScriptLexSemicolon &&
		(*i)->lex->id != kScriptLexColon &&
		(*i)->lex->id != kScriptLexSet &&
		(*i)->lex->id != kScriptLexComma
	) {
		goto _SkipWhile;
	}

	/*	Yep, if out parent is class then before building
		variable's arguments we must save it's node in parent's
		list with blocks. Why? Because our expresssion has next
		type : default default = ...; But after build we will
		lost variable's node and we have back it to our expression. */

	if (parent->parent->id == kScriptNodeClass) {
		this->_Push(&parent->blockList);
		parent->blockList.push_back(this->_CreateNode(parent->lex, parent->id));
	}
	else {
		this->_Push(&parent->argList);
	}

	while (LAME_TRUE) {

		/*	We can meet extra token in
			expression, so we don't
			have to parse variable anymore. */

		if (!(*i)->lex->IsUnknown() &&
			!(*i)->lex->IsConst() &&
			!(*i)->lex->IsMath() &&
			!(*i)->lex->IsBool() &&
            !(*i)->lex->IsLanguage()
		) {
            LAME_TODO("No brake for cast, check out other ways");
		}

		/*	Interesting moment, semicolon or comma. We can
			declare variable with one type, but divide it
			with comma, likes int a, b; I don't know will
			it works, but will tets it later. */

		if ((*i)->lex->id == kScriptLexSemicolon ||
			(*i)->lex->id == kScriptLexColon
		) {
			if ((*i)->lex->id == kScriptLexSemicolon) {
				this->nodeQueue_->push_back(this->_CreateNode(*i, kScriptNodeDefault));
			}
			if (parent->blockList.size() == 1) {
				parent->blockList.clear();
			}
			break;
		}

		/*	Anyway its simply an expression, so we
			only can save all its nodes. */

		else if (
			(*i)->lex->id == kScriptLexParenthesisR ||
			(*i)->lex->id == kScriptLexSet
		) {
			goto __SaveNode;
		}
		else {
		__SaveNode:
			this->nodeQueue_->push_back(this->_CreateNode(*i, kScriptNodeDefault));
			prevNode = this->nodeQueue_->back();
			i = this->_Build(this->nodeQueue_->back(), i);
		}
		if ((*i)->lex->id == kScriptLexSemicolon) {
			break;
		}
		if (++i == this->parser_->GetLexList().end()) {
			PostSyntaxError((*(i - 1))->line, "Semicolon lost", 1);
		}
	}

	this->_Pop();

_SkipWhile:
	parent->ShuntingYard();

	return i;
}

NodeBuilder::Iterator NodeBuilder::_BuildClass(NodePtr& parent, Iterator i) {

	/*	The easiest lex builder is class builder, cuz it has
		only it's name and body, all variables or methods will
		be build in recursive _Build function. But appended to
		class object. */

	if (parent->id != kScriptNodeAnonymous) {
		__Inc(i);
	}

	/*	If class hasn't name we will throw an exception */

	if (!(*i)->lex->IsUnknown() && parent->id != kScriptNodeAnonymous) {
		PostSyntaxError((*i)->line, "Unable to create class without name", 1);
	}

	/* Generating node with class name */

	parent->Type(this->_CreateNode(*i, kScriptNodeDefault));
	Uint32 modificators = 0;
	parent->flags = this->modificators_;

	/* Allow all modificators for class */

	_AllowModificators(parent, -1);
	this->modificators_ = 0;

	if (this->parser_->GetLexList().end() - i > 3 &&
		(*(i + 0))->lex->id == kScriptLexDefault &&
		(*(i + 1))->lex->id == kScriptLexBellow &&
		(*(i + 2))->lex->id == kScriptLexDefault &&
		(*(i + 3))->lex->id == kScriptLexAbove
	) {
		i = _BuildTemplate(parent->typeNode, i);
	}

	if (parent->id != kScriptNodeAnonymous) {
		__Inc(i);
	}

	while (LAME_TRUE) {
		
		/*	Left or right braces, especially nothing.
			Simple building. */

		if ((*i)->lex->id == kScriptLexBraceL) {
			this->_Push(&parent->blockList);
		}
		else if ((*i)->lex->id == kScriptLexBraceR) {
			this->_Pop();
			break;
		}

		/*	Fix for constructor */

		else if (this->parser_->GetLexList().end() - i > 2 &&
			(*(i + 0))->lex->id == kScriptLexDefault &&
			(*(i + 1))->lex->id == kScriptLexParenthesisL &&
			(*(i + 2))->lex->id == kScriptLexParenthesisR
		) {
			if ((*i)->word == parent->typeNode->word) {
				i = this->parser_->GetLexList().insert(i, new LexNode("void", (*i)->line, Lex::Find(kScriptLexDefault)));
			}
			goto __SaveNode;
		}

		/*	If we've met 'extend' key word, then we
			must remember that our class extends
			something, but only if it's lex is default.
			And afeter extend key word and class name we
			must have 'implements' or 'body', if its wrong
			we will throw an exception. */

		else if ((*i)->lex->id == kScriptLexExtends) {
			__Inc(i);
			parent->Extend(this->_CreateNode(*i, kScriptNodeDefault));
			if ((*(i + 1))->lex->id != kScriptLexBraceL &&
				(*(i + 1))->lex->id != kScriptLexImplements
			) {
				PostSyntaxError((*(i + 1))->line, "Illegal token (%s)", (*(i + 1))->word.data());
			}
		}

		/*	But also we can met 'implements' key word. It can
			store more then one class, so we have to look
			through all nodes and push it to parent's implement
			list. */

		else if ((*i)->lex->id == kScriptLexImplements) {
			__Inc(i);
			while (TRUE) {
				parent->Implement(this->_CreateNode(*i, kScriptNodeDefault));
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

		/*	If we've met modificator ... we use another
			modificator builder instead of default, cuz ...
			i don't remeber why, maybe its extra. */

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
			case kScriptLexDecprecated:
				modificators |= kScriptFlagDeprecated;
				break;
			case kScriptLexOverride:
				modificators |= kScriptFlagOverride;
				break;
			default:
				break;
			}
		}

		/*	Build another class nodes with saved. */

		else {
		__SaveNode:
			this->nodeQueue_->push_back(this->_CreateNode(*i, kScriptNodeDefault));
			i = this->_Build(this->nodeQueue_->back(), i);
			this->nodeQueue_->back()->flags |= modificators;
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

	/*	Condition is a lex, which builds node with condition block.
		Condition block is every languages construction with own scope
		likes if/else/while/for/catch/try etc. Its one of the most difficult
		thing, cuz it has many conditions in its building. */

	__Inc(i);

	Uint32 extraParenseses = 0;
	Uint32 modificators = 0;
	Bool isSingleExpression = 0;
	Uint32 expressionLength = 0;
	Uint32 totalExpCount = 0;
	Bool isForEach = 0;

	parent->flags = this->modificators_;
	_AllowModificators(parent, 0);
	this->modificators_ = 0;

	/*	Yep, we can avoid braces in our construction,
		for example, 'while(true);' - not allowed
		construction for Java, cuz we must catch and
		interruption there, btw we have to implement it. */

	if ((*i)->lex->id != kScriptLexBraceL && (
		parent->lex->lex->id == kScriptLexElse ||
		parent->lex->lex->id == kScriptLexDo ||
		parent->lex->lex->id == kScriptLexTry ||
		parent->lex->lex->id == kScriptLexFinally)
	) {
		goto __AvoidBraces;
	}

	/*	Some language construction has flag, which means that it can
		exists without parentheses, likes try, else, finally.
		We check it, cuz shunting yard alghoritm will simple
		sort it without any exceptions. But we will have an
		error in expression, cuz language constructions has
		lower priority, then other blocks. Btw we can grow it's
		priority and write language constructions likes in swift
		or lua lanugages : 'if a > b' { ... }', but its Java and
		not needed. */

	if ((parent->lex->lex->flags & kScriptLexFlagWoParentheses) == 0) {
		if ((*i)->lex->id != kScriptLexParenthesisL) {
			PostSyntaxError((*i)->line, "Left parenthesis has been lost", 1);
		}
	}

	while (LAME_TRUE) {

		/*	If we've met left brace, but
			havn't finished building arguments, then
			we've lost right parenthesis, else 
			can succesfully begin block building.
			Else some constructions, which has block
			from braces can have only one expression in
			block, likes 'if (1) value = 100;' and we
			must catch it and prevent mistakes. */

		if ((*i)->lex->id == kScriptLexBraceL) {
			if (this->nodeQueue_ == &parent->argList) {
				PostSyntaxError((*i)->line, "Right parenthesis has been lost", 1);
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

		/*	If we've met semicolon and parent
			node name is 'for' then we are
			working with foreach construction. */

		else if ((*i)->lex->id == kScriptLexColon) {
			isForEach = TRUE;
		}

		/*	Left/Right parentheses will build
			list with arguments, later it will
			be combined with parent's expression
			and 'if (1) { ... }' will be converted
			to '1 if { ... }'. But not now, ony after
			order. */

		else if ((*i)->lex->id == kScriptLexParenthesisL) {
			if (extraParenseses > 0) {
				++extraParenseses;
				goto __SaveNode;
			}
			else {
				this->_Push(&parent->argList);
				extraParenseses = 1;
			}
		}
		else if ((*i)->lex->id == kScriptLexParenthesisR) {
			if (extraParenseses == 1) {
				this->_Pop();
				if ((*(i + 1))->lex->id == kScriptLexSemicolon) {
					break;
				}
				else if ((*(i + 1))->lex->id != kScriptLexBraceL) {
				__AvoidBraces:
					this->_Push(&parent->blockList);
					isSingleExpression = LAME_TRUE;
					if ((*i)->lex->id != kScriptLexParenthesisR) {
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

		/*	Hmm, one more modificator condition, i don't
			think that it will work. And why i left final
			modificator for lanugage construction?
			Likes : 'final if(1) { ... }', i'm too silly. */

		else if ((*i)->lex->IsModificator()) {
			switch ((*i)->lex->id) {
			case kScriptLexFinal:
				modificators |= kScriptFlagFinal;
				break;
			default:
				PostSyntaxError((*i)->line, "You can use this modificator only in classes (%s)", (*i)->word.data());
			}
		}

		/*	Semicolon condition for single expression
			block. */

		else if ((*i)->lex->id == kScriptLexSemicolon) {
			if (parent->word == "for") {
				if (expressionLength > 0) {
					parent->lex->args++;
				}
				++totalExpCount;
				expressionLength = 0;
			}
			if (!isSingleExpression) {
				goto __SaveNode;
			}
			isSingleExpression = LAME_FALSE;
			this->_Pop();
			break;
		}

		/*	Else we will build other
			nodes with _Build method. */

		else {
		__SaveNode:
			this->nodeQueue_->push_back(this->_CreateNode(*i, kScriptNodeDefault));
			if (modificators) {
				this->nodeQueue_->back()->flags = modificators;
			}
			++expressionLength;
			modificators = 0;
			i = this->_Build(this->nodeQueue_->back(), i);
			if (this->nodeQueue_->back()->id == kScriptNodeVariable) {
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

	if (isForEach) {
		parent->lex->args = 2;
		parent->flags |= kScriptFlagForEach;
	}
	else if (parent->word == "for") {
		if (expressionLength > 0) {
			parent->lex->args++;
		}
		if (++totalExpCount != 3) {
			PostSyntaxError(parent->lex->line, "For construction must have 3 expressions (%d)", totalExpCount);
		}
	}

	return i;
}

NodeBuilder::Iterator NodeBuilder::_BuildEntry(NodePtr& parent, Iterator i) {

	/*	Build only for entry, where we can store
		anything, cuz it root. so we can simply
		save it's block list and build it recursively. */

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

NodeBuilder::Iterator NodeBuilder::_BuildArray(NodePtr& parent, Iterator i) {

	/*	We can met something likes array in our code,
		for example : 'int[] array;'. We can't analize []
		token (its different with '[' or ']' token), but
		we can remove that node and change parent's node flags
		to show our code analizer or translator that its an array. */

	if (!parent->lex->lex->IsUnknown()) {
		PostSyntaxError(parent->lex->line, "Illegal token before brackets (%s)", parent->lex->word.data());
	}
	parent->flags |= kScriptFlagArray;
	i = this->parser_->GetLexList().erase(i + 1);

	__Dec(i);

	return i;
}

NodeBuilder::Iterator NodeBuilder::_BuildArgumentList(NodePtr& parent, Iterator i) {

	/*	We can met something likes argument list in our code,
		for example : 'int... argumentList;'. We can't analize ...
		token, we can remove that node and change parent's node flags
		to show our code analizer or translator that its an array. */

	if (!parent->lex->lex->IsUnknown()) {
		PostSyntaxError(parent->lex->line, "Illegal token before brackets (%s)", parent->lex->word.data());
	}
	parent->flags |= kScriptFlagArgumentList;
	i = this->parser_->GetLexList().erase(i + 1);

	__Dec(i);

	return i;
}

NodeBuilder::Iterator NodeBuilder::_BuildTemplate(NodePtr& parent, Iterator i) {

	parent->Template(this->_CreateNode(*(i + 2), kScriptNodeDefault));

	i = this->parser_->GetLexList().erase(i + 1);
	i = this->parser_->GetLexList().erase(i);
	i = this->parser_->GetLexList().erase(i);

	__Dec(i);

	if ((*(i + 0))->lex->id == kScriptLexClass ||
		(*(i + 0))->lex->id == kScriptLexInterface
	) {
		__Dec(i);
	}

	parent->flags |= kScriptFlagTemplate;

	return i;
}

NodeBuilder::Iterator NodeBuilder::_BuildCatch(NodePtr& parent, Iterator i) {

	Iterator oldI = i;

	__Inc(i);

	while (TRUE) {
		if ((*i)->lex->id == kScriptLexParenthesisL) {
			this->_Push(&parent->argList);
			__Inc(i);
			while ((*i)->lex->id != kScriptLexParenthesisR) {
				if (!((*(i + 0))->lex->IsUnknown() &&
					(*(i + 1))->lex->IsUnknown())
				) {
					PostSyntaxError((*i)->line, "Illegal token in method's arguments (%s)", (*i)->word.data());
				}
				NodePtr var = this->_CreateNode(*(i + 1), kScriptNodeVariable);
				var->Type(this->_CreateNode(*i, kScriptNodeDefault));
				__Inc(i);
				__Inc(i);
				this->nodeQueue_->push_back(var);
				if ((*i)->lex->id == kScriptLexComma) {
					__Inc(i);
				}
			}
			if (parent->argList.size() != 1) {
				PostSyntaxError((*i)->line, "Catch must have one parameter", 1);
			}
			this->_Pop();
			break;
		}
		else {
			PostSyntaxError((*i)->line, "Illegal token as catch arguments (%s)", (*i)->word.data());
		}
	}

	oldI = this->parser_->GetLexList().erase(oldI + 2);
	oldI = this->parser_->GetLexList().erase(oldI) - 2;

	return oldI;
}

NodeBuilder::Iterator NodeBuilder::_BuildNew(NodePtr& parent, Iterator i) {

	/*	Why do we need to build operator new?
		Cuz we can allocate memory for object, but
		object may have anonimous classes, that mean
		that we can't catch this condition in
		build function. Anonymous class has next
		view : new {class} {constructor} {body}, so
		first we have to skip 'new', class name and it's
		constructor. */
    
	NodePtr classNode = NULL;
	Iterator classPosition;

	/*	Skip new */
	__Inc(i);

	/*	Create class node */
	classNode = this->_CreateNode(*i, kScriptNodeAnonymous);
	this->nodeQueue_->push_back(classNode);
	classPosition = i;
	__Inc(i);
	if (this->_IsTemplate(i)) {
		i = this->_BuildTemplate(classNode, i - 1) + 1;
	}
	classNode->Type(classNode);
	if ((*i)->lex->id != kScriptLexParenthesisL) {
		PostSyntaxError((*i)->line, "Lost left parenthesis in constructor invoke (%s)", (*i)->word.data());
	}
	i = this->_SkipArguments(i - 1);
	__Inc(i);

	/*	If we have left brace here, then
		we have anonymous class. Anonymous
		class's type will be his own, cuz
		later we will create scope for this
		node, but it won't has name. */

	if ((*i)->lex->id == kScriptLexBraceL) {
		i = this->_BuildClass(classNode, i);
	}
    
    /*  Scope builder requires class not
        to have anonymous identifier. */
    
//    classNode->id = kScriptNodeDefault;

	return i;
}

NodeBuilder::Iterator NodeBuilder::_BuildEnum(NodePtr& parent, Iterator i) {

	NodePtr enumField = NULL;

	parent->flags |= kScriptFlagEnum;

	__Inc(i);

	if ((*i)->lex->id != kScriptLexDefault) {
		PostSyntaxError((*i)->line, "Enum's name mustn't be reserved word or language token (%s)",
			(*i)->word.data());
	}

	Node temporaryTypeNode((*i)->word, kScriptNodeDefault, *i, parent, parent);
	parent->Type(&temporaryTypeNode);

	__Inc(i);

	if ((*i)->lex->id != kScriptLexBraceL) {
		PostSyntaxError((*i)->line, "Lost left brace after enum's name (%s)",
			(*i)->word.data());
	}

	__Dec(i);
	__Dec(i);

	return this->_BuildClass(parent, i);
}

NodeBuilder::Iterator NodeBuilder::_BuildTernary(NodePtr& parent, Iterator i) {

	NodePtr ternaryNode = NULL;

	__Inc(i);

	while ((*i)->lex->id != kScriptLexColon) {
		ternaryNode = this->_CreateNode(*i, kScriptNodeDefault);
		i = this->_Build(ternaryNode, i);
		parent->blockList.push_back(ternaryNode);
		__Inc(i);
	}

	__Inc(i);

	while ((*i)->lex->id != kScriptLexSemicolon) {
		ternaryNode = this->_CreateNode(*i, kScriptNodeDefault);
		i = this->_Build(ternaryNode, i);
		parent->elseList.push_back(ternaryNode);
		__Inc(i);
	}

	if (parent->blockList.empty() || parent->elseList.empty()) {
		PostSyntaxError((*i)->line, "Lost expression in ternary operator", 0);
	}

	return i;
}

Uint32 NodeBuilder::_GetCountOfArguments(Iterator i) {

	/*	Function have the same architecture
		that another builder, but it don't
		store link to parent's node and don't
		move pointer, cuz we use only to
		calculate count of arguments. It
		simplier then compute it in argument
		blocks via command symbols etc. */

	Uint32 argumentsCount = 0;
	Uint32 extraParenseses = 0;

	__Inc(i);

	while (LAME_TRUE) {
		if ((*i)->lex->id == kScriptLexParenthesisL ||
			(*i)->lex->id == kScriptLexBracketL
		) {
			if (extraParenseses > 0) {
				++extraParenseses;
				goto __NextNode;
			}
			else {
				extraParenseses = 1;
			}
		}
		else if (
			(*i)->lex->id == kScriptLexParenthesisR ||
			(*i)->lex->id == kScriptLexBracketR
		) {
			if (extraParenseses == 1) {
				break;
			}
			else {
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

NodeBuilder::Iterator NodeBuilder::_SkipArguments(Iterator i) {

	/*	Function have the same architecture
		that another builder, but it don't
		store link to parent's node but
		move pointer and calculates count of
		arguments. It simplier then compute
		it in argument blocks via command symbols etc. */

	Uint32 extraParenseses = 0;

	__Inc(i);

	while (LAME_TRUE) {
		if ((*i)->lex->id == kScriptLexParenthesisL ||
			(*i)->lex->id == kScriptLexBracketL
		) {
			if (extraParenseses > 0) {
				++extraParenseses;
				goto __NextNode;
			}
			else {
				extraParenseses = 1;
			}
		}
		else if (
			(*i)->lex->id == kScriptLexParenthesisR ||
			(*i)->lex->id == kScriptLexBracketR
		) {
			if (extraParenseses == 1) {
				break;
			}
			else {
				--extraParenseses;
			}
		}
		else {
			this->nodeQueue_->push_back(this->_CreateNode(*i, kScriptNodeDefault));
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

	return i;
}

NodeBuilder::Iterator NodeBuilder::_Build(NodePtr& node, Iterator i) {

	/*	Array Lex : 'int[] array;' */

	if (this->_IsArray(i + 1)) {
		i = this->_BuildArray(node, i);
	}

	/*	New */

	if (this->_IsNew(i)) {
		i = this->_BuildNew(node, i);
	}

	/*	Enum */

	if (this->_IsEnum(i)) {
		i = this->_BuildEnum(node, i);
	}

	/*	Ternary */

	if (this->_IsTernary(i)) {
		i = this->_BuildTernary(node, i);
	}

	/*	Argument List Lex : 'int... argumentList;' */

	if (this->_IsArguments(i + 1)) {
		i = this->_BuildArray(node, i);
	}

	/*	Fix for prefix increment/decrement. */

	if (this->_CheckLex(i + 1, { kScriptLexIncrement }) ||
		this->_CheckLex(i + 1, { kScriptLexDecrement })
	) {
		if (!(*(i + 2))->lex->IsUnknown()) {
			if ((*(i + 1))->lex->id == kScriptLexIncrement) {
				this->incStack_.push_back(*(i));
			}
			else {
				this->decStack_.push_back(*(i));
			}
			i = this->parser_->GetLexList().erase(i + 1) - 1;
		}
	}

	/*	Template Lex : 'Array<Object> objectArray;' */

	if (this->_IsTemplate(i + 1)) {
		i = _BuildTemplate(node, i);
	}

	/*	Fix for cast */

	if (this->_IsCast(i + 1)) {
		__Inc(i);
		i = this->parser_->GetLexList().erase(i);
        LexNodePtr castLex = new LexNode((*i)->word, (*i)->line, Lex::Find(kScriptLexCast));
        i = this->parser_->GetLexList().erase(i);
		i = this->parser_->GetLexList().erase(i);
        i = this->parser_->GetLexList().insert(i, castLex);
		__Dec(i);
	}

	if ((*i)->lex->id == kScriptLexCatch) {
		i = this->_BuildCatch(node, i);
	}

	if (node->lex->lex->IsLanguage()) {

		/*	Class/Interface Lex : 'if (...) {...};' */

		if (node->lex->lex->id == kScriptLexClass ||
			node->lex->lex->id == kScriptLexInterface
		) {
			i = this->_BuildClass(node, i);
		}

		/*	Language Lex : 'if (...) {...};' */

		else if (node->lex->lex->IsCondition()) {
			i = this->_BuildCondition(node, i);
		}

		/*	Building modificators. */

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
			case kScriptLexDecprecated:
				this->modificators_ |= kScriptFlagDeprecated;
				break;
			case kScriptLexOverride:
				this->modificators_ |= kScriptFlagOverride;
				break;
			default:
				break;
			}
		}
	}
	else {

		if (this->_IsMethod(i)) {
			i = this->_BuildFunction(node, i);
		}
		else if (this->_IsVariable(i)) {
			i = this->_BuildVariable(node, i);
		}
		else if (
			this->_IsInvoke(i) ||
			this->_IsAlloc(i)
		) {
			if ((*(i + 1))->lex->id == kScriptLexParenthesisL) {
				node->id = kScriptNodeInvoke;
			}
			else {
				node->id = kScriptNodeAlloc;
			}
			node->lex->args = this->_GetCountOfArguments(i);
		}
	}

	if (i == this->parser_->GetLexList().end()) {
		__Dec(i);
	}

	/*	After creating new node, we will
		check it's power and save as parent.
		Not every node can be parent, so 
		after building some shit we must
		find previous parent. */

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

	/*	Reset node builder, remove all
		nodes and clear all stacks and
		quques. */

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

	/*	Associate lex with it's node ID. */

	if (lex->lex->id == kScriptLexClass ||
		lex->lex->id == kScriptLexEnum
	) {
		id = kScriptNodeClass;
	}
	else if (lex->lex->id == kScriptLexInterface) {
		id = kScriptNodeInterface;
	}
	else if (lex->lex->IsCondition()) {
		id = kScriptNodeCondition;
	}

	/*	Create new node and push it to node list. */

	NodePtr node(new Node(lex->word, id, lex, this->parentNode_, this->prevNode_));
	this->nodeList_.push_back(node);

	/*	Check node for 'power' and save its parent. */

	if (node->lex->lex->IsCondition() ||
		node->id == kScriptNodeFunction ||
		node->id == kScriptNodeClass ||
		node->id == kScriptNodeAnonymous ||
		node->id == kScriptNodeInterface ||
		node->id == kScriptNodeCondition
	) {
		this->parentNode_ = node;
	}

	this->prevNode_ = node;

	return node;
}

NodePtr NodeBuilder::_RemoveNode(NodePtr node) {

	NodePtr parent = node->parent;

	/*	If we're going to remove node, we have
		to find it in parent block and remove from
		it, after we must remove all his children.
		We wont release it's memory, node still in
		node list. */

	if (node->parent) {

		Deque<NodePtr>::iterator i = node->parent->blockList.begin();

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

Void NodeBuilder::_Push(Deque<NodePtr>* stack) {

	/*	Push current node's stack to builder queue.
		It helps to generate tree like structure and
		we can easylly seek for errors or mistakes in code. */

	if (this->nodeQueue_) {
		this->stackNodeQueue_.push_back(this->nodeQueue_);
	}
	this->nodeQueue_ = stack;
}

Deque<NodePtr>* NodeBuilder::_Pop(Void) {

	this->_ApplySemicolon();

	/*	Pop current parent's stack from queue, that
		means that we've finished building some
		node's component, for example arguments or block. */

	if (!this->stackNodeQueue_.size()) {
		this->nodeQueue_ = LAME_NULL;
	}
	else {
		this->nodeQueue_ = this->stackNodeQueue_.back();
		this->stackNodeQueue_.pop_back();
	}
	return this->nodeQueue_;
}

Bool NodeBuilder::_CheckLex(Iterator i, Vector<LexID> lexList) {

	Uint32 okCount = 0;

	if (this->parser_->GetLexList().end() - i < (Sint32)lexList.size()) {
		return FALSE;
	}

	for (LexID lexID : lexList) {
		if (lexID != (*i++)->lex->id) {
			if (okCount >= 3 && FALSE) {
				PostSyntaxError((*i)->line, "Not finished expression at (%s) position", (*i)->word.data());
			}
			return FALSE;
		}
		else {
			++okCount;
		}
		if (this->parser_->GetLexList().end() == i) {
			PostSyntaxError((*(i - 1))->line, "Not finished expression at (%s) position", (*(i - 1))->word.data());
		}
	}

	return TRUE;
}

Void NodeBuilder::_DeclareSequence(LexSequenceID id, Vector<LexID> lexList) {

	if (sequenceList_.empty()) {
		sequenceList_.resize(kScriptLexSequenceAmount);
	}

	sequenceList_[id] = lexList;
}

Bool NodeBuilder::_CheckSequence(Iterator i, LexSequenceID id) {

	if (this->sequenceList_.empty() || this->sequenceList_[id].empty()) {
		throw Exception("Unable to check iterator with empty sequence", 0);
	}

	return this->_CheckLex(i, this->sequenceList_[id]);
}

Void NodeBuilder::_ApplySemicolon(Void) {

	static LexNode incLex("++", 0, Lex::Find(kScriptLexIncrement));
	static LexNode decLex("--", 0, Lex::Find(kScriptLexDecrement));

	for (LexNodePtr lexNode : this->incStack_) {
		this->nodeQueue_->push_back(this->_CreateNode(&incLex, kScriptNodeDefault));
		this->nodeQueue_->push_back(this->_CreateNode(lexNode, kScriptNodeDefault));
	}

	for (LexNodePtr lexNode : this->decStack_) {
		this->nodeQueue_->push_back(this->_CreateNode(&decLex, kScriptNodeDefault));
		this->nodeQueue_->push_back(this->_CreateNode(lexNode, kScriptNodeDefault));
	}

	this->incStack_.clear();
	this->decStack_.clear();
}

NodeBuilder::NodeBuilder() {

	this->_Reset();

	this->_DeclareSequence(kScriptLexSequenceDefault, {
		kScriptLexDefault
	});

	this->_DeclareSequence(kScriptLexSequenceArguments, {
		kScriptLexArgumentList
	});

	this->_DeclareSequence(kScriptLexSequenceArray, {
		kScriptLexArray
	});

	this->_DeclareSequence(kScriptLexSequenceTemplate, {
		kScriptLexBellow,
		kScriptLexDefault,
		kScriptLexAbove
	});

	this->_DeclareSequence(kScriptLexSequenceCast, {
		kScriptLexParenthesisL,
		kScriptLexDefault,
		kScriptLexParenthesisR
	});

	this->_DeclareSequence(kScriptLexSequenceMethod, {
		kScriptLexDefault,
		kScriptLexDefault,
		kScriptLexParenthesisL
	});

	this->_DeclareSequence(kScriptLexSequenceVariable, {
		kScriptLexDefault,
		kScriptLexDefault
	});

	this->_DeclareSequence(kScriptLexSequenceInvoke, {
		kScriptLexDefault,
		kScriptLexParenthesisL
	});

	this->_DeclareSequence(kScriptLexSequenceAlloc, {
		kScriptLexDefault,
		kScriptLexBracketL
	});

	this->_DeclareSequence(kScriptLexSequenceNew, {
		kScriptLexNew,
		kScriptLexDefault
	});

	this->_DeclareSequence(kScriptLexSequenceEnum, {
		kScriptLexEnum
	});

	this->_DeclareSequence(kScriptLexSequenceTernary, {
		kScriptLexTernary
	});
}

NodeBuilder::~NodeBuilder() {
	this->_Reset();
}

LAME_END2
