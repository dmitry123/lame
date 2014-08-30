#ifndef __LAME_SCRIPT__CLASS__
#define __LAME_SCRIPT__CLASS__

#include "Object.h"

LAME_BEGIN2(Script)

class LAME_API Class : public Object {
	friend class Variable;
	friend class Interface;
public:
	Class(BufferRefC name, ScopePtr parent, Uint32 size = SizeOf);
	~Class();
public:
	Bool Equal(ObjectPtrC object) override;
	ObjectPtr Clone(BufferRefC name, ObjectPtr parent) override;
	Void Trace(Uint32 offset) override;
	HashType Hash(Void) override;
	Uint32 Size(Void) override;
	ClassPtr GetClass(Void) override;
public:
	inline ObjectPtr GetExtend() { return this->extendClass; }
	inline Set<ObjectPtr>& GetImplements() { return this->implementClass; }
	inline Uint32 GetPriority() const { return this->priority; }
public:
	inline Void SetPriority(Uint32 priority) {
		this->priority = priority;
	}
	inline Uint32 GetPriority() {
		return this->priority;
	}
public:
	Void Extend(ObjectPtr object);
	Void Implement(ObjectPtr object);
	Void New(ObjectPtr object);
    Void CheckInheritance(Void);
private:
	Class(BufferRefC name, ScopePtr parent, Type type, Uint32 size = SizeOf);
private:
	ObjectPtr extendClass;
	Set<ObjectPtr> implementClass;
	Uint32 priority;
	Uint32 sizeOf;
};

LAME_END2

#endif // ~__LAME_SCRIPT__CLASS__