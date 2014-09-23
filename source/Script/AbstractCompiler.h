#ifndef __LAME_SCRIPT_ABSTRACT_COMPILER__
#define __LAME_SCRIPT_ABSTRACT_COMPILER__

#include "Assembler.h"
#include "ByteCode.h"
#include "Segment.h"
#include "StackVar.h"
#include "CodeBuilder.h"
#include "NodeWalker.h"

#include <unordered_set>

LAME_BEGIN2(Script)

class LAME_API AbstractCompiler : public NodeWalker::Listener {
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
		virtual Void onCondition(ObjectPtr object) = 0;
		virtual Void onReturn(ObjectPtr object) = 0;
		virtual Void onLoad(ObjectPtr object) = 0;
		virtual Void onStore(ObjectPtr object) = 0;
		virtual Void onInvoke(ObjectPtr method) = 0;
		virtual Void onCast(ObjectPtr object, ClassPtr type) = 0;
		virtual Void onClone(ObjectPtr object) = 0;
		virtual Void onNew(ObjectPtr object, Uint32 size) = 0;
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
		inline Void onBinary(ObjectPtr left, ObjectPtr right) override;
		inline Void onUnary(ObjectPtr object)                 override;
		inline Void onCondition(ObjectPtr object)             override;
		inline Void onReturn(ObjectPtr object)                override;
		inline Void onLoad(ObjectPtr object)                  override;
		inline Void onStore(ObjectPtr object)                 override;
		inline Void onInvoke(ObjectPtr method)                override;
		inline Void onCast(ObjectPtr object, ClassPtr type)   override;
		inline Void onClone(ObjectPtr object)                 override;
		inline Void onNew(ObjectPtr object, Uint32 size)      override;
	} *InvokerPtr;
public:
	AbstractCompiler() : Listener(&nodeWalker),
		invoker(this), byteCode(NULL)
	{
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
		delete inspector;
	}
private:
	UnorderedSet<InspectorPtr> inspectorSet;
	Invoker invoker;
public:
	Void Run(CodeBuilderPtr   codeBuilder,
		     SyntaxBuilderPtr syntaxBuilder,
			 ScopePtr         rootScope,
			 SegmentPtr       codeSegment);
public:
	typedef struct {
		SegmentPtr argument;
		SegmentPtr body;
		SegmentPtr otherwise;
	} SegmentInfo, *SegmentInfoPtr;
private:
	typedef Deque<NodePtr>  NodeList;
	typedef Deque<NodePtr>& NodeListRef;
private:
	Void onBinary(NodePtr n) override;
	Void onUnary(NodePtr n) override;
	Void onTernary(NodePtr n) override;
	Void onCondition(NodePtr n) override;
	Void onInvoke(NodePtr n) override;
	Void onNew(NodePtr n) override;
	Void onReturn(NodePtr n) override;
	Void onIndex(NodePtr n) override;
	Void onLoad(NodePtr n) override;
private:
	Void onMethodBegin(MethodPtr m) override;
	Void onMethodEnd(MethodPtr m) override;
public:
	inline ByteCodePtr GetByteCode()      { return this->byteCode;      }
	inline Uint32      GetSegmentOffset() { return this->segmentOffset; }
public:
	inline SegmentInfoPtr GetSegmentInfo() {
		return &this->segmentInfo;
	}
private:
	Void _Write(ObjectPtr object);
	Void _StrongCast(VariablePtr variable, ObjectPtr type);
	Void _Compile(NodeListRef nodeList, SegmentPtr segment);
private:
	NodeWalker nodeWalker;
	Vector<Uint32> addressStack;
	Stack<SegmentPtr> segmentStack;
	Vector<SegmentPtr> segmentList;
	Uint32 segmentOffset;
	SyntaxBuilderPtr nodeBuilder;
	Uint32 methodHash;
	ByteCodePtr byteCode;
	SegmentInfo segmentInfo;
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

Void AbstractCompiler::Invoker::onCondition(ObjectPtr object) {
	for (InspectorPtr i : this->GetCompiler()->inspectorSet) {
		i->onCondition(object);
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

Void AbstractCompiler::Invoker::onNew(ObjectPtr object, Uint32 size) {
	for (InspectorPtr i : this->GetCompiler()->inspectorSet) {
		i->onNew(object, size);
	}
}

LAME_END2

#endif // ~__LAME_SCRIPT_ABSTRACT_COMPILER__