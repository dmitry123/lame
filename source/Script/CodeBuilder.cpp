#include "CodeBuilder.h"

LAME_BEGIN2(Script)

CodeBuilder::~CodeBuilder() {

	for (CodeNodePtr n : this->codeList) {
		delete n;
	}
}

Void CodeBuilder::Build(SyntaxBuilderPtr nodeBuilder, ScopePtr rootScope) {

	NodePtr rootNode = nodeBuilder->GetRootNode();

	this->currentMethod = NULL;
	this->currentCode = NULL;
	this->lastResult = NULL;
	this->rememberedInvoke = NULL;
	this->currentNode = NULL;
	this->lastNode = NULL;
	this->lastSelection = NULL;
	this->wasItCondition = FALSE;

	this->_ForEachClass(rootScope);
	this->_ForEachMethod(rootScope);

	for (ObjectPtr i : this->methodList) {

		NodePtr n = i->GetMethod()->GetRootNode();

		this->codeList.push_back(
			new CodeNode(i->GetMethod()));

		this->currentNode = n;
		this->currentMethod = this->codeList.back();

		for (NodePtr n2 : n->argList) {
			if (!n2->var->CheckType(Object::Type::Variable)) {
				PostSyntaxError(n->lex->line, "Argument must be variable (%s)", n2->var->GetName().data());
			}
			n2->var->writes_ = 1;
		}

		ObjectPtr thisVar = NULL;

		this->_Run(i->GetMethod()->GetRootNode()->blockList);

		if (!i->GetMethod()->GetReturnType()->IsVoid()) {
			if (!i->GetMethod()->returnVar) {
				PostSyntaxError(n->lex->line, "Non-void method %s(%s) must return (%s)", i->GetName().data(),
					i->GetMethod()->GetFormattedArguments().data(), i->GetMethod()->GetReturnType()->GetName().data());
			}
		}
	}
}

Void CodeBuilder::_Run(NodeListRef nodeList, Bool makeBackup) {

	ObjectPtr var0;
	StackVar stackBackup;

	if (makeBackup) {
		stackBackup = this->variableStack;
	}

	Uint32 listLength = (Uint32)this->nodeList.size();

	for (NodePtr n : nodeList) {

		if (n->id == kScriptNodeClass ||
			n->id == kScriptNodeInterface ||
			n->id == kScriptNodeFunction
		) {
			continue;
		}

		this->currentNode = n;

		if (n->var && n->var->CheckType(Object::Type::Method)) {
			n->var->SetNode(n);
		}

		if (this->rememberedInvoke) {
			if (n->lex->lex->id != kScriptLexDirected) {
				this->_Invoke(this->rememberedInvoke);
			}
			this->rememberedInvoke = NULL;
		}

		try {
			if (n->lex->lex->id == kScriptLexReturn) {
				this->_Return(n);
			}
			else if (n->id == kScriptNodeCondition) {
				this->_Condition(n);
			}
			else if (n->id == kScriptNodeInvoke) {
				this->_Invoke(n);
				if (this->rememberedInvoke) {
					goto _SaveNode;
				}
			}
			else if (n->lex->lex->id == kScriptLexSemicolon) {
				this->_Finish(n);
			}
			else if (n->lex->lex->id == kScriptLexNew) {
				this->_New(n);
			}
			else if (n->lex->lex->id == kScriptLexDirected) {
				this->_Selection(n);
			}
			else if (n->id == kScriptNodeAlloc) {
				if (n->parent->id != kScriptNodeClass) {
					this->_Array(n);
				}
				else {
					this->_Invoke(n);
				}
			}
			else if (n->lex->lex->IsUnknown() || n->lex->lex->IsConst()) {
			_SaveNode:
				if (!n->var) {
					this->variableStack.GetNodeList().push_back(n);
					this->variableStack.GetVarList().push_back(NULL);
				} else {
					this->variableStack.Push(VariablePtr(n->var));
				}
				this->_Save(n);
			}
			else if (n->lex->lex->IsMath() || n->lex->lex->IsBool()) {
				if (n->lex->args == 1) {
					this->_Unary(n);
				}
				else if (n->lex->args == 2) {
					this->_Binary(n);
				}
				else if (n->lex->args == 3) {
					this->_Ternary(n);
				}
			}

			this->lastNode = n;
		}
		catch (SyntaxException& e) {
			throw SyntaxException(e.Line(), e.GetErrorBuffer());
		}
		catch (ScriptException& e) {
			throw SyntaxException(n->lex->line, e.GetErrorBuffer());
		}
	}

	if (this->variableStack.Size()) {
		this->lastResult = this->variableStack.Back()
			->GetVariable();
	}

	if (makeBackup) {
		this->variableStack = stackBackup;
	}

	if (nodeList.empty()) {
		return;
	}
	nodeList.clear();

	auto it = this->nodeList.begin();

	if (!listLength) {
		it = this->nodeList.begin();
	}

	if (this->nodeList.size() == listLength) {
		listLength = 0;
	}

	it += listLength;

	for (Uint32 i = 0; i < this->nodeList.size() - listLength; i++) {

		NodePtr n = *(it + i);

		switch (n->lex->lex->id) {
		case kScriptLexWhile:
			if (n->argList.size() != 1) {
				break;
			}
			if ((var0 = n->argList.back()->var) &&
				var0->CheckModificator(Object::Modificator::Constant)
			) {
				if (!var0->GetVariable()->v.intValue) {
					continue;
				}
			}
			break;
		default:
			if (!n->finalNode) {
				break;
			}
			for (NodePtr n2 : n->finalNode->blockList) {
				if (n->lex->lex->id == kScriptLexSwitch &&
					n2->lex->lex->id == kScriptLexBreak
				) {
					continue;
				}
				nodeList.push_back(n2);
			}
			continue;
		}

		nodeList.push_back(n);
	}

	this->wasItCondition = FALSE;
	
	listLength = (Sint32)this->nodeList.size() - (Sint32)listLength;

	while (listLength--) {
		this->nodeList.pop_back();
	}
}

Void CodeBuilder::_Read(NodePtr n, VariablePtr& left, VariablePtr& right) {

	if (this->variableStack.Size() < n->lex->args || !n->lex->args) {
		PostSyntaxError(n->lex->line, "Operator \"%s\" requires %d arguments",
			n->word.data(), n->lex->args);
	}

	if (n->lex->args > 1) {

		right = this->variableStack.Back()
			->GetVariable();

		this->variableStack.GetVarList().pop_back();

		if (n->lex->lex->id != kScriptLexDirected) {
			if (!right) {
				PostSyntaxError(n->lex->line, "Undeclared variable (%s)",
					this->variableStack.GetNodeList().back()->word.data());
			}
			this->variableStack.GetNodeList().pop_back();
		}
	}

	left = this->variableStack.Back()
		->GetVariable();

	this->variableStack.GetVarList().pop_back();

	if (!left) {
		PostSyntaxError(n->lex->line, "Undeclared variable (%s)",
			this->variableStack.GetNodeList().back()->word.data());
	}

	if (n->lex->lex->id != kScriptLexDirected) {
		this->variableStack.GetNodeList().pop_back();
	}
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
	PostSyntaxError(this->currentNode->lex->line, "Unable to cast from (%s) to (%s)",
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

	PostSyntaxError(this->currentNode->lex->line, "Unable to cast from (%s) to (%s)",
		right->GetName().data(), left->GetName().data());

_CastOk:
	if (left != right) {
		// cast var -> type
	}

	this->variableStack.Push(VariablePtr(type));
}

Void CodeBuilder::_ForEachClass(ScopePtr rootScope) {

	for (ObjectPtr i : rootScope->GetClassSet()) {
		if (!i->CheckModificator(Object::Modificator::Primitive) && i->GetNode()) {
			this->classList.push_back(i);
		}
	}

	for (auto i : rootScope->GetHashMap()) {
		this->_ForEachClass(i.second);
	}
}

Void CodeBuilder::_ForEachMethod(ScopePtr rootScope) {

	for (ObjectPtr i : rootScope->GetMethodSet()) {
		if (!i->CheckModificator(Object::Modificator::Native) && i->GetMethod()->GetRootNode()) {
			this->methodList.push_back(i);
		}
	}

	for (ObjectPtr i : rootScope->GetClassSet()) {
		if (!i->CheckModificator(Object::Modificator::Primitive) && i->GetNode()) {
			this->_ForEachMethod(i);
		}
	}
}

Void CodeBuilder::_Binary(NodePtr n) {

	VariablePtr sourceVar;
	VariablePtr leftVar;
	VariablePtr rightVar;
	Bool wasItConst;
    
    if (n->lex->args != 2) {
        throw ScriptException("Unable to perform non-binary operator (%s)",
            n->word.data());
    }

	this->_Read(n, leftVar, rightVar);

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
				this->_Cast(leftVar, sourceVar->GetClass());
			}
		}

		if (leftVar->GetClass()->Hash() != rightVar->GetClass()->Hash()) {
			if (rightVar->GetClass()->Hash() != sourceVar->GetClass()->Hash()) {
				this->_Cast(rightVar, sourceVar->GetClass());
			}
		}
	}
	else {
		sourceVar = leftVar;

		if (leftVar->GetClass()->Hash() != rightVar->GetClass()->Hash()) {
			this->_Cast(rightVar, sourceVar);
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
	_SaveNode:
		this->_Save(n);
	}

	if (n->lex->lex->IsLogic()) {
		if (!leftVar->IsIntegerLike()) {
			PostSyntaxError(this->currentNode->lex->line,
				"Unable to apply binary operator to non-integer type (%s)",
				leftVar->GetClass()->GetName().data());
		}
		if (!rightVar->IsIntegerLike()) {
			PostSyntaxError(this->currentNode->lex->line,
				"Unable to apply binary operator to non-integer type (%s)",
				rightVar->GetClass()->GetName().data());
		}
	}

	switch (this->currentNode->lex->lex->id) {
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
			if (sourceVar->v.intValue) {
				this->variableStack.Push(sourceVar->Root()
					->Find("true")->GetVariable());
			}
			else {
				this->variableStack.Push(sourceVar->Root()
					->Find("false")->GetVariable());
			}
		}
		else {
			this->variableStack.Push(VariablePtr(Scope::classBoolean));
		}
	}
	else {
		this->variableStack.Push(sourceVar);
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

Void CodeBuilder::_Unary(NodePtr n) {

	VariablePtr leftVar;

	this->_Read(n, leftVar, leftVar);

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
		this->variableStack.Push(VariablePtr(n->var->GetClass()));
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
			this->variableStack.Push(leftVar);
		}
		else {
			this->variableStack.Push(leftVar);
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
		} else {
			leftVar->reads_++;
		}
	}
}

Void CodeBuilder::_Ternary(NodePtr n) {

	VariablePtr backVar;
	VariablePtr var0;
	VariablePtr var1;

	this->_Read(n, backVar, backVar);

	this->_Run(n->blockList);
	var0 = this->lastResult;

	this->_Run(n->elseList);
	var1 = this->lastResult;

	if (var0->GetClass() != var1->GetClass()) {
		PostSyntaxError(n->lex->line, "Different result types in ternary expression (%s, %s)",
			var0->GetClass()->GetName().data(), var1->GetClass()->GetName().data());
	}
}

Void CodeBuilder::_New(NodePtr n) {

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

		this->_Invoke(n->typeNode);
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
			this->_Run(n->typeNode->argList, TRUE);
			if (!this->lastResult->GetClass()->IsIntegerLike()) {
				PostSyntaxError(n->lex->line, "Array size must be integer value (%s)",
					this->lastResult->GetClass()->GetName().data());
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

	this->variableStack.Push(resultVar);
}

Void CodeBuilder::_Selection(NodePtr n) {

	VariablePtr leftVar;
	VariablePtr rightVar;
	Buffer fieldName;
	ObjectPtr fieldObject;
	NodePtr fieldNode;

	this->_Read(n, leftVar, rightVar);

	fieldNode = this->variableStack.GetNodeList().back();
	fieldName = fieldNode->word;

	this->variableStack.GetNodeList().pop_back();
	this->variableStack.GetNodeList().pop_back();

	if (leftVar->GetVarType() != Variable::Var::Object &&
		leftVar->GetVarType() != Variable::Var::Array
	) {
		PostSyntaxError(n->lex->line, "You can access to object's fields only (%s)", leftVar->GetName().data());
	}

	if (fieldNode->id == kScriptNodeInvoke) {
		this->_Invoke(fieldNode);
		fieldObject = fieldNode->var;
	}
	else {
		if (leftVar->GetVarType() == Variable::Var::Array) {
			fieldObject = Scope::classArray->Find(fieldName, FALSE);
		}
		else {
			fieldObject = leftVar->GetClass()->Find(fieldName, FALSE);
		}
	}

	if (!fieldObject) {
		PostSyntaxError(n->lex->line, "Undeclared object's field (%s.%s)",
			leftVar->GetClass()->GetName().data(), fieldName.data());
	}

	if (fieldObject->CheckModificator(Object::Modificator::Private) ||
		fieldObject->CheckModificator(Object::Modificator::Protected)
	) {
		if (!leftVar->GetOwner()->Find(fieldObject->Hash())) {
			PostSyntaxError(n->lex->line, "Unable to get private/protected field (%s.%s)",
				leftVar->GetClass()->GetName().data(), fieldName.data());
		}
	}

	fieldObject->SetThis(leftVar);

	if (!fieldObject->CheckType(Object::Type::Method)) {
		this->variableStack.Push(VariablePtr(fieldObject));
	}

	this->lastNode->var = fieldObject;
	this->lastSelection = fieldObject;
}

Void CodeBuilder::_Condition(NodePtr n) {

	VariablePtr var0;
	VariablePtr var1;

	this->wasItCondition = TRUE;

	switch (n->lex->lex->id) {
	case kScriptLexIf:

		this->_Run(n->argList, TRUE);

		if (!this->lastResult) {
			PostSyntaxError(n->lex->line, "Lost expression's result", 0)
		}
		var0 = this->lastResult;

		/* Check result for boolean type */
		if (!var0->GetClass()->IsBooleanLike()) {
			goto _BooleanError;
		}

		if (var0->CheckModificator(Object::Modificator::Constant)) {
			if (var0->v.intValue == TRUE) {
				this->_Run(n->blockList, TRUE);
				if (n->elseNode) {
					n->elseNode->blockList.clear();
				}
				n->finalNode = n;
			}
			else {
				n->blockList.clear();
				if (n->elseNode) {
					this->_Run(n->elseNode->blockList, TRUE);
				}
				n->finalNode = n->elseNode;
			}
		}
		else {
			this->_Run(n->blockList, TRUE);
			if (n->elseNode) {
				this->_Run(n->elseNode->blockList, TRUE);
			}
		}

		break;
	case kScriptLexSwitch:

		this->_Run(n->argList, TRUE);

		if (!this->lastResult) {
			PostSyntaxError(n->lex->line, "Lost expression's result", 0)
		}

		var0 = this->lastResult;

		for (NodePtr n2 : n->blockList) {

			this->_Run(n2->argList, TRUE);

			if (!this->lastResult) {
				PostSyntaxError(n->lex->line, "Lost expression's result", 0)
			}

			var1 = this->lastResult;

			if (!var1->CheckModificator(Object::Modificator::Constant)) {
				PostSyntaxError(n2->lex->line, "Switch's case must be constant value/expression (%s)",
					var1->GetName().data());
			}

			if (var0->CheckModificator(Object::Modificator::Constant)) {
				if (!n->finalNode && var0->Equal(var1)) {
					n->finalNode = n2;
				}
			}

			this->_Run(n2->blockList, TRUE);
		}

		break;
	case kScriptLexElse:

		this->_Run(n->blockList, TRUE);

		break;
	case kScriptLexWhile:

		if ((n->flags & kScriptFlagWhileHasDo) != 0) {
			break;
		}

		this->_Run(n->argList, TRUE);

		/* Read result */
		if (!this->lastResult) {
			PostSyntaxError(n->lex->line, "Lost expression's result", 0)
		}
		var0 = this->lastResult;

		/* Check result for boolean type */
		if (!var0->GetClass()->IsBooleanLike()) {
			goto _BooleanError;
		}

		this->_Run(n->blockList, TRUE);

		break;
	case kScriptLexDo:

		this->_Run(n->elseNode->argList, TRUE);

		/* Read result */
		if (!this->lastResult) {
			PostSyntaxError(n->lex->line, "Lost expression's result", 0)
		}
		var0 = this->lastResult;

		/* Check result for boolean type */
		if (!var0->GetClass()->IsBooleanLike()) {
			goto _BooleanError;
		}

		if (var0->CheckModificator(Object::Modificator::Constant)) {
			if (!var0->GetVariable()->v.intValue) {
				n->finalNode = n;
			}
		}

		this->_Run(n->blockList, TRUE);

		break;
	case kScriptLexFor:

		/* Compile for's entry in current segment (doesn't metter) */
		this->_Run(n->forInfo.beginList, TRUE);
		this->_Run(n->forInfo.conditionList, TRUE);

		/* Read result */
		if (!this->lastResult) {
			PostSyntaxError(n->lex->line, "Lost expression's result", 0)
		}
		var0 = this->lastResult;

		/* Check result for boolean type */
		if (!var0->GetClass()->IsBooleanLike()) {
			goto _BooleanError;
		}

		this->_Run(n->blockList, TRUE);
		this->_Run(n->forInfo.nextList, TRUE);

		break;
	case kScriptLexTry:
	case kScriptLexCatch:
	case kScriptLexFinally:
		this->_Run(n->blockList, TRUE);
		break;
	default:
		break;
	}

	goto _SkipErrors;
_BooleanError:
	PostSyntaxError(n->lex->line, "Construction (%s) requires boolean argument, not (%s)", n->word.data(),
		var0->GetClass()->GetName().data());

_SkipErrors:
	this->_Save(n);
}

Void CodeBuilder::_Invoke(NodePtr n) {

	ObjectPtr methodVar;
	Vector<ClassPtr> objectList;
	Uint32 invokeHash;
	Buffer formattedParameters;
	ObjectPtr scope;
	Buffer methodName;

	/*	This is fix for directed selection, cuz
	we don't know where is static member invoke and
	where invocation via selector */

	if (this->currentNode->lex->lex->id != kScriptLexNew &&
		this->currentNode->lex->lex->id != kScriptLexDirected && !this->rememberedInvoke
	) {
		this->rememberedInvoke = n;
	}

	if (this->rememberedInvoke) {
		return;
	}

	/*	Looks like construction invoke, but via
	class name */
	if (n->word == n->parent->word) {
		PostSyntaxError(n->lex->line, "Use 'this' or 'super' method to invoke class constructor", 0);
	}

	/*	We have to get all variables from stack
	and save it in another list to build
	invocation hash number to find nessesary
	method fast and generate parameters */

	for (Uint32 i = 0; i < n->argList.size(); i++) {

		VariablePtr var = (*(n->argList.end() - i - 1))->var
			->GetVariable();

		objectList.push_back(var->GetClass());
		formattedParameters.append(var->GetClass()->GetName());

		if (i != n->lex->args - 1) {
			formattedParameters.append(", ");
		}
	}

	/*	Compute invoke hash for current
	parameters */

	invokeHash = Method::ComputeInvokeHash(objectList);

	/*	Try to get current scope as method, but
	just in case, we can find it in parent
	nodes */

	if (!(scope = n->var)) {

		NodePtr n2 = n;

		while (n2 && !n2->var) {
			n2 = n2->parent;
		}

		scope = n2->var;
	}

	/*	But if we have 'this' or 'super' names then
	we're going to invoke constructor, so we
	have to save parent's name */

	if (n->word == "this") {
		methodName = scope->GetName();
	}
	else if (n->word == "super") {
		methodName = scope->GetClass()->GetExtend()->GetName();
	}
	else {
		methodName = n->word;
	}

	/*	If we found metohd's variable, then we
	can save it in node's variable */

	if ((methodVar = scope->Find(Uint64(methodName.GetHash32()) << 32 | invokeHash)) && !n->var) {
		n->var = methodVar;
	}

	struct OrderedMethod {
	public:
		MethodPtr method;
		Uint32 distance;
	public:
		inline bool operator < (const OrderedMethod& om) const {
			return this->distance < om.distance;
		}
	};

	/*	But, if we can't find out method
	fast (it means, that sended parameters
	will be the same, like in method without
	weak cast, like foo(100, 20, 3) can apply
	fast invoke fot foo(byte, byte, byte) method),
	else we have to find method with the same name
	but with another parameters types which can be
	casted to sended arguments */

	if (!methodVar) {

		Vector<OrderedMethod> methodList;
		Uint32 distance = 0;
		Set<ObjectPtr> methodSet;

		if (!scope->CheckType(Object::Type::Class)) {

			methodSet = scope->GetParent()->GetMethodSet();

			if (this->lastSelection) {
				methodSet = this->lastSelection->GetClass()->GetMethodSet();
			}
		}
		else {
			methodSet = scope->GetMethodSet();
		}

		for (ObjectPtr m : methodSet) {
			if (methodName == m->GetName() && n->lex->args == m->GetMethod()->GetAttributeHash().size()) {
				for (Uint32 i = 0; i < n->lex->args; i++) {
					if (objectList[i]->GetPriority() > m->GetMethod()->GetAttributeHash()[i]->GetPriority()) {
						goto _WrongMethod1;
					}
					distance += labs(objectList[i]->GetPriority() - m->GetMethod()->GetAttributeHash()[i]->GetPriority());
				}
				methodList.push_back({ m->GetMethod(), distance });
			_WrongMethod1:
				distance = 0;
			}
		}

		/*	Build set with all class
		constructors */

		Set<ObjectPtr> constructorSet;

		for (ObjectPtr m : methodSet) {
			if (m->GetName() == m->GetOwner()->GetName()) {
				constructorSet.insert(m);
			}
		}

		if (this->currentNode->lex->lex->id == kScriptLexNew && !formattedParameters.length()) {
			if (constructorSet.empty()) {
				return;
			}
		}

		if (formattedParameters.empty()) {
			formattedParameters = "void";
		}

		if (methodList.empty()) {

			Buffer path = this->lastSelection ?
				this->lastSelection->GetClass()->GetName() : n->word;

			PostSyntaxError(n->lex->line, "Undeclared method %s/%s(%s)", path.data(),
				methodName.data(), formattedParameters.data());
		}

		/*	Order founded method and get first
		method with highest precedence */

		if (methodList.size() > 1) {
			std::sort(methodList.begin(), methodList.end());
		}

		methodVar = methodList.begin()->method;
		n->var = methodVar;
	}

#if 1 /* Don't sure, cuz it crashes selection */

	/*	Load all method's local variables, then
	apply call instruction and return something */

	if (methodVar->GetMethod()->returnVar) {
		this->variableStack.Push(methodVar->GetMethod()->returnVar);
	}
#endif
}

Void CodeBuilder::_Return(NodePtr n) {

	VariablePtr returnVar;
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

	returnVar = NULL;

	if (n->lex->args > 0) {
		this->_Read(n, returnVar, returnVar);
	}
    
	methodVar->returnVar = returnVar;

	if (returnVar) {
		if (methodVar->returnVar->GetClass() != methodVar->GetReturnType()) {
			PostSyntaxError(n->lex->line, "Incompatible types (%s -> %s)",  methodVar->returnVar->GetClass()
				->GetName().data(), methodVar->GetReturnType()->GetName().data());
		}
		this->_Cast(methodVar->returnVar, methodVar->GetReturnType());
	}
}

Void CodeBuilder::_Finish(NodePtr n) {

	if (this->lastNode->lex->lex->IsLeft() && this->variableStack.Size()) {
		this->variableStack.Pop();
	}

#if 1 /* Yes, its an error, but not now */
	for (ObjectPtr v : this->variableStack.GetVarList()) {
		if (v && v->GetNode() && v->GetNode()->id == kScriptNodeDefault) {
			PostSyntaxError(this->currentNode->lex->line, "Unexcepted token in expression (%s)",
				this->variableStack.Back()->GetName().data());
		}
	}
#endif

	this->lastSelection = NULL;
	this->variableStack.Clear();
}

Void CodeBuilder::_Array(NodePtr n) {

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

	this->_Read(n, leftVar, leftVar);

	if (!leftVar->GetClass()->IsIntegerLike()) {
		PostSyntaxError(n->lex->line, "Array index must be integer value, found (%s)",
			leftVar->GetClass()->GetName().data());
	}

	this->variableStack.Push(
		VariablePtr(n->var->GetClass()));
}

Void CodeBuilder::_Save(NodePtr n) {

	this->nodeList.push_back(n);
}

Void CodeBuilder::_Push(CodeNodePtr codeNode) {

	this->codeStack.push(codeNode);
	this->currentCode = codeNode;
}

CodeNodePtr CodeBuilder::_Pop(Void) {

	if (this->codeStack.empty()) {
		return NULL;
	}

	CodeNodePtr codeNode
		= this->codeStack.top();

	this->codeStack.pop();

	return codeNode;
}

LAME_END2