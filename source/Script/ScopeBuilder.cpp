#include "ScopeBuilder.h"
#include "Exception.h"
#include "Variable.h"
#include "Method.h"
#include "Interface.h"
#include "Class.h"
#include "Internal.h"

#include <float.h>

LAME_BEGIN2(Script)

static Bool _MoveNode(NodePtr node, Bool isMoveConstruciton = FALSE) {

	Bool result = FALSE;

	/*	Get last element in stack. Now we're going to swap
		arguments parent with argument expression, it will help us
		to prevent different unsuitable situations in future, when we'll
		perform code. For example, if we have expression 'if(1)', then
		we have to cut it into 2 expressions and apply shunting yard for every,
		whereupon we'll swap our expressions and push in one stack. Final
		view should be : '1 if'. We've saved count of arguments in node 'new' */

	if (node->id == kScriptNodeFunction ||
		(node->id == kScriptNodeCondition && !isMoveConstruciton) ||
		node->lex->lex->id == kScriptLexCatch
	) {
		goto _Seek;
	}

	if (node->parent != NULL &&
		node->parent->blockList.size() > 0 &&
		node->argList.size() > 0
	) {
		Deque<NodePtr>* nodeStack = NULL;

		/*	Get element from parent's stack */
		auto position = std::find(
			node->parent->blockList.begin(),
			node->parent->blockList.end(), node);

		if (position == node->parent->blockList.end()) {
			position = std::find(node->parent->argList.begin(), node->parent->argList.end(), node);
			if (position == node->parent->argList.end()) {
				PostSyntaxError(node->parent->lex->line, "Node (%s) hasn't been appended to his parent (%s)", node->word.data(), node->parent->word.data());
			}
			else {
				nodeStack = &node->parent->argList;
			}
		}
		else {
			nodeStack = &node->parent->blockList;
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
	if (node->id == kScriptNodeCondition) {
		for (NodePtr n : node->argList) {
			if (_MoveNode(n, isMoveConstruciton)) {
				goto _Seek;
			}
		}
	}

_Again:
	for (NodePtr n : node->blockList) {
		if (_MoveNode(n, isMoveConstruciton)) {
			goto _Again;
		}
	}

	return result;
}

ScopeBuilder::ScopeBuilder() {
	this->rootConstruction = new Class("$", NULL);
	this->lastVar = this->rootConstruction;
}

ScopeBuilder::~ScopeBuilder() {
	delete this->rootConstruction;
}

Void ScopeBuilder::Build(NodeBuilderPtr nodeBuilder, ScopePtr rootScope) {

	NodePtr rootNode = nodeBuilder->GetRootNode();

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
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachModificatorSet, this), kScriptNodeClass);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachClassPrototype, this), kScriptNodeAnonymous);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachInterfacePrototype, this), kScriptNodeInterface);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachModificatorSet, this), kScriptNodeInterface);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachInterfaceDeclare, this), kScriptNodeInterface);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachClassDeclare, this), kScriptNodeClass);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachClassDeclare, this), kScriptNodeAnonymous);
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

	/*	Mathod/Variable declare can be divided onto next blocks:
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
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachConstruction, this), kScriptNodeCondition);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachVariableDeclare, this), kScriptNodeUnknown);
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachModificatorSet, this), kScriptNodeVariable);

	/*	Check for empty nodes */

	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachNodeFind, this), kScriptNodeUnknown);
    
    /*  Check every class/interface inheritance lexes */
    
    this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachCheckInheritance, this), kScriptNodeInterface);
    this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachCheckInheritance, this), kScriptNodeClass);
    this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachCheckInheritance, this), kScriptNodeAnonymous);
    
	/*	Now we can move all other nodes which was in language constructions. That
		mean that we've allocated memory for there variables in there scopes
		after what move it's variables in parent scope to avoid variable redeclaration
		and now we can use next construction :
		'for (int i = 0; ...)' { ... }' and 'i' variable
		will be allocated into 'for' scope and ordered expression will have next
		view : 'i 0 = ... for' and we won't have any conflicts with other language
		constructions. */

	_MoveNode(rootNode, TRUE);

	/*	Node trace (only for debugging) */
    
	this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachNodeTrace, this), kScriptNodeUnknown);
    this->_ForEachNode(rootNode, rootScope, ForEachNode(&ScopeBuilder::_ForEachNodeFlush, this), kScriptNodeUnknown);
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
		if ((n->flags & kScriptFlagAbstract) != 0) {
			n->var->SetModificator(Class::Modificator::Abstract);
            ObjectPtr(n->var->GetParent())->SetModificator(Class::Modificator::Abstract);
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

	if (n->id == kScriptNodeAnonymous) {
		n->var = this->scope->Add(
			new Class("", this->scope));
	}
	else{
		n->var = this->scope->Add(
			new Class(n->typeNode->word, this->scope));
	}

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
	ObjectPtr classExtend;

	if (!n->var) {
		n->var = this->scope->Find(n->typeNode->word);
	}

	classVar = n->var->GetClass();

	if (!n->var) {
		PostSyntaxError(n->lex->line, "Undeclared class (%s)", n->typeNode->word.data());
	}

	if (n->id == kScriptNodeAnonymous) {
		if (!(classExtend = this->scope->Find(n->word))) {
			PostSyntaxError(n->lex->line, "Undeclared class (%s)", n->word.data());
		}
		classVar->Extend(classExtend);
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
        n->var = scope->Add(new Variable(n->word, scope, typeClass->GetClass(), TRUE));
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

	n->var = scope->Find(n->typeNode->word);

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

		Sint64 intValue = ParseIntValue(n->word.data());
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
        
        Float64 floatValue = ParseFloatValue(n->word.data());
        ClassPtr floatClass;
        
        if (floatValue >= FLT_MIN && floatValue <= FLT_MAX) {
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

		n->var->GetVariable()->SetString(ParseStringValue(n->word.data()))
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
		PostSyntaxError(n->lex->line, "Method redeclaration (%s.%s)",
			scope->GetName().data(), n->word.data());
	}

	if (methodObject->GetName() == this->scope->GetName() && !returnType->IsVoid()) {
		PostSyntaxError(n->lex->line, "You can't declare custom constructor (%s) with another return type (%s)",
			methodObject->GetName().data(), returnType->GetName().data());
	}
    
    if ((n->flags & kScriptFlagImplemented) != 0) {
        methodObject->GetMethod()->SetRootNode(n);
    }

	n->var = methodObject;
	n->methodHash = methodObject->GetMethod()->GetInvokeHash();
	n->var->SetNode(n);
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
            n->var = scope->Add(new Variable(n->word, scope, typeClass->GetClass(), TRUE));
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
				auto i = std::find(n->parent->blockList.begin(), n->parent->blockList.end(), n);
				if (i != n->parent->blockList.end()) {
					if ((*(i + 1))->lex->lex->id != kScriptLexDirected) {
						goto _UndeclaredVariable;
					}
				}
				else {
				_UndeclaredVariable:
					PostSyntaxError(n->lex->line, "Undeclared variable (%s)", n->word.data());
				}
			}
		}
	}
}

Void ScopeBuilder::_ForEachConstruction(NodePtr n) {

	if (!n->var) {

		if (n->lex->lex->id == kScriptLexElse) {
			if (!lastVar->GetNode() || lastVar->GetNode()->lex->lex->id != kScriptLexIf) {
				PostSyntaxError(n->lex->line, "Else must have If block (%s)", lastVar->GetName().data());
			}
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
		}

		if (!n->parent->var) {
			PostSyntaxError(n->lex->line, "Language construction in illegal place (%s)", n->parent->word.data());
		}

		do {
			n->var = n->parent->var->Add(new Class("", this->scope));
		} while (!n->var);

		n->var->SetNode(n);
		n->parent->var->GetConditionSet().insert(n->var);
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
        n->var->Flush();
    }
}

Void ScopeBuilder::_ForEachNode(NodePtr node, ScopePtr scope, ForEachNode callback, NodeID id) {

	if (id == kScriptNodeUnknown || (id != kScriptNodeUnknown && node->id == id)) {
		callback(node);
	}
    
	if (node->id == kScriptNodeClass ||
		node->id == kScriptNodeInterface ||
		node->id == kScriptNodeFunction ||
		node->id == kScriptNodeCondition ||
		node->id == kScriptNodeAnonymous
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
		node->id == kScriptNodeFunction ||
		node->id == kScriptNodeCondition ||
		node->id == kScriptNodeAnonymous
	)  {
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

LAME_END2