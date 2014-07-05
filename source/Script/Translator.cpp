#include "Translator.h"
#include "Exception.h"
#include "Class.h"
#include "Method.h"
#include "Variable.h"
#include "Interface.h"
#include "GlobalScope.h"
#include "Internal.h"
#include "Array.h"
#include "ScopeBuilder.h"

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

Void NodePerformer::Perform(NodeBuilderPtr builder) {
	this->builder = builder;
	this->Evaluate(builder->GetRootNode()->blockList);
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

	type = ScopeBuilder::_FindClass(
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
			PostSyntaxError(n->lex->line, "You can't get access to private/protected objects from this scope (%s)", right->word.data());
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
	}

	try {
		Error e = left->var->GetClass()->Evaluate(
			operatorMap[n->lex->lex->id], result, with);

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

Void NodePerformer::_ReadArguments(NodePtr node) {

	if (nodeStack.size() < node->lex->args) {
		PostSyntaxError(node->lex->line, "Operator \"%s\" requires %d arguments", node->word.data(), node->lex->args);
	}

	this->argsArray.clear();

	for (Uint32 i = 0; i < node->lex->args; i++) {
		this->argsArray.push_back(nodeStack.back());
		nodeStack.pop_back();
	}
}

LAME_END2
