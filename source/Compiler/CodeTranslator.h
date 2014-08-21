#ifndef __LAME_COMPILER_CODE_TRANSLATOR__
#define __LAME_COMPILER_CODE_TRANSLATOR__

#include "VirtualCompiler.h"

LAME_BEGIN2(Compiler)

using namespace Core;
using namespace Script;

class LAME_API CodeTranslator : public VirtualCompiler {
public:
    Void Analize(NodeBuilderPtr nodeBuilder, ScopePtr rootScope) {
        this->VirtualCompiler::Analize(nodeBuilder, rootScope);
    }
private:
    Void OnBinary(VariablePtr left, VariablePtr right) override;
    Void OnUnary(VariablePtr var) override;
	Void OnReturn(ClassPtr var) override;
    Void OnLoad(VariablePtr var) override;
    Void OnStore(VariablePtr var) override;
	Void OnCast(VariablePtr source, ClassPtr type) override;
	Void OnInvoke(MethodPtr method) override;
};

LAME_END2

#endif // ~__LAME_COMPILER_CODE_TRANSLATOR__