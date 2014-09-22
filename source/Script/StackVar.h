#ifndef __LAME_COMPILTER_VARIABLE_CONTAINER__
#define __LAME_COMPILTER_VARIABLE_CONTAINER__

#include "Define.h"

LAME_BEGIN2(Script)

class LAME_API StackVar {
public:
	Void Push(ObjectPtr var);
	ObjectPtr Pop(Void);
	Void Clear(Void);
public:
	inline ObjectPtr Back() { return this->varList.back(); }
    inline Uint32    Size() { return this->nodeList.size(); }
public:
	inline Vector<ObjectPtr>& GetVarList()  { return this->varList; }
    inline Vector<NodePtr>&   GetNodeList() { return this->nodeList; }
public:
	Vector<ObjectPtr>::iterator begin() {
        return this->varList.begin();
    }
	Vector<ObjectPtr>::iterator end() {
        return this->varList.end();
    }
private:
	Vector<ObjectPtr> varList;
    Vector<NodePtr> nodeList;
};

LAME_END2

#endif // ~__LAME_COMPILTER_VARIABLE_CONTAINER__