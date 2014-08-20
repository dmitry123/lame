#ifndef __LAME_COMPILTER_VARIABLE_CONTAINER__
#define __LAME_COMPILTER_VARIABLE_CONTAINER__

#include "Define.h"

LAME_BEGIN2(Compiler)

class LAME_API VariableStack {
public:
    Void Push(VariablePtr var);
    VariablePtr Pop(Void);
public:
    inline VariablePtr Back() {
        return this->varList.back();
    }
    inline Void Clear() {
        this->varList.clear();
        this->nameList.clear();
    }
    inline Uint32 Size() {
        return Uint32(this->varList.size());
    }
public:
    inline Vector<VariablePtr>& GetVarList() {
        return this->varList;
    }
    inline Vector<Buffer>& GetNameList() {
        return this->nameList;
    }
public:
    Vector<VariablePtr>::iterator begin() {
        return this->varList.begin();
    }
    Vector<VariablePtr>::iterator end() {
        return this->varList.end();
    }
private:
    Vector<VariablePtr> varList;
    Vector<Buffer> nameList;
};

LAME_END2

#endif // ~__LAME_COMPILTER_VARIABLE_CONTAINER__