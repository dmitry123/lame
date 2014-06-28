#ifndef __LAME_SCRIPT__SCOPE__
#define __LAME_SCRIPT__SCOPE__

#include "Define.h"
#include "Object.h"

LAME_BEGIN2(Script)

class LAME_API ScriptScope {
	friend class ScriptScopeController;
public:
	typedef Pair<Buffer, Uint32> StringPair;
	typedef Pair<Uint32, Uint32> HashPair;
	typedef Map<StringPair, ScriptObjectPtr> StringMap;
	typedef Map<HashPair, ScriptObjectPtr> HashMap;
public:
	inline ScriptObjectPtr Find(BufferRefC buffer) {
		return this->Find(buffer.data());
	}
public:
	ScriptObjectPtr Add(ScriptObjectPtr object);
	Void Remove(ScriptObjectPtr var);
	ScriptObjectPtr Find(StringC name);
	ScriptObjectPtr Find(Uint32 hash);
	Void Merge(ScriptScopePtrC scope);
	Void Clone(ScriptScopePtrC scope);
	Uint32 Amount(Void);
	Void Clear(Void);
	Void Trace(Uint32 offset);
public:
	inline StringMap& GetStringMap() {
		return this->stringMap_;
	}
	inline HashMap& GetHashMap() {
		return this->hashMap_;
	}
public:
	~ScriptScope();
private:
	ScriptScope() {
		// locked
	}
private:
	StringMap stringMap_;
	HashMap hashMap_;
};

LAME_END2

#endif // ~__LAME_SCRIPT__SCOPE__