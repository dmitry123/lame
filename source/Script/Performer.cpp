#include "Performer.h"
#include "Exception.h"
#include "Class.h"
#include "Method.h"
#include "Variable.h"
#include "Interface.h"
#include "GlobalScope.h"
#include "Internal.h"
#include "Array.h"

LAME_BEGIN2(Script)

static Map<LexID, Class::Operator> operatorMap =
{
	{ kScriptLexDefault, Class::Operator::Unkown },
	{ kScriptLexAdd, Class::Operator::Add },
	{ kScriptLexAddSet, Class::Operator::Add },
	{ kScriptLexSub, Class::Operator::Sub },
	{ kScriptLexSubSet, Class::Operator::Sub },
	{ kScriptLexMul, Class::Operator::Mul },
	{ kScriptLexMulSet, Class::Operator::Mul },
	{ kScriptLexDiv, Class::Operator::Div },
	{ kScriptLexDivSet, Class::Operator::Div },
	{ kScriptLexMod, Class::Operator::Mod },
	{ kScriptLexModSet, Class::Operator::Mod },
	{ kScriptLexAbove, Class::Operator::Above },
	{ kScriptLexEqual, Class::Operator::Equal },
	{ kScriptLexNotEqual, Class::Operator::NotEqual },
	{ kScriptLexSet, Class::Operator::Move },
	{ kScriptLexBitShiftR, Class::Operator::ShiftR },
	{ kScriptLexBitShiftSetR, Class::Operator::ShiftR },
	{ kScriptLexBitShiftL, Class::Operator::ShiftL },
	{ kScriptLexBitShiftSetL, Class::Operator::ShiftL },
	{ kScriptLexBitAnd, Class::Operator::BitAnd },
	{ kScriptLexBitAndSet, Class::Operator::BitAnd },
	{ kScriptLexBitOr, Class::Operator::BitOr },
	{ kScriptLexBitOrSet, Class::Operator::BitOr },
	{ kScriptLexBitXor, Class::Operator::BitXor },
	{ kScriptLexBitXorSet, Class::Operator::BitXor },
	{ kScriptLexBellowEqual, Class::Operator::BellowE },
	{ kScriptLexAboveEqual, Class::Operator::AboveE },
	{ kScriptLexAnd, Class::Operator::And },
	{ kScriptLexOr, Class::Operator::Or },
	{ kScriptLexIncrement, Class::Operator::Inc },
	{ kScriptLexDecrement, Class::Operator::Dec },
	{ kScriptLexSizeof, Class::Operator::Sizeof },
	{ kScriptLexBitNot, Class::Operator::BitNot },
	{ kScriptLexNot, Class::Operator::Not }
};

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

Void NodePerformer::Perform(NodeBuilderPtr builder) {

	if (!builder) {
		throw InvalidArgumentException("Builder mustn't be NULL");
	}

	this->builder = builder;

	_MoveNode(this->builder->GetRootNode());

	this->_ForEachNode([](NodePtr n) {
		if (n->lex->args && n->lex->args != n->lex->lex->args) {
			printf("%s(%d) ", n->word.data(), n->lex->args);
		}
		else {
			printf("%s ", n->word.data());
		}
	});

	this->_ForEachNode(_ForEachClassPrototype, kScriptNodeClass);
	this->_ForEachNode(_ForEachInterfacePrototype, kScriptNodeInterface);
	this->_ForEachNode(_ForEachInterfaceDeclare, kScriptNodeInterface);
	this->_ForEachNode(_ForEachClassDeclare, kScriptNodeClass);
	this->_ForEachNode(_ForEachConstDeclare, kScriptNodeDefault);
	this->_ForEachNode(_ForEachVariableDeclare, kScriptNodeVariable);
	this->_ForEachNode(_ForEachClassInherit, kScriptNodeClass);
	this->_ForEachNode(_ForEachInterfaceInherit, kScriptNodeInterface);

	this->_ForEachNode([](NodePtr n) {
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
	});

	this->Evaluate(this->builder->GetRootNode()->blockList);
}

Bool NodePerformer::Evaluate(NodeListRef nodeList) {

	VarList varStack;

	if (!this->builder) {
		return FALSE;
	}

	NodeList::iterator i = nodeList.begin();

	while (i != nodeList.end()) {

		NodePtr n = *i;

		if (n->id == kScriptNodeClass ||
			n->id == kScriptNodeInterface
		) {
			goto _Continue;
		}

		//if (n->id == kScriptNodeAlloc) {
		//	if (!n->argList.size()) {
		//		PostSyntaxError(n->lex->line, "Empty array's index", 0);
		//	}
		//	varStack.clear();
		//	this->Evaluate(n->argList, varStack);
		//	if (varStack.size() > 1) {
		//		PostSyntaxError(n->lex->line, "Unexcepted token in expression", 0);
		//	}
		//	else if (varStack.size() == 0) {
		//		PostSyntaxError(n->lex->line, "Invalid variable's expression", 0);
		//	}
		//	nodeStack.push_back({ n, varStack.back() });
		//}

		if (n->id == kScriptNodeInvoke ||
			n->id == kScriptNodeAlloc
		) {
			this->methodHash = 0;
			for (Uint32 i = 0; i < n->lex->args; i++) {
				n->argList.push_back(this->nodeStack.back().node);
				this->nodeStack.pop_back();
			}
		}
		
		if (n->lex->lex->IsUnknown() ||
			n->lex->lex->IsConst()
		) {
			nodeStack.push_back({ n, n->var });
		}
		else if (n->lex->lex->id == kScriptLexNew) {
			this->_EvaluateNew(n);
		}
		else if (
			n->lex->lex->IsMath() ||
			n->lex->lex->IsBool() ||
			n->lex->lex->id == kScriptLexDirected ||
			n->lex->lex->id == kScriptLexMediated
		) {
			if (n->lex->args == 1) {
				this->_EvaluateExpression1(n);
			} else if (n->lex->args == 2) {
				this->_EvaluateExpression2(n);
			} else {
				this->_EvaluateExpression0(n);
			}
		}

	_Continue:
		++i;
	}

	if (nodeStack.size() > 1 && 0) {
		PostSyntaxError(nodeStack.back().node->lex->line, "Unexcepted expression (%s)", nodeStack.back().node->word.data());
	}

	for (VarNode& n : nodeStack) {
		if (!n.var) {
			n.var = n.node->parent->GetScope()->GetVarScope()->Find(n.node->word);
			if (!n.var) {
				PostSyntaxError(n.node->lex->line, "Undeclared variable (%s)", n.node->word.data());
			}
			n.node->var = n.var;
		}
	}

	return TRUE;
}

Void NodePerformer::_EvaluateNew(NodePtr node) {

	ObjectPtr type;
	ObjectPtr result;

	this->_ReadArguments(node);

	type = FindClass(
		this->argsArray[0].node->parent,
		this->argsArray[0].node->word);

	if (type->CheckModificator(Class::Modificator::Primitive)) {

		VariablePtr var = this->argsArray[0].var->GetVariable();

		result = node->parent->GetScope()->GetTempScope()
			->Add(new Array("", node, type->GetClass(), var->GetInteger()));
	}
	else {
		result = node->parent->GetScope()->GetTempScope()
			->Add(new Variable("", type));

		type->GetClass()->New(result);

		NodePtr n;

		if ((n = result->GetClass()->GetNode()) != NULL) {

			n->var = result->GetVariable()->GetObject();

			for (NodePtr n : n->blockList) {
				if (n->id == kScriptNodeVariable && n->blockList.size()) {
					this->Evaluate(n->blockList);
				}
			}
		}
	}

	nodeStack.push_back({ node, result });
}

Void NodePerformer::_EvaluateExpression0(NodePtr n) {

}

Void NodePerformer::_EvaluateExpression1(NodePtr node) {

}

Void NodePerformer::_EvaluateExpression2(NodePtr n) {

	VarNodePtr r;
	VarNodePtr l;

	this->_ReadArguments(n);

	r = &this->argsArray[0];
	l = &this->argsArray[1];

	NodePtr right = r->node;
	NodePtr left = l->node;

	if (r->var) {
		right->var = r->var;
	}
	if (l->var) {
		left->var = l->var;
	}

	if (n->lex->lex->id == kScriptLexDirected ||
		n->lex->lex->id == kScriptLexMediated
	) {
		if (!left->var) {
			if (!(left->var = left->parent->GetScope()->GetVarScope()->Find(left->word))) {
				if (!(left->var = left->parent->GetScope()->GetClassScope()->Find(left->word))) {
					PostSyntaxError(left->lex->line, "Undeclared variable or class (%s)", left->word.data());
				}
			}
		}
		if (!left->var->CheckType(Class::Type::Variable)) {
			PostSyntaxError(left->lex->line, "Unable to apply directed selection to non-variable object (%s)", left->word.data());
		}
	}
	else {
		if (!left->var) {
			if (!(left->var = left->parent->GetScope()->GetVarScope()->Find(left->word))) {
				PostSyntaxError(left->lex->line, "Undeclared variable (%s)", left->word.data());
			}
		}
		if (!right->var) {
			if (!(right->var = right->parent->GetScope()->GetVarScope()->Find(right->word))) {
				PostSyntaxError(right->lex->line, "Undeclared variable (%s)", right->word.data());
			}
		}
	}

	ObjectPtr result = left->var;
	ObjectPtr with = right->var;

	if (n->lex->lex->id == kScriptLexDirected ||
		n->lex->lex->id == kScriptLexMediated
	) {
		Uint32 methodHash = 0;
		ClassPtr varClass;

		varClass = left->var->GetVariable()->GetObject();

		if (right->id == kScriptNodeInvoke ||
			right->id == kScriptNodeAlloc
		) {
			Vector<VariablePtr> varList;
			for (NodePtr n : right->argList) {
				varList.push_back(n->var->GetVariable());
			}
			methodHash = Method::ComputeInvokeHash(varList);
			result = varClass->FindMethod(right->word, methodHash);
			if (!result) {
				PostSyntaxError(n->lex->line, "Undeclared method (%s)", right->word.data());
			}
			result->GetMethod()->Invoke(this, varList);
		}
		else {
			if (!varClass) {
				PostSyntaxError(n->lex->line, "Null pointer (%s)", result->GetName().data());
			}
			result = varClass->FindVariable(right->word);
			if (!result) {
				PostSyntaxError(n->lex->line, "Undeclared field (%s)", right->word.data());
			}
		}

		if (result->CheckModificator(Class::Modificator::Private) ||
			result->CheckModificator(Class::Modificator::Protected)
		) {
			int a = 0;
			//PostSyntaxError(n->lex->line, "You can't get access to private/protected objects from this scope (%s)", right->word.data());
		}

		left->var = result;

		goto _SaveNode;
	}
	else {
		if (n->lex->lex->IsRight() && result->CheckType(Class::Type::Variable)) {

			result = left->parent->GetScope()->GetTempScope()
				->Add(new Variable("", left->var->GetClass(), left));

			left->var->Clone(result);
		}

		//if (right->var->GetName().length() > 0) {
		//	with = right->parent->GetScope()->GetTempScope()
		//		->Add(new Variable("", right->var->GetClass(), right));
		//	right->var->Clone(with);
		//}
	}

	try {
		Error e = left->var->GetClass()->Evaluate(operatorMap[n->lex->lex->id], result, with);

		if (e != Error::NoError) {
			PostSyntaxError(n->lex->line, "%s (%s)", e.GetDescription(), left->var->GetClass()->GetName().data());
		}
	}
	catch (ClassInvalidCastException) {

		PostSyntaxError(n->lex->line, "Unable to cast from (%s) to (%s)",
			result->GetClass()->GetName().data(),
			with->GetClass()->GetName().data());
	}

_SaveNode:
	if (n->lex->lex->IsRight()) {
		nodeStack.push_back({ left, result });
	}
}

Void NodePerformer::_ForEachNode(ForEachNode callback, NodeID id) {
	_ForEachNode(this->builder->GetRootNode(), callback, id);
}

Void NodePerformer::_ReadArguments(NodePtr node) {

	if (nodeStack.size() < node->lex->args) {
		PostSyntaxError(node->lex->line, "Operator \"%s\" requires %d arguments", node->word.data(), node->lex->args);
	}

	argsArray.clear();

	for (Uint32 i = 0; i < node->lex->args; i++) {
		this->argsArray.push_back(nodeStack.back());
		nodeStack.pop_back();
	}
}

Void NodePerformer::_ForEachClassPrototype(NodePtr n) {

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

Void NodePerformer::_ForEachInterfacePrototype(NodePtr n) {

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

Void NodePerformer::_ForEachClassDeclare(NodePtr n) {

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
			NodePerformer::FindClass(n, m->typeName);

		if (!typeClass) {
			PostSyntaxError(m->lex->line, "Undeclared type (%s)", m->typeName.data());
		}

		if (m->id == kScriptNodeFunction) {

			Vector<ClassPtr> methodParameters;

			for (NodePtr n : m->argList) {

				ObjectPtr parameterClass = 
					NodePerformer::FindClass(m->parent, n->typeName);

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

			NodePerformer::_ForEachNode(m, [](NodePtr n) {

				ObjectPtr typeClass =
					NodePerformer::FindClass(n->parent, n->typeName);

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

Void NodePerformer::_ForEachInterfaceDeclare(NodePtr n) {

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
			NodePerformer::FindClass(n, m->typeName);

		if (!typeClass) {
			PostSyntaxError(m->lex->line, "Undeclared type (%s)", m->word.data());
		}

		if (m->id == kScriptNodeFunction) {

			Vector<ClassPtr> methodParameters;

			for (NodePtr n : m->argList) {

				ObjectPtr parameterClass =
					NodePerformer::FindClass(m->parent, n->typeName);

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

Void NodePerformer::_ForEachClassInherit(NodePtr n) {

	Error error;
	ObjectPtr typeClass;

	if (n->extends.size() > 0) {
		if (!(typeClass = NodePerformer::FindClass(n->parent, n->extends))) {
			PostSyntaxError(n->lex->line, "Undeclared class (%s)", n->extends.data());
		}
		n->var->GetClass()->Extend(typeClass->GetClass());
	}
	else {
		n->var->GetClass()->Extend(GlobalScope::classObject);
	}

	for (BufferRefC b : n->implements) {
		if (!(typeClass = NodePerformer::FindClass(n->parent, b))) {
			PostSyntaxError(n->lex->line, "Undeclared interface (%s)", b.data());
		}
		if ((error = n->var->GetClass()->Implement(typeClass->GetInterface())) != Error::NoError) {
			PostSyntaxError(n->lex->line, "%s (%s)", error.GetDescription(), b.data());
		}
	}
}

Void NodePerformer::_ForEachInterfaceInherit(NodePtr n) {

	ObjectPtr typeClass;

	if (n->extends.size() > 0) {
		PostSyntaxError(n->lex->line, "Interfaces can't extend classes (%s)", n->extends.data());
	}

	n->var->GetInterface()->Extend(GlobalScope::classObject);

	for (BufferRefC b : n->implements) {
		if (!(typeClass = NodePerformer::FindClass(n->parent, n->extends))) {
			PostSyntaxError(n->lex->line, "Undeclared interface (%s)", b.data());
		}
		n->var->GetClass()->Implement(typeClass->GetInterface());
	}
}

Void NodePerformer::_ForEachConstDeclare(NodePtr n) {

	if (n->lex->lex->IsConst()) {
		NodePerformer::_DeclareConstant(n);
	}
}

Void NodePerformer::_ForEachVariableDeclare(NodePtr n) {

	if (n->parent->id != kScriptNodeEntry) {
		return;
	}

	ObjectPtr typeClass =
		NodePerformer::FindClass(n->parent, n->typeName);

	if (!typeClass) {
		PostSyntaxError(n->lex->line, "Undeclared type (%s)", n->word.data());
	}

	if (n->isArray) {
		n->var = n->parent->GetScope()->GetVarScope()->Add(
			new Array(n->word, n, typeClass));
	}
	else {
		n->var = n->parent->GetScope()->GetVarScope()->Add(
			new Variable(n->word, typeClass, n));
	}

	if (!n->var) {
		PostSyntaxError(n->lex->line, "Variable redeclaration (%s)", n->word.data());
	}
}

ObjectPtr NodePerformer::_Find(Object::Type type, NodePtr node, BufferRefC name) {

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
	PostSyntaxError(savedNode->lex->line, "Undeclared class, variable or method (%s)", name.data());

	return NULL;
}

Void NodePerformer::_DeclareConstant(NodePtr node) {

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

Void NodePerformer::_ForEachNode(NodePtr node, ForEachNode callback, NodeID id) {

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
