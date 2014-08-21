#include "CodeAnalizer.h"

LAME_BEGIN2(Compiler)

Void CodeAnalizer::OnBinary(VariablePtr left, VariablePtr right) {
}

Void CodeAnalizer::OnUnary(VariablePtr var) {
}

Void CodeAnalizer::OnReturn(ClassPtr var) {
}

Void CodeAnalizer::OnLoad(VariablePtr var) {
}

Void CodeAnalizer::OnStore(VariablePtr var) {
}

Void CodeAnalizer::OnCast(VariablePtr source, ClassPtr var) {
}

Void CodeAnalizer::OnInvoke(MethodPtr method) {
}

LAME_END2