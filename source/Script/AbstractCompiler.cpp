#include "AbstractCompiler.h"

LAME_BEGIN2(Script)

Void AbstractCompiler::Run(
	CodeBuilderPtr   codeBuilder,
	SyntaxBuilderPtr syntaxBuilder,
	ScopePtr         rootScope,
	SegmentPtr       codeSegment
) {
	for (ObjectPtr m : codeBuilder->GetMethodList()) {

		NodePtr n = m->GetMethod()->GetRootNode();

		this->currentNode = n;
		this->currentMethod = m;

		for (NodePtr n2 : n->argList) {
			this->invoker.onStore(n2->var);
		}

		printf("%s %s%s(%s) {\n", m->GetMethod()->GetReturnType()->GetName().data(),
			m->GetPath().data(), m->GetName().data(), m->GetMethod()->GetFormattedArguments().data());

		ObjectPtr thisVar = NULL;

		if (!m->CheckModificator(Object::Modificator::Static)) {
			if (m->GetName() == "<init>" || m->GetName() == m->GetOwner()->GetName()) {
				if (m->GetNode() && m->GetNode()->id != kScriptNodeEntry || !m->GetNode()) {
					this->invoker.onClone(ObjectPtr(m->GetOwner()));
				}
			}
			if ((thisVar = m->Find("this", FALSE, Uint32(Object::Type::Variable)))) {
				this->invoker.onStore(thisVar);
			}
		}

		Segment tmpMethodSegment;

		this->_Compile(m->GetMethod()->GetRootNode()->blockList,
			&tmpMethodSegment);

		if (m->GetMethod()->GetReturnType()->IsVoid()) {
			this->invoker.onReturn(m->GetMethod()->returnVar);
		}

		printf("}\n", tmpMethodSegment.GetSize());
	}
}

Void AbstractCompiler::_Run(NodeListRef nodeList) {

	for (NodePtr n : nodeList) {

		this->currentNode = n;

		if (n->lex->lex->id == kScriptLexReturn) {
			this->_Return(n);
		}
		else if (n->id == kScriptNodeCondition) {
			this->_Condition(n);
		}
		else if (n->id == kScriptNodeInvoke) {
			this->_Invoke(n);
		}
		else if (n->lex->lex->id == kScriptLexSemicolon) {
			this->variableStack.Clear();
		}
		else if (n->lex->lex->id == kScriptLexNew) {
			this->_New(n);
		}
		else if (n->id == kScriptNodeAlloc) {
			if (n->parent->id != kScriptNodeClass) {
				this->_Array(n);
			} else {
				this->_Invoke(n);
			}
		}
		else if (n->lex->lex->IsUnknown() ||
			n->lex->lex->IsConst()
		) {
			LAME_ASSERT(n->var);
			this->variableStack.Push(VariablePtr(n->var));
			if (!n->wasItLeft) {
				this->invoker.onLoad(n->var);
			}
		}
		else if (n->lex->lex->IsMath() || n->lex->lex->IsBool()) {
			if (n->lex->args == 1) {
				this->_Unary(n);
			}
			else if (n->lex->args == 2) {
				this->_Binary(n);
			}
		}
		else {
			PostSyntaxError(n->lex->line, "Fuck you Dmitry Savonin %d times",
				n->lex->lex->id);
		}
	}

	if (this->variableStack.Size()) {
		this->lastResult = this->variableStack.Back()
			->GetVariable();
	}
}

Void AbstractCompiler::_Read(NodePtr n, VariablePtr& left, VariablePtr& right) {

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

Void AbstractCompiler::_Binary(NodePtr n) {

	VariablePtr sourceVar;
	VariablePtr leftVar;
	VariablePtr rightVar;
	Bool wasItConst;

	this->_Read(n, leftVar, rightVar);

	if (n->lex->lex->IsRight()) {

		sourceVar = leftVar->GetClass()->GetPriority() >= rightVar->GetClass()->GetPriority() ?
			leftVar : rightVar;

		if (leftVar->GetClass()->Hash() != rightVar->GetClass()->Hash()) {
			if (leftVar->GetClass()->Hash() != sourceVar->GetClass()->Hash()) {
				this->invoker.onCast(leftVar, sourceVar->GetClass());
			}
		}

		if (leftVar->GetClass()->Hash() != rightVar->GetClass()->Hash()) {
			if (rightVar->GetClass()->Hash() != sourceVar->GetClass()->Hash()) {
				this->invoker.onCast(rightVar, sourceVar->GetClass());
			}
		}
	}
	else {
		sourceVar = leftVar;

		if (leftVar->GetClass()->Hash() != rightVar->GetClass()->Hash()) {
			this->invoker.onCast(rightVar, sourceVar->GetClass());
		}
	}

	if (rightVar->GetNode() && rightVar->GetNode()->wasItLeft) {
		this->invoker.onLoad(rightVar);
	}

	if (n->lex->lex->id != kScriptLexSet) {
		if (leftVar == sourceVar) {
			this->invoker.onBinary(leftVar, rightVar);
		} else {
			this->invoker.onBinary(rightVar, leftVar);
		}
	}

	wasItConst =
		leftVar->CheckModificator(Object::Modificator::Constant) &&
		rightVar->CheckModificator(Object::Modificator::Constant);

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
		if (n->lex->lex->IsLeft()) {
			this->invoker.onStore(leftVar);
			this->variableStack.Push(sourceVar);
		}
		else {
			this->variableStack.Push(sourceVar);
		}
	}
}

Void AbstractCompiler::_Unary(NodePtr n) {

	VariablePtr leftVar;

	this->_Read(n, leftVar, leftVar);

	if (n->lex->lex->id == kScriptLexCast) {
		this->invoker.onCast(leftVar, n->var->GetClass());
		this->variableStack.Push(VariablePtr(n->var->GetClass()));
	}
	else {
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
}

Void AbstractCompiler::_New(NodePtr n) {

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

		this->invoker.onNew(leftVar);

		ClassPtr initClass = leftVar->GetClass();
		ObjectPtr initMethod = NULL;

		while (initClass != initClass->classObject) {
			if (initClass && (initMethod = initClass->Find("<init>", FALSE))) {
				this->invoker.onInvoke(initMethod);
			}
			initClass = ClassPtr(initClass->GetExtend());
		}

		this->_Invoke(n->typeNode);
	}
	/* Array allocation */
	else {

		Uint32 offsetLength = 0;

		if (leftVar->GetClass()->CheckModificator(Object::Modificator::Primitive)) {
			offsetLength = leftVar->GetClass()->Size();
		}
		else {
			offsetLength = leftVar->Scope::Size();
		}

		Vector<VariablePtr> initList;

		for (NodePtr n2 : n->blockList) {
			if (n2->lex->lex->id != kScriptLexComma) {
				initList.push_back(VariablePtr(n2->var));
			}
		}

		if (!n->typeNode->argList.empty()) {
			this->_Run(n->typeNode->argList);
			this->_Read(n, leftVar, leftVar);
		}
		else {
			Uint32 offset = 0;

			LAME_TODO("Load initList.size()");
			LAME_TODO("New offsetLength");

			for (VariablePtr v : initList) {

				this->invoker.onClone(leftVar);

				v->SetAddress(offset);
				offset += offsetLength;
			}
		}

		resultVar = VariablePtr(Scope::classArray);
		Scope::classArray->SetTemplate(resultVar->GetClass());
	}

	this->variableStack.Push(resultVar);
}

Void AbstractCompiler::_Condition(NodePtr n) {

	VariablePtr var0;
	VariablePtr var1;

	Segment tmpArgSegment;
	Segment tmpMainSegment;
	Segment tmpElseSegment;

	switch (n->lex->lex->id) {
	case kScriptLexIf:

		/* Compile if's argument expression */
		this->_Compile(n->argList, &tmpArgSegment);

		/* Read result and check result for boolean type */
		if (!this->lastResult) {
			PostSyntaxError(n->lex->line, "Lost expression's result", 0)
		}
		if (!(var0 = this->lastResult)->GetClass()->IsBooleanLike()) {
			goto _BooleanError;
		}

		if (var0->CheckModificator(Object::Modificator::Constant)) {
			if (var0->v.intValue == TRUE) {
				this->_Run(n->blockList);
				if (n->elseNode) {
					n->elseNode->blockList.clear();
				}
				n->finalNode = n;
			}
			else {
				n->blockList.clear();
				if (n->elseNode) {
					this->_Run(n->elseNode->blockList);
				}
				n->finalNode = n->elseNode;
			}
		}
		else {
			this->_Compile(n->blockList, &tmpMainSegment);
			if (n->elseNode) {
				this->_Compile(n->elseNode->blockList, &tmpElseSegment);
			}
		}

		this->segmentInfo.body = &tmpMainSegment;
		this->segmentInfo.otherwise = &tmpElseSegment;

		this->invoker.onCondition(var0);

		break;
	case kScriptLexSwitch:

		__asm int 3

		this->_Compile(n->argList, &tmpArgSegment);

		if (!this->lastResult) {
			PostSyntaxError(n->lex->line, "Lost expression's result", 0)
		}

		var0 = this->lastResult;

		for (NodePtr n2 : n->blockList) {

			this->_Run(n2->argList);

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

			this->_Run(n2->blockList);
		}

		break;
	case kScriptLexElse:
		break;
	case kScriptLexWhile:

		if ((n->flags & kScriptFlagWhileHasDo) != 0) {
			break;
		}

		/* Compile segment with argument expression */
		this->_Compile(n->argList, &tmpArgSegment);

		/* Read result and check result for boolean type */
		if (!this->lastResult) {
			PostSyntaxError(n->lex->line, "Lost expression's result", 0)
		}
		if (!(var0 = this->lastResult)->GetClass()->IsBooleanLike()) {
			goto _BooleanError;
		}

		/* Compile main block segment */
		this->_Compile(n->blockList, &tmpMainSegment);

		/* Assign segment info */
		this->segmentInfo.body = &tmpMainSegment;
		this->segmentInfo.argument = &tmpArgSegment;

		break;
	case kScriptLexDo:

		/* Compile segment with argument expression */
		this->_Compile(n->argList, &tmpArgSegment);

		/* Read result and check result for boolean type */
		if (!this->lastResult) {
			PostSyntaxError(n->lex->line, "Lost expression's result", 0)
		}
		if (!(var0 = this->lastResult)->GetClass()->IsBooleanLike()) {
			goto _BooleanError;
		}

		if (var0->CheckModificator(Object::Modificator::Constant)) {
			if (!var0->GetVariable()->v.intValue) {
				n->finalNode = n;
			}
		}

		/* Compile main block segment */
		this->_Compile(n->blockList, &tmpMainSegment);

		/* Assign segment info */
		this->segmentInfo.body = &tmpMainSegment;
		this->segmentInfo.argument = &tmpArgSegment;

		break;
	case kScriptLexFor:

		/* Compile for's entry in current segment (doesn't metter) */
		this->_Run(n->forInfo.beginList);
		this->_Run(n->forInfo.conditionList);

		/* Read result and check result for boolean type */
		if (!this->lastResult) {
			PostSyntaxError(n->lex->line, "Lost expression's result", 0)
		}
		if (!(var0 = this->lastResult)->GetClass()->IsBooleanLike()) {
			goto _BooleanError;
		}

		this->invoker.onCondition(var0);

		this->_Run(n->blockList);
		this->_Run(n->forInfo.nextList);

		break;
	case kScriptLexTry:
	case kScriptLexCatch:
	case kScriptLexFinally:
		this->_Run(n->blockList);
		break;
	default:
		break;
	}

	goto _SkipErrors;
_BooleanError:
	PostSyntaxError(n->lex->line, "Construction (%s) requires boolean argument, not (%s)", n->word.data(),
		var0->GetClass()->GetName().data());

_SkipErrors:
	this->segmentInfo.argument  = NULL;
	this->segmentInfo.body = NULL;
	this->segmentInfo.otherwise = NULL;
}

Void AbstractCompiler::_Invoke(NodePtr n) {

	/*	We have to get all variables from stack
	and save it in another list to build
	invocation hash number to find nessesary
	method fast and generate parameters */

	for (Uint32 i = 0; i < n->argList.size(); i++) {

		VariablePtr var = (*(n->argList.end() - i - 1))->var
			->GetVariable();

		this->invoker.onLoad(var);
	}

	this->invoker.onInvoke(n->var);

	/*	Load all method's local variables, then
	apply call instruction and return something */

	if (n->var->GetMethod() && n->var->GetMethod()->returnVar) {
		this->variableStack.Push(n->var->GetMethod()->returnVar);
	}
}

Void AbstractCompiler::_Return(NodePtr n) {

	VariablePtr returnVar;

	returnVar = NULL;

	if (n->lex->args > 0) {
		this->_Read(n, returnVar, returnVar);
		this->invoker.onLoad(returnVar);
	}
}

Void AbstractCompiler::_Array(NodePtr n) {

	VariablePtr leftVar;

	n->lex->args = 1;
	this->_Read(n, leftVar, leftVar);
	this->invoker.onLoad(n->var);
	this->invoker.onLoad(leftVar);

	this->variableStack.Push(
		VariablePtr(n->var->GetClass()));
}

Void AbstractCompiler::_Compile(NodeListRef nodeList, SegmentPtr segment) {

	if (!this->byteCode) {
		this->byteCode = new ByteCode(segment);
	}
	else {
		this->GetByteCode()->SetSegment(segment);
	}
	if (this->segmentList.empty()) {
		this->segmentOffset = segment->GetOffset();
	}
	if (!segment->GetOffset()) {
		segment->SetOffset(this->byteCode->GetPosition());
	}

	this->segmentList.push_back(segment);
	this->segmentStack.push(segment);

	this->_Run(nodeList);

	segment = this->segmentStack.top();
	this->segmentStack.pop();

	this->byteCode->SetSegment(!this->segmentStack.empty() ?
		this->segmentStack.top() : NULL);
}

LAME_END2