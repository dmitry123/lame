#ifndef __LAME_SCRIPT__METHOD__
#define __LAME_SCRIPT__METHOD__

#include "Object.h"
#include "Segment.h"

LAME_BEGIN2(Script)

class LAME_API Method : public Object {
	friend class Class;
public:
	typedef Void(*NativeMethod)(MethodPtr method);
public:
	Method(BufferRefC name, ScopePtr parent, ObjectPtr thisClass, ObjectPtr returnClass,
		Vector<ClassPtr> attributes = {});
public:
	inline ClassPtr GetClass() override {
		return this->thisClass->GetClass();
	}
	inline MethodPtr GetMethod() override {
		return this;
	}
public:
	Bool Equal(ObjectPtrC object) final override;
	ObjectPtr Clone(BufferRefC name, ObjectPtr parent) final override;
	Void Trace(Uint32 offset) final override;
	HashType Hash(Void) final override;
	Uint32 Size(Void) final override;
public:
	Void SetNativeMethod(NativeMethod method);
	Buffer FormatArguments(Void);
public:
	inline Void SetThis(ObjectPtr thisClass)        { this->thisClass = thisClass;     }
	inline Void SetReturnType(ClassPtr returnClass) { this->returnClass = returnClass; }
	inline Void SetRootNode(NodePtr node)           { this->rootNode = node;           }
public:
	inline ObjectPtr        GetThis()          { return this->thisClass;      }
	inline ClassPtr         GetReturnType()    { return this->returnClass;    }
	inline NodePtr          GetRootNode()      { return this->rootNode;       }
	inline Vector<ClassPtr> GetAttributeHash() { return this->attributesHash; }
	inline NativeMethod     GetNativeMethod()  { return this->nativeMethod;   }
	inline Uint32           GetInvokeHash()    { return this->invokeHash;     }
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
	Segment segment;
public:
	VariablePtr returnVar;
};

LAME_END2

#endif // ~__LAME_SCRIPT__METHOD__