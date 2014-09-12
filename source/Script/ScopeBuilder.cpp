#include "ScopeBuilder.h"
#include "Variable.h"
#include "Method.h"
#include "Interface.h"
#include "Class.h"

#include <float.h>

LAME_BEGIN2(Script)

static Bool _MoveNode(NodePtr node, Bool strict = FALSE) {

	Bool result = FALSE;

	/*	Get last element in stack. Now we're going to swap
		arguments parent with argument expression, it will help us
		to prevent different unsuitable situations in future, when we'll
		perform code. For example, if we have expression 'if(1)', then
		we have to cut it into 2 expressions and apply shunting yard for every,
		whereupon we'll swap our expressions and push in one stack. Final
		view should be : '1 if'. We've saved count of arguments in node 'new' */

	if (node->id != kScriptNodeVariable &&
		node->id != kScriptNodeAlloc &&
		node->lex->lex->id != kScriptLexNew &&
		node->id != kScriptNodeDefault
	) {
		if (!strict) {
			goto _Seek;
		}
	}

	if (node->parent != NULL && node->argList.size() > 0) {

		Deque<NodePtr>* nodeStack = NULL;

		/*	Get element from parent's stack */
		auto position = std::find(
			node->parent->blockList.begin(),
			node->parent->blockList.end(), node);

#define _If(_list) \
	if (!nodeStack && (position = std::find(_list.begin(), _list.end(), node)) != _list.end()) {\
		nodeStack = &_list; \
	}

		_If(node->parent->forInfo.beginList);
		_If(node->parent->forInfo.conditionList);
		_If(node->parent->forInfo.nextList);
		_If(node->parent->blockList);
		_If(node->parent->argList);

		if (node->parent->elseNode) {
			_If(node->parent->elseNode->blockList);
		}

		if (node->parent->lex->lex->id == kScriptLexSwitch) {
			for (NodePtr n : node->parent->blockList) {
				_If(n->blockList);
			}
		}

		if (node->parent->typeNode) {
		}

		if (!nodeStack) {
			PostSyntaxError(node->parent->lex->line, "Node (%s) hasn't been appended to his parent (%s)", node->word.data(), node->parent->word.data());
		}

		/*	Push argument list in parent's block stack */
		if (node->id != kScriptNodeVariable) {
			nodeStack->insert(position,
				node->argList.begin(), node->argList.end());
		}
		else {
			nodeStack->insert(position + 1,
				node->argList.begin(), node->argList.end());
		}

		node->argList.clear();

		result = TRUE;
	}

_Seek:
	if (node->elseNode) {
		_MoveNode(node->elseNode);
	}

	if (node->id == kScriptNodeCondition) {
		for (NodePtr n : node->argList) {
			if (_MoveNode(n)) {
				goto _Seek;
			}
		}
	}
_Again0:
	for (NodePtr n : node->blockList) {
		if (_MoveNode(n)) {
			goto _Again0;
		}
	}
_Again1:
	for (NodePtr n : node->forInfo.beginList) {
		if (_MoveNode(n)) {
			goto _Again1;
		}
	}
_Again2:
	for (NodePtr n : node->forInfo.conditionList) {
		if (_MoveNode(n)) {
			goto _Again2;
		}
	}
_Again3:
	for (NodePtr n : node->forInfo.nextList) {
		if (_MoveNode(n)) {
			goto _Again3;
		}
	}

	for (NodePtr n : node->switchInfo.caseList) {
	_Again5:
		for (NodePtr n2 : n->blockList) {
			if (_MoveNode(n2)) {
				goto _Again5;
			}
		}
	}

	return result;
}

static ObjectPtr _FindClass(ScopePtr scope, NodePtr node) {

	ObjectPtr scopeClass = NULL;
	NodePtr nextNode = NULL;
	ObjectPtr classType = NULL;
	Buffer classPath;

	if (!node->next) {
		return scope->Find(node->word);
	}

	nextNode = node;

	while (nextNode) {
		if (!classType) {
			if (!(classType = scope->Find(nextNode->word, TRUE, Uint32(Object::Type::Class)))) {
				if (!(classType = scope->Find(nextNode->word, TRUE, Uint32(Object::Type::Interface)))) {
					goto _Null;
				}
			}
		}
		else {
			scopeClass = classType;
			if (!(classType = scopeClass->Find(nextNode->word, FALSE, Uint32(Object::Type::Class)))) {
				if (!(classType = scopeClass->Find(nextNode->word, FALSE, Uint32(Object::Type::Interface)))) {
					goto _Null;
				}
			}
		}
		classPath += nextNode->word + '/';
		nextNode = nextNode->next;
	}

	if (!classPath.empty()) {
		classPath.pop_back();
	}
	
	node->word = classType->GetName();
	node->next = NULL;
	node->var = classType;

	return classType;

_Null:
	node->word = classPath + nextNode->word;
	return NULL;
}

ScopeBuilder::ScopeBuilder() {
	this->lastVar = NULL;
}

ScopeBuilder::~ScopeBuilder() {
}

Void ScopeBuilder::Build(NodePtr rootNode, ScopePtr rootScope) {

	/*	Move all consutions's nodes from blocks
		to parent's block */

	_MoveNode(rootNode);

	/*	Copy root scope to root node */
	rootNode->var = ObjectPtr(rootScope);

	/*	Save root scope in stack as default */
	this->_Push(rootScope);

	/*	Class/Interface declare can be divided onto several blocks:
			1. Declaring class/interface prototypes (without variables, methods or classes)
			2. Apply class/interface modificators
			3. Declaring class/interface methods and variables */

	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachClassPrototype, this), kScriptNodeClass);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachClassStatic, this), kScriptNodeStatic);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachClassPrototype, this), kScriptNodeAnonymous);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachInterfacePrototype, this), kScriptNodeInterface);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachInterfacePrototype, this), kScriptNodeAnnotation);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachInterfaceDeclare, this), kScriptNodeInterface);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachInterfaceDeclare, this), kScriptNodeAnnotation);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachClassDeclare, this), kScriptNodeClass);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachClassDeclare, this), kScriptNodeAnonymous);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachClassVariableDeclare, this), kScriptNodeVariable);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachModificatorSet, this), kScriptNodeClass);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachModificatorSet, this), kScriptNodeInterface);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachModificatorSet, this), kScriptNodeAnnotation);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachMethodDeclare, this), kScriptNodeFunction);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachModificatorSet, this), kScriptNodeFunction);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachConstruction, this), kScriptNodeCondition);

	/*	Class/Interface inheritance:
			1. We have to apply inheritance for classes
			2. Apply inheritance for interfaces */

	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachClassInherit, this), kScriptNodeClass);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachClassInherit, this), kScriptNodeAnonymous);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachInterfaceInherit, this), kScriptNodeInterface);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachInterfaceInherit, this), kScriptNodeAnnotation);

	/*	Constant declare. That block allocate memory
		for constant variables and push it to root
		scope. */

	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachConstDeclare, this), kScriptNodeDefault);

	/*	Mathod/Variable declare can be divided onto next blocks:
			1. Declare method (method block and it's arguments)
			2. Apply modificators for method (public, private, static etc)
			3. Now we can register methods (after apply modificators we can
				declare 'this' variable in non-static method, which parent is class)
			4. Declare variable (this block includes declaration and registration
				to avoid using variables before it's declaration)
			5. Now we can apply modificators for variables */

	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachMethodRegister, this), kScriptNodeFunction);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachVariableDeclare, this), kScriptNodeVariable);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachModificatorSet, this), kScriptNodeVariable);

	/*	Check for empty nodes */
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachNodeFind, this), kScriptNodeUnknown);

	/*  Check every class/interface inheritance lexes */
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachCheckInheritance, this), kScriptNodeInterface);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachCheckInheritance, this), kScriptNodeAnnotation);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachCheckInheritance, this), kScriptNodeClass);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachCheckInheritance, this), kScriptNodeAnonymous);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachClassInit, this), kScriptNodeClass);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachClassInit, this), kScriptNodeAnonymous);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachNodeFlush, this), kScriptNodeUnknown);

	/* Declare default method <init> in other classes */

	for (ObjectPtr c : rootScope->Filter([](ObjectPtr c) -> Bool { 
		return c->CheckType(Object::Type::Class);
	})) {
		if (c->CheckModificator(Object::Modificator::Primitive)) {
			continue;
		}

		if (c->Find("<init>", FALSE)) {
			continue;
		}

		MethodPtr initMethod = c->Add(new Method("<init>", c,
			c, Scope::classVoid))->GetMethod();

		initMethod->SetNativeMethod([] (MethodPtr m) {
			/* Ignore */
		});
		initMethod->SetModificator(Object::Modificator::Internal);
	}
    
#if 0 /* Node trace (only for debugging) */
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachNodeTrace, this), kScriptNodeUnknown);
#endif
}

Void ScopeBuilder::_ForEachClassStatic(NodePtr n) {

	if (n->parent->id != kScriptNodeClass) {
		PostSyntaxError(n->lex->line, "Static block must be in class (%s)",
			n->parent->word.data());
	}

	if (!n->var) {
		n->var = new Class("", scope);
	}
}

Void ScopeBuilder::_ForEachNodeTrace(NodePtr n) {

	static StringC spaceTokens = "\t\n\r\a\b";

	if (n->lex->args && n->lex->args != n->lex->lex->args) {
		printf("%s(%d) ", n->word.data(), n->lex->args);
	}
	else {
		if (n->word.length() == 1 && strchr(spaceTokens, n->word[0])) {
			printf("%d ", n->word[0]);
		}
		else {
			printf("%s ", n->word.data());
		}
	}
}

Void ScopeBuilder::_ForEachModificatorSet(NodePtr n) {

	if (n->var) {
		if ((n->flags & kScriptFlagOverride) != 0) {
			n->var->SetModificator(Class::Modificator::Override);
		}
		if ((n->flags & kScriptFlagDeprecated) != 0) {
			n->var->SetModificator(Class::Modificator::Deprecated);
		}
		if ((n->flags & kScriptFlagFinal) != 0) {
			n->var->SetModificator(Class::Modificator::Final);
		}
		if ((n->flags & kScriptFlagPublic) != 0) {
			n->var->SetModificator(Class::Modificator::Public);
		}
		if ((n->flags & kScriptFlagPrivate) != 0) {
			n->var->SetModificator(Class::Modificator::Private);
		}
		if ((n->flags & kScriptFlagProtected) != 0) {
			n->var->SetModificator(Class::Modificator::Protected);
		}
		if ((n->flags & kScriptFlagStatic) != 0) {
			n->var->SetModificator(Class::Modificator::Static);
		}
		if ((n->flags & kScriptFlagNative) != 0) {
			n->var->SetModificator(Class::Modificator::Native);
		}
		if ((n->flags & kScriptFlagNative) != 0) {
			n->var->SetModificator(Class::Modificator::Native);
		}
		if ((n->flags & kScriptFlagAbstract) != 0) {
			n->var->SetModificator(Class::Modificator::Abstract);
			if (ObjectPtr(n->var->GetParent())->CheckType(Object::Type::Class)) {
				ObjectPtr(n->var->GetParent())->SetModificator(Class::Modificator::Abstract);
			}
		}
		if ((n->flags & kScriptFlagSynchronized) != 0) {
			n->var->SetModificator(Class::Modificator::Synchronized);
		}
	}
}

Void ScopeBuilder::_ForEachNodeFind(NodePtr n) {

	if ((n->flags & kScriptFlagAnnotation) != 0) {
		if (!(n->var = _FindClass(scope, n))) {
			PostSyntaxError(n->lex->line, "Undeclared annotation (%s)",
				n->word.data());
		}
		n->var->SetModificator(Object::Modificator::Annotation);
	}

	if (n->var) {
		n->var->SetNode(n);
	}
}

Void ScopeBuilder::_ForEachClassPrototype(NodePtr n) {

	if (!n->typeNode) {
		return;
	}

	if (n->id == kScriptNodeAnonymous) {
		do {
			n->var = this->scope->Add(new Class("", this->scope));
		} while (!n->var);
	}
	else {
		n->var = this->scope->Add(new Class(n->typeNode->word, this->scope));
	}

	if (!n->var) {
		PostSyntaxError(n->lex->line, "Class redeclaration (%s)", n->typeNode->word.data());
	}

	if ((n->flags & kScriptFlagEnum) != 0) {
		n->var->SetModificator(Object::Modificator::Enum);
	}

	n->var->SetNode(n);
}

Void ScopeBuilder::_ForEachInterfacePrototype(NodePtr n) {

	n->var = this->scope->Add(
		new Interface(n->typeNode->word, this->scope));

	if (!n->var) {
		PostSyntaxError(n->lex->line, "Interface redeclaration (%s)", n->typeNode->word.data());
	}

	n->var->SetNode(n);
}

Void ScopeBuilder::_ForEachClassDeclare(NodePtr n) {

	if (!n->typeNode) {
		return;
	}

	if (!n->var) {
		n->var = _FindClass(scope, n->typeNode);
	}

	if (!n->var) {
		PostSyntaxError(n->lex->line, "Undeclared class (%s)", n->typeNode->word.data());
	}

	if (n->typeNode->templateNode) {
		ObjectPtr classT = n->var->Scope::Add(this->scope->classObject->Clone(
			n->typeNode->templateNode->word, n->var));
		if (!classT) {
			PostSyntaxError(n->lex->line, "Class redeclaration (%s)", n->typeNode->templateNode->word.data());
		}
		classT->SetModificator(Object::Modificator::Private)
			->SetTemplate(classT->GetClass());
	}
}

Void ScopeBuilder::_ForEachClassVariableDeclare(NodePtr n) {

	ObjectPtr typeClass;

	if (!(n->parent && n->parent->id == kScriptNodeClass)) {
		return;
	}

	if ((n->parent->flags & kScriptFlagEnum) != 0) {
		typeClass = Scope::classInt;
	}
	else {
		if (!n->typeNode) {
			if (!(typeClass = scope->Find(n->word))) {
				PostSyntaxError(n->lex->line, "Undeclared variable (%s)", n->word.data());
			}
			n->var = typeClass;
			typeClass = typeClass->GetClass();
		}
		else {
			typeClass = _FindClass(scope, n->typeNode);
		}
	}

	if (!typeClass) {
		PostSyntaxError(n->lex->line, "Undeclared type (%s)", n->typeNode->word.data());
	}

	if ((n->typeNode->flags & kScriptFlagArray) != 0) {
        n->var = scope->Add(new Variable(n->word, scope, typeClass->GetClass(), TRUE));
	}
	else if (!n->var) {
		n->var = scope->Add(new Variable(n->word, scope, typeClass->GetClass()));
	}

	if (!n->var) {
		PostSyntaxError(n->lex->line, "Variable redeclaration (%s)", n->word.data());
	}

	if ((n->parent->flags & kScriptFlagEnum) != 0) {
		n->var->SetModificator(Object::Modificator::Final);
	}

	if (ObjectPtr(this->scope)->CheckType(Object::Type::Class)) {
		n->var->SetFieldID(this->scope->Size() - n->var->Size());
		n->var->SetThis(ObjectPtr(this->scope));
	}

	if (n->typeNode && n->typeNode->templateNode) {
		if (!typeClass->GetTemplate()) {
			PostSyntaxError(n->lex->line, "Class (%s) non-template", typeClass->GetName().data());
		}
		ObjectPtr classT = n->var->Find(n->typeNode->templateNode->word);
		if (!classT) {
			PostSyntaxError(n->lex->line, "Undeclared class (%s)", n->typeNode->templateNode->word.data());
		}
		if (classT->CheckModificator(Class::Modificator::Primitive)) {
			PostSyntaxError(n->lex->line, "Template class mustn't be primitive type (%s)", n->templateNode->word.data());
		}
		ClassPtr classT2 = classT->GetClass();
		n->var->Scope::Add(classT->Clone(
			typeClass->GetTemplate()->GetName(), n->var));
		n->var->SetTemplate(classT2);
	}
}

Void ScopeBuilder::_ForEachInterfaceDeclare(NodePtr n) {

	n->var = _FindClass(scope, n->typeNode);

	if (!n->var) {
		PostSyntaxError(n->lex->line, "Undeclared interface (%s)", n->typeNode->word.data());
	}
}

Void ScopeBuilder::_ForEachClassInherit(NodePtr n) {

	NodePtr extendNode = NULL;
	ObjectPtr extendClass = NULL;

#if 0
	if (n->id == kScriptNodeAnonymous) {
		if (!(classExtend = _FindClass(scope, n)) {
			PostSyntaxError(n->lex->line, "Undeclared class (%s)", n->word.data());
		}
		classVar->Extend(classExtend);
	}
#endif

	if (!(extendNode = n->classInfo.extendNode)) {
		extendClass = scope->classObject;
	}
	else {
		if (!(extendClass = _FindClass(scope, extendNode))) {
			PostSyntaxError(n->lex->line, "Undeclared class (%s)", extendNode->word.data());
		}
	}

	n->var->GetClass()->Extend(extendClass);

	extendClass = NULL;
	extendNode = NULL;

	for (NodePtr n2 : n->classInfo.implementNode) {

		extendNode = n2;

		if (!(extendClass = _FindClass(scope, n2))) {
			PostSyntaxError(n->lex->line, "Undeclared interface (%s)", extendNode->word.data());
		}

		n->var->GetClass()->Implement(extendClass);
	}

	for (ObjectPtr m : n->var->GetMethodSet()) {
		m->GetMethod()->SetThis(n->var);
		for (NodePtr n2 : n->var->GetNode()->blockList) {
			if (n2->word == m->GetName()) {
				m->GetMethod()->SetRootNode(n2);
				break;
			}
		}
	}
}

Void ScopeBuilder::_ForEachInterfaceInherit(NodePtr n) {

	ObjectPtr typeClass;

	if (n->classInfo.extendNode) {
		PostSyntaxError(n->lex->line, "Interfaces can't extend classes (%s)", n->classInfo.extendNode->word.data());
	}

	for (NodePtr n2 : n->classInfo.implementNode) {
		if (!(typeClass = n->var->Find(n2->word))) {
			PostSyntaxError(n->lex->line, "Undeclared interface (%s)", n2->word.data());
		}
		n->var->GetClass()->Implement(typeClass->GetInterface());
	}

	for (ObjectPtr m : n->var->GetMethodSet()) {
		m->GetMethod()->SetThis(n->var);
		for (NodePtr n2 : n->var->GetNode()->blockList) {
			if (n2->word == m->GetName()) {
				m->GetMethod()->SetRootNode(n2);
				break;
			}
		}
	}
}

Void ScopeBuilder::_ForEachConstDeclare(NodePtr n) {

	if (!n->lex->lex->IsConst()) {
		return;
	}

	ScopePtr globalScope = scope->Root();

	if ((n->var = globalScope->Find(n->word))) {
		return;
	}

	if (n->lex->lex->id == kScriptLexInt) {

		Sint64 intValue = FileParser::ParseIntValue(n->word.data());
		ClassPtr intClass;

		if (intValue >= -0x7f - 1 && intValue <= 0x7f) {
			intClass = globalScope->classByte;
		}
		else if (intValue >= -0x7fff - 1 && intValue <= 0x7fff) {
			intClass = globalScope->classShort;
		}
		else if (intValue >= -0x7fffffff - 1 && intValue <= 0x7fffffff) {
			intClass = globalScope->classInt;
		}
		else {
			intClass = globalScope->classLong;
		}

		n->var = globalScope->Add(
			new Variable(n->word, globalScope, intClass));

		n->var->GetVariable()->SetInteger(intValue)
			->SetModificator(Class::Modificator::Final)
			->SetModificator(Class::Modificator::Constant);
	}
	else if (n->lex->lex->id == kScriptLexFloat) {
        
		Float64 floatValue = FileParser::ParseFloatValue(n->word.data());
        ClassPtr floatClass;
        
		if (floatValue >= -FLT_MAX && floatValue <= FLT_MAX) {
            floatClass = globalScope->classFloat;
        } else {
            floatClass = globalScope->classDouble;
        }

		n->var = globalScope->Add(new Variable(
            n->word, globalScope, floatClass));

		n->var->GetVariable()->SetFloat(floatValue)
			->SetModificator(Class::Modificator::Final)
			->SetModificator(Class::Modificator::Constant);
	}
	else if (n->lex->lex->id == kScriptLexString) {

		n->var = globalScope->Add(
			new Variable(n->word, globalScope, globalScope->classString));

		n->var->GetVariable()->SetString(FileParser::ParseStringValue(n->word.data()))
			->SetModificator(Class::Modificator::Final)
			->SetModificator(Class::Modificator::Constant);
	}
	else if (n->lex->lex->id == kScriptLexCharacter) {

		n->var = globalScope->Add(
			new Variable(n->word, globalScope, globalScope->classChar));

		n->var->GetVariable()->SetInteger(n->word[0])
			->SetModificator(Class::Modificator::Final)
			->SetModificator(Class::Modificator::Constant);
	}
}

Void ScopeBuilder::_ForEachMethodDeclare(NodePtr n) {

	ObjectPtr returnType;
	ObjectPtr methodObject;
	Vector<ClassPtr> methodAttributes;

	returnType = _FindClass(scope, n->typeNode);

	if (!returnType) {
		PostSyntaxError(n->typeNode->lex->line, "Undeclared type (%s)", n->typeNode->word.data());
	}

	for (NodePtr n2 : n->argList) {

		ObjectPtr argType = _FindClass(scope, n2->typeNode);

		if (!argType) {
			PostSyntaxError(n2->typeNode->lex->line, "Undeclared type (%s)", n2->typeNode->word.data());
		}

		methodAttributes.push_back(argType->GetClass());
	}

	MethodPtr method = new Method(n->word, scope, ObjectPtr(scope), returnType, methodAttributes);

	if ((n->flags & kScriptFlagImplemented)) {
		method->SetModificator(Object::Modificator::Implemented);
	}

	method->SetNode(n);
	n->var = method;

	if (ObjectPtr(scope)->CheckModificator(Object::Modificator::Construction)) {
		goto _DeclareError;
	}

	if (!ObjectPtr(this->scope)->CheckType(Object::Type::Class) &&
		!ObjectPtr(this->scope)->CheckType(Object::Type::Interface)
	) {
	_DeclareError:
		PostSyntaxError(n->lex->line, "Unable to declare method in non-class scope %s/%s(%s)",
			this->scope->GetName().data(), n->word.data(), method->GetFormattedArguments().data());
	}

	if (!(methodObject = scope->Add(method))) {
		PostSyntaxError(n->lex->line, "Method redeclaration %s.%s(%s)",
			scope->GetName().data(), n->word.data(), method->GetFormattedArguments().data());
	}

	if (methodObject->GetName() == this->scope->GetName() && !returnType->IsVoid()) {
		PostSyntaxError(n->lex->line, "You can't declare custom constructor (%s) with another return type (%s)",
			methodObject->GetName().data(), returnType->GetName().data());
	}
    
    if ((n->flags & kScriptFlagImplemented) != 0) {
        methodObject->GetMethod()->SetRootNode(n);
    }

	n->methodInfo.invokeHash = methodObject->GetMethod()->GetInvokeHash();
	n->var = methodObject;
}

Void ScopeBuilder::_ForEachMethodRegister(NodePtr n) {

	ObjectPtr methodObject = n->var;

	if (!methodObject) {
		return;
	}

	if (n->parent->id == kScriptNodeClass ||
		n->parent->id == kScriptNodeInterface ||
        n->parent->id == kScriptNodeAnonymous
	) {
		if (!methodObject->CheckModificator(Object::Modificator::Static)) {

			methodObject->Scope::Add(new Variable("this", methodObject, ClassPtr(this->scope)));

			if (ClassPtr(this->scope)->GetExtend()) {
				methodObject->Scope::Add(new Variable("super", methodObject, ClassPtr(this->scope)->GetExtend()->GetClass()));
			}
		}
	}
}

Void ScopeBuilder::_ForEachVariableDeclare(NodePtr n) {

	ObjectPtr typeClass;

	if (n->parent && (n->parent->id == kScriptNodeClass)) {
		return;
	}

	if (!n->typeNode) {
		if (!(typeClass = _FindClass(scope, n->typeNode))) {
			PostSyntaxError(n->lex->line, "Undeclared variable (%s)", n->word.data());
		}
		n->var = typeClass;
		typeClass = typeClass->GetClass();
	}
	else {
		typeClass = _FindClass(scope, n->typeNode);
	}

	if (!typeClass) {
		PostSyntaxError(n->lex->line, "Undeclared type (%s)",
			n->typeNode->word.data());
	}

	if (!typeClass->CheckType(Object::Type::Class) && !typeClass->CheckType(Object::Type::Interface)) {
		PostSyntaxError(n->lex->line, "Variable's type must be class or primitive type (%s)",
			typeClass->GetName().data());
	}

	if (n->typeNode && (n->typeNode->flags & kScriptFlagArray) != 0) {
        n->var = scope->Add(new Variable(n->word, scope, typeClass->GetClass(), TRUE));
	}
	else {
		n->var = scope->Add(new Variable(n->word, scope, typeClass->GetClass()));
	}

	if (!n->var) {
		PostSyntaxError(n->lex->line, "Variable redeclaration (%s)", n->word.data());
	}

	if (n->typeNode && n->typeNode->templateNode) {
		if (!typeClass->GetTemplate()) {
			PostSyntaxError(n->lex->line, "Class (%s) non-template", typeClass->GetName().data());
		}
		ObjectPtr classT = n->var->Find(n->typeNode->templateNode->word);
		if (!classT) {
			PostSyntaxError(n->lex->line, "Undeclared class (%s)", n->typeNode->templateNode->word.data());
		}
		if (classT->CheckModificator(Class::Modificator::Primitive)) {
			PostSyntaxError(n->lex->line, "Template class mustn't be primitive type (%s)", n->templateNode->word.data());
		}
		ClassPtr classT2 = classT->GetClass();
		n->var->Scope::Add(classT->Clone(
			typeClass->GetTemplate()->GetName(), n->var));
		n->var->SetTemplate(classT2);
	}
}

Void ScopeBuilder::_ForEachConstruction(NodePtr n) {

	if (!n->var) {

		Bool hasSameScope = !lastVar || !lastVar->GetNode() ? FALSE :
			lastVar->GetNode()->parent == n->parent;

		if (!hasSameScope) {
			goto _SkipErrorCheck;
		}

		if (n->lex->lex->id == kScriptLexElse) {
			if (!lastVar->GetNode() || (lastVar->GetNode()->lex->lex->id != kScriptLexIf &&
				lastVar->GetNode()->lex->lex->id != kScriptLexElse)
			) {
				PostSyntaxError(n->lex->line, "Else must have If block (%s)", lastVar->GetName().data());
			}
			lastVar->GetNode()->elseNode = n;
		}

		if (n->lex->lex->id == kScriptLexFinally ||
			n->lex->lex->id == kScriptLexCatch
		) {
			if (!lastVar->GetNode()) {
				PostSyntaxError(n->lex->line, "Finally/Catch must have Try block (%s)", lastVar->GetName().data());
			}
			if (lastVar->GetNode()->lex->lex->id != kScriptLexFinally &&
				lastVar->GetNode()->lex->lex->id != kScriptLexCatch &&
				lastVar->GetNode()->lex->lex->id != kScriptLexTry
			) {
				PostSyntaxError(n->lex->line, "Finally/Catch must have Try block (%s)", lastVar->GetName().data());
			}
		}

		if (lastVar->GetNode() && lastVar->GetNode()->lex->lex->id == kScriptLexDo) {
			if (n->lex->lex->id != kScriptLexWhile) {
				PostSyntaxError(n->lex->line, "Do must have While block (%s)", n->word.data());
			}
			lastVar->GetNode()->elseNode = n;
			lastVar->GetNode()->argList = n->argList;
			n->argList.clear();
			n->flags |= kScriptFlagWhileHasDo;
		}

	_SkipErrorCheck:
		if (!n->parent->var) {
			PostSyntaxError(n->lex->line, "Language construction in illegal place (%s)", n->parent->word.data());
		}

		n->var = n->parent->var->Add(new Class("", scope))
			->SetModificator(Object::Modificator::Construction);

		n->var->SetNode(n);
		lastVar = n->var->GetClass();
		n->var->SetNode(n);
	}
}

Void ScopeBuilder::_ForEachCheckInheritance(NodePtr n) {

	if (n->var) {
		if (n->var->CheckType(Object::Type::Class) ||
			n->var->CheckType(Object::Type::Interface)
		) {
			n->var->GetClass()->CheckInheritance();
		}
	}
}

Void ScopeBuilder::_ForEachNodeFlush(NodePtr n) {

	if (n->var) {

		/*	There is a problem with scope's path
		which has been declared in temporary scope,
		cuz we can't store all elements in it's scope
		at nessesary time, cuz we can have
		constructions likes:
		
		interface I { class A {} }
		class B { interface Z {} }

		Where we can't declare class A before
		interface I and can't declare interface Z
		before class B. The easiest solution is
		to create temporary scope for I or B and
		after I/B declaration reallocate it's scope
		and merge with temporary */

		n->var->path = n->var->GetParent()->Path();
    }
	else {
		n->var = scope->Find(n->word);
	}
}

Void ScopeBuilder::_ForEachClassInit(NodePtr n) {

	ObjectPtr classVar = n->var;

	MethodPtr initMethod = classVar->Add(new Method("<init>", classVar,
		classVar, Scope::classVoid))->GetMethod();

	initMethod->SetModificator(Object::Modificator::Internal);
	initMethod->SetRootNode(n);

	initMethod->Scope::Add(new Variable("this", initMethod, ClassPtr(classVar)));

	if (ClassPtr(classVar)->GetExtend()) {
		initMethod->Scope::Add(new Variable("super", initMethod,
			ClassPtr(classVar)->GetExtend()->GetClass()));
	}

#if 0 /* Found better solution */
	Set<ObjectPtr> constructorSet;

	for (ObjectPtr m : classVar->GetMethodSet()) {
		if (m->GetName() == m->GetOwner()->GetName()) {
			constructorSet.insert(m);
		}
	}

	for (ObjectPtr m : constructorSet) {
		classVar->Add(new Method("this", classVar, classVar, Scope::classVoid, m->GetMethod()->GetAttributeHash()))
			->GetMethod()->SetRootNode(m->GetMethod()->GetRootNode());
	}

	constructorSet.clear();

	for (ObjectPtr m : classVar->GetClass()->GetExtend()->GetMethodSet()) {
		if (m->GetName() == m->GetOwner()->GetName()) {
			constructorSet.insert(m);
		}
	}

	for (ObjectPtr m : constructorSet) {
		classVar->Add(new Method("super", classVar, classVar, Scope::classVoid, m->GetMethod()->GetAttributeHash()))
			->GetMethod()->SetRootNode(m->GetMethod()->GetRootNode());
	}

	constructorSet.clear();
#endif
}

Void ScopeBuilder::_ForEachNode(NodePtr node, ScopePtr scope, ForEachNode callback, NodeID id) {

	if (id == kScriptNodeUnknown || (id != kScriptNodeUnknown && node->id == id)) {
		callback(node);
	}
	//if (node->typeNode) {
	//	if ((id == kScriptNodeUnknown || node->typeNode->id == kScriptNodeAnonymous) &&
	//		node->id != kScriptNodeVariable && node->id != kScriptNodeClass && node->id != kScriptNodeInterface
	//	) {
	//		if (node->typeNode->word == "A") {
	//			__asm int 3
	//		}
	//		callback(node->typeNode);
	//	}
	//}

	if (node->typeNode && node != node->typeNode) {
		if (node->lex->lex->id == kScriptLexNew) {
			_ForEachNode(node->typeNode, scope, callback, id);
		}
	}

	if (node->elseNode) {
		_ForEachNode(node->elseNode, scope, callback, id);
	}

	if (node->id == kScriptNodeClass ||
		node->id == kScriptNodeInterface ||
		node->id == kScriptNodeFunction ||
		node->id == kScriptNodeCondition ||
		node->id == kScriptNodeAnonymous ||
		node->id == kScriptNodeStatic
	) {
		if (node->var) {
			this->_Push(node->var);
		}
	}

	for (NodePtr n : node->argList) {
		_ForEachNode(n, scope, callback, id);
	}
	for (NodePtr n : node->blockList) {
		_ForEachNode(n, scope, callback, id);
	}
	for (NodePtr n : node->elseList) {
		_ForEachNode(n, scope, callback, id);
	}
	for (NodePtr n : node->forInfo.beginList) {
		_ForEachNode(n, scope, callback, id);
	}
	for (NodePtr n : node->forInfo.conditionList) {
		_ForEachNode(n, scope, callback, id);
	}
	for (NodePtr n : node->forInfo.nextList) {
		_ForEachNode(n, scope, callback, id);
	}

	if (node->id == kScriptNodeClass ||
		node->id == kScriptNodeInterface ||
		node->id == kScriptNodeFunction ||
		node->id == kScriptNodeCondition ||
		node->id == kScriptNodeAnonymous ||
		node->id == kScriptNodeStatic
	) {
		if (node->var) {
			this->_Pop();
		}
	}
}

Void ScopeBuilder::_Push(ScopePtr scope) {

	this->scopeStack.push_back(scope);
	this->scope = scope;
	this->lastStack.push_back(this->lastVar);
}

ScopePtr ScopeBuilder::_Pop(Void) {

	if (this->scopeStack.empty()) {
		return this->scope;
	}

	this->scopeStack.pop_back();
	this->scope = this->scopeStack.back();
	this->lastVar = this->lastStack.back();
	this->lastStack.pop_back();

	return this->scope;
}

Void ScopeBuilder::_BuildClassList(NodePtr node) {

	if (node->id == kScriptNodeClass ||
		node->id == kScriptNodeAnonymous ||
		node->id == kScriptNodeInterface
	) {
		this->classList.push_back(node);
	}

	for (NodePtr n : node->blockList) {
		this->_BuildClassList(n);
	}
}

Buffer ScopeBuilder::_GetNodePath(NodePtr node) {

	Stack<NodePtr> stack;
	Buffer path;
	NodePtr native;

	native = node;

	while (node->parent) {
		stack.push(node);
		node = node->parent;
	}

	while (!stack.empty()) {
		if (stack.top()->typeNode) {
			path += stack.top()->typeNode->word;
		}
		else {
			path += stack.top()->word;
		}
		stack.pop();
		if (!stack.empty()) {
			path += '/';
		}
		++native->depth;
	}

	return path;
}

LAME_END2