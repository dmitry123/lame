#ifndef __LAME_COMPILER_CODE_TRANSLATOR__
#define __LAME_COMPILER_CODE_TRANSLATOR__

#include "VirtualCompiler.h"

LAME_BEGIN2(Compiler)

class LAME_API CodeTranslator : public VirtualCompiler {
	typedef VariablePtr VariablePtr;
public:
	Void Run(NodeBuilderPtr nodeBuilder, ScopePtr rootScope, SegmentPtr codeSegment) {
		this->VirtualCompiler::Run(nodeBuilder, rootScope, codeSegment);
    }
private:
    Void OnBinary(VariablePtr left, VariablePtr right) override;
    Void OnUnary(VariablePtr var) override;
	Void OnTernary(NodePtr node, Bool state) override;
	Void OnReturn(ClassPtr var) override;
    Void OnLoad(VariablePtr var) override;
    Void OnStore(VariablePtr var) override;
	Void OnCast(VariablePtr source, ClassPtr type) override;
	Void OnInvoke(MethodPtr method) override;
};

LAME_END2

#endif // ~__LAME_COMPILER_CODE_TRANSLATOR__