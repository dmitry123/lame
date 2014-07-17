#include "CodeTranslator.h"
#include "Assembler.h"

LAME_BEGIN2(Compiler)

static Void _TranslateMov(Uint32 command, VariablePtr source, VariablePtr left, VariablePtr right, Bool isFloat = FALSE) {
	source->v = left->v;
}

static Void _TranslateMath(Uint32 command, VariablePtr source, VariablePtr left, VariablePtr right, Bool isFloat = FALSE) {

	switch (command) {
		/* Integer Operators */
	case ADD: source->v.intValue = left->v.intValue + right->v.intValue; break;
	case SUB: source->v.intValue = left->v.intValue - right->v.intValue; break;
	case MUL: source->v.intValue = left->v.intValue * right->v.intValue; break;
	case DIV: source->v.intValue = left->v.intValue / right->v.intValue; break;
	case AND: source->v.intValue = left->v.intValue & right->v.intValue; break;
	case OR:  source->v.intValue = left->v.intValue | right->v.intValue; break;
	case XOR: source->v.intValue = left->v.intValue ^ right->v.intValue; break;
		/* Float Operators */
	case FADD: source->v.floatValue = left->v.floatValue + right->v.floatValue; break;
	case FSUB: source->v.floatValue = left->v.floatValue - right->v.floatValue; break;
	case FMUL: source->v.floatValue = left->v.floatValue * right->v.floatValue; break;
	case FDIV: source->v.floatValue = left->v.floatValue / right->v.floatValue; break;
	}
}

static Void _TranslateCondition(Uint32 cmp, Uint32 command, VariablePtr source, VariablePtr left, VariablePtr right) {

#define _MAKEJUMP(_operator) \
	source->v.intValue = cmp == CMP ? \
		left->v.intValue _operator right->v.intValue : \
		left->v.floatValue _operator right->v.floatValue;

	switch (command) {
	case JA: _MAKEJUMP(<= ); break;
	case JNBE: _MAKEJUMP(<= ); break;
	case JAE: _MAKEJUMP(<); break;
	case JNB: _MAKEJUMP(<); break;
	case JB: _MAKEJUMP(>= ); break;
	case JNAE: _MAKEJUMP(>= ); break;
	case JBE: _MAKEJUMP(>); break;
	case JNA: _MAKEJUMP(>); break;
	case JE: _MAKEJUMP(!= ); break;
	case JNE: _MAKEJUMP(== ); break;
	}
}

static Void _TranslateSingle(Uint32 command, VariablePtr source) {

	switch (command) {
	case INC: source->v.intValue++; break;
	case DEC: source->v.intValue--; break;
	case LNOT: source->v.intValue = ~source->v.intValue; break;
	case NOT: source->v.intValue = !source->v.intValue; break;
	}
}

Void CodeTranslator::AnalizeBinary(VariablePtr source, VariablePtr left, VariablePtr right) {

	if (this->currentNode->lex->lex->IsLeft()) {
		printf("%s %s %s\n",
			source->GetName().data(),
			this->currentNode->word.data(),
			right->GetName().data());
	}
	else {
		printf("%s = %s %s %s\n",
			source->GetName().data(),
			left->GetName().data(),
			this->currentNode->word.data(), right->GetName().data());
	}
}

Void CodeTranslator::AnalizeUnary(VariablePtr source, VariablePtr left) {

	printf("%s = %s%s\n",
		source->GetName().data(),
		this->currentNode->word.data(),
		left->GetName().data());
}

Void CodeTranslator::AnalizeNew(VariablePtr source, VariablePtr left, ClassPtr right) {

	if (left == NULL) {
		printf("%s = new %s()\n",
			source->GetName().data(),
			right->GetName().data());
	}
	else {
		printf("%s = new %s[%s]\n",
			source->GetName().data(),
			right->GetName().data(),
			left->GetName().data());
	}
}

Void CodeTranslator::AnalizeSelection(VariablePtr source, VariablePtr left, VariablePtr right) {

}

Void CodeTranslator::AnalizeCondition(NodePtr n, VariablePtr source) {

}

Void CodeTranslator::AnalizeCast(VariablePtr source, VariablePtr left) {
	printf("%s = %s\n", source->GetName().data(), left->GetName().data());
}

Void CodeTranslator::AnalizeInvoke(NodePtr n, VariablePtr source) {

}

Void CodeTranslator::AnalizePush(VariablePtr var) {
	printf("push %s\n", var->GetName().data());
	this->lowLevelStack->Push(var);
}

Void CodeTranslator::AnalizePop(VariablePtr& var) {
	printf("pop %s\n", var->GetName().data());
	var = this->lowLevelStack->Pop();
}

Void CodeTranslator::_New(Uint32 command) {
	this->byteCodePrinter->New(Asm(command));
}

Void CodeTranslator::_Read1(VariablePtr& left) {

	if (this->varStack.size() < 1) {
		throw Exception("No more elements in stack");
	}
	left = this->varStack.back();
	this->varStack.pop_back();
	this->nameStack.pop_back();
}

Void CodeTranslator::_Read2(VariablePtr& left, VariablePtr& right) {

	if (this->varStack.size() < 2) {
		throw Exception("No more elements in stack");
	}
	right = this->varStack.back();
	this->varStack.pop_back();
	this->nameStack.pop_back();
	left = this->varStack.back();
	this->varStack.pop_back();
	this->nameStack.pop_back();
}

Void CodeTranslator::_Write1(VariablePtr var) {

	if (var->CheckModificator(Object::Modificator::Register)) {
		this->byteCodePrinter->Write(var->GetName());
	}
	else {
		this->byteCodePrinter->Write(var->GetAddress());
	}
}

Void CodeTranslator::_Jump(Uint32 address) {
	this->byteCodePrinter->Write(address, ByteCodePrinter::LineType::Jump);
}

Void CodeTranslator::_Const(Uint32 value) {
	this->byteCodePrinter->Write(value, ByteCodePrinter::LineType::Const);
}

Void CodeTranslator::_Return(VariablePtr var) {

}

Void CodeTranslator::_Mov(Uint32 command, VariablePtr source, VariablePtr left, VariablePtr right) {

}

Void CodeTranslator::_Math(Uint32 command, VariablePtr source, VariablePtr left, VariablePtr right) {

}

Void CodeTranslator::_Bool(Uint32 cmp, Uint32 command, VariablePtr source, VariablePtr left, VariablePtr right) {
	
}

Void CodeTranslator::_Unary(Uint32 command, VariablePtr source) {

}

LAME_END2