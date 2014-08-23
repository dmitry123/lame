#include "VirtualCompiler.h"

LAME_BEGIN2(Compiler)

using namespace Core;
using namespace Script;

Void VirtualCompiler::_Run(NodeListRef nodeList) {

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
				this->variableStack.Push(VariablePtr(n->var));
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
}

Void VirtualCompiler::_Read(NodePtr n, VariablePtr& left, VariablePtr& right) {

	if (this->variableStack.Size() < n->lex->args || !n->lex->args) {
		PostSyntaxError(n->lex->line, "Invalid operator parameters, \"%s\" requires %d arguments", n->word.data(), n->lex->args);
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
	}
	else if (left->IsIntegerLike()) {
		if (right->IsIntegerLike() && right->GetPriority() > left->GetPriority()) {
			goto _CastOk;
		}
		if (right->IsFloatLike() && right->GetPriority() > left->GetPriority()) {
			goto _CastOk;
		}
	}

	goto _CastError;
	goto _CastOk;

_CastError:

	if (var->CheckModificator(Object::Modificator::Constant)) {
		if (right->GetPriority() <= left->GetPriority()) {
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

Void VirtualCompiler::Run(NodeBuilderPtr nodeBuilder, ScopePtr rootScope, SegmentPtr codeSegment) {

	this->nodeBuilder = nodeBuilder;
	this->rootScope = rootScope;

	this->_ForEachClass(this->rootScope);
	this->_ForEachMethod(this->rootScope);

	if (!ObjectPtr(rootScope)->GetSegment()) {
		throw ScriptException("Root scope must have segment, run scope builder");
	}

	this->byteCode = new ByteCode(codeSegment);
	this->_Push(codeSegment);

	Sint64 previousEnumValue = -1;

	for (ObjectPtr i : this->classList) {
		if (i->CheckModificator(Object::Modificator::Enum)) {
			for (NodePtr n : i->GetNode()->blockList) {
				n->var->GetVariable()->v.intValue = ++previousEnumValue;
			}
		}
		else {
			if (i->GetNode()->id != kScriptNodeCondition) {
				this->_Run(i->GetNode()->blockList);
			}
		}
	}

	for (ObjectPtr i : this->methodList) {

		i->SetPosition(ObjectPtr(rootScope)->GetSegment()->GetSize());

		this->_Print(" // %s%s (%s)\n", i->GetPath().data(), i->GetName().data(),
			i->GetMethod()->GetFormattedArguments().data());

		NodePtr n = i->GetMethod()->GetRootNode();

		for (NodePtr n2 : n->argList) {
			if (!n2->var->CheckType(Object::Type::Variable)) {
				PostSyntaxError(n->lex->line, "Argument must be variable (%s)", n2->var->GetName().data());
			}
			this->OnStore(n2->var->GetVariable());
		}

		this->_Run(i->GetMethod()->GetRootNode()->blockList);

		if (i->GetMethod()->GetReturnType()->IsVoid()) {
			this->OnReturn(NULL);
		}

		if (!i->GetMethod()->GetReturnType()->IsVoid()) {
			if (!i->GetMethod()->returnVar) {
				PostSyntaxError(n->lex->line, "Non-void method (%s) must return (%s)", i->GetName().data(),
					i->GetMethod()->GetReturnType()->GetName().data());
			}
		}
	}

	this->_Run(this->nodeBuilder->GetRootNode()->blockList);
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

	//this->_Print(" // - %s %s %s\n", leftVar->GetName().data(),
	//	n->word.data(), rightVar->GetName().data());

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

		//sourceVar->wasInStack = TRUE;
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
		this->variableStack.Return(sourceVar);
	}
}

Void VirtualCompiler::_Unary(NodePtr n) {

	VariablePtr leftVar;

	this->_Read(n, leftVar, leftVar);

	//this->_Print(" // - %s%s\n", n->word.data(),
	//	leftVar->GetName().data());

	if (!leftVar->wasInStack) {
		this->OnLoad(leftVar);
	}

	if (n->lex->lex->id == kScriptLexCast) {
		if (!n->var->CheckType(Object::Type::Class)) {
			PostSyntaxError(n->lex->line, "Unable to apply strong cast to non-class object (%s)",
				n->var->GetName().data());
		}
		this->_StrongCast(leftVar, n->var->GetClass());
	}
	else {
		this->OnUnary(leftVar);
		this->variableStack.Return(leftVar);
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

	VariablePtr leftVar;
	VariablePtr rightVar;

	this->_Read(n, leftVar, leftVar);

	if (leftVar->CheckModificator(Object::Modificator::Primitive)) {
		this->_Read(n, rightVar, rightVar);
		if (!rightVar->CheckType(Object::Type::Variable)) {
			PostSyntaxError(n->lex->line, "Array parameter must be variable (%s)", rightVar->GetName().data());
		}
		if (rightVar->GetVarType() != Variable::Var::Integer) {
			PostSyntaxError(n->lex->line, "Array parameter must be integer value (%s)", rightVar->GetName().data());
		}
		// new
	}
	else {
		if (leftVar->CheckModificator(Object::Modificator::Primitive)) {
			PostSyntaxError(n->lex->line, "Unable to apply 'new' for primitive type (%s)", leftVar->GetName().data());
		}
		// new
		ObjectPtr classVar = leftVar->Find(leftVar->Hash());
		ObjectPtr constructorVar = leftVar->Find(leftVar->GetName());
		// push
		if (classVar->GetNode()) {
			for (NodePtr n2 : classVar->GetNode()->blockList) {
				this->_Run(n2->blockList);
			}
		}
		if (constructorVar && FALSE) {
			if (constructorVar->GetMethod()->GetRootNode()) {
				this->_Run(constructorVar->GetMethod()->GetRootNode()->blockList);
			}
			else if (constructorVar->GetMethod()->GetNativeMethod()) {
				constructorVar->GetMethod()->GetNativeMethod()(constructorVar->GetMethod());
			}
			else {
				PostSyntaxError(n->lex->line, "Non-implemented class constructor (%s)", constructorVar->GetName().data());
			}
		}
		// pop
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

	fieldObject = leftVar->GetClass()->Find(fieldName);

	if (!fieldObject) {
		PostSyntaxError(n->lex->line, "Undeclared object's field (%s.%s)", leftVar->GetClass()->GetName().data(), fieldName.data());
	}
	rightVar = fieldObject->GetVariable();

	// selection

	this->variableStack.Return(rightVar);
	//	varStack.push_back(rightVar);
	//	nameStack.push_back(fieldName);
}

Void VirtualCompiler::_Condition(NodePtr n) {

	VariablePtr var0;

	SegmentPtr blockSegment = NULL;
	SegmentPtr mySegment = NULL;
	Segment    argSegment;

	if (this->segmentStack.empty()) {
		goto _SegmentError;
	}

	mySegment = this->segmentStack.top();

	switch (n->lex->lex->id) {
	case kScriptLexIf:

		/* Set segment's offset */
		argSegment.SetOffset(mySegment->GetPosition() - 7);

		/* Compile list with arguments */
		this->_Push(&argSegment);
		this->_Run(n->argList);
		this->_Pop();

		/* Read result */
		this->_Read(n, var0, var0);

		/* Check result for boolean type */
		if (!var0->GetClass()->IsBooleanLike()) {
			goto _BooleanError;
		}

		/* Load result from stack if needed */
		if (!var0->wasInStack) {
			this->OnLoad(var0);
		}

		blockSegment = n->var->GetSegment();

		/* Compile construction's block */
		this->_Push(blockSegment);
		this->_Run(n->blockList);
		this->_Pop();

		/* Move arguments segment to parent's */
		mySegment->Merge(&argSegment);

		/* Set jump before compiled block */
		this->GetByteCode()->New(JNZ)->Write(mySegment->GetSize() +
			mySegment->GetOffset() + blockSegment->GetSize() - 2);

		/* Move block segment to parent's */
		mySegment->Merge(blockSegment);

		/* For debug */
		this->GetByteCode()->New(NOOP);

		break;
	case kScriptLexElse:
		break;
	case kScriptLexWhile:

		/*	Set segment's offset, where 7 is 
			segment's offset (5 + 2). Digit two is
			magic (its really magic) and 5 is size
			of jump instruction. */

		argSegment.SetOffset(mySegment->GetPosition() - 7);

		/* Compile list with arguments */
		this->_Push(&argSegment);
		this->_Run(n->argList);
		this->_Pop();

		/* Read result */
		this->_Read(n, var0, var0);

		/* Check result for boolean type */
		if (!var0->GetClass()->IsBooleanLike()) {
			goto _BooleanError;
		}

		/* Load result from stack if needed */
		if (!var0->wasInStack) {
			this->OnLoad(var0);
		}

		blockSegment = n->var->GetSegment();

		/* Compile construction's block */
		this->_Push(blockSegment);
		this->_Run(n->blockList);
		this->_Pop();

		/* Move arguments segment to parent's */
		mySegment->Merge(&argSegment);

		/* Set jump before compiled block */
		this->GetByteCode()->New(JNZ)->Write(mySegment->GetPosition() +
			blockSegment->GetSize() - 2 + 5);

		/* Move block segment to parent's */
		mySegment->Merge(blockSegment);

		/* Set jump after compiled block for jump */
		this->GetByteCode()->New(JUMP)->Write(argSegment.GetOffset());

		/* For debug */
		this->GetByteCode()->New(NOOP);

		break;
	case kScriptLexDo:
		break;
	case kScriptLexFor:
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
	PostSyntaxError(n->lex->line, "Construction (%s) requires boolean argument", n->word.data());

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

	if (n->var->CheckModificator(Object::Modificator::Primitive)) {
		this->_Read(n, leftVar, leftVar);
		if (!leftVar->CheckType(Object::Type::Variable)) {
			PostSyntaxError(n->lex->line, "Array parameter must be variable (%s)", leftVar->GetName().data());
		}
		this->variableStack.Push(leftVar->GetVariable());
	}
	else {

		for (Uint32 i = 0; i < n->lex->args; i++) {

			VariablePtr var = *(this->variableStack.GetVarList().end() - i - 1);

			objectList.push_back(var->GetClass());
			formattedParameters.append(var->GetClass()->GetName());

			if (i != n->lex->args - 1) {
				formattedParameters.append(", ");
			}
		}

		this->_Print(" // - %s(%s)\n", n->word.data(), formattedParameters.data());

		for (Uint32 i = 0; i < n->lex->args; i++) {
			this->OnLoad(this->variableStack.Pop());
		}

		invokeHash = Method::ComputeInvokeHash(objectList);
		methodHash = n->var->GetNode()->methodHash;
		methodVar = n->var->Find(Uint64(n->var->GetHash32()) << 32 | invokeHash);

		struct OrderedMethod {
		public:
			MethodPtr method;
			Uint32 distance;
		public:
			inline bool operator < (const OrderedMethod& om) const {
				return this->distance < om.distance;
			}
		};

		Vector<OrderedMethod> methodList;
		Uint32 distance = 0;

		if (!methodVar) {
			for (ObjectPtr m : n->var->GetParent()->GetMethodSet()) {
				if (n->word == m->GetName() && n->lex->args == m->GetMethod()->GetAttributeHash().size()) {
					for (Uint32 i = 0; i < n->lex->args; i++) {
						if (objectList[i]->GetPriority() > m->GetMethod()->GetAttributeHash()[i]->GetPriority()) {
							goto _WrongMethod;
						}
						distance += std::labs(objectList[i]->GetPriority() - m->GetMethod()->GetAttributeHash()[i]->GetPriority());
					}
					methodList.push_back({ m->GetMethod(), distance });
				_WrongMethod:
					distance = 0;
				}
			}

			if (methodList.empty()) {
				PostSyntaxError(n->lex->line, "Undeclared method %s(%s)", n->word.data(), formattedParameters.data());
			}

			std::sort(methodList.begin(), methodList.end());

			methodVar = methodList.begin()->method;
		}

		this->OnInvoke(methodVar->GetMethod());
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

	OnCast(methodVar->returnVar, methodVar->GetReturnType());

	if (returnVar) {
		this->OnReturn(methodVar->GetReturnType());
	}
}

Void VirtualCompiler::_Finish(NodePtr n) {

	if (this->variableStack.Size() > 1) {

		this->variableStack.Pop();

#if 0 /* Yes, its an error, but not now */
		PostSyntaxError(this->variableStack.Back()->GetNode()->lex->line, "Unexcepted token in expression (%s)",
			this->variableStack.Back()->GetNode()->lex->line);
#endif
	}

	this->variableStack.Clear();
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