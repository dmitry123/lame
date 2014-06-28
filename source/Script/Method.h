#ifndef __LAME_SCRIPT__METHOD__
#define __LAME_SCRIPT__METHOD__

#include "Object.h"
#include "Error.h"

LAME_BEGIN2(Script)

class LAME_API ScriptMethod : public ScriptObject {
	friend class ScriptClass;
public:
	typedef Void(*NativeMethod)(ScriptMethodPtr method);
public:
	ScriptMethod(
		BufferRefC methodName,
		ScriptObjectPtr thisClass,
		ScriptObjectPtr returnClass,
		Vector<Uint32> attributes = {});
public:
	ScriptClassPtr GetClass() override;
public:
	inline ScriptMethodPtr GetMethod() override {
		return this;
	}
public:
	Void Trace(Uint32 offset) override;
public:
	ScriptError SetNativeMethod(NativeMethod method);
	ScriptError Invoke(Vector<ScriptVarPtr> attributes = {});
private:
	Void SetThis(ScriptVarPtr thisClass) {
		this->thisClass = thisClass;
	}
	Void SetReturnType(ScriptClassPtr returnClass) {
		this->returnClass = returnClass;
	}
public:
	inline ScriptNodePtr GetRootNode() {
		return this->rootNode;
	}
	inline ScriptVarPtr GetThis() {
		return this->thisClass;
	}
	inline NativeMethod GetNativeMethod() {
		return this->nativeMethod;
	}
private:
	Vector<Uint32> attributesHash;
	ScriptClassPtr returnClass;
	ScriptVarPtr thisClass;
	NativeMethod nativeMethod;
	ScriptNodePtr rootNode;
};

LAME_END2

#endif // ~__LAME_SCRIPT__METHOD__