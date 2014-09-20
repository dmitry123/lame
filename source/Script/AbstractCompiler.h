#ifndef __LAME_SCRIPT_ABSTRACT_COMPILER__
#define __LAME_SCRIPT_ABSTRACT_COMPILER__

#include "Assembler.h"
#include "ByteCode.h"
#include "Segment.h"
#include "StackVar.h"
#include "CodeBuilder.h"

#include <unordered_set>

LAME_BEGIN2(Script)

class LAME_API AbstractCompiler {
	template <class T> using UnorderedSet = std::unordered_set<T>;
public:
	typedef class Inspector {
	protected:
		Inspector(AbstractCompilerPtr abstractCompiler) :
			abstractCompiler(abstractCompiler)
		{
		}
	public:
		virtual Void onBinary(ObjectPtr left, ObjectPtr right) = 0;
		virtual Void onUnary(ObjectPtr object) = 0;
		virtual Void onCondition(NodePtr node) = 0;
		virtual Void onReturn(ObjectPtr object) = 0;
		virtual Void onLoad(ObjectPtr object) = 0;
		virtual Void onStore(ObjectPtr object) = 0;
		virtual Void onInvoke(ObjectPtr method) = 0;
		virtual Void onCast(ObjectPtr object, ClassPtr type) = 0;
		virtual Void onClone(ObjectPtr object) = 0;
	public:
		inline AbstractCompilerPtr GetCompiler() {
			return this->abstractCompiler;
		}
	private:
		AbstractCompilerPtr abstractCompiler;
	} *InspectorPtr;
public:
	typedef class Invoker : public Inspector {
	public:
		Invoker(AbstractCompilerPtr abstractCompiler) :
			Inspector(abstractCompiler)
		{
		}
	public:
		Void onBinary(ObjectPtr left, ObjectPtr right) override;
		Void onUnary(ObjectPtr object)                 override;
		Void onCondition(NodePtr node)                 override;
		Void onReturn(ObjectPtr object)                override;
		Void onLoad(ObjectPtr object)                  override;
		Void onStore(ObjectPtr object)                 override;
		Void onInvoke(ObjectPtr method)                override;
		Void onCast(ObjectPtr object, ClassPtr type)   override;
		Void onClone(ObjectPtr object)                 override;
	} *InvokerPtr;
public:
	AbstractCompiler() :
		invoker(this)
	{
		this->currentMethod = NULL;
		this->byteCode = NULL;
		this->lastResult = NULL;
	}
public:
	virtual ~AbstractCompiler() {
		delete this->byteCode;
	}
public:
	inline InspectorPtr Inspect(InspectorPtr inspector) {
		if (this->inspectorSet.count(inspector)) {
			return NULL;
		}
		return *this->inspectorSet.insert(inspector).first;
	}
	inline Void Release(InspectorPtr inspector) {
		if (this->inspectorSet.count(inspector)) {
			this->inspectorSet.erase(inspector);
		}
	}
private:
	UnorderedSet<InspectorPtr> inspectorSet;
	Invoker invoker;
public:
	Void Run(CodeBuilderPtr   codeBuilder,
		     SyntaxBuilderPtr syntaxBuilder,
			 ScopePtr         rootScope,
			 SegmentPtr       codeSegment);
private:
	Void _CompileMethods(CodeBuilderPtr codeBuilder,
		SegmentPtr codeSegment);
private:
	typedef Core::Deque<NodePtr> NodeList;
	typedef Core::Deque<NodePtr>& NodeListRef;
public:
	inline NodePtr     GetCurrentNode()   { return this->currentNode;   }
	inline ByteCodePtr GetByteCode()      { return this->byteCode;      }
	inline ObjectPtr   GetCurrentMethod() { return this->currentMethod; }
	inline ScopePtr    GetRootScope()     { return this->rootScope;     }
	inline Uint32      GetSegmentOffset() { return this->segmentOffset; }
	inline ObjectPtr   GetLastResult()    { return this->lastResult;    }
private:
	Void _Run(NodeListRef nodeList, Bool backupStack = 0);
	Void _Read(NodePtr node, VariablePtr& left, VariablePtr& right);
	Void _Write(ObjectPtr object);
	Void _Cast(VariablePtr variable, ObjectPtr type);
	Void _StrongCast(VariablePtr variable, ObjectPtr type);
	Void _Binary(NodePtr node);
	Void _Unary(NodePtr node);
	Void _New(NodePtr node);
	Void _Selection(NodePtr node);
	Void _Condition(NodePtr node);
	Void _Invoke(NodePtr node);
	Void _Return(NodePtr node);
	Void _Test(NodePtr node);
private:
	Void _Push(SegmentPtr segment);
	SegmentPtr _Pop(Void);
private:
	Vector<ObjectPtr> classList;
	Vector<ObjectPtr> methodList;
	Vector<Uint32> addressStack;
	Stack<SegmentPtr> segmentStack;
	Vector<SegmentPtr> segmentList;
	Uint32 segmentOffset;
	ObjectPtr currentMethod;
	StackVar variableStack;
	SyntaxBuilderPtr nodeBuilder;
	Uint32 methodHash;
	ScopePtr rootScope;
	NodePtr currentNode;
	ByteCodePtr byteCode;
	VariablePtr lastResult;
};

Void AbstractCompiler::Invoker::onBinary(ObjectPtr left, ObjectPtr right) {
	for (InspectorPtr i : this->GetCompiler()->inspectorSet) {
		i->onBinary(left, right);
	}
}

Void AbstractCompiler::Invoker::onUnary(ObjectPtr object) {
	for (InspectorPtr i : this->GetCompiler()->inspectorSet) {
		i->onUnary(object);
	}
}

Void AbstractCompiler::Invoker::onCondition(NodePtr node) {
	for (InspectorPtr i : this->GetCompiler()->inspectorSet) {
		i->onCondition(node);
	}
}

Void AbstractCompiler::Invoker::onReturn(ObjectPtr object) {
	for (InspectorPtr i : this->GetCompiler()->inspectorSet) {
		i->onReturn(object);
	}
}

Void AbstractCompiler::Invoker::onLoad(ObjectPtr object) {
	for (InspectorPtr i : this->GetCompiler()->inspectorSet) {
		i->onLoad(object);
	}
}

Void AbstractCompiler::Invoker::onStore(ObjectPtr object) {
	for (InspectorPtr i : this->GetCompiler()->inspectorSet) {
		i->onStore(object);
	}
}

Void AbstractCompiler::Invoker::onInvoke(ObjectPtr method) {
	for (InspectorPtr i : this->GetCompiler()->inspectorSet) {
		i->onInvoke(method);
	}
}

Void AbstractCompiler::Invoker::onCast(ObjectPtr object, ClassPtr type) {
	for (InspectorPtr i : this->GetCompiler()->inspectorSet) {
		i->onCast(object, type);
	}
}

Void AbstractCompiler::Invoker::onClone(ObjectPtr object) {
	for (InspectorPtr i : this->GetCompiler()->inspectorSet) {
		i->onClone(object);
	}
}

LAME_END2

#endif // ~__LAME_SCRIPT_ABSTRACT_COMPILER__