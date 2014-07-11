#include "ScopeBuilder.h"
#include "Exception.h"
#include "Variable.h"
#include "Method.h"
#include "AbstractClass.h"
#include "Interface.h"
#include "Class.h"
#include "GlobalScope.h"
#include "Array.h"
#include "Internal.h"

LAME_BEGIN2(Script)

static Bool _MoveNode(NodePtr node) {

	Bool result = FALSE;

	/* Get last element in stack. Now we're going to swap
	arguments parent with argument expression, it will help us
	to prevent different unsuitable situations in future, when we'll
	perform code. For example, if we have expression 'if(1)', then
	we have to cut it into 2 expressions and apply shunting yard for every,
	whereupon we'll swap our expressions and push in one stack. Final
	view should be : '1 if'. We've saved count of arguments in node 'new' */

	if (node->parent != NULL && node->parent->blockList.size() > 0 &&
		node->argList.size() > 0 && node->id != kScriptNodeFunction
	) {
		/* Get element from parent's stack */
		auto position = std::find(
			node->parent->blockList.begin(),
			node->parent->blockList.end(), node);

		/* Push argument list in parent's block stack */
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

Void ScopeBuilder::Build(NodeBuilderPtr nodeBuilder, ScopeControllerPtr scopeController) {

	NodePtr rootNode = nodeBuilder->GetRootNode();

	if (!nodeBuilder) {
		throw Exception("Builder mustn't be NULL");
	}

	_MoveNode(nodeBuilder->GetRootNode());

	this->_ForEachNode(rootNode, [](NodePtr n) {
		if (n->lex->args && n->lex->args != n->lex->lex->args) {
			printf("%s(%d) ", n->word.data(), n->lex->args);
		} else {
			printf("%s ", n->word.data());
		}
	}, kScriptNodeUnknown);

	this->_ForEachNode(rootNode, _ForEachClassPrototype, kScriptNodeClass);
	this->_ForEachNode(rootNode, _ForEachInterfacePrototype, kScriptNodeInterface);
	this->_ForEachNode(rootNode, _ForEachInterfaceDeclare, kScriptNodeInterface);
	this->_ForEachNode(rootNode, _ForEachClassDeclare, kScriptNodeClass);
	this->_ForEachNode(rootNode, _ForEachConstDeclare, kScriptNodeDefault);
	this->_ForEachNode(rootNode, _ForEachVariableDeclare, kScriptNodeVariable);
	this->_ForEachNode(rootNode, _ForEachClassInherit, kScriptNodeClass);
	this->_ForEachNode(rootNode, _ForEachInterfaceInherit, kScriptNodeInterface);
	this->_ForEachNode(rootNode, _ForEachVariableRegister, kScriptNodeDefault);

	this->_ForEachNode(rootNode, [](NodePtr n) {
		if (n->var) {
			if ((n->modificators & kScriptFlagFinal) != 0) {
				n->var->SetModificator(Class::Modificator::Final);
			}
			if ((n->modificators & kScriptFlagPublic) != 0) {
				n->var->SetModificator(Class::Modificator::Public);
			}
			if ((n->modificators & kScriptFlagPrivate) != 0) {
				n->var->SetModificator(Class::Modificator::Private);
			}
			if ((n->modificators & kScriptFlagProtected) != 0) {
				n->var->SetModificator(Class::Modificator::Protected);
			}
			if ((n->modificators & kScriptFlagStatic) != 0) {
				n->var->SetModificator(Class::Modificator::Static);
			}
			if ((n->modificators & kScriptFlagNative) != 0) {
				n->var->SetModificator(Class::Modificator::Native);
			}
		}
	}, kScriptNodeUnknown);
}

Void ScopeBuilder::_ForEachClassPrototype(NodePtr n) {

	ScopeControllerPtr scope;

	if ((scope = n->parent->GetScope()) != NULL) {
		n->var = scope->GetClassScope()->Add(new Class(n->typeName, n));
		if (!n->var) {
			PostSyntaxError(n->lex->line, "Class redeclaration (%s)", n->typeName.data());
		}
	}
	else {
		PostSyntaxError(n->lex->line, "Unable to declare class (%s) in that place", n->typeName.data());
	}
}

Void ScopeBuilder::_ForEachInterfacePrototype(NodePtr n) {

	ScopeControllerPtr scope;

	if ((scope = n->parent->GetScope()) != NULL) {
		n->var = scope->GetClassScope()->Add(new Interface(n->typeName, n));
		if (!n->var) {
			PostSyntaxError(n->lex->line, "Interface redeclaration (%s)", n->typeName.data());
		}
	}
	else {
		PostSyntaxError(n->lex->line, "Unable to declare class (%s) in that place", n->typeName.data());
	}
}

Void ScopeBuilder::_ForEachClassDeclare(NodePtr n) {

	ScopeControllerPtr scope;

	if ((scope = n->parent->GetScope()) != NULL) {
		n->var = scope->GetClassScope()->Find(n->typeName);
		if (!n->var) {
			PostSyntaxError(n->lex->line, "Undeclared class (%s)", n->typeName.data());
		}
	}
	else {
		PostSyntaxError(n->lex->line, "Unable to find class (%s) in that place", n->typeName.data());
	}

	for (NodePtr m : n->blockList) {

		ObjectPtr typeClass =
			ScopeBuilder::_FindClass(n, m->typeName);

		if (!typeClass && m->typeName == n->templates) {
			n->var->SetTemplateClass(NULL);
		}

#if 0 // Template type is NULL
		if (!typeClass) {
			PostSyntaxError(m->lex->line, "Undeclared type (%s)", m->typeName.data());
		}
#endif

		if (m->id == kScriptNodeFunction) {

			Vector<ClassPtr> methodParameters;

			for (NodePtr n : m->argList) {

				ObjectPtr parameterClass =
					ScopeBuilder::_FindClass(m->parent, n->typeName);

				if (!parameterClass) {
					PostSyntaxError(n->lex->line, "Undeclared type (%s)", n->typeName.data());
				}

				methodParameters.push_back(parameterClass->GetClass());
			}

			m->var = n->GetScope()->GetMethodScope()->Add(
				new Method(m->word, m, n->var, typeClass, methodParameters));

			if (!m->var) {
				PostSyntaxError(m->lex->line, "Method redeclaration (%s)", m->word.data());
			}

			if (m->isMethodImpl) {
				m->var->GetMethod()->SetRootNode(m);
			}

			ScopeBuilder::_ForEachNode(m, [](NodePtr n) {

				ObjectPtr typeClass =
					ScopeBuilder::_FindClass(n->parent, n->typeName);

				if (!typeClass) {
					PostSyntaxError(n->lex->line, "Undeclared type (%s)", n->word.data());
				}
				else if (!n->parent->GetScope()->GetVarScope()->Add(new Variable(n->word, typeClass, n))) {
					PostSyntaxError(n->lex->line, "Variable redeclaration (%s)", n->word.data());
				}

			}, kScriptNodeVariable);
		}
		else if (m->id == kScriptNodeVariable) {

			m->var = n->GetScope()->GetVarScope()->Add(
				new Variable(m->word, typeClass, m));

			if (!m->var) {
				PostSyntaxError(m->lex->line, "Variable redeclaration (%s)", m->word.data());
			}
		}
	}
}

Void ScopeBuilder::_ForEachInterfaceDeclare(NodePtr n) {

	ScopeControllerPtr scope;

	if ((scope = n->parent->GetScope()) != NULL) {
		n->var = scope->GetClassScope()->Find(n->typeName);
		if (!n->var) {
			PostSyntaxError(n->lex->line, "Undeclared interface (%s)", n->word.data());
		}
	}
	else {
		PostSyntaxError(n->lex->line, "Unable to find interface (%s) in that place", n->typeName.data());
	}

	for (NodePtr m : n->blockList) {

		ObjectPtr typeClass =
			ScopeBuilder::_FindClass(n, m->typeName);

		if (!typeClass) {
			PostSyntaxError(m->lex->line, "Undeclared type (%s)", m->word.data());
		}

		if (m->id == kScriptNodeFunction) {

			Vector<ClassPtr> methodParameters;

			for (NodePtr n : m->argList) {

				ObjectPtr parameterClass =
					ScopeBuilder::_FindClass(m->parent, n->typeName);

				if (!parameterClass) {
					PostSyntaxError(n->lex->line, "Undeclared type (%s)", n->typeName.data());
				}

				methodParameters.push_back(parameterClass->GetClass());
			}

			m->var = n->GetScope()->GetMethodScope()->Add(
				new Method(m->word, m, n->var, typeClass, methodParameters));

			if (!m->var) {
				PostSyntaxError(m->lex->line, "Method redeclaration (%s)", m->word.data());
			}
			else if (m->isMethodImpl) {
				PostSyntaxError(m->lex->line, "Interfaces mustn't have implemented methods (%s)", m->word.data());
			}
		}
		else if (m->id == kScriptNodeVariable) {
			PostSyntaxError(m->lex->line, "You can't store variables in interfaces", m->word.data());
		}
	}
}

Void ScopeBuilder::_ForEachClassInherit(NodePtr n) {

	Error error;
	ObjectPtr typeClass;

	if (n->extends.size() > 0) {
		if (!(typeClass = ScopeBuilder::_FindClass(n->parent, n->extends))) {
			PostSyntaxError(n->lex->line, "Undeclared class (%s)", n->extends.data());
		}
		n->var->GetClass()->Extend(typeClass->GetClass());
	}
	else {
		n->var->GetClass()->Extend(GlobalScope::classObject);
	}

	for (BufferRefC b : n->implements) {
		if (!(typeClass = ScopeBuilder::_FindClass(n->parent, b))) {
			PostSyntaxError(n->lex->line, "Undeclared interface (%s)", b.data());
		}
		if ((error = n->var->GetClass()->Implement(typeClass->GetInterface())) != Error::NoError) {
			PostSyntaxError(n->lex->line, "%s (%s)", error.GetDescription(), b.data());
		}
	}
}

Void ScopeBuilder::_ForEachInterfaceInherit(NodePtr n) {

	ObjectPtr typeClass;

	if (n->extends.size() > 0) {
		PostSyntaxError(n->lex->line, "Interfaces can't extend classes (%s)", n->extends.data());
	}

	n->var->GetInterface()->Extend(GlobalScope::classObject);

	for (BufferRefC b : n->implements) {
		if (!(typeClass = ScopeBuilder::_FindClass(n->parent, n->extends))) {
			PostSyntaxError(n->lex->line, "Undeclared interface (%s)", b.data());
		}
		n->var->GetClass()->Implement(typeClass->GetInterface());
	}
}

Void ScopeBuilder::_ForEachConstDeclare(NodePtr n) {

	if (n->lex->lex->IsConst()) {
		ScopeBuilder::_DeclareConstant(n);
	}
}

Void ScopeBuilder::_ForEachVariableDeclare(NodePtr n) {

	ObjectPtr templateClass;

	if (n->parent->id != kScriptNodeEntry) {
		return;
	}

	ObjectPtr typeClass =
		ScopeBuilder::_FindClass(n->parent, n->typeName);

	if (!typeClass) {
		PostSyntaxError(n->lex->line, "Undeclared type (%s)", n->typeName.data());
	}

	if (n->isArray) {
		n->var = n->parent->GetScope()->GetVarScope()->Add(
			new Array(n->word, n, typeClass));
	}
	else {
		n->var = n->parent->GetScope()->GetVarScope()->Add(
			new Variable(n->word, typeClass, n));
	}

	if (n->templates.length() > 0) {
		if (!(templateClass = ScopeBuilder::_FindClass(n->parent, n->templates))) {
			PostSyntaxError(n->lex->line, "Undeclared class (%s)", n->typeName.data());
		}
		else if (templateClass->CheckModificator(Class::Modificator::Primitive)) {
			PostSyntaxError(n->lex->line, "Template class mustn't be primitive (%s)", n->typeName.data());
		}
		n->var->SetTemplateClass(templateClass->GetClass());
	}

	if (!n->var) {
		PostSyntaxError(n->lex->line, "Variable redeclaration (%s)", n->word.data());
	}
}

Void ScopeBuilder::_ForEachVariableRegister(NodePtr n) {

	if (!n->var && n->lex->lex->id == kScriptLexDefault) {

		n->var = _FindVariable(n->parent, n->word);

		if (!n->var) {
#if 0
			PostSyntaxError(n->lex->line, "Undeclared variable (%s)", n->word.data());
#endif
		}
	}
}

ObjectPtr ScopeBuilder::_FindClass(NodePtr node, BufferRefC name) {
	return _Find(Object::Type::Class, node, name);
}

ObjectPtr ScopeBuilder::_FindVariable(NodePtr node, BufferRefC name) {
	return _Find(Object::Type::Variable, node, name);
}

ObjectPtr ScopeBuilder::_Find(Object::Type type, NodePtr node, BufferRefC name) {

	NodePtr savedNode;
	ObjectPtr object;

	savedNode = node;

	if (node->var == NULL && node->id != kScriptNodeEntry) {
		goto __ClassNotFound;
	}

	while (node != NULL) {

		ScopeControllerPtr scope = node->GetScope();

		if (scope == NULL) {
			PostSyntaxError(node->lex->line, "Node (%s) hasn't scope, its impossible", node->word.data());
		}

		if ((object = scope->Find(type, name.data())) != NULL) {
			return object;
		}

		node = node->parent;
	}

__ClassNotFound:
#if 0
	PostSyntaxError(savedNode->lex->line, "Undeclared class, variable or method (%s)", name.data());
#endif

	return NULL;
}

Void ScopeBuilder::_DeclareConstant(NodePtr node) {

	GlobalScope globalScope;

	if ((node->var = globalScope->GetTempScope()->Find(node->word))) {
		return;
	}

	if (node->lex->lex->id == kScriptLexInt) {

		node->var = globalScope->GetTempScope()->Add(
			new Variable(node->word, GlobalScope::classInt));

		node->var->GetVariable()->SetInteger(ParseIntValue(node->word.data()))
			->SetModificator(Class::Modificator::Final);
	}
	else if (node->lex->lex->id == kScriptLexFloat) {

		node->var = globalScope->GetTempScope()->Add(
			new Variable(node->word, GlobalScope::classFloat));

		node->var->GetVariable()->SetFloat(ParseFloatValue(node->word.data()))
			->SetModificator(Class::Modificator::Final);
	}
	else if (node->lex->lex->id == kScriptLexString) {

		node->var = globalScope->GetTempScope()->Add(
			new Variable(node->word, GlobalScope::classString));

		node->var->GetVariable()->SetString(ParseStringValue(node->word.data()))
			->SetModificator(Class::Modificator::Final);
	}
}

Void ScopeBuilder::_ForEachNode(NodePtr node, ForEachNode callback, NodeID id) {

	if (id == kScriptNodeUnknown || id != kScriptNodeUnknown && node->id == id) {
		callback(node);
	}

	for (NodePtr n : node->argList) {
		_ForEachNode(n, callback, id);
	}

	for (NodePtr n : node->blockList) {
		_ForEachNode(n, callback, id);
	}
}

LAME_END2