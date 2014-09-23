#include "CodeBuilder.h"

LAME_BEGIN2(Script)

CodeBuilder::~CodeBuilder() {
}

Void CodeBuilder::Build(SyntaxBuilderPtr nodeBuilder, ScopePtr rootScope) {

	NodePtr rootNode = nodeBuilder->GetRootNode();

	for (MethodPtr m : this->nodeWalker.GetMethodSet()) {

		if (!m->GetRootNode()) {
			continue;
		}

		for (NodePtr n : m->GetRootNode()->argList) {
			n->var->writes_ = 1;
		}
	}

	this->nodeWalker.Walk(this, this, rootNode, rootScope, this);
}

Void CodeBuilder::onBinary(NodePtr n) {

	ObjectPtr sourceVar;
	ObjectPtr leftVar;
	ObjectPtr rightVar;

	Bool wasItConst;
	Bool justSaved;

	LAME_ASSERT(n->lex->args == 2);

	this->GetWalker()->Test(n);

	leftVar  = this->GetWalker()->GetLeft();
	rightVar = this->GetWalker()->GetRight();

	if (n->lex->lex->IsLeft() && (leftVar->CheckModificator(Object::Modificator::Constant) ||
		leftVar->CheckModificator(Object::Modificator::Final))
	) {
		PostSyntaxError(n->lex->line, "Unable to apply (%s) to constant variable (%s)",
			n->word.data(), leftVar->GetName().data());
	}

	if (leftVar->CheckType(Object::Type::Variable)) {
		if (n->lex->lex->IsRight()) {
			if (!leftVar->writes_ && !leftVar->CheckModificator(Object::Modificator::Constant)) {
				PostSyntaxError(n->lex->line, "Variable (%s) might not have been initialized",
					leftVar->GetName().data());
			}
		}
	}

	if (rightVar->CheckType(Object::Type::Variable)) {
		if (!rightVar->writes_ && !rightVar->CheckModificator(Object::Modificator::Constant)) {
			PostSyntaxError(n->lex->line, "Variable (%s) might not have been initialized",
				rightVar->GetName().data());
		}
	}

	if (n->lex->lex->IsRight()) {

		sourceVar = leftVar->GetClass()->GetPriority() >= rightVar->GetClass()->GetPriority() ?
			leftVar : rightVar;

		if (leftVar->GetClass()->Hash() != rightVar->GetClass()->Hash()) {
			if (leftVar->GetClass()->Hash() != sourceVar->GetClass()->Hash()) {
				this->_Cast(leftVar->GetVariable(), sourceVar->GetClass());
			}
		}

		if (leftVar->GetClass()->Hash() != rightVar->GetClass()->Hash()) {
			if (rightVar->GetClass()->Hash() != sourceVar->GetClass()->Hash()) {
				this->_Cast(rightVar->GetVariable(), sourceVar->GetClass());
			}
		}
	}
	else {
		sourceVar = leftVar;

		if (leftVar->GetClass()->Hash() != rightVar->GetClass()->Hash()) {
			this->_Cast(rightVar->GetVariable(), sourceVar);
		}

		if (n->lex->lex->id == kScriptLexSet) {
			leftVar->GetNode()->wasItLeft = TRUE;
		}
	}

	wasItConst = FALSE;

	if (leftVar->CheckModificator(Object::Modificator::Constant) &&
		rightVar->CheckModificator(Object::Modificator::Constant)
	) {
		if (this->GetCalculator()->Compute(n, leftVar, rightVar)) {

			this->nodeList.pop_back();
			this->nodeList.pop_back();
				
			this->_Save(leftVar->GetNode());

			sourceVar = leftVar->GetNode()
				->var->GetVariable();

			wasItConst = TRUE;
		}
		else {
			goto _SaveNode;
		}
	}
	else {
	_SaveNode: justSaved = TRUE;
		this->_Save(n);
	}

	if (n->lex->lex->IsLogic()) {
		if (!leftVar->IsIntegerLike()) {
			PostSyntaxError(this->GetWalker()->GetNode()->lex->line,
				"Unable to apply binary operator to non-integer type (%s)",
				leftVar->GetClass()->GetName().data());
		}
		if (!rightVar->IsIntegerLike()) {
			PostSyntaxError(this->GetWalker()->GetNode()->lex->line,
				"Unable to apply binary operator to non-integer type (%s)",
				rightVar->GetClass()->GetName().data());
		}
	}

	switch (this->GetWalker()->GetNode()->lex->lex->id) {
	case kScriptLexMul:
	case kScriptLexMulSet:
	case kScriptLexDiv:
	case kScriptLexDivSet:
	case kScriptLexMod:
	case kScriptLexModSet:
	case kScriptLexSub:
	case kScriptLexSubSet:
		if ((!leftVar->GetClass()->IsIntegerLike() &&
			!leftVar->GetClass()->IsFloatLike()) ||
			(!rightVar->GetClass()->IsIntegerLike() &&
			!rightVar->GetClass()->IsFloatLike())
		) {
			PostSyntaxError(n->lex->line, "Unable to apply binary operator (%s) to non-number type (%s, %s)",
				n->word.data(), leftVar->GetClass()->GetName().data(), rightVar->GetClass()->GetName().data());
		}
		break;
	case kScriptLexAdd:
	case kScriptLexAddSet:
		if ((!leftVar->GetClass()->IsIntegerLike() &&
			!leftVar->GetClass()->IsFloatLike() &&
			!leftVar->GetClass()->IsStringLike()) ||
			(!rightVar->GetClass()->IsIntegerLike() &&
			!rightVar->GetClass()->IsFloatLike() &&
			!rightVar->GetClass()->IsStringLike())
		) {
			PostSyntaxError(n->lex->line, "Unable to apply binary operator (%s) to non-number or string type (%s, %s)",
				n->word.data(), leftVar->GetClass()->GetName().data(), rightVar->GetClass()->GetName().data());
		}
		break;
	case kScriptLexBitShiftL:
	case kScriptLexBitShiftSetL:
	case kScriptLexBitShiftR:
	case kScriptLexBitShiftSetR:
	case kScriptLexBitShiftU:
	case kScriptLexBitShiftSetU:
	case kScriptLexBitAnd:
	case kScriptLexBitAndSet:
	case kScriptLexBitXor:
	case kScriptLexBitXorSet:
	case kScriptLexBitOr:
	case kScriptLexBitOrSet:
		if (!leftVar->GetClass()->IsIntegerLike() &&
			!rightVar->GetClass()->IsIntegerLike()
		) {
			PostSyntaxError(n->lex->line, "Unable to apply binary operator (%s) to non-interger type (%s, %s)",
				n->word.data(), leftVar->GetClass()->GetName().data(), rightVar->GetClass()->GetName().data());
		}
		break;
	case kScriptLexSet:
		break;
	case kScriptLexBelow:
	case kScriptLexAbove:
	case kScriptLexBelowEqual:
	case kScriptLexAboveEqual:
		if ((!leftVar->GetClass()->IsIntegerLike() &&
			!leftVar->GetClass()->IsFloatLike() &&
			!leftVar->GetClass()->IsStringLike()) ||
			(!rightVar->GetClass()->IsIntegerLike() &&
			!rightVar->GetClass()->IsFloatLike() &&
			!rightVar->GetClass()->IsStringLike())
		) {
			PostSyntaxError(n->lex->line, "Unable to apply binary operator (%s) to non-comparable type (%s, %s)",
				n->word.data(), leftVar->GetClass()->GetName().data(), rightVar->GetClass()->GetName().data());
		}
		break;
	case kScriptLexEqual:
	case kScriptLexNotEqual:
		if ((!leftVar->GetClass()->IsIntegerLike() &&
			!leftVar->GetClass()->IsFloatLike() &&
			!leftVar->GetClass()->IsObjectLike()) ||
			(!rightVar->GetClass()->IsIntegerLike() &&
			!rightVar->GetClass()->IsFloatLike() &&
			!rightVar->GetClass()->IsObjectLike())
		) {
			PostSyntaxError(n->lex->line, "Unable to apply binary operator (%s) to non-comparable type (%s, %s)",
				n->word.data(), leftVar->GetClass()->GetName().data(), rightVar->GetClass()->GetName().data());
		}
		break;
	case kScriptLexAnd:
	case kScriptLexOr:
		if (!leftVar->GetClass()->IsBooleanLike() &&
			!rightVar->GetClass()->IsBooleanLike()
		) {
			PostSyntaxError(n->lex->line, "Unable to apply binary operator (%s) to non-boolean type (%s, %s)",
				n->word.data(), leftVar->GetClass()->GetName().data(), rightVar->GetClass()->GetName().data());
		}
		break;
	default:
		break;
	}

	if (n->lex->lex->IsBool()) {
		if (wasItConst) {
			if (sourceVar->GetVariable()->v.intValue) {
				this->GetWalker()->Push(sourceVar->Root()
					->Find("true")->GetVariable());
			}
			else {
				this->GetWalker()->Push(sourceVar->Root()
					->Find("false")->GetVariable());
			}
		}
		else {
			this->GetWalker()->Push(VariablePtr(Scope::classBoolean));
		}
	}
	else {
		this->GetWalker()->Push(sourceVar, !wasItConst && !justSaved);
	}

	if (n->lex->lex->IsLeft()) {
		if (leftVar->CheckType(Object::Type::Variable)) {
			leftVar->writes_++;
		}
		if (rightVar->CheckType(Object::Type::Variable)) {
			rightVar->reads_++;
		}
	}
	else {
		if (leftVar->CheckType(Object::Type::Variable)) {
			leftVar->reads_++;
		}
		if (rightVar->CheckType(Object::Type::Variable)) {
			rightVar->reads_++;
		}
	}
}

Void CodeBuilder::onUnary(NodePtr n) {

	this->GetWalker()->Test(n);

	VariablePtr leftVar = this->GetWalker()
		->GetLeft()->GetVariable();

	if (n->lex->lex->IsLeft() && leftVar->CheckModificator(Object::Modificator::Constant)) {
		PostSyntaxError(n->lex->line, "Unable to apply (%s) to constant variable (%s)",
			n->word.data(), leftVar->GetName().data());
	}

	if (n->lex->lex->id == kScriptLexCast) {
		if (!n->var->CheckType(Object::Type::Class)) {
			PostSyntaxError(n->lex->line, "Unable to apply strong cast to non-class object (%s)",
				n->var->GetName().data());
		}
		this->_StrongCast(leftVar, n->var->GetClass());
		this->GetWalker()->Push(VariablePtr(n->var->GetClass()));
	}
	else {
		if (leftVar->CheckModificator(Object::Modificator::Constant)) {
			if (this->GetCalculator()->Compute(n, leftVar)) {

				this->nodeList.pop_back();
				this->_Save(leftVar->GetNode());

				leftVar = leftVar->GetNode()
					->var->GetVariable();
			}
			else {
				goto _SaveNode;
			}
		}
		else {
		_SaveNode:
			this->_Save(n);
		}

		if (n->lex->lex->id == kScriptLexPostfixIncrement ||
			n->lex->lex->id == kScriptLexPostfixDecrement ||
			n->lex->lex->id == kScriptLexPrefixIncrement ||
			n->lex->lex->id == kScriptLexPrefixDecrement
		) {
			this->GetWalker()->Push(leftVar);
		}
		else {
			this->GetWalker()->Push(leftVar);
		}
	}

	switch (n->lex->lex->id) {
	case kScriptLexPostfixIncrement:
	case kScriptLexPrefixIncrement:
	case kScriptLexPostfixDecrement:
	case kScriptLexPrefixDecrement:
	case kScriptLexBitNot:
	case kScriptLexNot:
		if (!leftVar->CheckType(Object::Type::Variable) || !leftVar->GetClass()->IsIntegerLike()) {
			PostSyntaxError(n->lex->line, "Unable to apply (%s) to non-integer type",
				n->word.data());
		}
		break;
	case kScriptLexInstanceof:
		if (!leftVar->CheckType(Object::Type::Class) || !leftVar->GetClass()->IsObjectLike()) {
			PostSyntaxError(n->lex->line, "Unable to apply (%s) to non-integer type",
				n->word.data());
		}
		break;
	case kScriptLexReturn:
		if (!leftVar->CheckType(Object::Type::Variable)) {
			PostSyntaxError(n->lex->line, "Unable to apply (%s) non-variable object",
				n->word.data());
		}
		break;
	case kScriptLexUnaryMinus:
	case kScriptLexSub:
	case kScriptLexUnaryPlus:
	case kScriptLexAdd:
		if (!leftVar->CheckType(Object::Type::Variable) || (!leftVar->GetClass()->IsIntegerLike() &&
			!leftVar->GetClass()->IsFloatLike())
		) {
			PostSyntaxError(n->lex->line, "Unable to apply (%s) to non-number type",
				n->word.data());
		}
		break;
	default:
		break;
	}

	if (leftVar->CheckType(Object::Type::Variable)) {
		if (n->lex->lex->IsLeft()) {
			leftVar->writes_++;
		}
		else {
			leftVar->reads_++;
		}
	}
}

Void CodeBuilder::onTernary(NodePtr n) {

	VariablePtr var0;
	VariablePtr var1;

	this->GetWalker()->Test(n);

	VariablePtr backVar = this->GetWalker()
		->GetLeft()->GetVariable();

	this->GetWalker()->Run(n->blockList);
	var0 = this->GetWalker()->GetResult()
		->GetVariable();

	this->GetWalker()->Run(n->elseList);
	var1 = this->GetWalker()->GetResult()
		->GetVariable();

	if (var0->GetClass() != var1->GetClass()) {
		PostSyntaxError(n->lex->line, "Different result types in ternary expression (%s, %s)",
			var0->GetClass()->GetName().data(), var1->GetClass()->GetName().data());
	}
}

Void CodeBuilder::onCondition(NodePtr n) {

	VariablePtr var0;
	VariablePtr var1;

	switch (n->lex->lex->id) {
	case kScriptLexIf:

		this->GetWalker()->Run(n->argList, TRUE);

		/*	Test variable for boolean type
		and get result */

		var0 = this->GetWalker()->TestArgument(Object::classBoolean)
			->GetVariable();

		if (var0->CheckModificator(Object::Modificator::Constant)) {
			if (var0->v.intValue == TRUE) {
				this->GetWalker()->Run(n->blockList, TRUE);
				if (n->elseNode) {
					n->elseNode->blockList.clear();
				}
				n->finalNode = n;
			}
			else {
				n->blockList.clear();
				if (n->elseNode) {
					this->GetWalker()->Run(n->elseNode->blockList, TRUE);
				}
				n->finalNode = n->elseNode;
			}
		}
		else {
			this->GetWalker()->Run(n->blockList, TRUE);
			if (n->elseNode) {
				this->GetWalker()->Run(n->elseNode->blockList, TRUE);
			}
		}

		break;
	case kScriptLexSwitch:

		this->GetWalker()->Run(n->argList, TRUE);

		/*	Test variable for boolean type
		and get result */

		var0 = this->GetWalker()->TestArgument(Object::classBoolean)
			->GetVariable();

		for (NodePtr n2 : n->blockList) {

			this->GetWalker()->Run(n2->argList, TRUE);

			var1 = this->GetWalker()->TestArgument(NULL)
				->GetVariable();

			if (!var1->CheckModificator(Object::Modificator::Constant)) {
				PostSyntaxError(n2->lex->line, "Switch's case must be constant value/expression (%s)",
					var1->GetName().data());
			}

			if (var0->CheckModificator(Object::Modificator::Constant)) {
				if (!n->finalNode && var0->Equal(var1)) {
					n->finalNode = n2;
				}
			}

			this->GetWalker()->Run(n2->blockList, TRUE);
		}

		break;
	case kScriptLexElse:

		this->GetWalker()->Run(n->blockList, TRUE);

		break;
	case kScriptLexWhile:

		if ((n->flags & kScriptFlagWhileHasDo) != 0) {
			break;
		}

		this->GetWalker()->Run(n->argList, TRUE);

		/*	Test variable for boolean type
		and get result */

		var0 = this->GetWalker()->TestArgument(Object::classBoolean)
			->GetVariable();

		this->GetWalker()->Run(n->blockList, TRUE);

		break;
	case kScriptLexDo:

		this->GetWalker()->Run(n->elseNode->argList, TRUE);

		/*	Test variable for boolean type
		and get result */

		var0 = this->GetWalker()->TestArgument(Object::classBoolean)
			->GetVariable();

		if (var0->CheckModificator(Object::Modificator::Constant)) {
			if (!var0->GetVariable()->v.intValue) {
				n->finalNode = n;
			}
		}

		this->GetWalker()->Run(n->blockList, TRUE);

		break;
	case kScriptLexFor:

		/* Compile for's entry in current segment (doesn't metter) */
		this->GetWalker()->Run(n->forInfo.beginList, TRUE);
		this->GetWalker()->Run(n->forInfo.conditionList, TRUE);

		/*	Test variable for boolean type
		and get result */

		var0 = this->GetWalker()->TestArgument(Object::classBoolean)
			->GetVariable();

		this->GetWalker()->Run(n->blockList, TRUE);
		this->GetWalker()->Run(n->forInfo.nextList, TRUE);

		break;
	case kScriptLexTry:
	case kScriptLexCatch:
	case kScriptLexFinally:
		this->GetWalker()->Run(n->blockList, TRUE);
		break;
	default:
		break;
	}
}

Void CodeBuilder::onInvoke(NodePtr n) {

}

Void CodeBuilder::onNew(NodePtr n) {

	VariablePtr leftVar = VariablePtr(n->typeNode->var);
	VariablePtr resultVar = leftVar;

	if (!leftVar) {
		PostSyntaxError(n->lex->line, "Undeclared class (%s)",
			n->typeNode->word.data());
	}

	if (!leftVar->CheckType(Object::Type::Class)) {
		PostSyntaxError(n->lex->line, "Can't allocate memory for virtual object (%s%s)",
			leftVar->GetPath().data(), leftVar->GetName().data());
	}

	/* Constructor invocation */
	if ((n->flags & kScriptFlagInvocation) != 0) {

		ClassPtr initClass = leftVar->GetClass();
		ObjectPtr initMethod = NULL;
	}
	/* Array allocation */
	else {

		Uint32 offsetLength = 0;

		if (n->typeNode->lex->args != 1) {
			PostSyntaxError(n->lex->line, "Array size expression's result must be 1 value (%s)",
				n->lex->args);
		}

		offsetLength = leftVar->GetClass()->CheckModificator(Object::Modificator::Primitive) ?
			leftVar->GetClass()->Size() : leftVar->Scope::Size();

		Vector<VariablePtr> initList;

		for (NodePtr n2 : n->blockList) {
			if (n2->lex->lex->id != kScriptLexComma) {
				initList.push_back(VariablePtr(n2->var));
			}
		}

		Uint32 offset = 0;

		if (!n->typeNode->argList.empty()) {
			this->GetWalker()->Run(n->typeNode->argList, TRUE);
			if (!this->GetWalker()->GetResult()->GetClass()->IsIntegerLike()) {
				PostSyntaxError(n->lex->line, "Array size must be integer value (%s)",
					this->GetWalker()->GetResult()->GetClass()->GetName().data());
			}
		}
		else {
			for (VariablePtr v : initList) {
				v->SetAddress(offset);
				offset += offsetLength;
			}
		}

		resultVar = VariablePtr(Scope::classArray);
		Scope::classArray->SetTemplate(resultVar->GetClass());
	}

	this->_Save(n);

	this->GetWalker()->Push(resultVar,
		FALSE);
}

Void CodeBuilder::onReturn(NodePtr n) {

	MethodPtr methodVar;
	NodePtr methodNode;

	methodNode = n->parent;

	while (methodNode) {

		if (methodNode->var && methodNode->var->CheckType(Object::Type::Method)) {
			break;
		}

		methodNode = methodNode->parent;
	}

	if (!methodNode || !(methodVar = methodNode->var->GetMethod())) {
		PostSyntaxError(n->lex->line, "Push can only be in method or function", 0);
	}

	if (methodVar->GetReturnType()->IsVoid()) {
		if (n->lex->args) {
			PostSyntaxError(n->lex->line, "Void method %s(%s) can't return anything",
				methodVar->GetName().data(), methodVar->GetFormattedArguments().data());
		}
	}
	else {
		if (!n->lex->args) {
			PostSyntaxError(n->lex->line, "Non-void method %s(%s) must return (%s)", n->parent->word.data(),
				methodVar->GetFormattedArguments().data(), methodVar->GetReturnType()->GetName().data());
		}
	}

	if (n->lex->args > 0) {

		this->GetWalker()->Test(n);

		methodVar->returnVar = this->GetWalker()->GetLeft()
			->GetVariable();
	}

	if (methodVar->returnVar) {
		if (methodVar->returnVar->GetClass() != methodVar->GetReturnType()) {
			PostSyntaxError(n->lex->line, "Incompatible types (%s -> %s)", methodVar->returnVar->GetClass()
				->GetName().data(), methodVar->GetReturnType()->GetName().data());
		}
		this->_Cast(methodVar->returnVar, methodVar->GetReturnType());
	}

	this->_Save(n);
}

Void CodeBuilder::onIndex(NodePtr n) {

	VariablePtr leftVar;

	if (!n->var) {
		PostSyntaxError(n->lex->line, "Undeclared array (%s)",
			n->word.data());
	}

	if (!n->var->CheckType(Object::Type::Variable) || n->var->GetVariable()->GetVarType() != Variable::Var::Array) {
		PostSyntaxError(n->lex->line, "You can't assume indexing only to arrays (%s)",
			n->word.data());
	}

	n->lex->args = 1;

	this->GetWalker()->Test(n);

	leftVar = this->GetWalker()->GetLeft()
		->GetVariable();

	if (!leftVar->GetClass()->IsIntegerLike()) {
		PostSyntaxError(n->lex->line, "Array index must be integer value, found (%s)",
			leftVar->GetClass()->GetName().data());
	}

	this->GetWalker()->Push(
		VariablePtr(n->var->GetClass()));
}

static Bool _CheckObjectCast(ClassPtr left, ClassPtr right) {

	if (!left->GetExtend() || !right->GetExtend()) {
		return FALSE;
	}

	if (left == right || left->GetExtend()->GetClass() == right) {
		return TRUE;
	}

	for (ObjectPtr i : left->GetImplements()) {
		if (i->GetClass() == right) {
			return TRUE;
		}
	}

	ObjectPtr c = left->GetExtend()->GetClass()->GetExtend();

	while (c) {
		if (_CheckObjectCast(c->GetClass(), right)) {
			return TRUE;
		}
		c = c->GetClass()->GetExtend();
	}

	for (ObjectPtr c : left->GetImplements()) {
		if (_CheckObjectCast(c->GetClass(), right)) {
			return TRUE;
		}
	}

	return FALSE;
}

Void CodeBuilder::_Cast(VariablePtr var, ObjectPtr type) {

	ClassPtr left = var->GetClass();
	ClassPtr right = type->GetClass();

	if (left == right) {
		goto _CastOk;
	}

	if (var->CheckModificator(Object::Modificator::Constant) && var->GetName() == "null") {
		if (type->GetVariable()->GetVarType() == Variable::Var::Array ||
			type->GetVariable()->GetVarType() == Variable::Var::Object
		) {
			goto _CastOk;
		}
	}

	if (type->CheckType(Object::Type::Variable) &&
		var->CheckType(Object::Type::Variable)
	) {
		if (type->GetVariable()->GetVarType() == Variable::Var::Array &&
			var->GetVariable()->GetVarType() == Variable::Var::Array
		) {
			if (left == right) {
				goto _CastOk;
			}
		}
		else {
			if (type->GetVariable()->GetVarType() == Variable::Var::Array ||
				var->GetVariable()->GetVarType() == Variable::Var::Array
			) {
				goto _CastFail;
			}
		}
	}

	if (left->IsFloatLike()) {
		if (right->IsFloatLike() && right->GetPriority() > left->GetPriority()) {
			goto _CastOk;
		}
	}
	else if (left->IsIntegerLike()) {
		if (right->IsIntegerLike() && right->GetPriority() > left->GetPriority()) {
			goto _CastOk;
		}
		if (right->IsFloatLike() && right->GetPriority() > left->GetPriority()) {
			goto _CastOk;
		}
	}
	else if (left->IsBooleanLike()) {
		/* Uncastable */
	}
	else if (left->IsStringLike()) {
		if (right->IsIntegerLike() || right->IsFloatLike()) {
			goto _CastOk;
		}
	}
	else {
		if (_CheckObjectCast(left, right)) {
			goto _CastOk;
		}
	}

	if (var->GetVarType() == Variable::Var::Array) {
		if (type->CheckType(Object::Type::Variable)) {
			if (left != right) {
				goto _CastFail;
			}
		}
		else {
			if (!type->GetTemplate() || type->GetTemplate() != right) {
				goto _CastFail;
			}
		}
	}

	if (var->GetClass() == Scope::classArray) {
		if (!var->GetTemplate() || var->GetTemplate() != left) {
			goto _CastFail;
		}
		goto _CastOk;
	}

	if (var->CheckModificator(Object::Modificator::Constant)) {
		if (right->GetPriority() > left->GetPriority()) {
			goto _CastOk;
		}
	}

_CastFail:
	PostSyntaxError(this->GetWalker()->GetNode()->lex->line, "Unable to cast from (%s) to (%s)",
		left->GetName().data(), right->GetName().data());

_CastOk:
	if (left != right) {
		// cast var -> type
	}
}

Void CodeBuilder::_StrongCast(VariablePtr var, ClassPtr type) {

	ClassPtr left = type->GetClass();
	ClassPtr right = var->GetClass();

	if (left == right) {
		goto _CastOk;
	}

	if ((left->IsIntegerLike() || left->IsFloatLike()) &&
		(right->IsIntegerLike() || right->IsFloatLike())
	) {
		goto _CastOk;
	}
	else {
		if (left->IsObject() && right->IsObject()) {
			goto _CastOk;
		}
	}

	PostSyntaxError(this->GetWalker()->GetNode()->lex->line, "Unable to cast from (%s) to (%s)",
		right->GetName().data(), left->GetName().data());

_CastOk:
	if (left != right) {
		// cast var -> type
	}

	this->GetWalker()->Push(VariablePtr(type));
}

Void CodeBuilder::_Save(NodePtr n) {
	this->nodeList.push_back(n);
}

LAME_END2