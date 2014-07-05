#ifndef __LAME_SCRIPT__ARRAY__
#define __LAME_SCRIPT__ARRAY__

#include "Variable.h"

LAME_BEGIN2(Script)

class LAME_API Array : public Variable {
public:
	Array(BufferRefC name, NodePtr node, ObjectPtr classType, Uint32 size = 0);
public:
	inline ArrayPtr GetArray() override {
		return this;
	}
public:
	ObjectPtr At(Uint32 index);
public:
	Bool Equal(ObjectPtrC object) const override;
	Error Clone(ObjectPtrC object) override;
	Void Trace(Uint32 offset) override;
private:
	Vector<ObjectPtr> arrayList;
};

LAME_END2

#endif // ~__LAME_SCRIPT__ARRAY__