#ifndef __LAME_SCRIPT__SCOPE__
#define __LAME_SCRIPT__SCOPE__

#include "Define.h"

LAME_BEGIN2(Script)

class LAME_API Scope : public Observer <Object> {
	typedef Hashable64::HashType Hash;
public:
	typedef Map<Hashable64::HashType, ObjectPtr> HashMap;
	typedef Map<Buffer, ObjectPtr> StringMap;
public:
	ObjectPtr Add(ObjectPtr object);
	Void Remove(ObjectPtr var);
	ObjectPtr Find(Hash hash);
	ObjectPtr Find(BufferRefC name);
	Void Merge(ScopePtrC scope);
	Void Clone(ScopePtrC scope);
	Void Move(ScopePtrC scope);
	Uint32 Amount(Void);
	Void Clear(Void);
	Void Trace(Uint32 offset);
	Uint32 Size(Void);
	Void Update(ObjectPtr object) override;
	ScopePtr Root(Void);
	Buffer Path(Void);
	Void Flush(Void);
public:
	inline HashMap& GetHashMap() { return this->hashMap_; }
	inline ScopePtr GetParent() { return this->parentScope_; }
	inline BufferRefC GetName() { return this->scopeName_; }
public:
	inline Set<ObjectPtr>& GetPublicSet() { return this->publicSet_; }
	inline Set<ObjectPtr>& GetStaticSet() { return this->staticSet_; }
	inline Set<ObjectPtr>& GetMethodSet() { return this->methodSet_; }
	inline Set<ObjectPtr>& GetVariableSet() { return this->variableSet_; }
	inline Set<ObjectPtr>& GetClassSet() { return this->classSet_; }
public:
	Scope(BufferRefC name, ScopePtr parent);
	~Scope();
public:
	static ScopePtr GetRootScope(Void);
public:
	static ClassPtr classChar;
	static ClassPtr classByte;
	static ClassPtr classBoolean;
	static ClassPtr classShort;
	static ClassPtr classInt;
	static ClassPtr classLong;
	static ClassPtr classFloat;
	static ClassPtr classDouble;
	static ClassPtr classVoid;
	static ClassPtr classString;
	static ClassPtr classObject;
	static ClassPtr classClass;
	static ClassPtr classUnknown;
private:
	HashMap hashMap_;
	StringMap stringMap_;
	Bool isOwner_;
	ScopePtr parentScope_;
	Set<ObjectPtr> publicSet_;
	Set<ObjectPtr> staticSet_;
	Set<ObjectPtr> methodSet_;
	Set<ObjectPtr> variableSet_;
	Set<ObjectPtr> classSet_;
	Buffer scopeName_;
};

LAME_END2

#endif // ~__LAME_SCRIPT__SCOPE__