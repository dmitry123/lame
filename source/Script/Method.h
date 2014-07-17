#ifndef __LAME_SCRIPT__METHOD__
#define __LAME_SCRIPT__METHOD__

#include "Object.h"

LAME_BEGIN2(Script)

class LAME_API Method : public Object {
	friend class Class;
public:
	typedef Void(*NativeMethod)(MethodPtr method);
public:
	Method(BufferRefC name, ScopePtr parent, ObjectPtr thisClass, ObjectPtr returnClass,
		Vector<ClassPtr> attributes = {});
public:
	ClassPtr GetClass() override;
	MethodPtr GetMethod() override;
public:
	Bool Equal(ObjectPtrC object) final override;
	ObjectPtr Clone(BufferRefC name, ObjectPtr parent) final override;
	Void Trace(Uint32 offset) final override;
	HashType Hash(Void) final override;
	Uint32 Size(Void) final override;
	Void Release(Void) final override;
public:
	Void SetNativeMethod(NativeMethod method);
	Void Invoke(Vector<VariablePtr> attributes = {});
public:
	Void SetThis(ObjectPtr thisClass) { this->thisClass = thisClass; }
	inline ObjectPtr GetThis() { return this->thisClass; }
	Void SetReturnType(ClassPtr returnClass) { this->returnClass = returnClass; }
	inline ClassPtr GetReturnType() { return this->returnClass; }
	inline Void SetRootNode(NodePtr node) { this->rootNode = node; }
	inline NodePtr GetRootNode() { return this->rootNode; }
public:
	inline NativeMethod GetNativeMethod() { return this->nativeMethod; }
	inline Uint32 GetInvokeHash() { return this->invokeHash; }
public:
	static Uint32 ComputeInvokeHash(Vector<ClassPtr>& classList);
	static Uint32 ComputeInvokeHash(Vector<VariablePtr>& classList);
private:
	Vector<ClassPtr> attributesHash;
	ClassPtr returnClass;
	ObjectPtr thisClass;
	NativeMethod nativeMethod;
	NodePtr rootNode;
	Uint32 invokeHash;
};

LAME_END2

#endif // ~__LAME_SCRIPT__METHOD__