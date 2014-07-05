#ifndef __LAME_SCRIPT__SCOPE__
#define __LAME_SCRIPT__SCOPE__

#include "Define.h"
#include "Object.h"

LAME_BEGIN2(Script)

class LAME_API Scope {
	friend class ScopeController;
public:
	typedef Pair<Buffer, Uint64> StringPair;
	typedef Pair<Uint32, Uint64> HashPair;
    typedef Pair<StringPair, ObjectPtr> MapStringPair;
    typedef Pair<HashPair, ObjectPtr> MapHashPair;
	typedef Map<StringPair, ObjectPtr> StringMap;
	typedef Map<HashPair, ObjectPtr> HashMap;
public:
	inline ObjectPtr Find(BufferRefC buffer, Uint32 invokeHash = -1) {
		return this->Find(buffer.data(), invokeHash);
	}
public:
	ObjectPtr Add(ObjectPtr object);
	Void Remove(ObjectPtr var);
	ObjectPtr Find(StringC name, Uint32 invokeHash = -1);
	ObjectPtr Find(Uint32 hash, Uint32 invokeHash = -1);
	Void Merge(ScopePtrC scope);
	Void Clone(ScopePtrC scope);
	Void Move(ScopePtrC scope);
	Uint32 Amount(Void);
	Void Clear(Void);
	Void Trace(Uint32 offset);
	Uint32 Size(Void);
	Void Write(Uint8P buffer, Uint32P offset);
public:
	inline StringMap& GetStringMap() {
		return this->stringMap_;
	}
	inline HashMap& GetHashMap() {
		return this->hashMap_;
	}
public:
	~Scope();
private:
	Scope() {
		isOwner_ = TRUE;
	}
private:
	StringMap stringMap_;
	HashMap hashMap_;
	Bool isOwner_;
};

LAME_END2

#endif // ~__LAME_SCRIPT__SCOPE__