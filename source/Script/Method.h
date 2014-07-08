#ifndef __LAME_SCRIPT__METHOD__
#define __LAME_SCRIPT__METHOD__

#include "Object.h"
#include "Error.h"
#include "Node.h"

LAME_BEGIN2(Script)

class LAME_API Method : public Object {
	friend class Class;
public:
	typedef Void(*NativeMethod)(MethodPtr method);
public:
	Method(
		BufferRefC methodName,
		NodePtr node,
		ObjectPtr thisClass,
		ObjectPtr returnClass,
		Vector<ClassPtr> attributes = {});
public:
	ClassPtr GetClass() override;
public:
	inline MethodPtr GetMethod() override {
		return this;
	}
public:
	Void Trace(Uint32 offset) override;
	Uint64 Hash(Void) const override;
public:
	Error SetNativeMethod(NativeMethod method);
	Error Invoke(Vector<VariablePtr> attributes = {});
	Bool CompareArguments(MethodPtr method);
private:
	Void SetThis(VariablePtr thisClass) {
		this->thisClass = thisClass;
	}
	Void SetReturnType(ClassPtr returnClass) {
		this->returnClass = returnClass;
	}
public:
	inline Void SetRootNode(NodePtr node) {
		this->rootNode = node;
	}
	inline NodePtr GetRootNode() {
		return this->rootNode;
	}
	inline VariablePtr GetThis() {
		return this->thisClass;
	}
	inline NativeMethod GetNativeMethod() {
		return this->nativeMethod;
	}
	inline Uint32 GetInvokeHash() {
		return this->invokeHash;
	}
public:
	static Uint32 ComputeInvokeHash(Vector<ClassPtr>& classList);
	static Uint32 ComputeInvokeHash(Vector<VariablePtr>& classList);
private:
	Vector<ClassPtr> attributesHash;
	ClassPtr returnClass;
	VariablePtr thisClass;
	NativeMethod nativeMethod;
	NodePtr rootNode;
	Uint32 invokeHash;
};

LAME_END2

#endif // ~__LAME_SCRIPT__METHOD__