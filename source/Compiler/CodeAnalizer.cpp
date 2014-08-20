#include "CodeAnalizer.h"
#include "LowLevelStack.h"
#include "Assembler.h"

LAME_BEGIN2(Compiler)

Void CodeAnalizer::OnBinary(VariablePtr left, VariablePtr right) {
}

Void CodeAnalizer::OnUnary(VariablePtr var) {
}

Void CodeAnalizer::OnReturn(VariablePtr var) {
}

Void CodeAnalizer::OnLoad(VariablePtr var) {
}

Void CodeAnalizer::OnStore(VariablePtr var) {
}

Void CodeAnalizer::OnCast(VariablePtr source, VariablePtr var) {
}

Void CodeAnalizer::OnInvoke(MethodPtr method) {
}

LAME_END2