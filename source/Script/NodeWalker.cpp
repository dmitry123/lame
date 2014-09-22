#include "NodeWalker.h"

LAME_BEGIN2(Script)

Void NodeWalker::Walk(ListenerPtr nodeListener, NodePtr rootNode, ScopePtr rootScope, VoidP userData) {

	LAME_ASSERT(nodeListener != NULL);
	LAME_ASSERT(rootNode     != NULL);
	LAME_ASSERT(rootScope    != NULL);

	this->nodeListener = nodeListener;
	this->rootNode = rootNode;
	this->rootScope = rootScope;
	this->userData = userData;

	this->lastSelection = NULL;
	this->currentNode = NULL;
	this->leftVar = NULL;
	this->rightVar = NULL;
	this->lastResult = NULL;
	this->lastNode = NULL;
	this->rememberedInvoke = NULL;

	if (!methodSet.empty()) {
		methodSet.clear();
	}
	stackVar.Clear();

	this->_FindMethod(rootScope);

	for (MethodPtr m : this->methodSet) {

		NodePtr n = m->GetRootNode();

		this->currentNode = n;
		this->currentMethod = m;

		for (NodePtr n2 : n->argList) {
			if (!n2->var->CheckType(Object::Type::Variable)) {
				PostSyntaxError(n->lex->line, "Argument must be variable (%s)", n2->var->GetName().data());
			}
			n2->var->writes_ = 1;
		}

		this->Run(m->GetRootNode()->blockList, TRUE);

		if (!m->GetReturnType()->IsVoid()) {
			if (!m->returnVar) {
				PostSyntaxError(n->lex->line, "Non-void method %s(%s) must return (%s)", m->GetName().data(),
					m->GetFormattedArguments().data(), m->GetReturnType()->GetName().data());
			}
		}
	}
}

Void NodeWalker::Run(NodeList nodeList, Bool backupStack) {

	StackVar stackBackup;

	if (backupStack) {
		stackBackup = this->stackVar;
	}

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
				this->nodeListener->onInvoke(this->rememberedInvoke);
			}
			this->rememberedInvoke = NULL;
		}

		try {
			if (n->lex->lex->id == kScriptLexReturn) {
				this->nodeListener->onReturn(n);
			}
			else if (n->id == kScriptNodeCondition) {
				this->nodeListener->onCondition(n);
			}
			else if (n->id == kScriptNodeInvoke) {

				/*	This is fix for directed selection, cuz
				we don't know where is static member invoke and
				where invocation via selector */

				if (this->currentNode->lex->lex->id != kScriptLexNew &&
					this->currentNode->lex->lex->id != kScriptLexDirected && !this->rememberedInvoke
				) {
					this->rememberedInvoke = n;
				}
				if (!this->rememberedInvoke) {
					this->nodeListener->onInvoke(n);
				}
			}
			else if (n->lex->lex->id == kScriptLexSemicolon) {
				this->_Cleanup();
			}
			else if (n->lex->lex->id == kScriptLexNew) {
				this->nodeListener->onNew(n);
			}
			else if (n->lex->lex->id == kScriptLexDirected) {
				this->_Selection(n);
			}
			else if (n->id == kScriptNodeAlloc) {
				if (n->parent->id != kScriptNodeClass) {
					this->nodeListener->onIndex(n);
				}
				else {
					this->nodeListener->onInvoke(n);
				}
			}
			else if (
				n->lex->lex->IsUnknown() ||
				n->lex->lex->IsConst()
			) {
				this->Push(n->var);
			}
			else if (n->lex->lex->IsMath() || n->lex->lex->IsBool()) {
				if (n->lex->args == 1) {
					this->nodeListener->onUnary(n);
				}
				else if (n->lex->args == 2) {
					this->nodeListener->onBinary(n);
				}
				else if (n->lex->args == 3) {
					this->nodeListener->onTernary(n);
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

	if (backupStack) {
		this->stackVar = stackBackup;
	}
}

Void NodeWalker::Push(ObjectPtr object) {

	if (!object) {
		this->stackVar.GetNodeList().push_back(this->currentNode);
		this->stackVar.GetVarList().push_back(NULL);
	}
	else {
		this->stackVar.Push(object);
	}
}

ObjectPtr NodeWalker::Pop(Void) {

	ObjectPtr var = this->stackVar.Pop();

	if (!this->stackVar.Size()) {
		PostSyntaxError(this->currentNode->lex->line, "Excepted variable for (%s)",
			this->currentNode->word.data());
	}

	return var;
}

Void NodeWalker::Test(NodePtr node) {

	ObjectPtr left;
	ObjectPtr right;

	if (this->stackVar.Size() < node->lex->args || !node->lex->args) {
		PostSyntaxError(node->lex->line, "Excepted %d arguments for (%s)",
			node->lex->args, node->word.data());
	}

	if (node->lex->args > 1) {

		right = this->stackVar.Back()
			->GetVariable();

		this->stackVar.GetVarList().pop_back();

		if (node->lex->lex->id != kScriptLexDirected) {
			if (!right) {
				PostSyntaxError(node->lex->line, "Undeclared variable (%s)",
					this->stackVar.GetNodeList().back()->word.data());
			}
			this->stackVar.GetNodeList().pop_back();
		}
	}

	left = this->stackVar.Back()
		->GetVariable();

	this->stackVar.GetVarList().pop_back();

	if (!left) {
		PostSyntaxError(node->lex->line, "Undeclared variable (%s)",
			this->stackVar.GetNodeList().back()->word.data());
	}

	if (node->lex->lex->id != kScriptLexDirected) {
		this->stackVar.GetNodeList().pop_back();
	}

	this->leftVar = left;
	this->rightVar = right;
}

Void NodeWalker::_FindMethod(ScopePtr scope) {

	for (auto i : scope->GetHashMap()) {
		if (i.second->CheckType(Object::Type::Method)) {
			this->methodSet.insert(i.second->GetMethod());
		}
		this->_FindMethod(i.second);
	}
}

Void NodeWalker::_Cleanup(Void) {

	if (this->lastNode->lex->lex->IsLeft() && this->stackVar.Size()) {
		this->stackVar.Pop();
	}

	for (ObjectPtr v : this->stackVar.GetVarList()) {
		if (v && v->GetNode() && v->GetNode()->id == kScriptNodeDefault) {
			PostSyntaxError(this->currentNode->lex->line, "Unexcepted token in expression (%s)",
				this->stackVar.Back()->GetName().data());
		}
	}

	this->lastSelection = NULL;
	this->leftVar = NULL;
	this->rightVar = NULL;
	this->lastResult = NULL;
	this->lastNode = NULL;
	this->rememberedInvoke = NULL;

	this->stackVar.Clear();
}

Void NodeWalker::_Selection(NodePtr node) {

	ObjectPtr leftVar;
	ObjectPtr rightVar;
	Buffer fieldName;
	ObjectPtr fieldObject;
	NodePtr fieldNode;

	this->Test(node);

	rightVar = this->GetRight();
	leftVar = this->GetLeft();

	fieldNode = this->stackVar.GetNodeList().back();
	fieldName = fieldNode->word;

	this->stackVar.GetNodeList().pop_back();
	this->stackVar.GetNodeList().pop_back();

	auto rightVarType = rightVar->GetVariable()->GetVarType();
	auto leftVarType = leftVar->GetVariable()->GetVarType();

	if (leftVarType != Variable::Var::Object &&
		rightVarType != Variable::Var::Array
	) {
		PostSyntaxError(node->lex->line, "You can access to object's fields only (%s)",
			leftVar->GetName().data());
	}

	if (fieldNode->id == kScriptNodeInvoke) {
		this->_Invoke(fieldNode);
		fieldObject = fieldNode->var;
	}
	else {
		if (leftVarType == Variable::Var::Array) {
			fieldObject = Scope::classArray->Find(fieldName, FALSE);
		} else {
			fieldObject = leftVar->GetClass()->Find(fieldName, FALSE);
		}
	}

	if (!fieldObject) {
		PostSyntaxError(node->lex->line, "Undeclared object's field (%s.%s)",
			leftVar->GetClass()->GetName().data(), fieldName.data());
	}

	if (fieldObject->CheckModificator(Object::Modificator::Private) ||
		fieldObject->CheckModificator(Object::Modificator::Protected)
	) {
		if (!leftVar->GetOwner()->Find(fieldObject->Hash())) {
			PostSyntaxError(node->lex->line, "Unable to get private/protected field (%s.%s)",
				leftVar->GetClass()->GetName().data(), fieldName.data());
		}
	}

	fieldObject->SetThis(leftVar);

	if (!fieldObject->CheckType(Object::Type::Method)) {
		this->stackVar.Push(fieldObject);
	}

	this->lastNode->var = fieldObject;
	this->lastSelection = fieldObject;
}

Void NodeWalker::_Invoke(NodePtr n) {

	ObjectPtr methodVar;
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

	/*	If we have nessesary variable in node
	then we can skip this action */

	if (n->var) {
		return;
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

	/*	But, if we can't find our method fast (it means, that sended
	parameters will be the same, like in method without weak cast,
	like foo(100, 20, 3) can apply fast invoke for foo(byte, byte, byte) method),
	else we have to find method with the same name but with another parameters
	types which can be casted to sended arguments */

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

	/*	Return method's return variable and
	push it stack */

	if (methodVar->GetMethod()->returnVar) {
		this->stackVar.Push(methodVar->GetMethod()->returnVar);
	}
}

LAME_END2