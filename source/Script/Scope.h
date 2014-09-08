#ifndef __LAME_SCRIPT__SCOPE__
#define __LAME_SCRIPT__SCOPE__

#include "Node.h"

LAME_BEGIN2(Script)

class LAME_API Scope {
	friend class ScopeBuilder;
private:
	typedef Hashable64::HashType Hash;
public:
	typedef Map<Hashable64::HashType, ObjectPtr> HashMap;
	typedef Map<Buffer, ObjectPtr> StringMap;
public:
	ObjectPtr Add(ObjectPtr object);
	Void Remove(ObjectPtr var);
	ObjectPtr Find(Hash hash, Bool withDepth = TRUE, Uint32 objectType = FALSE);
	ObjectPtr Find(BufferRefC name, Bool withDepth = TRUE, Uint32 objectType = FALSE);
	Void Merge(ScopePtrC scope);
	Void Clone(ScopePtrC scope);
	Void Move(ScopePtrC scope);
	Uint32 Amount(Void);
	Void Clear(Void);
	Void Trace(Uint32 offset);
	Uint32 Size(Void);
	ScopePtr Root(Void);
	Buffer Path(Void);
public:
	template <class F> inline Set<ObjectPtr> Filter(F f) {

		Set<ObjectPtr> result;

		for (auto i : this->hashMap_) {
			if (f(i.second)) {
				result.insert(i.second);
			}
		}

		return result;
	}
public:
	inline HashMap&   GetHashMap() { return this->hashMap_;     }
	inline ScopePtr   GetParent()  { return this->parentScope_; }
	inline BufferRefC GetName()    { return this->scopeName_;   }
	inline ScopePtr   GetOwner()   { return this->ownerScope_;  }
public:
	inline Set<ObjectPtr>& GetMethodSet() { return this->methodSet_; }
	inline Set<ObjectPtr>& GetClassSet() { return this->classSet_; }
public:
	Scope(BufferRefC name, ScopePtr parent);
	~Scope();
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
	static ClassPtr classArray;
private:
	HashMap hashMap_;
	StringMap stringMap_;
	Bool isOwner_;
	ScopePtr parentScope_;
	Set<ObjectPtr> methodSet_;
	Set<ObjectPtr> variableSet_;
	Set<ObjectPtr> classSet_;
	Buffer scopeName_;
	ScopePtr ownerScope_;
};

LAME_END2

#endif // ~__LAME_SCRIPT__SCOPE__