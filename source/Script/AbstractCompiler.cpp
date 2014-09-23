#include "AbstractCompiler.h"

LAME_BEGIN2(Script)

Void AbstractCompiler::Run(
	CodeBuilderPtr   codeBuilder,
	SyntaxBuilderPtr syntaxBuilder,
	ScopePtr         rootScope,
	SegmentPtr       codeSegment
) {
	this->nodeWalker.Walk(this, NULL, syntaxBuilder->GetRootNode(), rootScope);
}

Void AbstractCompiler::onBinary(NodePtr n) {

	VariablePtr sourceVar;
	VariablePtr leftVar;
	VariablePtr rightVar;
	Bool wasItConst;

	this->GetWalker()->Test(n);

	leftVar = this->GetWalker()->GetLeft()
		->GetVariable();

	rightVar = this->GetWalker()->GetRight()
		->GetVariable();

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

	if (n->lex->lex->id != kScriptLexSet) {
		if (leftVar == sourceVar) {
			this->invoker.onBinary(leftVar, rightVar);
		}
		else {
			this->invoker.onBinary(rightVar, leftVar);
		}
	}

	wasItConst =
		leftVar->CheckModificator(Object::Modificator::Constant) &&
		rightVar->CheckModificator(Object::Modificator::Constant);

	if (n->lex->lex->IsBool()) {
		if (wasItConst) {
			if (sourceVar->v.intValue) {
				this->GetWalker()->Push(sourceVar->Root()
					->Find("true")->GetVariable());
			}
			else {
				this->GetWalker()->Push(sourceVar->Root()
					->Find("false")->GetVariable());
			}
		}
		else {
			this->GetWalker()->Push(VariablePtr(
				Scope::classBoolean));
		}
	}
	else {
		if (n->lex->lex->IsLeft()) {
			this->invoker.onStore(leftVar);
		}
		this->GetWalker()->Push(sourceVar->GetClass());
	}
}

Void AbstractCompiler::onUnary(NodePtr n) {

	this->GetWalker()->Test(n);

	VariablePtr leftVar = this->GetWalker()->GetLeft()
		->GetVariable();

	if (n->lex->lex->id == kScriptLexCast) {
		this->invoker.onCast(leftVar, n->var->GetClass());
		this->GetWalker()->Push(n->var->GetClass());
	}
	else {
		if (n->lex->lex->id == kScriptLexPostfixIncrement ||
			n->lex->lex->id == kScriptLexPostfixDecrement ||
			n->lex->lex->id == kScriptLexPrefixIncrement ||
			n->lex->lex->id == kScriptLexPrefixDecrement
		) {
			this->GetWalker()->Push(leftVar->GetClass());
		}
		else {
			this->GetWalker()->Push(leftVar->GetClass());
		}
	}
}

Void AbstractCompiler::onTernary(NodePtr n) {

}

Void AbstractCompiler::onCondition(NodePtr n) {

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
		var0 = this->GetWalker()->TestArgument(Scope::classBoolean)
			->GetVariable();

		if (var0->CheckModificator(Object::Modificator::Constant)) {
			if (var0->v.intValue == TRUE) {
				this->_Compile(n->blockList, &tmpMainSegment);
				if (n->elseNode) {
					n->elseNode->blockList.clear();
				}
				n->finalNode = n;
			}
			else {
				n->blockList.clear();
				if (n->elseNode) {
					this->_Compile(n->blockList, &tmpMainSegment);
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

		/* Read result and check result for boolean type */
		var0 = this->GetWalker()->TestArgument(Scope::classBoolean)
			->GetVariable();

		for (NodePtr n2 : n->blockList) {

			this->_Compile(n->blockList, NULL);

			if (!this->GetWalker()->GetResult()) {
				PostSyntaxError(n->lex->line, "Lost expression's result", 0)
			}

			var1 = this->GetWalker()->GetResult()
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

			this->_Compile(n2->blockList, NULL);
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
		var0 = this->GetWalker()->TestArgument(Scope::classBoolean)
			->GetVariable();

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
		var0 = this->GetWalker()->TestArgument(Scope::classBoolean)
			->GetVariable();

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
		//this->_Run(n->forInfo.beginList);
		//this->_Run(n->forInfo.conditionList);

		///* Read result and check result for boolean type */
		//if (!this->lastResult) {
		//	PostSyntaxError(n->lex->line, "Lost expression's result", 0)
		//}
		//if (!(var0 = this->lastResult)->GetClass()->IsBooleanLike()) {
		//	goto _BooleanError;
		//}

		//this->invoker.onCondition(var0);

		//this->_Run(n->blockList);
		//this->_Run(n->forInfo.nextList);

		break;
	case kScriptLexTry:
	case kScriptLexCatch:
	case kScriptLexFinally:
		//this->_Run(n->blockList);
		break;
	default:
		break;
	}

	this->segmentInfo.argument = NULL;
	this->segmentInfo.body = NULL;
	this->segmentInfo.otherwise = NULL;
}

Void AbstractCompiler::onInvoke(NodePtr n) {

	for (ObjectPtr v : n->methodInfo.argList) {
		this->invoker.onLoad(v);
	}

	this->invoker.onInvoke(n->var);

	if (n->var->GetMethod() && n->var->GetMethod()->returnVar) {
		this->GetWalker()->Push(n->var->GetMethod()->returnVar
			->GetClass());
	}
}

Void AbstractCompiler::onNew(NodePtr n) {

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

		this->invoker.onNew(leftVar, 0);

		ClassPtr initClass = leftVar->GetClass();
		ObjectPtr initMethod = NULL;

		while (initClass != initClass->classObject) {
			if (initClass && (initMethod = initClass->Find("<init>", FALSE))) {
				this->invoker.onInvoke(initMethod);
			}
			initClass = ClassPtr(initClass->GetExtend());
		}
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
			__asm int 3
			//this->_Compile(n->typeNode->argList, NULL);

			//this->_Run(n->typeNode->argList);
			//this->_Read(n, leftVar, leftVar);
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

	this->GetWalker()->Push(resultVar->GetClass());
}

Void AbstractCompiler::onReturn(NodePtr n) {

	if (n->lex->args > 0) {

		this->GetWalker()->Test(n);

		if (this->GetWalker()->GetLeft()->CheckType(Object::Type::Variable)) {
			this->invoker.onLoad(this->GetWalker()->GetLeft()->GetVariable());
		}

		this->invoker.onReturn(this->GetWalker()->GetLeft());
	}
	else {
		this->invoker.onReturn(Object::classVoid);
	}
}

Void AbstractCompiler::onIndex(NodePtr n) {

	VariablePtr leftVar;

	n->lex->args = 1;
	this->GetWalker()->Test(n);

	leftVar = this->GetWalker()->GetLeft()
		->GetVariable();

	this->invoker.onLoad(n->var);
	this->invoker.onLoad(leftVar);

	this->GetWalker()->Push(VariablePtr(
		n->var->GetClass()));
}

Void AbstractCompiler::onLoad(NodePtr n) {

	if (n->var) {
		this->invoker.onLoad(n->var);
	}
}

Void AbstractCompiler::onMethodBegin(MethodPtr m) {

	NodePtr n = m->GetMethod()->GetRootNode();

	printf("%s %s%s(%s) {\n", m->GetMethod()->GetReturnType()->GetName().data(),
		m->GetPath().data(), m->GetName().data(), m->GetMethod()->FormatArguments().data());

	for (NodePtr n2 : n->argList) {
		this->invoker.onStore(n2->var);
	}

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
}

Void AbstractCompiler::onMethodEnd(MethodPtr m) {

	if (m->GetMethod()->GetReturnType()->IsVoid()) {
		this->invoker.onReturn(Object::classVoid);
	}

	printf("}\n");
}

Void AbstractCompiler::_Compile(NodeListRef nodeList, SegmentPtr segment) {

	if (nodeList.empty()) {
		return;
	}

	if (segment) {
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
	}

	this->GetWalker()->Run(nodeList);

	if (segment) {

		LAME_ASSERT(this->segmentStack.empty());

		segment = this->segmentStack.top();
		this->segmentStack.pop();

		this->byteCode->SetSegment(!this->segmentStack.empty() ?
			this->segmentStack.top() : NULL);
	}
}

LAME_END2