#include "ScopeBuilder.h"
#include "Exception.h"
#include "Variable.h"
#include "Method.h"
#include "Interface.h"
#include "Class.h"
#include "Internal.h"

#include <functional>

LAME_BEGIN2(Script)

static Bool _MoveNode(NodePtr node) {

	Bool result = FALSE;

	/*	Get last element in stack. Now we're going to swap
		arguments parent with argument expression, it will help us
		to prevent different unsuitable situations in future, when we'll
		perform code. For example, if we have expression 'if(1)', then
		we have to cut it into 2 expressions and apply shunting yard for every,
		whereupon we'll swap our expressions and push in one stack. Final
		view should be : '1 if'. We've saved count of arguments in node 'new' */

	if (node->parent != NULL && node->parent->blockList.size() > 0 &&
		node->argList.size() > 0 && node->id != kScriptNodeFunction
	) {
		/*	Get element from parent's stack */
		auto position = std::find(
			node->parent->blockList.begin(),
			node->parent->blockList.end(), node);

		if (position == node->parent->blockList.end()) {
			PostSyntaxError(node->parent->lex->line, "Node (%s) hasn't been appended to his parent (%s)", node->word.data(), node->parent->word.data());
		}

		/*	Push argument list in parent's block stack */
		if (node->id != kScriptNodeVariable) {
			node->parent->blockList.insert(position,
				node->argList.begin(), node->argList.end());
		}
		else {
			node->parent->blockList.insert(position + 1,
				node->argList.begin(), node->argList.end());
		}

		node->argList.clear();

		result = TRUE;
	}

_Again:
	for (NodePtr n : node->blockList) {
		if (_MoveNode(n)) {
			goto _Again;
		}
	}

	return result;
}

Void ScopeBuilder::Build(NodeBuilderPtr nodeBuilder, ScopePtr rootScope) {

	if (!nodeBuilder) {
		throw Exception("Builder mustn't be NULL");
	}

	NodePtr rootNode = nodeBuilder->GetRootNode();

	this->_Push(rootScope);

	_MoveNode(nodeBuilder->GetRootNode());

	/*	Node trace (only for debugging) */

	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachNodeTrace, this), kScriptNodeUnknown);

	/*	Class/Interface declare can be divided into several blocks:
			1. Declaring class/interface prototypes (without variables, methods or classes)
			2. Apply class/interface modificators
			3. Declaring class/interface methods and variables */

	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachClassPrototype, this), kScriptNodeClass);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachModificatorSet, this), kScriptNodeClass);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachInterfacePrototype, this), kScriptNodeInterface);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachModificatorSet, this), kScriptNodeInterface);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachInterfaceDeclare, this), kScriptNodeInterface);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachClassDeclare, this), kScriptNodeClass);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachClassVariableDeclare, this), kScriptNodeVariable);

	/*	Class/Interface inheritance:
			1. We have to apply inheritance for classes
			2. Apply inheritance for interfaces */

	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachClassInherit, this), kScriptNodeClass);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachInterfaceInherit, this), kScriptNodeInterface);

	/*	Constant declare. That block allocate memory
		for constant variables and push it to root
		scope. */

	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachConstDeclare, this), kScriptNodeDefault);

	/*	Mathod/Variable declare can be divided into next blocks:
			1. Declare method (method block and it's arguments)
			2. Apply modificators for method (public, private, static etc)
			3. Now we can register methods (after apply modificators we can
				declare 'this' variable in non-static method, which parent is class)
			4. Declare variable (this block includes declaration and registration
				to avoid using variables before it's declaration)
			5. Now we can apply modificators for variables */

	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachMethodDeclare, this), kScriptNodeFunction);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachModificatorSet, this), kScriptNodeFunction);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachMethodRegister, this), kScriptNodeFunction);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachVariableDeclare, this), kScriptNodeUnknown);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachModificatorSet, this), kScriptNodeVariable);

	/*	Check for empty nodes */

	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachNodeFind, this), kScriptNodeUnknown);
}

Void ScopeBuilder::_ForEachNodeTrace(NodePtr n) {

	if (n->lex->args && n->lex->args != n->lex->lex->args) {
		printf("%s(%d) ", n->word.data(), n->lex->args);
	}
	else {
		printf("%s ", n->word.data());
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
	}
}

Void ScopeBuilder::_ForEachNodeFind(NodePtr n) {

	if (n->id != kScriptNodeClass && n->id != kScriptNodeInterface) {
		if (!n->var) {
			n->var = this->scope->Find(n->word);
		}
	}
}

Void ScopeBuilder::_ForEachClassPrototype(NodePtr n) {

	n->var = this->scope->Add(
		new Class(n->typeNode->word, this->scope));

	if (!n->var) {
		PostSyntaxError(n->lex->line, "Class redeclaration (%s)", n->typeNode->word.data());
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

	ClassPtr classVar;

	n->var = this->scope->Find(n->typeNode->word);
	classVar = n->var->GetClass();

	if (!n->var) {
		PostSyntaxError(n->lex->line, "Undeclared class (%s)", n->typeNode->word.data());
	}

	if (n->typeNode->templateNode) {
		ClassPtr classT = n->var->Scope::Add(this->scope->classObject->Clone(
			n->typeNode->templateNode->word, n->var))->SetModificator(Object::Modificator::Private)->GetClass();
		n->var->SetTemplateClass(classT);
	}
}

Void ScopeBuilder::_ForEachClassVariableDeclare(NodePtr n) {

	ObjectPtr typeClass;

	if (!(n->parent && n->parent->id == kScriptNodeClass)) {
		return;
	}

	if (!n->typeNode) {
		if (!(typeClass = scope->Find(n->word))) {
			PostSyntaxError(n->lex->line, "Undeclared variable (%s)", n->word.data());
		}
		n->var = typeClass;
		typeClass = typeClass->GetClass();
	}
	else {
		typeClass = scope->Find(n->typeNode->word);
	}

	if (!typeClass) {
		PostSyntaxError(n->lex->line, "Undeclared type (%s)", n->typeNode->word.data());
	}

	if ((n->flags & kScriptFlagArray) != 0) {
		LAME_TODO("Add array support!");
	}
	else if (!n->var) {
		n->var = scope->Add(new Variable(n->word, scope, typeClass->GetClass()));
	}

	if (!n->var) {
		PostSyntaxError(n->lex->line, "Variable redeclaration (%s)", n->word.data());
	}

	if (n->typeNode && n->typeNode->templateNode) {
		if (!typeClass->GetTemplateClass()) {
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
			typeClass->GetTemplateClass()->GetName(), n->var));
		n->var->SetTemplateClass(classT2);
	}
}

Void ScopeBuilder::_ForEachInterfaceDeclare(NodePtr n) {

	ClassPtr classVar;

	n->var = scope->Find(n->typeNode->word);
	classVar = n->var->GetClass();

	if (!n->var) {
		PostSyntaxError(n->lex->line, "Undeclared interface (%s)", n->word.data());
	}
}

Void ScopeBuilder::_ForEachClassInherit(NodePtr n) {

	ObjectPtr typeClass;

	if (n->extendNode) {
		if (!(typeClass = scope->Find(n->extendNode->word))) {
			PostSyntaxError(n->lex->line, "Undeclared class (%s)", n->extendNode->word.data());
		}
		n->var->GetClass()->Extend(typeClass->GetClass());
	}
	else {
		n->var->GetClass()->Extend(scope->classObject);
	}

	for (NodePtr n2 : n->implementNode) {
		if (!(typeClass = scope->Find(n2->word))) {
			PostSyntaxError(n->lex->line, "Undeclared interface (%s)", n2->word.data());
		}
		n->var->GetClass()->Implement(typeClass);
	}

	n->var->Flush();

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

	if (n->extendNode) {
		PostSyntaxError(n->lex->line, "Interfaces can't extend classes (%s)", n->extendNode->word.data());
	}

	for (NodePtr n2 : n->implementNode) {
		if (!(typeClass = n->var->Find(n2->word))) {
			PostSyntaxError(n->lex->line, "Undeclared interface (%s)", n2->word.data());
		}
		n->var->GetClass()->Implement(typeClass->GetInterface());
	}

	n->var->Flush();

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

		n->var = globalScope->Add(
			new Variable(n->word, globalScope, globalScope->classInt));

		n->var->GetVariable()->SetInteger(ParseIntValue(n->word.data()))
			->SetModificator(Class::Modificator::Final)
			->SetModificator(Class::Modificator::Constant);
	}
	else if (n->lex->lex->id == kScriptLexFloat) {

		n->var = globalScope->Add(
			new Variable(n->word, globalScope, globalScope->classFloat));

		n->var->GetVariable()->SetFloat(ParseFloatValue(n->word.data()))
			->SetModificator(Class::Modificator::Final)
			->SetModificator(Class::Modificator::Constant);
	}
	else if (n->lex->lex->id == kScriptLexString) {

		n->var = globalScope->Add(
			new Variable(n->word, globalScope, globalScope->classString));

		n->var->GetVariable()->SetString(ParseStringValue(n->word.data()))
			->SetModificator(Class::Modificator::Final)
			->SetModificator(Class::Modificator::Constant);
	}
}

Void ScopeBuilder::_ForEachMethodDeclare(NodePtr n) {

	ObjectPtr returnType;
	ObjectPtr methodObject;
	Vector<ClassPtr> methodAttributes;

	returnType = scope->Find(n->typeNode->word);

	if (!returnType) {
		PostSyntaxError(n->typeNode->lex->line, "Undeclared type (%s)", n->typeNode->word.data());
	}

	for (NodePtr n2 : n->argList) {

		ObjectPtr argType = scope->Find(n2->typeNode->word);

		if (!argType) {
			PostSyntaxError(n2->typeNode->lex->line, "Undeclared type (%s)", n2->typeNode->word.data());
		}

		methodAttributes.push_back(argType->GetClass());
	}

	methodObject = scope->Add(new Method(n->word, scope, ObjectPtr(scope), returnType, methodAttributes));

	if (!methodObject) {
		PostSyntaxError(n->lex->line, "Method redeclaration (%s)", n->word.data());
	}

	if (methodObject->GetName() == this->scope->GetName() && !returnType->IsVoid()) {
		PostSyntaxError(n->lex->line, "You can't declare custom constructor (%s) with another return type (%s)",
			methodObject->GetName().data(), returnType->GetName().data());
	}

	n->var = methodObject;
	methodObject->GetMethod()->SetRootNode(n);
	n->methodHash = methodObject->GetMethod()->GetInvokeHash();
	n->var->SetNode(n);
}

Void ScopeBuilder::_ForEachMethodRegister(NodePtr n) {

	ObjectPtr methodObject = n->var;

	if (!methodObject) {
		return;
	}

	if (n->parent->id == kScriptNodeClass ||
		n->parent->id == kScriptNodeInterface
	) {
		if (!methodObject->CheckModificator(Object::Modificator::Static)) {
			methodObject->Scope::Add(new Variable("this", methodObject, ClassPtr(this->scope)));
			if (ClassPtr(this->scope)->GetExtend()) {
				methodObject->Scope::Add(new Variable("super", methodObject, ClassPtr(this->scope)->GetExtend()->GetClass()));
			}
		}
	}

	if (!methodObject->GetMethod()->GetReturnType()->IsVoid()) {
		Bool isReturnFound = FALSE;
		for (NodePtr n : methodObject->GetNode()->blockList) {
			if (n->lex->lex->id == kScriptLexReturn) {
				isReturnFound = TRUE;
				n->lex->args = 1;
				break;
			}
		}
		if (!isReturnFound) {
			PostSyntaxError(n->lex->line, "Non-void method (%s) must return (%s)", methodObject->GetName().data(), methodObject->GetMethod()->GetReturnType()->GetName().data());
		}
	}
	else {
		for (NodePtr n : methodObject->GetNode()->blockList) {
			if (n->lex->lex->id == kScriptLexReturn) {
				n->lex->args = 0;
				break;
			}
		}
	}
}

Void ScopeBuilder::_ForEachVariableDeclare(NodePtr n) {

	ObjectPtr typeClass;

	if (n->id == kScriptNodeVariable) {

		if (n->parent && n->parent->id == kScriptNodeClass) {
			return;
		}

		if (!n->typeNode) {
			if (!(typeClass = scope->Find(n->word))) {
				PostSyntaxError(n->lex->line, "Undeclared variable (%s)", n->word.data());
			}
			n->var = typeClass;
			typeClass = typeClass->GetClass();
		}
		else {
			typeClass = scope->Find(n->typeNode->word);
		}

		if (!typeClass) {
			PostSyntaxError(n->lex->line, "Undeclared type (%s)", n->typeNode->word.data());
		}

		if ((n->flags & kScriptFlagArray) != 0) {
			LAME_TODO("Add array support!");
		}
		else if (!n->var) {
			n->var = scope->Add(new Variable(n->word, scope, typeClass->GetClass()));
		}

		if (!n->var) {
			PostSyntaxError(n->lex->line, "Variable redeclaration (%s)", n->word.data());
		}

		if (n->typeNode && n->typeNode->templateNode) {
			if (!typeClass->GetTemplateClass()) {
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
				typeClass->GetTemplateClass()->GetName(), n->var));
			n->var->SetTemplateClass(classT2);
		}
	}
	else if (n->id == kScriptNodeDefault) {
		if (!n->var && n->lex->lex->id == kScriptLexDefault) {
			n->var = scope->Find(n->word);
			if (!n->var) {
				PostSyntaxError(n->lex->line, "Undeclared variable (%s)", n->word.data());
			}
		}
	}
}

Void ScopeBuilder::_ForEachNode(NodePtr node, ScopePtr scope, ForEachNode callback, NodeID id) {

	if (id == kScriptNodeUnknown || id != kScriptNodeUnknown && node->id == id) {
		callback(node);
	}

	if (node->id == kScriptNodeClass ||
		node->id == kScriptNodeInterface ||
		node->id == kScriptNodeFunction
	)  {
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

	if (node->id == kScriptNodeClass ||
		node->id == kScriptNodeInterface ||
		node->id == kScriptNodeFunction
	)  {
		if (node->var) {
			this->_Pop();
		}
	}
}

Void ScopeBuilder::_Push(ScopePtr scope) {

	this->scopeStack.push_back(scope);
	this->scope = scope;
}

ScopePtr ScopeBuilder::_Pop(Void) {

	if (this->scopeStack.empty()) {
		return this->scope;
	}

	this->scopeStack.pop_back();
	this->scope = this->scopeStack.back();

	return this->scope;
}

LAME_END2