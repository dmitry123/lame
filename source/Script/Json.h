#ifndef __LAME_SCRIPT__JSON__
#define __LAME_SCRIPT__JSON__

#include "Define.h"
#include "Exception.h"

LAME_BEGIN

typedef class ScriptJsonNode *ScriptJsonNodePtr;
typedef class ScriptJson *ScriptJsonPtr;
typedef class ScriptJson ScriptNativeJson;

class LAME_API ScriptJsonNode {
public:
	ScriptJsonNode() {
	}
public:
	ScriptJsonNode(Buffer fieldName) :
		fieldName(fieldName)
	{
	}
public:
	ScriptNativeInt GetInt(Void);
	ScriptNativeFloat GetFloat(Void);
	ScriptNativeString GetString(Void);
	ScriptNativeBool GetBool(Void);
	ScriptNativeJson GetJson(Void);
public:
	Void SetInt(ScriptNativeInt value);
	Void SetFloat(ScriptNativeFloat value);
	Void SetString(ScriptNativeString value);
	Void SetBool(ScriptNativeBool value);
	Void SetJson(ScriptNativeJson value);
public:
	Buffer fieldName;
	Buffer fieldValue;
};

class LAME_API ScriptJson {
public:
	ScriptJson() {
		jsonRoot_.reset(new ScriptJsonNode());
	}
public:
	Buffer GenerateString(Void);
	ScriptJson& Parse(Buffer string);
public:
	inline SharedPtr<ScriptJsonNode> GetRoot() {
		return this->jsonRoot_;
	}
public:
	inline operator Buffer() {
		return this->GenerateString();
	}
private:
	SharedPtr<ScriptJsonNode> jsonRoot_;
};

LAME_END

#endif // ~__LAME_SCRIPT__JSON__