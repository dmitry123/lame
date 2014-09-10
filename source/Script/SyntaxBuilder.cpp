#include "SyntaxBuilder.h"

/*	Safe increment for parser's list iterator, it will
	check for end of list and throws an exception if we can't
	finish our lex building. */

#define __Inc(_i) \
	if (_i == this->fileParser->GetLexList().end() || ++_i == this->fileParser->GetLexList().end()) { \
		_asm {int 3} PostSyntaxError((*(_i-1))->line, "Not finished expression at (%s)", (*(_i-1))->word.data()); \
	}
#define __Dec(_i) \
	--_i

LAME_BEGIN2(Script)

SyntaxBuilder::SyntaxBuilder(Void) {

	this->sequenceMatcher.Declare(kScriptLexSequenceArguments, {
		kScriptLexUnknown, kScriptLexArgumentList
	});
	this->sequenceMatcher.Declare(kScriptLexSequenceArray, {
		kScriptLexUnknown, kScriptLexArray
	});
	this->sequenceMatcher.Declare(kScriptLexSequenceTemplate, {
		kScriptLexBelow, kScriptLexUnknown, kScriptLexAbove
	});
	this->sequenceMatcher.Declare(kScriptLexSequenceCast, {
		kScriptLexParenthesisL, kScriptLexUnknown, kScriptLexParenthesisR
	});
	this->sequenceMatcher.Declare(kScriptLexSequenceMethod, {
		kScriptLexUnknown, kScriptLexUnknown, kScriptLexParenthesisL
	});
	this->sequenceMatcher.Declare(kScriptLexSequenceVariable, {
		kScriptLexUnknown, kScriptLexUnknown
	});
	this->sequenceMatcher.Declare(kScriptLexSequenceInvoke, {
		kScriptLexUnknown, kScriptLexParenthesisL
	});
	this->sequenceMatcher.Declare(kScriptLexSequenceAlloc, {
		kScriptLexUnknown, kScriptLexBracketL
	});
	this->sequenceMatcher.Declare(kScriptLexSequenceNew, {
		kScriptLexNew, kScriptLexUnknown
	});
	this->sequenceMatcher.Declare(kScriptLexSequenceStatic, {
		kScriptLexStatic, kScriptLexBraceL
	});
	this->sequenceMatcher.Declare(kScriptLexSequenceSuper, {
		kScriptLexBelow, kScriptLexTernary
	});
}

SyntaxBuilder::~SyntaxBuilder(Void) {
}

Void SyntaxBuilder::Build(FileParserPtr fileParser, PackagePtr packageManager) {

	this->fileParser = fileParser;
	this->packageManager = packageManager;

	/*	We have to create default root node with
		main initialize code. It will be abstract without
		any braces but will have own scope. Programmer will
		have to write code in root node's block likes JavaScript
		and it will be succesfully translated into LameByteCode. */

	LexNodePtr lexNode = new LexNode("#", 0, Lex::Find(kScriptLexUnknown));

	/*	Reset current node builder. It will release all allocated
		memory for nodes and clear node tree. */

	this->_Reset();

	/*	Initializing script parser and insert root lex node
		into the top of the list. */

	this->fileParser = fileParser;
	this->_List().push_front(lexNode);

	/*	Creating root node, which will be the entry point
		of our script code, but clang can't take reference
		to temporary variable */

    auto __it = this->_End() - 1;
    
    this->rootNode = this->_Create(__it, kScriptNodeEntry);

	/*	Find first object in parser's list with lexes and
		get it's iterator, _BuildEntry function will build
		entry node and invoke recursive _Build method, which
		will identity all script lexes and generate all nodes. */

	Iterator first = fileParser->GetLexList().begin() + 1;

	if (first != this->_End()) {
		 first = this->Entry(this->rootNode, first);
	}

	this->_Order(this->rootNode);
}

SyntaxBuilder::Iterator SyntaxBuilder::If(NodePtr& node, Iterator i) {

	if (node->lex == *i) {
		__Inc(i);
	}

	if ((*i)->lex->id != kScriptLexParenthesisL) {
		PostSyntaxError((*i)->line, "Lost left parenthesis in (%s) construction",
			node->word.data());
	}

	i = this->Arguments(node, i);

	__Inc(i);

	if ((*i)->lex->id != kScriptLexBraceL) {
		while ((*i)->lex->id != kScriptLexSemicolon) {
			i = this->_Append(&node->blockList, i);
			if (node->blockList.back()->id != kScriptNodeDefault) {
				break;
			}
			if ((*i)->lex->id != kScriptLexSemicolon) {
				__Inc(i);
			}
		}
	}
	else {
		__Inc(i);
		while ((*i)->lex->id != kScriptLexBraceR) {
			i = this->_Append(&node->blockList, i);
			if (_WasItBrace(node->blockList.back())) {
				__Inc(i);
			}
			else {
				if ((*i)->lex->id != kScriptLexBraceR) {
					__Inc(i);
				}
			}
		}
	}

	if (i + 1 != this->_End()) {
		__Inc(i);
		if ((*i)->lex->id == kScriptLexElse) {
			node->elseNode = this->_Create(i);
			i = this->Else(node->elseNode, i);
			this->parentNode = this->parentNode->parent;
		}
		else {
			__Dec(i);
		}
	}

	return i;
}

SyntaxBuilder::Iterator SyntaxBuilder::Else(NodePtr& node, Iterator i) {

	if (node->lex == *i) {
		__Inc(i);
	}

	if ((*i)->lex->id == kScriptLexParenthesisL) {
		PostSyntaxError((*i)->line, "Unexcepted left parenthesis in (%s) construction",
			node->word.data());
	}

	if ((*i)->lex->id != kScriptLexBraceL) {
		while ((*i)->lex->id != kScriptLexSemicolon) {
			i = this->_Append(&node->blockList, i);
			if (node->blockList.back()->id != kScriptNodeDefault) {
				break;
			}
			if ((*i)->lex->id != kScriptLexSemicolon) {
				__Inc(i);
			}
		}
	}
	else {
		__Inc(i);
		while ((*i)->lex->id != kScriptLexBraceR) {
			i = this->_Append(&node->blockList, i);
			if (_WasItBrace(node->blockList.back())) {
				__Inc(i);
			}
			else {
				if ((*i)->lex->id != kScriptLexBraceR) {
					__Inc(i);
				}
			}
		}
	}

	return i;
}

SyntaxBuilder::Iterator SyntaxBuilder::While(NodePtr& node, Iterator i) {
	return this->If(node, i);
}

SyntaxBuilder::Iterator SyntaxBuilder::Do(NodePtr& node, Iterator i) {

	i = this->Else(node, i);

	__Inc(i);

	if ((*i)->lex->id != kScriptLexWhile) {
		PostSyntaxError((*i)->line, "Lost (while) block after (do) construction before (%s)", (*i)->word.data());
	}
	else {
		__Dec(i);
	}

	return i;
}

SyntaxBuilder::Iterator SyntaxBuilder::For(NodePtr& node, Iterator i) {

	Bool isForEach = FALSE;

	if (node->lex == *i) {
		__Inc(i);
	}

	if ((*i)->lex->id != kScriptLexParenthesisL) {
		PostSyntaxError((*i)->line, "Lost left parenthesis in (%s) construction",
			node->word.data());
	}

	__Inc(i);

	Iterator k;
	Uint32 j = 0;

	while ((*i)->lex->id != kScriptLexSemicolon) {
		if ((*i)->lex->id == kScriptLexColon) {
			isForEach = TRUE; ++j;
		}
		if (!(*i)->lex->IsExpression()) {
			PostSyntaxError((*i)->line, "Illegal token in expression (%s)",
				(*i)->word.data());
		}
		i = this->_Append(&node->forInfo.beginList, i);
		if ((*i)->lex->id != kScriptLexSemicolon) {
			__Inc(i);
		}
	}

	__Inc(i);

	while ((*i)->lex->id != kScriptLexSemicolon) {
		if (!(*i)->lex->IsExpression()) {
			PostSyntaxError((*i)->line, "Illegal token in expression (%s)",
				(*i)->word.data());
		}
		i = this->_Append(&node->forInfo.conditionList, i);
		if ((*i)->lex->id != kScriptLexSemicolon) {
			__Inc(i);
		}
	}

	__Inc(i);

	while ((*i)->lex->id != kScriptLexParenthesisR) {
		if (!(*i)->lex->IsExpression()) {
			PostSyntaxError((*i)->line, "Illegal token in expression (%s)",
				(*i)->word.data());
		}
		i = this->_Append(&node->forInfo.nextList, i);
		if ((*i)->lex->id != kScriptLexSemicolon) {
			__Inc(i);
		}
	}

	__Inc(i);

#if 0
	for (NodePtr n : node->forInfo.beginList) {
		node->argList.push_back(n);
	}
	for (NodePtr n : node->forInfo.conditionList) {
		node->argList.push_back(n);
	}
	for (NodePtr n : node->forInfo.nextList) {
		node->argList.push_back(n);
	}
#endif

	if (isForEach) {
		if (node->forInfo.beginList.size() != 1 || 
			node->forInfo.nextList.size() != 1
		) {
			PostSyntaxError(node->lex->line, "Invalid parameters in 'foreach' construction", 0);
		}
		node->flags |= kScriptFlagForEach;
	}

	if ((*i)->lex->id != kScriptLexBraceL) {
		while ((*i)->lex->id != kScriptLexSemicolon) {
			i = this->_Append(&node->blockList, i);
			if (node->blockList.back()->id != kScriptNodeDefault) {
				break;
			}
			if ((*i)->lex->id != kScriptLexSemicolon) {
				__Inc(i);
			}
		}
	}
	else {
		__Inc(i);
		while ((*i)->lex->id != kScriptLexBraceR) {
			i = this->_Append(&node->blockList, i);
			if (_WasItBrace(node->blockList.back())) {
				__Inc(i);
			}
			else {
				if ((*i)->lex->id != kScriptLexBraceR) {
					__Inc(i);
				}
			}
		}
	}

	return i;
}

SyntaxBuilder::Iterator SyntaxBuilder::Try(NodePtr& node, Iterator i) {

	if (node->lex == *i) {
		__Inc(i);
	}

	if ((*i)->lex->id == kScriptLexParenthesisL) {
		PostSyntaxError((*i)->line, "Unexcepted left parenthesis in (%s) construction",
			node->word.data());
	}

	if ((*i)->lex->id != kScriptLexBraceL) {
		PostSyntaxError((*i)->line, "Lost left brace after (try) keyword (%s)", (*i)->word.data());
	}

	__Inc(i);
	while ((*i)->lex->id != kScriptLexBraceR) {
		i = this->_Append(&node->blockList, i);
		if (_WasItBrace(node->blockList.back())) {
			__Inc(i);
		}
		else {
			if ((*i)->lex->id != kScriptLexBraceR) {
				__Inc(i);
			}
		}
	}
	__Inc(i);

	if ((*i)->lex->id != kScriptLexCatch && (*i)->lex->id != kScriptLexFinally) {
		PostSyntaxError((*i)->line, "Lost (catch/finally) block after (try) construction before (%s)", (*i)->word.data());
	}
	else {
		__Dec(i);
	}

	return i;
}

SyntaxBuilder::Iterator SyntaxBuilder::Catch(NodePtr& node, Iterator i) {

	if (node->lex == *i) {
		__Inc(i);
	}

	if ((*i)->lex->id != kScriptLexParenthesisL) {
		PostSyntaxError((*i)->line, "Lost left parenthesis in (%s) construction",
			node->word.data());
	}

	__Inc(i);

	while ((*i)->lex->id != kScriptLexParenthesisR) {
		if (!(*i)->lex->IsUnknown()) {
			PostSyntaxError((*i)->line, "Illegal token in catch's expression (%s)",
				(*i)->word.data());
		}
		NodePtr typeNode = this->_Create(i);
		i = this->Directed(typeNode, i);
		__Inc(i);
		NodePtr varNode = this->_Create(i, kScriptNodeVariable);
		varNode->typeNode = typeNode;
		node->argList.push_back(varNode);
		if ((*i)->lex->id != kScriptLexParenthesisR) {
			__Inc(i);
		}
	}

	__Inc(i);

	if ((*i)->lex->id != kScriptLexBraceL) {
		PostSyntaxError((*i)->line, "Lost left brace after (catch) keyword (%s)", (*i)->word.data());
	}

	__Inc(i);
	while ((*i)->lex->id != kScriptLexBraceR) {
		i = this->_Append(&node->blockList, i);
		if (_WasItBrace(node->blockList.back())) {
			__Inc(i);
		}
		else {
			if ((*i)->lex->id != kScriptLexBraceR) {
				__Inc(i);
			}
		}
	}

	return i;
}

SyntaxBuilder::Iterator SyntaxBuilder::Finally(NodePtr& node, Iterator i) {

	if (node->lex == *i) {
		__Inc(i);
	}

	if ((*i)->lex->id == kScriptLexParenthesisL) {
		PostSyntaxError((*i)->line, "Unexcepted left parenthesis in (%s) construction",
			node->word.data());
	}

	if ((*i)->lex->id != kScriptLexBraceL) {
		PostSyntaxError((*i)->line, "Lost left brace after (finally) keyword (%s)", (*i)->word.data());
	}

	__Inc(i);
	while ((*i)->lex->id != kScriptLexBraceR) {
		i = this->_Append(&node->blockList, i);
		if (_WasItBrace(node->blockList.back())) {
			__Inc(i);
		}
		else {
			if ((*i)->lex->id != kScriptLexBraceR) {
				__Inc(i);
			}
		}
	}

	return i;
}

SyntaxBuilder::Iterator SyntaxBuilder::Variable(NodePtr& node, Iterator i) {

	/*	This lex is made for variables, but only variable
		declaration, likes in methods or classes, for example :
		'int value = 100;' or 'class A { public int value = 100; }' */

	NodePtr typeNode = 0;
	NodePtr varNode = 0;

	__Inc(i);

	/*	Our method lex has next syntax : default default; etc. It mean that parent
		node will be 'default' - its a type (void, int, float etc), its wrong
		and we must swap with variable's name (first and second node). First we have to create
		main variables's node instead of parent (variable type). */

	node->id = kScriptNodeVariable;
	varNode = this->_Create(i, kScriptNodeVariable);
	varNode->flags = node->flags;
	typeNode = node;
	varNode->Type(typeNode);
	node = varNode;
	node->flags = this->modificatorFlags;
	i = this->Directed(typeNode, i);

	if (this->_IsArray(i)) {
		i = this->Array(typeNode, i);
	}

	/*	We can create declare variable with limited modificators
		if it separates not in class scope (Only final modificator).
		If our parent is class, then allow all modificators. */

	if (node->parent->id == kScriptNodeClass) {
		_AllowModificators(node, -1 & ~kScriptFlagOverride & ~kScriptFlagSynchronized);
	}
	else {
		_AllowModificators(node, kScriptFlagFinal);
	}

#if 0
	/*	Yep, if our parent is class then before building
		variable's arguments we must save it's node in parent's
		list with blocks. Why? Because our expresssion has next
		type : default default = ...; But after build we will
		lost variable's node and we have back it to our expression. */

	if (node->parent->id == kScriptNodeClass) {
		node->blockList.push_back(this->_Create(node->lex, parent->id));
	}
#endif

	if (node->lex == *i) {
		__Inc(i);
	}

	if ((*i)->lex->id != kScriptLexSemicolon &&
		(*i)->lex->id != kScriptLexSet &&
		(*i)->lex->id != kScriptLexComma
	) {
		PostSyntaxError((*i)->line, "Illegal token after variable declaration (%s)", (*i)->word.data());
	}

	varNode = node;

	__Inc(i);

	if ((*i)->lex->id == kScriptLexBraceL) {
		if (!(typeNode->flags & kScriptFlagArray)) {
			PostSyntaxError((*i)->line, "Can't apply intialize-list to non array type",
				typeNode->word.data());
		}
		i = this->Initialize(node, i);
		__Inc(i);
		if ((*i)->lex->id != kScriptLexSemicolon) {
			PostSyntaxError((*i)->line, "Lost semicolon", 0)
		}
	}
	else {
		__Dec(i);
		while ((*i)->lex->id != kScriptLexSemicolon) {
			if ((*i)->lex->id == kScriptLexComma) {
				__Inc(i);
				while (varNode->next) {
					varNode = varNode->next;
				}
				varNode->next = this->_Create(i);
				varNode = varNode->next;
				varNode->typeNode = node->typeNode;
				varNode->id = node->id;
			}
			else {
				if (!(*i)->lex->IsExpression()) {
					PostSyntaxError((*i)->line, "Illegal token in expression (%s)",
						(*i)->word.data());
				}
			}
			i = this->_Append(&varNode->argList, i);
			if ((*i)->lex->id != kScriptLexSemicolon) {
				__Inc(i);
			}
		}
	}

	i = this->_Append(&node->argList, i);

	return i;
}

SyntaxBuilder::Iterator SyntaxBuilder::Method(NodePtr& node, Iterator i) {

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

	functionNode = this->_Create(i, kScriptNodeFunction);
	typeNode = node;
	functionNode->Type(typeNode);
	node = functionNode;
	this->parentNode = node;
	this->modificatorFlags = 0;
	i = this->Directed(typeNode, i);

	if (node->lex == *i) {
		__Inc(i);
	}

	if ((*i)->lex->id != kScriptLexParenthesisL) {
		PostSyntaxError((*i)->line, "Lost method's left parenthesis (%s)",
			(*i)->word.data());
	}

	__Inc(i);

	while ((*i)->lex->id != kScriptLexParenthesisR) {
		if ((*i)->lex->id != kScriptLexUnknown) {
			PostSyntaxError((*i)->line, "Illegal token in expression (%s)", (*i)->word.data());
		}
		NodePtr typeNode = this->_Create(i);
		i = this->Type(typeNode, i);
		__Inc(i);
		if ((*i)->lex->id != kScriptLexUnknown) {
			PostSyntaxError((*i)->line, "Illegal token in expression (%s)", (*i)->word.data());
		}
		NodePtr variableNode = this->_Create(i, kScriptNodeVariable);
		variableNode->Type(typeNode);
		node->argList.push_back(variableNode);
		if (this->_IsArray(i)) {
			i = this->Array(typeNode, i);
		}
		__Inc(i);
		if ((*i)->lex->id != kScriptLexComma &&
			(*i)->lex->id != kScriptLexParenthesisR
		) {
			PostSyntaxError((*i)->line, "Illegal token in expression (%s)", (*i)->word.data());
		}
		if ((*i)->lex->id != kScriptLexParenthesisR) {
			__Inc(i);
		}
	}
	__Inc(i);

	if ((*i)->lex->id == kScriptLexThrows) {
		__Inc(i);
		while ((*i)->lex->id != kScriptLexBraceL && (*i)->lex->id != kScriptLexSemicolon) {
			if ((*i)->lex->id != kScriptLexUnknown) {
				PostSyntaxError((*i)->line, "Illegal token in expression (%s)", (*i)->word.data());
			}
			i = this->_Append(&node->methodInfo.throwsList, i);
			if ((*i)->lex->id != kScriptLexBraceL && (*i)->lex->id != kScriptLexSemicolon) {
				__Inc(i);
			}
		}
		if (node->methodInfo.throwsList.empty()) {
			PostSyntaxError((*i)->line, "Method can't throws nothing", 0);
		}
	}

	if ((*i)->lex->id != kScriptLexSemicolon) {
		node->flags |= kScriptFlagImplemented;
	}
	else {
		return i;
	}

	if ((*i)->lex->id != kScriptLexBraceL) {
		PostSyntaxError((*i)->line, "Lost left brace in method implementation (%s)", (*i)->word.data());
	}

	__Inc(i);

	while ((*i)->lex->id != kScriptLexBraceR) {
		i = this->_Append(&node->blockList, i);
		if (_WasItBrace(node->blockList.back())) {
			__Inc(i);
		}
		else {
			if ((*i)->lex->id != kScriptLexBraceR) {
				__Inc(i);
			}
		}
	}

	return i;
}

SyntaxBuilder::Iterator SyntaxBuilder::Class(NodePtr& node, Iterator i) {

	Uint32 modificators = 0;
	Uint8 modificatorCount = 0;
	NodePtr field = NULL;

	/*	The easiest lex builder is class builder, cuz it has
	only it's name and body, all variables or methods will
	be build in recursive _Build function. But appended to
	class object. */

	if (node->id != kScriptNodeAnonymous) {
		__Inc(i);
	}

	/*	If class hasn't name we will throw an exception */

	if (!(*i)->lex->IsUnknown() && node->id != kScriptNodeAnonymous) {
		PostSyntaxError((*i)->line, "Unable to create class without name", 1);
	}

	/* Generating node with class name */

	if (node->id != kScriptNodeAnonymous) {
		node->typeNode = this->_Create(i, kScriptNodeDefault);
	}

	/* Allow all modificators for class */

	node->flags = this->modificatorFlags;
	_AllowModificators(node, -1);
	this->modificatorFlags = 0;

	if (node->id != kScriptNodeAnonymous) {
		__Inc(i);
	}

	if (this->_IsTemplate(i)) {
		i = this->Template(node, i);
	}

	if (node->lex == *i) {
		__Inc(i);
	}

	if ((*i)->lex->id == kScriptLexExtends) {
		__Inc(i);
		while ((*i)->lex->id != kScriptLexBraceL && (*i)->lex->id != kScriptLexImplements) {
			if (node->classInfo.extendNode) {
				if ((*i)->lex->id != kScriptLexDirected) {
					PostSyntaxError((*i)->line, "Unexcepted token in expression (%s)", (*i)->word.data());
				}
				NodePtr nextNode = node->classInfo.extendNode;
				while (nextNode->next) {
					nextNode = nextNode->next;
				}
				__Inc(i);
				nextNode->next = this->_Create(i);
				i = this->Directed(nextNode->next, i);
			}
			else {
				node->classInfo.extendNode = this->_Create(i);
				i = this->Directed(node->classInfo.extendNode, i);
			}
			__Inc(i);
		}
		if (!node->classInfo.extendNode) {
			PostSyntaxError((*i)->line, "Class can't extends nothing, found (%s)", (*i)->word.data());
		}
	}

	if ((*i)->lex->id == kScriptLexImplements) {
		__Inc(i);
		while ((*i)->lex->id != kScriptLexBraceL) {
			if (!node->classInfo.implementNode.empty()) {
				if ((*i)->lex->id != kScriptLexDirected) {
					PostSyntaxError((*i)->line, "Illegal token after (implements) keyword (%s)", (*i)->word.data());
				}
			}
			NodePtr implementNode = this->_Create(i);
			node->classInfo.implementNode.push_back(implementNode);
			i = this->Directed(implementNode, i);
			__Inc(i);
			while ((*i)->lex->id != kScriptLexBraceL) {
				if ((*i)->lex->id != kScriptLexComma) {
					PostSyntaxError((*i)->line, "Unexcepted token in expression (%s)", (*i)->word.data());
				}
				__Inc(i);
				NodePtr implementNode = this->_Create(i);
				node->classInfo.implementNode.push_back(implementNode);
				i = this->Directed(implementNode, i);
				__Inc(i);
			}
			if ((*i)->lex->id != kScriptLexBraceL) {
				PostSyntaxError((*i)->line, "Illegal token after (implements) keyword (%s)", (*i)->word.data());
			}
		}
		if (node->classInfo.implementNode.empty()) {
			PostSyntaxError((*i)->line, "Class can't implements nothing, found (%s)", (*i)->word.data());
		}
	}

	if ((*i)->lex->id != kScriptLexBraceL) {
		PostSyntaxError((*i)->line, "Lost left brace in class declaration (%s)", (*i)->word.data());
	}
	__Inc(i);

	while ((*i)->lex->id != kScriptLexBraceR) {
		if ((*i)->lex->IsModificator()) {
			__Inc(i);
			__Dec(i);
			if ((*i)->lex->id == kScriptLexStatic && ((*(i + 1)))->lex->id == kScriptLexBraceL) {
				goto __SaveNode;
			}
			switch ((*i)->lex->id) {
			case kScriptLexPublic:       modificators |= kScriptFlagPublic;    ++modificatorCount; break;
			case kScriptLexPrivate:      modificators |= kScriptFlagPrivate;   ++modificatorCount; break;
			case kScriptLexProtected:    modificators |= kScriptFlagProtected; ++modificatorCount; break;
			case kScriptLexStatic:       modificators |= kScriptFlagStatic;                        break;
			case kScriptLexNative:       modificators |= kScriptFlagNative;                        break;
			case kScriptLexFinal:        modificators |= kScriptFlagFinal;                         break;
			case kScriptLexAbstract:     modificators |= kScriptFlagAbstract;                      break;
			case kScriptLexDecprecated:  modificators |= kScriptFlagDeprecated;                    break;
			case kScriptLexOverride:     modificators |= kScriptFlagOverride;                      break;
			case kScriptLexSynchronized: modificators |= kScriptFlagSynchronized;                  break;
			default:
				break;
			}
		}
		else {
			__SaveNode:
			if ((*i)->word == node->typeNode->word) {
				__Inc(i);
				if ((*i)->lex->id == kScriptLexParenthesisL) {
					__Dec(i);
					i = this->fileParser->GetLexList().insert(i,
						new LexNode("void", (*i)->line, Lex::Find(kScriptLexUnknown)));
					__Inc(i);
				}
				__Dec(i);
			}
			field = this->_Append(i);
			if (field->id != kScriptNodeVariable &&
				field->id != kScriptNodeFunction &&
				field->id != kScriptNodeClass &&
				field->id != kScriptNodeInterface &&
				field->id != kScriptNodeStatic
			) {
				PostSyntaxError((*i)->line, "Illegal token in class (%s)", field->word.data());
			}
			if (modificatorCount > 1) {
				PostSyntaxError((*i)->line, "You can't use public/private/protected modificators together", 0);
			}
			field->flags |= modificators;
			modificators = 0;
			modificatorCount = 0;
			node->blockList.push_back(field);
			if (_WasItBrace(field)) {
				__Inc(i); continue;
			}
		}
		if ((*i)->lex->id != kScriptLexBraceR) {
			__Inc(i);
		}
	}

	return i;
}

SyntaxBuilder::Iterator SyntaxBuilder::New(NodePtr& node, Iterator i) {

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
	classNode = this->_Create(i, kScriptNodeDefault);
	node->typeNode = classNode;
	i = this->Directed(classNode, i);
	__Inc(i);
	if (this->sequenceMatcher.Match(kScriptLexSequenceTemplate, i, this->_End())) {
		i = this->Template(classNode, i - 1) + 1;
	}
	classNode->typeNode = classNode;
	if ((*i)->lex->id != kScriptLexParenthesisL && 
		(*i)->lex->id != kScriptLexBracketL &&
		(*i)->lex->id != kScriptLexArray
	) {
		PostSyntaxError((*i)->line, "Lost left parenthesis or left bracket (%s)", (*i)->word.data());
	}
	if ((*i)->lex->id == kScriptLexParenthesisL) {
		node->flags |= kScriptFlagInvocation;
	}
	i = this->Arguments(node, i);
	node->typeNode->lex->args = node->lex->args;
	__Inc(i);

	/*	If we have left brace here, then
		we have anonymous class. Anonymous
		class's type will be his own, cuz
		later we will create scope for this
		node, but it won't has name. */

	if ((*i)->lex->id == kScriptLexBraceL) {
		classNode->classInfo.extendNode = classNode;
		i = this->Class(classNode, i);
		classNode->id = kScriptNodeAnonymous;
	}
	else if ((*i)->lex->id == kScriptLexArray) {
		node->lex->args = 0;
		node->typeNode->lex->args = 0;
		__Inc(i);
		if ((*i)->lex->id != kScriptLexBraceL) {
			PostSyntaxError((*i)->line, "Excepted array initializer before (%s)", (*i)->word.data());
		}
		__Inc(i);
		while ((*i)->lex->id != kScriptLexBraceR) {
			NodePtr n = this->_Append(i);
			if (n->id != kScriptNodeDefault) {
				PostSyntaxError((*i)->line, "Array initializer must be expression (%s)", n->word.data());
			}
			if (!this->_WasItBrace(n)) {
				__Inc(i);
			}
			node->blockList.push_back(n);
		}
	}
	else if ((*i)->lex->id == kScriptLexBracketL) {
		__Inc(i);
		while ((*i)->lex->id != kScriptLexBracketR) {
			NodePtr n = this->_Append(i);
			if ((*i)->lex->id != kScriptLexBracketR || n->id == kScriptNodeAlloc) {
				__Inc(i);
			}
			node->argList.push_back(n);
		}
	}
	else {
		classNode->id = kScriptNodeDefault;
		LAME_TODO("this->parentNode = this->parentNode->parent");
	}

	if ((*i)->lex->id != kScriptLexSemicolon) {
		__Inc(i);
	}

	if ((*i)->lex->id != kScriptLexSemicolon) {
		PostSyntaxError((*i)->line, "Lost semicolon before (%s)", (*i)->word.data());
	}

	return i;
}

SyntaxBuilder::Iterator SyntaxBuilder::Arguments(NodePtr& node, Iterator i) {

	Uint32 argCount = 0;
	Uint32 extraParentheses = 0;
	Bool emptyParentheses = 0;

	if (node->lex == *i) {
		__Inc(i);
	}
	if ((*i)->lex->id != kScriptLexParenthesisL) {
		return i - 1;
	}
	__Inc(i);

	this->wasLastParenthesis = TRUE;

	while ((*i)->lex->id != kScriptLexParenthesisR || extraParentheses) {
		i = this->_Append(&node->argList, i);
		if (node->argList.back()->id != kScriptNodeInvoke) {
			if ((*i)->lex->id == kScriptLexParenthesisL) {
				++extraParentheses;
			}
			else if ((*i)->lex->id == kScriptLexParenthesisR) {
				if (extraParentheses) {
					--extraParentheses;
				}
				emptyParentheses = TRUE;
			}
			if ((*i)->lex->id != kScriptLexParenthesisR || emptyParentheses) {
				__Inc(i);
			}
			else if (this->wasLastParenthesis) {
				wasLastParenthesis = FALSE;
				__Inc(i);
			}
		}
		else {
			__Inc(i);
		}
		if ((*i)->lex->id != kScriptLexParenthesisR && !extraParentheses) {
			if ((*i)->lex->id != kScriptLexComma) {
#if 0 /* Not needed */
				PostSyntaxError((*i)->line, "Lost comma after argument, before (%s)", (*i)->word.data());
#endif
			} else {
				++argCount;
				__Inc(i);
			}
		}
		emptyParentheses = FALSE;
	}

	if (!node->argList.empty()) {
		++argCount;
	}

	node->lex->args = argCount;

	return i;
}

SyntaxBuilder::Iterator SyntaxBuilder::Template(NodePtr& node, Iterator i) {

	NodePtr typeNode = node->typeNode
		? node->typeNode : node;

	if ((*i)->lex->id != kScriptLexBelow) {
		return i;
	}

	i = this->fileParser->GetLexList().erase(i);

	typeNode->templateNode = this->_Create(i, kScriptNodeDefault);

	if ((*i)->lex->id == kScriptLexTernary) {
		__Inc(i);
		if ((*i)->lex->id == kScriptLexExtends) {
			__Inc(i);
			if ((*i)->lex->id != kScriptLexUnknown) {
				PostSyntaxError((*i)->line, "Excepted identifier (%s)",
					(*i)->word.data());
			}
			typeNode->templateNode->classInfo.extendNode =
				this->_Create(i);
		}
		else if ((*i)->word == "super") {
			__Inc(i);
			if ((*i)->lex->id != kScriptLexUnknown) {
				PostSyntaxError((*i)->line, "Excepted identifier (%s)",
					(*i)->word.data());
			}
			typeNode->templateNode->classInfo.superNode =
				this->_Create(i);
		}
		else {
			PostSyntaxError((*i)->line, "Unexcepted token in super template expression (%s)",
				(*i)->word.data());
		}
	}

	__Inc(i);
	i = this->fileParser->GetLexList().erase(i);

	node->flags |= kScriptFlagTemplate;

	return i;
}

SyntaxBuilder::Iterator SyntaxBuilder::Entry(NodePtr& node, Iterator i) {

	/*	Build only for entry, where we can store
		anything, cuz its root. So we can simply
		save it's block list and build it recursively. */

	while (LAME_TRUE) {
		i = this->_Append(&node->blockList, i);
		if (++i == this->_End()) {
			break;
		}
	}

	return i;
}

SyntaxBuilder::Iterator SyntaxBuilder::Array(NodePtr& node, Iterator i) {

	/*	We can met something likes array in our code,
		for example : 'int[] array;'. We can't analize []
		token (its different with '[' or ']' token), but
		we can remove that node and change parent's node flags
		to show our code analizer or translator that its an array. */

	if (!node->lex->lex->IsUnknown()) {
		PostSyntaxError(node->lex->line, "Illegal token before brackets (%s)", node->lex->word.data());
	}
	node->flags |= kScriptFlagArray;
	i = this->_List().erase(i + 1);
	__Dec(i);

	return i;
}

SyntaxBuilder::Iterator SyntaxBuilder::Enum(NodePtr& node, Iterator i) {

	NodePtr enumField = NULL;

	node->flags |= kScriptFlagEnum;

	__Inc(i);

	if ((*i)->lex->id != kScriptLexUnknown) {
		PostSyntaxError((*i)->line, "Enum's name mustn't be reserved word or language token (%s)",
			(*i)->word.data());
	}

	node->typeNode = this->_Create(i);

	__Inc(i);

	if ((*i)->lex->id != kScriptLexBraceL) {
		PostSyntaxError((*i)->line, "Lost left brace after enum's name (%s)",
			(*i)->word.data());
	}

	__Dec(i);
	__Dec(i);

	return this->Class(node, i);
}

SyntaxBuilder::Iterator SyntaxBuilder::Ternary(NodePtr& node, Iterator i) {

	NodePtr ternaryNode = NULL;

	__Inc(i);

	while ((*i)->lex->id != kScriptLexColon) {
		i = this->_Append(&node->blockList, i);
		__Inc(i);
	}

	__Inc(i);

	while ((*i)->lex->id != kScriptLexSemicolon) {
		i = this->_Append(&node->elseList, i);
		__Inc(i);
	}

	if (node->blockList.empty() || node->elseList.empty()) {
		PostSyntaxError((*i)->line, "Lost expression in ternary operator", 0);
	}

	return i;
}

SyntaxBuilder::Iterator SyntaxBuilder::Directed(NodePtr& node, Iterator i) {

	if (node->lex->lex->id != kScriptLexUnknown) {
		return i;
	}
	__Inc(i);
	if ((*i)->lex->id != kScriptLexDirected) {
		return i - 1;
	}

	while ((*i)->lex->id != kScriptLexUnknown) {
		__Inc(i);
		NodePtr nextNode = node;
		while (nextNode->next) {
			nextNode = nextNode->next;
		}
		nextNode->next = this->_Create(i);
		__Inc(i);
		if ((*i)->lex->id != kScriptLexDirected) {
			break;
		}
	}

	return i - 1;
}

SyntaxBuilder::Iterator SyntaxBuilder::Static(NodePtr& node, Iterator i) {

	if (node->parent->id != kScriptNodeClass) {
		PostSyntaxError(node->lex->line, "Static construction must be in class, found (%s)", node->parent->word.data());
	}

	node->id = kScriptNodeStatic;

	__Inc(i);
	__Inc(i);

	while ((*i)->lex->id != kScriptLexBraceR) {
		NodePtr element = this->_Append(i);
		if (element->id == kScriptNodeFunction ||
			element->id == kScriptNodeClass ||
			element->id == kScriptNodeInterface ||
			element->id == kScriptNodeAnonymous
		) {
			PostSyntaxError((*i)->line, "Illegal construciton in static block (%s)",
				element->word.data());
		}
		node->blockList.push_back(element);
		if (_WasItBrace(node->blockList.back())) {
			__Inc(i);
		}
		else {
			if ((*i)->lex->id != kScriptLexBraceR) {
				__Inc(i);
			}
		}
	}

	return i;
}

SyntaxBuilder::Iterator SyntaxBuilder::Switch(NodePtr& node, Iterator i) {

	node->id = kScriptNodeCondition;

	if (node->lex == *i) {
		__Inc(i);
	}

	if ((*i)->lex->id != kScriptLexParenthesisL) {
		PostSyntaxError((*i)->line, "Lost left parenthesis in (%s) construction",
			node->word.data());
	}

	i = this->Arguments(node, i);

	__Inc(i);

	if ((*i)->lex->id != kScriptLexBraceL) {
		PostSyntaxWarning((*i)->line, "Lost left brace in 'switch' construction (%s)",
			(*i)->word.data());
	}

	__Inc(i);

	while ((*i)->lex->id != kScriptLexBraceR) {
		if ((*i)->lex->id != kScriptLexDefault &&
			(*i)->lex->id != kScriptLexCase
		) {
			PostSyntaxError((*i)->line, "Illegal token in 'switch' construction (%s)",
				(*i)->word.data());
		}
		NodePtr switchNode = this->_Create(i);
		node->blockList.push_back(switchNode);
		if ((*i)->lex->id == kScriptLexCase) {
			__Inc(i);
			while ((*i)->lex->id != kScriptLexColon) {
				switchNode->argList.push_back(this->_Create(i));
				__Inc(i);
			}
		}
		else {
			__Inc(i);
		}
		if ((*i)->lex->id != kScriptLexColon) {
			PostSyntaxError((*i)->line, "Excepted colon in 'case' block (%s)",
				(*i)->word.data());
		}
		__Inc(i);
		while (
			(*i)->lex->id != kScriptLexCase &&
			(*i)->lex->id != kScriptLexDefault &&
			(*i)->lex->id != kScriptLexBraceR
		) {
			NodePtr caseNode = this->_Append(i);
			switchNode->blockList.push_back(caseNode);
			if ((*i)->lex->id == kScriptLexBreak) {
				if (caseNode->parent->lex->lex->id == kScriptLexSwitch) {
					if (switchNode->switchInfo.hasBreak) {
						PostSyntaxError((*i)->line, "Unreachable statement", 0);
					}
					switchNode->switchInfo.hasBreak = TRUE;
				}
			}
			__Inc(i);
		}
	}

	return i;
}

SyntaxBuilder::Iterator SyntaxBuilder::Type(NodePtr& node, Iterator i) {

	i = this->Directed(node, i);

	if (this->_IsArray(i)) {
		i = this->Array(node, i);
	}

	if (this->_IsArguments(i)) {
		i = this->Arguments(node, i);
	}

	if (this->_IsTemplate(i) ||
		this->_IsSuper(i)
	) {
		i = this->Template(node, i);
	}

	return i;
}

SyntaxBuilder::Iterator SyntaxBuilder::Package(NodePtr& node, Iterator i) {

	if (node->lex == *i) {
		__Inc(i);
	}

	if (node->parent->id != kScriptNodeEntry) {
		PostSyntaxError((*i)->line, "Package statement must be in the root scope (%s)",
			node->parent->word.data());
	}

	NodePtr nextNode = node;

	if ((*i)->lex->id != kScriptLexUnknown) {
		PostSyntaxError((*i)->line, "Excepted identifier (%s)",
			(*i)->word.data());
	}

	while ((*i)->lex->id != kScriptLexSemicolon) {
		if ((*i)->lex->id != kScriptLexUnknown) {
			PostSyntaxError((*i)->line, "Excepted identifier (%s)",
				(*i)->word.data());
		}
		nextNode->next = this->_Create(i);
		if (nextNode->id != kScriptNodeDefault) {
			PostSyntaxError((*i)->line, "Excepted identifier (%s)",
				(*i)->word.data());
		}
		__Inc(i);
		if ((*i)->lex->id == kScriptLexSemicolon) {
			break;
		}
		if ((*i)->lex->id != kScriptLexDirected) {
			PostSyntaxError((*i)->line, "Lost dot or semicolon (%s)",
				(*i)->word.data());
		}
		__Inc(i);
		nextNode = nextNode->next;
	}

	this->packageManager->Register(node);

	return i;
}

SyntaxBuilder::Iterator SyntaxBuilder::Import(NodePtr& node, Iterator i) {

	if (node->lex == *i) {
		__Inc(i);
	}

	if (node->parent->id != kScriptNodeEntry) {
		PostSyntaxError((*i)->line, "Import statement must be in the root scope (%s)",
			node->parent->word.data());
	}

	NodePtr nextNode = node;

	if ((*i)->lex->id != kScriptLexUnknown) {
		PostSyntaxError((*i)->line, "Excepted identifier (%s)",
			(*i)->word.data());
	}

	while ((*i)->lex->id != kScriptLexSemicolon) {
		if ((*i)->lex->id != kScriptLexUnknown &&
			(*i)->lex->id != kScriptLexMul
		) {
			PostSyntaxError((*i)->line, "Excepted identifier (%s)",
				(*i)->word.data());
		}
		nextNode->next = this->_Create(i);
		if (nextNode->id != kScriptNodeDefault) {
			PostSyntaxError((*i)->line, "Excepted identifier (%s)",
				(*i)->word.data());
		}
		__Inc(i);
		if ((*i)->lex->id == kScriptLexSemicolon) {
			break;
		}
		if ((*i)->lex->id != kScriptLexDirected) {
			PostSyntaxError((*i)->line, "Lost dot or semicolon (%s)",
				(*i)->word.data());
		}
		__Inc(i);
		nextNode = nextNode->next;
	}

	this->packageManager->Import(node);

	return i;
}

SyntaxBuilder::Iterator SyntaxBuilder::Initialize(NodePtr& node, Iterator i) {

	Bool wasItInit = FALSE;

	if ((*i)->lex->id != kScriptLexBraceL) {
		return i;
	}
	__Inc(i);

	while ((*i)->lex->id != kScriptLexBraceR) {
		wasItInit = FALSE;
		NodePtr blockNode = this->_Create(i);
		node->blockList.push_back(blockNode);
		if ((*i)->lex->id == kScriptLexBraceL) {
			wasItInit = TRUE;
			i = this->Initialize(blockNode, i);
		}
		__Inc(i);
		while (
			(*i)->lex->id != kScriptLexComma &&
			(*i)->lex->id != kScriptLexBraceR
		) {
			blockNode->next = this->_Create(i);
			blockNode = blockNode->next;
			__Inc(i);
		}
		if ((*i)->lex->id == kScriptLexComma) {
			__Inc(i);
		}
		else {
			if ((*i)->lex->id == kScriptLexBraceR && !wasItInit) {
				break;
			}
			else {
				PostSyntaxError((*i)->line, "Illegal token in initialize list (%s)",
					(*i)->word.data());
			}
			__Inc(i);
		}
	}
		
	return i;
}

Bool SyntaxBuilder::_WasItBrace(NodePtr node) {

	return
		node->id == kScriptNodeCondition ||
		node->id == kScriptNodeClass ||
		node->id == kScriptNodeAnonymous ||
		node->id == kScriptNodeFunction ||
		node->id == kScriptNodeInterface ||
		node->id == kScriptNodeEntry ||
		node->id == kScriptNodeStatic;
}

NodePtr SyntaxBuilder::_Create(Iterator& i, NodeID nodeType) {

	/*	Associate lex with it's node ID. */

	if ((*i)->lex->id == kScriptLexClass ||
		(*i)->lex->id == kScriptLexEnum
	) {
		nodeType = kScriptNodeClass;
	}
	else if ((*i)->lex->id == kScriptLexInterface) {
		nodeType = kScriptNodeInterface;
	}
	else if ((*i)->lex->IsCondition()) {
		nodeType = kScriptNodeCondition;
	}

	if ((*i)->lex->id == kScriptLexStatic) {
		nodeType = kScriptNodeStatic;
	}

	/*	Create new node and push it to node list. */

	NodePtr node = new Node((*i)->word, nodeType, *i, this->parentNode, this->previousNode);
	this->nodeContainer.push_back(node);
	node->syntaxBuilder = this;

	/*	Check node for 'power' and save its parent. */

	if (node->lex->lex->IsCondition() ||
		node->id == kScriptNodeFunction ||
		node->id == kScriptNodeClass ||
		node->id == kScriptNodeAnonymous ||
		node->id == kScriptNodeInterface ||
		node->id == kScriptNodeCondition ||
		node->id == kScriptNodeEntry ||
		node->id == kScriptNodeStatic
	) {
		this->parentNode = node;
	}

	this->previousNode = node;

	return node;
}

NodePtr SyntaxBuilder::_Remove(NodePtr node) {

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
		this->_Remove(n);
	}
	node->blockList.clear();

	return parent;
}

NodePtr SyntaxBuilder::_Append(Iterator& i) {

	NodePtr node = this->_Create(i);

	if ((*i)->lex->id == kScriptLexImport) {
		i = this->Import(node, i);
	}
	if ((*i)->lex->id == kScriptLexPackage) {
		i = this->Package(node, i);
	}

	if (this->_IsStatic(i)) {
		i = this->Static(node, i);
	}
	else if (this->_IsArray(i)) {
		i = this->Array(node, i);
	}
	else if (this->_IsNew(i)) {
		i = this->New(node, i);
	}
	else if (this->_IsArguments(i + 1)) {
		i = this->Arguments(node, i);
	}

	if ((*i)->lex->id == kScriptLexEnum) {
		i = this->Enum(node, i);
	}
	else if ((*i)->lex->id == kScriptLexTernary) {
		i = this->Ternary(node, i);
	}

	if (this->_IsTemplate(i + 1)) {
		i = this->Template(node, i);
	}

	if (node->lex->lex->id == kScriptLexClass ||
		node->lex->lex->id == kScriptLexInterface
	) {
		i = this->Class(node, i);
	}
	else if (node->lex->lex->IsCondition()) {
		switch (node->lex->lex->id) {
			case kScriptLexIf:      i = this->If(node, i);      break;
			case kScriptLexElse:    i = this->Else(node, i);    break;
			case kScriptLexWhile:   i = this->While(node, i);   break;
			case kScriptLexDo:      i = this->Do(node, i);      break;
			case kScriptLexFor:     i = this->For(node, i);     break;
			case kScriptLexTry:     i = this->Try(node, i);     break;
			case kScriptLexCatch:   i = this->Catch(node, i);   break;
			case kScriptLexFinally: i = this->Finally(node, i); break;
			case kScriptLexSwitch:  i = this->Switch(node, i);  break;
        default:
            break;
		}
	}
	else if (node->lex->lex->IsModificator()) {
		switch ((*i)->lex->id) {
			case kScriptLexPublic:       this->modificatorFlags |= kScriptFlagPublic;       break;
			case kScriptLexPrivate:      this->modificatorFlags |= kScriptFlagPrivate;      break;
			case kScriptLexProtected:    this->modificatorFlags |= kScriptFlagProtected;    break;
			case kScriptLexStatic:       this->modificatorFlags |= kScriptFlagStatic;       break;
			case kScriptLexNative:       this->modificatorFlags |= kScriptFlagNative;       break;
			case kScriptLexFinal:        this->modificatorFlags |= kScriptFlagFinal;        break;
			case kScriptLexAbstract:     this->modificatorFlags |= kScriptFlagAbstract;     break;
			case kScriptLexDecprecated:  this->modificatorFlags |= kScriptFlagDeprecated;   break;
			case kScriptLexOverride:     this->modificatorFlags |= kScriptFlagOverride;     break;
			case kScriptLexSynchronized: this->modificatorFlags |= kScriptFlagSynchronized; break;
		default:
			break;
		}
	}

	if (this->_IsMethod(i)) {
		i = this->Method(node, i);
	}
	else if (this->_IsVariable(i)) {
		i = this->Variable(node, i);
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

		i = this->Arguments(node, i);
	}

	/*	Fix for cast */

	if (this->_IsCast(i + 1) && (*(i + 2))->lex->id == kScriptLexUnknown) {
		__Inc(i);
		i = this->_List().erase(i);
		(*i)->ChangeLex(Lex::Find(kScriptLexCast));
		(*i)->args = (*i)->lex->args;
		i = this->_List().erase(i + 1);
		__Dec(i);
		__Dec(i);
	}
	else if (this->_IsCast(i + 0) && (*(i + 1))->lex->id == kScriptLexUnknown) {
		i = this->_List().erase(i);
		node->word = (*i)->word;
		node->lex->ChangeLex(Lex::Find(kScriptLexCast));
		node->lex->args = node->lex->lex->args;
		i = this->_List().erase(i + 1);
		__Dec(i);
	}

	/* Fix for return */

	if ((*i)->lex->id == kScriptLexReturn) {
		__Inc(i);
		__Dec(i);
		if ((*(i + 1))->lex->id == kScriptLexSemicolon) {
			(*i)->args = 0;
		}
	}

	if (i == this->fileParser->GetLexList().end()) {
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
		node->id != kScriptNodeAlloc &&
		node->id != kScriptNodeAnonymous
	) {
		if (this->parentNode->parent) {
			this->parentNode = this->parentNode->parent;
		}
	}

	return node;
}

SyntaxBuilder::Iterator SyntaxBuilder::_Append(Deque<NodePtr>* list, Iterator i) {

	NodePtr node = this->_Append(i);

	if (node->id == kScriptNodeVariable) {
		while (node) {
			list->push_back(node);
			node = node->next;
		}
	}
	else {
		if (!node->lex->lex->IsModificator()) {
			list->push_back(node);
		}
	}

	return i;
}

Void SyntaxBuilder::_Order(NodePtr node) {

	node->ShuntingYard();

	for (NodePtr n : node->blockList) {
		this->_Order(n);
	}
	for (NodePtr n : node->argList) {
		this->_Order(n);
	}
	for (NodePtr n : node->elseList) {
		this->_Order(n);
	}
	for (NodePtr n : node->forInfo.beginList) {
		this->_Order(n);
	}
	for (NodePtr n : node->forInfo.conditionList) {
		this->_Order(n);
	}
	for (NodePtr n : node->forInfo.nextList) {
		this->_Order(n);
	}

	if (node->elseNode) {
		this->_Order(node->elseNode);
	}
}

Bool SyntaxBuilder::_Match(Uint32 id, Iterator i) {
	return this->sequenceMatcher.Match(id, i, this->fileParser->GetLexList().end());
}

Void SyntaxBuilder::_Reset(Void) {

	/*	Reset node builder, remove all
		nodes and clear all stacks and
		quques. */

	for (NodePtr n : this->nodeContainer) {
		delete n;
	}

	this->nodeContainer.clear();
	this->parentNode = 0;
	this->previousNode = 0;
	this->rootNode = 0;
	this->modificatorFlags = 0;
}

Void SyntaxBuilder::_AllowModificators(NodePtr node, Uint32 modificators) {

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
		((node->flags & kScriptFlagSynchronized) != 0 && !(modificators & kScriptFlagSynchronized)) ||
		((node->flags & kScriptFlagDeprecated) != 0 && !(modificators & kScriptFlagDeprecated))
	) {
		PostSyntaxError(node->lex->line, "Modificator in illegal place", 1);
	}
}

Void SyntaxBuilder::_Combine(NodePtr node) {

	static Deque<NodePtr>* prevList
		= &node->blockList;

	prevList = &node->blockList;

	for (NodePtr n : node->blockList) {
		if (n->lex->lex->id == kScriptLexParenthesisL) {
			this->_Combine(n);
		}
	}

	prevList = &node->argList;

	for (NodePtr n : node->argList) {
		if (n->lex->lex->id == kScriptLexParenthesisL) {
			this->_Combine(n);
		}
	}
}

LAME_END2