#include "VirtualCompiler.h"

LAME_BEGIN2(Compiler)

Void VirtualCompiler::_Run(NodeListRef nodeList, Bool makeBackup) {

	VariableStack stackBackup;

	if (makeBackup) {
		stackBackup = this->variableStack;
	}

	for (NodePtr n : nodeList) {

		this->currentNode = n;

		try {
			if (n->id == kScriptNodeClass ||
				n->id == kScriptNodeInterface ||
				n->id == kScriptNodeFunction
			) {
				continue;
			}

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
				this->_Finish(n);
			}
			else if (n->lex->lex->id == kScriptLexNew) {
				this->_New(n);
			}
			else if (n->lex->lex->id == kScriptLexDirected) {
				this->_Selection(n);
			}
			else if (n->id == kScriptNodeAlloc) {
				if (!n->var) {
					PostSyntaxError(n->lex->line, "Undeclared class (%s)", n->word.data());
				}
				this->_Invoke(n);
				if (n->parent->id != kScriptNodeClass) {
					goto _SaveNode;
				}
			}
			else if (n->lex->lex->IsUnknown() || n->lex->lex->IsConst()) {
			_SaveNode:
				if (!n->var) {
					this->variableStack.GetNameList().push_back(n->word);
					this->variableStack.GetVarList().push_back(NULL);
				}
				else {
					this->variableStack.Push(VariablePtr(n->var));
				}
			}
			else if (n->lex->lex->IsMath() || n->lex->lex->IsBool()) {
				if (n->lex->args == 1) {
					this->_Unary(n);
				}
				else if (n->lex->args == 2) {
					this->_Binary(n);
				}
				else if (n->lex->args == 3 && n->lex->lex->id == kScriptLexTernary) {
					this->_Ternary(n);
				}
				else {
					this->_Invoke(n);
				}
			}
		}
		catch (SyntaxException& e) {
			throw SyntaxException(e.Line(), e.GetErrorBuffer());
		}
		catch (ScriptException& e) {
			throw SyntaxException(n->lex->line, e.GetErrorBuffer());
		}
	}

	if (this->variableStack.Size()) {
		this->lastResult = this->variableStack.Back();
	}

	if (makeBackup) {
		this->variableStack = stackBackup;
	}
}

Void VirtualCompiler::_Read(NodePtr n, VariablePtr& left, VariablePtr& right) {

	if (this->variableStack.Size() < n->lex->args || !n->lex->args) {
		PostSyntaxError(n->lex->line, "Operator \"%s\" requires %d arguments", n->word.data(), n->lex->args);
	}

	if (n->lex->args > 1) {

		right = this->variableStack.Back();
		this->variableStack.GetVarList().pop_back();

		if (n->lex->lex->id != kScriptLexDirected) {
			if (!right) {
				PostSyntaxError(n->lex->line, "Undeclared variable (%s)", this->variableStack.GetNameList().data());
			}
			this->variableStack.GetNameList().pop_back();
		}
	}

	left = this->variableStack.Back();
	this->variableStack.GetVarList().pop_back();

	if (!left) {
		PostSyntaxError(n->lex->line, "Undeclared variable (%s)", this->variableStack.GetNameList().data());
	}

	if (n->lex->lex->id != kScriptLexDirected) {
		this->variableStack.GetNameList().pop_back();
	}
}

Void VirtualCompiler::_Write(VariablePtr var) {

	this->variableStack.Push(var);
	this->OnLoad(var);
}

Void VirtualCompiler::_Print(StringC message, ...) {

	VaList vaList;

	va_start(vaList, message);
	vprintf(message, vaList);
	va_end(vaList);
}

static Bool _CheckObjectCast(ClassPtr left, ClassPtr right) {

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

Void VirtualCompiler::_Cast(VariablePtr var, ObjectPtr type) {

	ClassPtr left = var->GetClass();
	ClassPtr right = type->GetClass();

	if (left->IsFloatLike()) {
		if (right->IsFloatLike() && right->GetPriority() > left->GetPriority()) {
			goto _CastOk;
		}
		if (right->IsIntegerLike()) {
			goto _CastError;
		}
		goto _CastError;
	}
	else if (left->IsIntegerLike()) {
		if (right->IsIntegerLike() && right->GetPriority() > left->GetPriority()) {
			goto _CastOk;
		}
		if (right->IsFloatLike() && right->GetPriority() > left->GetPriority()) {
			goto _CastOk;
		}
		goto _CastError;
	}
	else if (left->IsObject()) {
		if (_CheckObjectCast(left, right)) {
			goto _CastOk;
		}
	}

	goto _CastError;
	goto _CastOk;

_CastError:

	if (var->CheckModificator(Object::Modificator::Constant)) {
		if (right->GetPriority() > left->GetPriority()) {
			goto _CastOk;
		}
	}

	PostSyntaxError(this->currentNode->lex->line, "Unable to cast from (%s) to (%s)",
		left->GetName().data(), right->GetName().data());

_CastOk:

	this->OnCast(var, type->GetClass());
}

Void VirtualCompiler::_StrongCast(VariablePtr var, ClassPtr type) {

	ClassPtr left = type->GetClass();
	ClassPtr right = var->GetClass();

	if ((left->IsIntegerLike() || left->IsFloatLike()) &&
		(right->IsIntegerLike() || right->IsFloatLike())
	) {
		goto _CastOk;
	}
	else {
		goto _CastError;
	}

	goto _CastOk;

_CastError:

	PostSyntaxError(this->currentNode->lex->line, "Unable to cast from (%s) to (%s)",
		right->GetName().data(), left->GetName().data());

_CastOk:
	this->OnCast(var, type->GetClass());

	this->GetStack()->Push(VariablePtr(type));
}

Void VirtualCompiler::Run(SyntaxBuilderPtr nodeBuilder, ScopePtr rootScope, SegmentPtr codeSegment) {

	this->nodeBuilder = nodeBuilder;
	this->rootScope = rootScope;

	if (!ObjectPtr(rootScope)->GetSegment()) {
		throw ScriptException("Root scope must have segment, run scope builder first");
	}

	this->byteCode = new ByteCode(codeSegment);
	this->_Push(codeSegment);

	this->_ForEachClass(this->rootScope);
	this->_ForEachMethod(this->rootScope);

	this->lastResult = NULL;

	Sint64 previousEnumValue = -1;

	for (ObjectPtr i : this->classList) {
		if (i->CheckModificator(Object::Modificator::Enum)) {
			for (NodePtr n : i->GetNode()->blockList) {
				n->var->GetVariable()->v.intValue = ++previousEnumValue;
			}
		}
		else {
			//if (i->GetNode()->id != kScriptNodeCondition) {
				//this->_Run(i->GetNode()->blockList);
			//}
		}
	}

	Segment temporaryMethodSegment;
	temporaryMethodSegment.SetOffset(codeSegment->GetPosition());

	this->_CompileMethods(&temporaryMethodSegment);
	this->_CompileMethods(codeSegment);

	this->_Run(this->nodeBuilder->GetRootNode()->blockList);

	this->GetByteCode()->New(NOOP);
}

Void VirtualCompiler::_ForEachClass(ScopePtr rootScope) {

	for (ObjectPtr i : rootScope->GetClassSet()) {
		if (!i->CheckModificator(Object::Modificator::Primitive) && i->GetNode()) {
			this->classList.push_back(i);
		}
	}

	for (auto i : rootScope->GetHashMap()) {
		this->_ForEachClass(i.second);
	}
}

Void VirtualCompiler::_ForEachMethod(ScopePtr rootScope) {

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

Void VirtualCompiler::_Binary(NodePtr n) {

	VariablePtr sourceVar;
	VariablePtr leftVar;
	VariablePtr rightVar;

	this->_Read(n, leftVar, rightVar);

	if (n->lex->lex->IsLeft() && leftVar->CheckModificator(Object::Modificator::Constant)) {
		PostSyntaxError(n->lex->line, "Unable to apply (%s) to constant variable (%s)",
			n->word.data(), leftVar->GetName().data());
	}

	if (n->lex->lex->IsRight()) {

		sourceVar = leftVar->GetClass()->GetPriority() >= rightVar->GetClass()->GetPriority() ?
			leftVar : rightVar;

		if (!leftVar->wasInStack) {
			this->OnLoad(leftVar);
		}

		if (leftVar->GetClass()->Hash() != rightVar->GetClass()->Hash()) {
			if (leftVar->GetClass()->Hash() != sourceVar->GetClass()->Hash()) {
				this->_Cast(leftVar, sourceVar->GetClass());
			}
		}

		if (!rightVar->wasInStack) {
			this->OnLoad(rightVar);
		}

		if (leftVar->GetClass()->Hash() != rightVar->GetClass()->Hash()) {
			if (rightVar->GetClass()->Hash() != sourceVar->GetClass()->Hash()) {
				this->_Cast(rightVar, sourceVar->GetClass());
			}
		}
	}
	else {
		sourceVar = leftVar;

		if (!rightVar->wasInStack) {
			this->OnLoad(rightVar);
		}

		if (leftVar->GetClass()->Hash() != rightVar->GetClass()->Hash()) {
			this->_Cast(rightVar, sourceVar->GetClass());
		}

		rightVar->wasInStack = FALSE;
		sourceVar->wasInStack = FALSE;
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

	this->OnBinary(leftVar, rightVar);

	if (n->lex->lex->IsLeft() && n->lex->lex->id != kScriptLexSet) {
		this->OnStore(sourceVar);
	}

	if (n->lex->lex->IsBool()) {
		this->variableStack.Return(VariablePtr(Scope::classBoolean));
	}
	else {
		if (n->lex->lex->IsLeft()) {
			this->variableStack.Push(sourceVar);
		}
		else {
			this->variableStack.Return(sourceVar);
		}
	}
}

Void VirtualCompiler::_Unary(NodePtr n) {

	VariablePtr leftVar;

	this->_Read(n, leftVar, leftVar);

	if (n->lex->lex->IsLeft() && leftVar->CheckModificator(Object::Modificator::Constant)) {
		PostSyntaxError(n->lex->line, "Unable to apply (%s) to constant variable (%s)",
			n->word.data(), leftVar->GetName().data());
	}

	if (n->lex->lex->id == kScriptLexCast) {
		if (!leftVar->wasInStack) {
			this->OnLoad(leftVar);
		}
		if (!n->var->CheckType(Object::Type::Class)) {
			PostSyntaxError(n->lex->line, "Unable to apply strong cast to non-class object (%s)",
				n->var->GetName().data());
		}
		this->_StrongCast(leftVar, n->var->GetClass());
		this->variableStack.Return(VariablePtr(n->var->GetClass()));
	}
	else {
		if (n->lex->lex->id == kScriptLexIncrement ||
			n->lex->lex->id == kScriptLexDecrement ||
			n->lex->lex->id == kScriptLexPrefixIncrement ||
			n->lex->lex->id == kScriptLexPrefixDecrement
		) {
			this->OnUnary(leftVar);
			this->variableStack.Push(leftVar);
		}
		else {
			if (!leftVar->wasInStack) {
				this->OnLoad(leftVar);
			}
			this->OnUnary(leftVar);
			this->variableStack.Return(leftVar);
			if (n->lex->lex->IsLeft()) {
				this->OnStore(leftVar);
			}
		}
	}
}

Void VirtualCompiler::_Ternary(NodePtr n) {

	VariablePtr backVar;

	backVar = this->GetStack()->Back();

	this->_Run(n->blockList);

	if (!backVar->wasInStack) {
		this->OnLoad(backVar);
	}
	this->OnTernary(n, TRUE);

	this->_Run(n->elseList);

	if (!backVar->wasInStack) {
		this->OnLoad(backVar);
	}
	this->OnTernary(n, FALSE);
}

Void VirtualCompiler::_New(NodePtr n) {

	VariablePtr leftVar = VariablePtr(n->typeNode->var);

	/* Constructor invocation */
	if ((n->flags & kScriptFlagInvocation) != 0) {

		this->GetByteCode()->New(RNEW)
			->Write(leftVar->Scope::Size());

		ClassPtr initClass = leftVar->GetClass();
		ObjectPtr initMethod = NULL;

		while (initClass != initClass->classObject) {
			if ((initMethod = initClass->Find("<init>", FALSE))) {
				this->OnInvoke(initMethod->GetMethod());
			}
			initClass = ClassPtr(initClass->GetExtend());
		}

		this->_Invoke(n->typeNode);
	}
	/* Array allocation */
	else {
        __asm int 3
	}

	this->variableStack.Return(leftVar);
}

Void VirtualCompiler::_Selection(NodePtr n) {

	VariablePtr leftVar;
	VariablePtr rightVar;
	Buffer fieldName;
	ObjectPtr fieldObject;

	this->_Read(n, leftVar, rightVar);

	fieldName = this->variableStack.GetNameList().back();

	this->variableStack.GetNameList().pop_back();
	this->variableStack.GetNameList().pop_back();

	if (leftVar->GetVarType() != Variable::Var::Object) {
		PostSyntaxError(n->lex->line, "You can access to object's fields only (%s)", fieldName.data());
	}

	fieldObject = leftVar->GetClass()->Find(fieldName, FALSE);

	if (!fieldObject) {
		PostSyntaxError(n->lex->line, "Undeclared object's field (%s.%s)",
			leftVar->GetClass()->GetName().data(), fieldName.data());
	}

	rightVar = fieldObject->GetVariable();

	if (fieldObject->CheckModificator(Object::Modificator::Private) ||
		fieldObject->CheckModificator(Object::Modificator::Protected)
	) {
		if (!leftVar->GetOwner()->Find(rightVar->Hash())) {
			PostSyntaxError(n->lex->line, "Unable to get private/protected field (%s.%s)",
				leftVar->GetClass()->GetName().data(), fieldName.data());
		}
	}

	if (!leftVar->wasInStack) {

		this->OnLoad(leftVar);

		if (!rightVar->wasInStack) {
			this->OnLoad(rightVar);
		}
	}

	rightVar->SetThis(leftVar);

	this->variableStack.Return(rightVar);
}

Void VirtualCompiler::_Condition(NodePtr n) {

	VariablePtr var0;

	SegmentPtr blockSegment = NULL;
	SegmentPtr currentSegment = NULL;
	Segment    argSegment;
	Segment    nextSegment;

	Deque<NodePtr>::iterator i;

	/* Check segment stack as empty */
	if (this->segmentStack.empty()) {
		goto _SegmentError;
	}

	/* Get current segment */
	currentSegment = this->segmentStack.top();

	switch (n->lex->lex->id) {
	case kScriptLexIf:

		/* Set segment's offset */
		argSegment.SetOffset(currentSegment->GetPosition());

		/* Push segment and compile list with arguments */
		this->_Push(&argSegment);
		this->_Run(n->argList, TRUE);

		if (!this->lastResult) {
			PostSyntaxError(n->lex->line, "Lost expression's result", 0)
		}
		var0 = this->lastResult;

		/* Check result for boolean type */
		if (!var0->GetClass()->IsBooleanLike()) {
			goto _BooleanError;
		}

		/* Load result from stack if needed */
		if (!var0->wasInStack) {
			this->OnLoad(var0);
		}

		/* Pop if's segment */
		this->_Pop();

		/* Fetch block segment */
		blockSegment = n->var->GetSegment();
		blockSegment->SetOffset(
			currentSegment->GetPosition());

		/* Compile construction's block */
		this->_Push(blockSegment);
		this->_Run(n->blockList, TRUE);
		this->_Pop();

		/* Move arguments segment to parent's */
		currentSegment->Merge(&argSegment);

		/* Set jump before compiled block */
		this->GetByteCode()->New(JZ)->Write(currentSegment->GetPosition() +
			blockSegment->GetSize() + (n->elseNode ? 10 : 5));

		/* Move block segment to parent's */
		currentSegment->Merge(blockSegment);

		break;
	case kScriptLexElse:

		/* Set segment's offset */
		argSegment.SetOffset(currentSegment->GetPosition());

		/* Fetch block segment and set it's offset */
		blockSegment = n->var->GetSegment();

		/* Compile construction's block */
		this->_Push(blockSegment);
		this->_Run(n->blockList, TRUE);
		this->_Pop();

		/* Set block segment offset as compiled block size */
		blockSegment->SetOffset(currentSegment->GetPosition() + 5);
		blockSegment->Clear();

		/* Recompile construction's block */
		this->_Push(blockSegment);
		this->_Run(n->blockList, TRUE);
		this->_Pop();

		/* Move arguments segment to parent's */
		currentSegment->Merge(&argSegment);

		/* Set jump before compiled block */
		this->GetByteCode()->New(JUMP)->Write(currentSegment->GetPosition() +
			blockSegment->GetSize() + 5);

		/* Move block segment to parent's */
		currentSegment->Merge(blockSegment);

		break;
	case kScriptLexWhile:

		if ((n->flags & kScriptFlagWhileHasDo) != 0) {
			break;
		}

		/*	Set segment's offset */
		argSegment.SetOffset(currentSegment->GetPosition());

		/* Compile list with arguments */
		this->_Push(&argSegment);
		this->_Run(n->argList, TRUE);
		this->_Pop();

		/* Read result */
		if (!this->lastResult) {
			PostSyntaxError(n->lex->line, "Lost expression's result", 0)
		}
		var0 = this->lastResult;

		/* Check result for boolean type */
		if (!var0->GetClass()->IsBooleanLike()) {
			goto _BooleanError;
		}

		/* Load result from stack if needed */
		if (!var0->wasInStack) {
			this->OnLoad(var0);
		}

		/* Fetch block segment and set it's offset */
		blockSegment = n->var->GetSegment();
		blockSegment->SetOffset(
			currentSegment->GetPosition());

		/* Compile construction's block */
		this->_Push(blockSegment);
		this->_Run(n->blockList, TRUE);
		this->_Pop();

		/* Move arguments segment to parent's */
		currentSegment->Merge(&argSegment);

		/* Set jump before compiled block */
		this->GetByteCode()->New(JZ)->Write(currentSegment->GetPosition() +
			blockSegment->GetSize() + 10);

		/* Move block segment to parent's */
		currentSegment->Merge(blockSegment);

		/* Set jump after compiled block for jump */
		this->GetByteCode()->New(JUMP)->Write(
			argSegment.GetOffset());

		break;
	case kScriptLexDo:

		/*	Set segment's offset */
		argSegment.SetOffset(currentSegment->GetPosition());

		/* Fetch block segment */
		blockSegment = n->var->GetSegment();
		blockSegment->SetOffset(
			currentSegment->GetPosition());

		/* Compile construction's block */
		this->_Push(blockSegment);
		this->_Run(n->blockList, TRUE);
		this->_Pop();

		/* Compile list with arguments */
		this->_Push(&argSegment);
		this->_Run(n->argList, TRUE);
		this->_Pop();

		/* Move arguments segment to parent's */
		currentSegment->Merge(blockSegment);
		currentSegment->Merge(&argSegment);

		/* Read result */
		if (!this->lastResult) {
			PostSyntaxError(n->lex->line, "Lost expression's result", 0)
		}
		var0 = this->lastResult;

		/* Check result for boolean type */
		if (!var0->GetClass()->IsBooleanLike()) {
			goto _BooleanError;
		}

		/* Load result from stack if needed */
		if (!var0->wasInStack) {
			this->OnLoad(var0);
		}

		/* Set jump before compiled block */
		this->GetByteCode()->New(JNZ)->Write(argSegment.GetOffset());

		break;
	case kScriptLexFor:

		/* Compile for's entry in current segment (doesn't metter) */
		this->_Run(n->forInfo.beginList, TRUE);

		/* Set segment's offset */
		argSegment.SetOffset(currentSegment->GetPosition());

		/* Compile condition block in temporary segment */
		this->_Push(&argSegment);
		this->_Run(n->forInfo.conditionList, TRUE);
		this->_Pop();

		/* Read result */
		if (!this->lastResult) {
			PostSyntaxError(n->lex->line, "Lost expression's result", 0)
		}
		var0 = this->lastResult;

		/* Check result for boolean type */
		if (!var0->GetClass()->IsBooleanLike()) {
			goto _BooleanError;
		}

		/* Load result from stack if needed */
		if (!var0->wasInStack) {
			this->OnLoad(var0);
		}

		/* Fetch block segment and set it's offset */
		blockSegment = n->var->GetSegment();

		/* Set segment's offset */
		blockSegment->SetOffset(currentSegment->GetPosition());

		/* Compile construction's block */
		this->_Push(blockSegment);
		this->_Run(n->blockList, TRUE);
		this->_Pop();

		/* Move arguments and block segments to parent's */
		currentSegment->Merge(&argSegment);

		/* Set segment's offset */
		nextSegment.SetOffset(currentSegment->GetPosition());

		/* Compile next block */
		this->_Push(&nextSegment);
		this->_Run(n->forInfo.nextList, TRUE);
		this->_Pop();

		/* Set jump before compiled block */
		this->GetByteCode()->New(JZ)->Write(nextSegment.GetPosition() +
			blockSegment->GetSize() + 10);

		/* Move block segments to parent's */
		currentSegment->Merge(blockSegment);

		/* Move next segment to parent's */
		currentSegment->Merge(&nextSegment);

		/* Jump to begin */
		this->GetByteCode()->New(JUMP)->Write(argSegment.GetOffset());

		break;
	case kScriptLexTry:
		break;
	case kScriptLexCatch:
		break;
	case kScriptLexFinally:
		break;
	default:
		break;
	}

	goto _SkipErrors;

_BooleanError:
	PostSyntaxError(n->lex->line, "Construction (%s) requires boolean argument, not (%s)", n->word.data(),
		var0->GetClass()->GetName().data());

_SegmentError:
	PostSyntaxError(n->lex->line, "Segments mismatched at (%s)", n->word.data());

_SkipErrors:
	;
}

Void VirtualCompiler::_Invoke(NodePtr n) {

	VariablePtr leftVar;
	ObjectPtr methodVar;
	Uint32 methodHash;
	Vector<ClassPtr> objectList;
	Uint32 invokeHash;
	Buffer formattedParameters;
	ObjectPtr scope;
	Buffer methodName;

	/*	Looks like construction invoke, but via
		class name */
	if (n->word == n->parent->word) {
		PostSyntaxError(n->lex->line, "Use 'this' or 'super' method to invoke class constructor", 0);
	}
	
	/* Check for lost parameters */
	if (this->variableStack.Size() < n->lex->args) {
		PostSyntaxError(n->lex->line, "Lost %d parameters to invoke method (%s)",
			n->lex->args, n->word.data());
	}

	/*	We have to get all variables from stack
		and save it in another list to build
		invocation hash number to find nessesary
		method fast and generate parameters */

	for (Uint32 i = 0; i < n->lex->args; i++) {

		VariablePtr var = *(this->variableStack.GetVarList().end() - i - 1);

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

	if (!(scope = this->GetCurrentMethod())) {
		
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

		Set<ObjectPtr> methodSet = scope->CheckType(Object::Type::Class) ?
			scope->GetMethodSet() : scope->GetParent()->GetMethodSet();

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
			if (m->GetName() == methodName) {
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
			PostSyntaxError(n->lex->line, "Undeclared method %s(%s)", methodName.data(), formattedParameters.data());
		}

		/*	Order founded method and get first
			method with highest precedence */

		if (methodList.size() > 1) {
			std::sort(methodList.begin(), methodList.end());
		}

		methodVar = methodList.begin()->method;
	}

	/*	Load all method's local variables, then
		apply call instruction and return something */

	for (Uint32 i = 0; i < n->lex->args; i++) {
		this->OnLoad(this->variableStack.Pop());
	}

	this->OnInvoke(methodVar->GetMethod());

	if (methodVar->GetMethod()->returnVar) {
		this->variableStack.Return(methodVar->GetMethod()->returnVar);
	}
}

Void VirtualCompiler::_Return(NodePtr n) {

	VariablePtr returnVar;
	ClassPtr returnType;
	MethodPtr methodVar;
	NodePtr methodNode;

	methodNode = n->parent;

	while (methodNode) {

		if (methodNode->var && methodNode->var->CheckType(Object::Type::Method)) {
			break;
		}

		methodNode = methodNode->parent;
	}

	methodVar = methodNode->var->GetMethod();

	if (!methodVar) {
		PostSyntaxError(n->lex->line, "Return can only be in method or function", 0);
	}

	if (methodVar->GetReturnType()->IsVoid()) {
		if (n->lex->args) {
			PostSyntaxError(n->lex->line, "Void method (%s) can't return anything",
				n->parent->word.data());
		}
	}
	else {
		if (!n->lex->args) {
			PostSyntaxError(n->lex->line, "Non-void method (%s) must return (%s)",
				n->parent->word.data(), methodVar->GetReturnType()->GetName().data());
		}
	}

	returnVar = NULL;

	if (n->lex->args > 0) {
		this->_Read(n, returnVar, returnVar);
	}

	returnType = methodVar->GetReturnType();
	methodVar->returnVar = returnVar;

	if (returnVar) {
		OnCast(methodVar->returnVar, methodVar->GetReturnType());
		this->OnReturn(methodVar->GetReturnType());
	}
}

Void VirtualCompiler::_Finish(NodePtr n) {

#if 1 /* Yes, its an error, but not now */
    for (VariablePtr v : this->variableStack.GetVarList()) {
        if (v && v->GetNode() && v->GetNode()->id == kScriptNodeDefault) {
            PostSyntaxError(this->currentNode->lex->line, "Unexcepted token in expression (%s)",
                this->variableStack.Back()->GetName().data());
        }
    }
#endif

	this->variableStack.Clear();
}

Void VirtualCompiler::_CompileMethods(SegmentPtr segment) {

	ObjectPtr thisVar = NULL;

	for (ObjectPtr i : this->methodList) {

		Uint32 address = segment->GetSize();
		NodePtr n = i->GetMethod()->GetRootNode();

		this->currentMethod = i;

		i->SetPosition(segment->GetPosition());

		if (i->CheckModificator(Object::Modificator::Synchronized)) {
			this->GetByteCode()->New(Asm::SYNCE);
		}

		this->_Push(segment);

		for (NodePtr n2 : n->argList) {
			if (!n2->var->CheckType(Object::Type::Variable)) {
				PostSyntaxError(n->lex->line, "Argument must be variable (%s)", n2->var->GetName().data());
			}
			this->OnStore(n2->var->GetVariable());
		}

		if (!i->CheckModificator(Object::Modificator::Static)) {
			if (i->GetName() == "<init>" || i->GetName() == i->GetOwner()->GetName()) {
				this->GetByteCode()->New(RDUP);
			}
			if ((thisVar = i->Find("this", FALSE, Uint32(Object::Type::Variable)))) {
				this->OnStore(thisVar->GetVariable());
			}
		}

		this->_Run(i->GetMethod()->GetRootNode()->blockList);

		if (i->CheckModificator(Object::Modificator::Synchronized)) {
			this->GetByteCode()->New(Asm::SYNCQ);
		}

		Segment::HistoryPtr history = NULL;

		if ((history = this->GetByteCode()->GetSegment()->FetchHistory(address))) {
			history->object = i;
		}

		if (i->GetMethod()->GetReturnType()->IsVoid()) {
			this->OnReturn(NULL);
		}

		if (!i->GetMethod()->GetReturnType()->IsVoid()) {
			if (!i->GetMethod()->returnVar) {
				PostSyntaxError(n->lex->line, "Non-void method %s(%s) must return (%s)", i->GetName().data(),
					i->GetMethod()->GetFormattedArguments().data(), i->GetMethod()->GetReturnType()->GetName().data());
			}
		}

		this->_Pop();
	}
}

Void VirtualCompiler::_Push(SegmentPtr segment) {

	if (!segment) {
		throw ScriptException("Lost object's segment, can't push NULL object");
	}

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

SegmentPtr VirtualCompiler::_Pop(Void) {

	SegmentPtr segment;
	SegmentPtr next;

	if (this->segmentStack.empty() || !this->byteCode) {
		throw ScriptException("Unable to pop segment from stack, stack is empty");
	}

	segment = this->segmentStack.top();
	this->segmentStack.pop();
	next = this->segmentStack.top();
	this->GetByteCode()->SetSegment(next);

	return segment;
}

LAME_END2