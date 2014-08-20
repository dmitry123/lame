#include "VirtualCompiler.h"

LAME_BEGIN2(Compiler)

using namespace Core;
using namespace Script;

Void VirtualCompiler::Run(NodeListRef nodeList) {

	VariableStack variableStackBacup = this->variableStack;

	for (NodePtr n : nodeList) {
		this->currentNode = n;
		try {
			if (n->id == kScriptNodeClass ||
				n->id == kScriptNodeInterface ||
				n->id == kScriptNodeFunction
			) {
				continue;
			}
			else if (n->lex->lex->id == kScriptLexReturn) {
				this->_AnalizeReturn(n);
			}
			else if (n->id == kScriptNodeCondition) {
				this->_AnalizeCondition(n);
			}
			else if (n->id == kScriptNodeInvoke) {
				if (!n->var) {
					PostSyntaxError(n->lex->line, "Undeclared class (%s)", n->word.data());
				}
				if (n->lex->args == 1 && n->var->CheckType(Class::Type::Class)) {
					this->_AnalizeCast(n);
				}
				else {
					if (!n->var->CheckType(Object::Type::Class)) {
						this->_AnalizeInvoke(n);
					}
					else {
						goto _SaveNode;
					}
				}
			}
			else if (n->id == kScriptNodeAlloc) {
				if (!n->var) {
					PostSyntaxError(n->lex->line, "Undeclared class (%s)", n->word.data());
				}
				this->_AnalizeInvoke(n);
				if (n->parent->id != kScriptNodeClass) {
					goto _SaveNode;
				}
			}
			else if (n->lex->lex->IsUnknown() || n->lex->lex->IsConst()) {
			_SaveNode:
				if (n->id == kScriptNodeVariable) {
					this->variableStack.Push(VariablePtr(n->var));
				}
				else {
					this->variableStack.Push(VariablePtr(n->var));
				}
			}
			else if (n->lex->lex->id == kScriptLexSemicolon) {
				this->variableStack.Clear();
			}
			else if (n->lex->lex->id == kScriptLexNew) {
				this->_AnalizeNew(n);
			}
			else if (n->lex->lex->id == kScriptLexDirected) {
				this->_AnalizeSelection(n);
			}
			else if (n->lex->lex->IsMath() || n->lex->lex->IsBool()) {
				if (n->lex->args == 1) {
					this->_AnalizeUnary(n);
				}
				else if (n->lex->args == 2) {
					this->_AnalizeBinary(n);
				}
				else {
					this->_AnalizeInvoke(n);
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

	this->variableStack = variableStackBacup;
}

Void VirtualCompiler::Read(NodePtr n, VariablePtr& left, VariablePtr& right) {

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

Void VirtualCompiler::Write(VariablePtr var) {

	this->variableStack.Push(var);
	this->OnLoad(var);
}

Void VirtualCompiler::Print(StringC message, ...) {

	VaList vaList;

	va_start(vaList, message);
	vprintf(message, vaList);
	va_end(vaList);
}

Void VirtualCompiler::Analize(NodeBuilderPtr nodeBuilder, ScopePtr rootScope) {

	this->nodeBuilder = nodeBuilder;
	this->rootScope = rootScope;

	this->_ForEachClass(this->rootScope);
	this->_ForEachMethod(this->rootScope);

	for (ObjectPtr i : this->classList) {
		this->Run(i->GetNode()->blockList);
	}

	for (ObjectPtr i : this->methodList) {

		i->segmentCodePosition = this->byteCodePrinter->GetPosition();

		if (i->GetPath().length() > 0) {
			this->Print(" // %s/%s(%s)\n", i->GetPath().data(), i->GetName().data(),
				i->GetMethod()->GetFormattedArguments().data());
		}
		else {
			this->Print(" // %s(%s)\n", i->GetName().data(),
				i->GetMethod()->GetFormattedArguments().data());
		}

		NodePtr n = i->GetMethod()->GetRootNode();

		for (NodePtr n2 : n->argList) {
			if (!n2->var->CheckType(Object::Type::Variable)) {
				PostSyntaxError(n->lex->line, "Argument must be variable (%s)", n2->var->GetName().data());
			}
			this->OnStore(n2->var->GetVariable());
		}

		this->Run(i->GetMethod()->GetRootNode()->blockList);
	}

	this->Run(this->nodeBuilder->GetRootNode()->blockList);
}

Void VirtualCompiler::_ForEachClass(ScopePtr rootScope) {

	for (ObjectPtr i : rootScope->GetClassSet()) {
		if (!i->CheckModificator(Object::Modificator::Primitive) && i->GetNode()) {
			this->classList.push_back(i);
			this->_ForEachClass(i);
		}
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
			this->_ForEachClass(i);
		}
	}
}

Void VirtualCompiler::_AnalizeBinary(NodePtr n) {

	VariablePtr sourceVar;
	VariablePtr leftVar;
	VariablePtr leftVar2;
	VariablePtr rightVar;
	VariablePtr rightVar2;

	leftVar2 = this->variableStack.Back();

	this->Read(n, leftVar, rightVar);

	leftVar2 = leftVar;
	rightVar2 = rightVar;

	//this->Print(" // %s %s %s\n", leftVar->GetName().data(),
	//	n->word.data(), rightVar->GetName().data());

	if (n->lex->lex->IsRight()) {

		sourceVar = leftVar->GetClassType()->GetPriority() >= rightVar->GetClassType()->GetPriority() ?
		leftVar : rightVar;

		if (!leftVar->wasInStack) {
			this->OnLoad(leftVar);
		}

		sourceVar->wasInStack = TRUE;

		if (leftVar->GetClassType()->Hash() != rightVar->GetClassType()->Hash()) {
			if (leftVar->GetClassType()->Hash() != sourceVar->GetClassType()->Hash()) {
				this->OnCast(leftVar, sourceVar);
			}
			if (rightVar->GetClassType()->Hash() != sourceVar->GetClassType()->Hash()) {
				this->OnCast(rightVar, sourceVar);
			}
		}

		if (!rightVar->wasInStack) {
			this->OnLoad(rightVar);
		}
	}
	else {
		if (!rightVar->wasInStack) {
			this->OnLoad(rightVar);
		}

		rightVar->wasInStack = TRUE;

		if (leftVar->GetClassType()->Hash() != rightVar->GetClassType()->Hash()) {
			this->OnCast(leftVar, rightVar);
		}

		sourceVar = leftVar2;
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

	this->variableStack.Push(sourceVar);
}

Void VirtualCompiler::_AnalizeUnary(NodePtr n) {

	VariablePtr leftVar;

	this->Read(n, leftVar, leftVar);

	//this->Print(" // %s%s\n", n->word.data(),
	//	leftVar->GetName().data());

	this->OnLoad(leftVar);
	this->OnUnary(leftVar);

	leftVar->wasInStack = TRUE;
	this->variableStack.Push(leftVar);
}

Void VirtualCompiler::_AnalizeNew(NodePtr n) {

	VariablePtr leftVar;
	VariablePtr rightVar;

	this->Read(n, leftVar, leftVar);

	if (leftVar->CheckModificator(Object::Modificator::Primitive)) {
		this->Read(n, rightVar, rightVar);
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
				this->Run(n2->blockList);
			}
		}
		if (constructorVar && FALSE) {
			if (constructorVar->GetMethod()->GetRootNode()) {
				this->Run(constructorVar->GetMethod()->GetRootNode()->blockList);
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

	this->variableStack.Push(leftVar);
}

Void VirtualCompiler::_AnalizeSelection(NodePtr n) {

	VariablePtr leftVar;
	VariablePtr rightVar;
	Buffer fieldName;
	ObjectPtr fieldObject;

	this->Read(n, leftVar, rightVar);

	fieldName = this->variableStack.GetNameList().back();

	this->variableStack.GetNameList().pop_back();
	this->variableStack.GetNameList().pop_back();

	if (leftVar->GetVarType() != Variable::Var::Object) {
		PostSyntaxError(n->lex->line, "You can access to object's fields only (%s)", fieldName.data());
	}

	fieldObject = leftVar->GetClassType()->Find(fieldName);

	if (!fieldObject) {
		PostSyntaxError(n->lex->line, "Undeclared object's field (%s.%s)", leftVar->GetClass()->GetName().data(), fieldName.data());
	}
	rightVar = fieldObject->GetVariable();

	// selection

	this->variableStack.Push(rightVar);
	//	varStack.push_back(rightVar);
	//	nameStack.push_back(fieldName);
}

Void VirtualCompiler::_AnalizeCondition(NodePtr n) {

	VariablePtr var0;
	//	VariablePtr var1;
	//	VariablePtr var2;

	switch (n->lex->lex->id) {
	case kScriptLexIf:
		this->Read(n, var0, var0);
		break;
	case kScriptLexElse:
		break;
	case kScriptLexWhile:
		this->Read(n, var0, var0);
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
}

Void VirtualCompiler::_AnalizeCast(NodePtr n) {

	VariablePtr leftVar;
	ClassPtr rightVar;

	this->Read(n, leftVar, leftVar);

	rightVar = n->var->GetClass();

	__asm int 3

	this->variableStack.Push(leftVar);
}

Void VirtualCompiler::_AnalizeInvoke(NodePtr n) {

	VariablePtr leftVar;
	ObjectPtr methodVar;
	Uint32 methodHash;
	Vector<ClassPtr> objectList;
	Uint32 invokeHash;
	Buffer formattedParameters;

	if (n->var->CheckModificator(Object::Modificator::Primitive)) {
		this->Read(n, leftVar, leftVar);
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

		//this->Print(" // %s(%s)\n", n->word.data(), formattedParameters.data());

		for (Uint32 i = 0; i < n->lex->args; i++) {
			this->OnLoad(this->variableStack.Pop());
		}

		invokeHash = Method::ComputeInvokeHash(objectList);
		methodHash = n->var->GetNode()->methodHash;
		methodVar = n->var->Find(Uint64(n->var->GetPathHash32()) << 32 | invokeHash);

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

		if (!methodVar->GetMethod()->GetReturnType()->IsVoid()) {
			if (!methodVar->GetMethod()->returnVar) {
				PostSyntaxError(n->lex->line, "Non-void method (%s) must return (%s)", methodVar->GetName().data(),
					methodVar->GetMethod()->GetReturnType()->GetName().data());
			}
			this->variableStack.Push(methodVar->GetMethod()->returnVar);
		}
	}
}

Void VirtualCompiler::_AnalizeReturn(NodePtr n) {

	VariablePtr returnVar;
	ObjectPtr methodVar;

	if (n->lex->args == 1) {

		this->Read(n, returnVar, returnVar);
		this->variableStack.Push(returnVar);

		methodVar = returnVar;

		while (methodVar && !methodVar->CheckType(Object::Type::Method)) {
			methodVar = ObjectPtr(methodVar->GetParent());
		}

		if (methodVar) {
			methodVar->GetMethod()->returnVar = returnVar;
		}

		this->OnReturn(methodVar->GetMethod()->returnVar);
	}
	else {
		this->OnReturn(NULL);
	}
}

LAME_END2