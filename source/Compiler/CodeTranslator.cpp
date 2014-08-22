#include "CodeTranslator.h"
#include "Assembler.h"

LAME_BEGIN2(Compiler)

#define RILFD(s, r, i, l, f, d) \
    (s->GetClass()->IsInt() ? i : s->GetClass()->IsLong() ? l : s->GetClass()->IsFloat() ? f : s->GetClass()->IsDouble() ? d : r)

#define BCPNEW(s, r, i, l, d, f) \
    this->GetByteCode()->New(RILFD(s, r, i, l, f, d))

Void CodeTranslator::OnBinary(VariablePtr left, VariablePtr right) {

	ByteCodePtr bc = this->GetByteCode();

	switch (this->currentNode->lex->lex->id) {
	case kScriptLexMul:
	case kScriptLexMulSet:
		BCPNEW(left, NOOP, IMUL, LMUL, DMUL, FMUL);
		break;
	case kScriptLexDiv:
	case kScriptLexDivSet:
		BCPNEW(left, NOOP, IDIV, LDIV, DDIV, FDIV);
		break;
	case kScriptLexMod:
	case kScriptLexModSet:
		BCPNEW(left, NOOP, IMOD, LMOD, DMOD, FMOD);
		break;
	case kScriptLexAdd:
	case kScriptLexAddSet:
		BCPNEW(left, NOOP, IADD, LADD, DADD, FADD);
		break;
	case kScriptLexSub:
	case kScriptLexSubSet:
		BCPNEW(left, NOOP, ISUB, LSUB, DSUB, FSUB);
		break;
	case kScriptLexBitShiftL:
	case kScriptLexBitShiftSetL:
		BCPNEW(left, NOOP, ISHL, LSHL, NOOP, NOOP);
		break;
	case kScriptLexBitShiftR:
	case kScriptLexBitShiftSetR:
		BCPNEW(left, NOOP, ISHR, ISHR, NOOP, NOOP);
		break;
	case kScriptLexBitAnd:
	case kScriptLexBitAndSet:
		BCPNEW(left, NOOP, IAND, LAND, NOOP, NOOP);
		break;
	case kScriptLexBitXor:
	case kScriptLexBitXorSet:
		BCPNEW(left, NOOP, IXOR, LXOR, NOOP, NOOP);
		break;
	case kScriptLexBitOr:
	case kScriptLexBitOrSet:
		BCPNEW(left, NOOP, IOR, LOR, NOOP, NOOP);
		break;
	case kScriptLexSet:
		BCPNEW(left, RSTORE, ISTORE, LSTORE, DSTORE, FSTORE)
			->Write(left->GetAddress());
		break;
	case kScriptLexBellow:
		if (left->GetClass()->IsInt()) {
			bc->New(ICMPJNB)
				->Write(bc->GetPosition() + 11);
			bc->New(ICONST1);
			bc->New(JUMP)
				->Write(bc->GetPosition() + 6);
			bc->New(ICONST0);
		}
		else if (left->GetClass()->IsLong()) {
			bc->New(LCMP);
			bc->New(JNB)
				->Write(bc->GetPosition() + 11);
			bc->New(LCONST1);
			bc->New(JUMP)
				->Write(bc->GetPosition() + 6);
			bc->New(LCONST0);
		}
		else if (left->GetClass()->IsFloat()) {
			bc->New(FCMP);
			bc->New(JNB)
				->Write(bc->GetPosition() + 11);
			bc->New(FCONST1);
			bc->New(JUMP)
				->Write(bc->GetPosition() + 6);
			bc->New(FCONST0);
		}
		else if (left->GetClass()->IsDouble()) {
			bc->New(DCMP);
			bc->New(JNB)
				->Write(bc->GetPosition() + 11);
			bc->New(DCONST1);
			bc->New(JUMP)
				->Write(bc->GetPosition() + 6);
			bc->New(DCONST0);
		}
		break;
	case kScriptLexAbove:
		if (left->GetClass()->IsInt()) {
			bc->New(ICMPJNA)
				->Write(bc->GetPosition() + 11);
			bc->New(ICONST1);
			bc->New(JUMP)
				->Write(bc->GetPosition() + 6);
			bc->New(ICONST0);
		}
		else if (left->GetClass()->IsLong()) {
			bc->New(LCMP);
			bc->New(JNA)
				->Write(bc->GetPosition() + 11);
			bc->New(LCONST1);
			bc->New(JUMP)
				->Write(bc->GetPosition() + 6);
			bc->New(LCONST0);
		}
		else if (left->GetClass()->IsFloat()) {
			bc->New(FCMP);
			bc->New(JNA)
				->Write(bc->GetPosition() + 11);
			bc->New(FCONST1);
			bc->New(JUMP)
				->Write(bc->GetPosition() + 6);
			bc->New(FCONST0);
		}
		else if (left->GetClass()->IsDouble()) {
			bc->New(DCMP);
			bc->New(JNA)
				->Write(bc->GetPosition() + 11);
			bc->New(DCONST1);
			bc->New(JUMP)
				->Write(bc->GetPosition() + 6);
			bc->New(DCONST0);
		}
		break;
	case kScriptLexBellowEqual:
		if (left->GetClass()->IsInt()) {
			bc->New(ICMPJNE)
				->Write(bc->GetPosition() + 11);
			bc->New(ICONST1);
			bc->New(JUMP)
				->Write(bc->GetPosition() + 6);
			bc->New(ICONST0);
		}
		else if (left->GetClass()->IsLong()) {
			bc->New(LCMP);
			bc->New(JNE)
				->Write(bc->GetPosition() + 11);
			bc->New(LCONST1);
			bc->New(JUMP)
				->Write(bc->GetPosition() + 6);
			bc->New(LCONST0);
		}
		else if (left->GetClass()->IsFloat()) {
			bc->New(FCMP);
			bc->New(JNE)
				->Write(bc->GetPosition() + 11);
			bc->New(FCONST1);
			bc->New(JUMP)
				->Write(bc->GetPosition() + 6);
			bc->New(FCONST0);
		}
		else if (left->GetClass()->IsDouble()) {
			bc->New(DCMP);
			bc->New(JNE)
				->Write(bc->GetPosition() + 11);
			bc->New(DCONST1);
			bc->New(JUMP)
				->Write(bc->GetPosition() + 6);
			bc->New(DCONST0);
		}
		break;
	case kScriptLexAboveEqual:
		if (left->GetClass()->IsInt()) {
			bc->New(ICMPJNAE)
				->Write(bc->GetPosition() + 11);
			bc->New(ICONST1);
			bc->New(JUMP)
				->Write(bc->GetPosition() + 6);
			bc->New(ICONST0);
		}
		else if (left->GetClass()->IsLong()) {
			bc->New(LCMP);
			bc->New(JNAE)
				->Write(bc->GetPosition() + 11);
			bc->New(LCONST1);
			bc->New(JUMP)
				->Write(bc->GetPosition() + 6);
			bc->New(LCONST0);
		}
		else if (left->GetClass()->IsFloat()) {
			bc->New(FCMP);
			bc->New(JNAE)
				->Write(bc->GetPosition() + 11);
			bc->New(FCONST1);
			bc->New(JUMP)
				->Write(bc->GetPosition() + 6);
			bc->New(FCONST0);
		}
		else if (left->GetClass()->IsDouble()) {
			bc->New(DCMP);
			bc->New(JNAE)
				->Write(bc->GetPosition() + 11);
			bc->New(DCONST1);
			bc->New(JUMP)
				->Write(bc->GetPosition() + 6);
			bc->New(DCONST0);
		}
		break;
	case kScriptLexEqual:
		if (left->GetClass()->IsInt()) {
			bc->New(ICMPJNE)
				->Write(bc->GetPosition() + 11);
			bc->New(ICONST1);
			bc->New(JUMP)
				->Write(bc->GetPosition() + 6);
			bc->New(ICONST0);
		}
		else if (left->GetClass()->IsLong()) {
			bc->New(LCMP);
			bc->New(JNE)
				->Write(bc->GetPosition() + 11);
			bc->New(LCONST1);
			bc->New(JUMP)
				->Write(bc->GetPosition() + 6);
			bc->New(LCONST0);
		}
		else if (left->GetClass()->IsFloat()) {
			bc->New(FCMP);
			bc->New(JNE)
				->Write(bc->GetPosition() + 11);
			bc->New(FCONST1);
			bc->New(JUMP)
				->Write(bc->GetPosition() + 6);
			bc->New(FCONST0);
		}
		else if (left->GetClass()->IsDouble()) {
			bc->New(DCMP);
			bc->New(JNE)
				->Write(bc->GetPosition() + 11);
			bc->New(DCONST1);
			bc->New(JUMP)
				->Write(bc->GetPosition() + 6);
			bc->New(DCONST0);
		}
		break;
	case kScriptLexNotEqual:
		if (left->GetClass()->IsInt()) {
			bc->New(ICMPJE)
				->Write(bc->GetPosition() + 11);
			bc->New(ICONST1);
			bc->New(JUMP)
				->Write(bc->GetPosition() + 6);
			bc->New(ICONST0);
		}
		else if (left->GetClass()->IsLong()) {
			bc->New(LCMP);
			bc->New(JE)
				->Write(bc->GetPosition() + 11);
			bc->New(LCONST1);
			bc->New(JUMP)
				->Write(bc->GetPosition() + 6);
			bc->New(LCONST0);
		}
		else if (left->GetClass()->IsFloat()) {
			bc->New(FCMP);
			bc->New(JE)
				->Write(bc->GetPosition() + 11);
			bc->New(FCONST1);
			bc->New(JUMP)
				->Write(bc->GetPosition() + 6);
			bc->New(FCONST0);
		}
		else if (left->GetClass()->IsDouble()) {
			bc->New(DCMP);
			bc->New(JE)
				->Write(bc->GetPosition() + 11);
			bc->New(DCONST1);
			bc->New(JUMP)
				->Write(bc->GetPosition() + 6);
			bc->New(DCONST0);
		}
		break;
	case kScriptLexAnd:
		if (!left->IsBooleanLike()) {
			PostSyntaxError(this->currentNode->lex->line, "Unable to apply '&&' for non-boolean type (%s)",
				left->GetClass()->GetName().data());
		}
		bc->New(IAND);
		break;
	case kScriptLexOr:
		if (!left->IsBooleanLike()) {
			PostSyntaxError(this->currentNode->lex->line, "Unable to apply '||' for non-boolean type (%s)",
				left->GetClass()->GetName().data());
		}
		bc->New(IOR);
		break;
	}
}

Void CodeTranslator::OnUnary(VariablePtr left) {

	switch (this->currentNode->lex->lex->id) {
	case kScriptLexIncrement:
	case kScriptLexPrefixIncrement:
		BCPNEW(left, NOOP, IINC, LINC, NOOP, NOOP);
		break;
	case kScriptLexDecrement:
	case kScriptLexPrefixDecrement:
		BCPNEW(left, NOOP, IDEC, LDEC, NOOP, NOOP);
		break;
	case kScriptLexInstanceof:
		break;
	case kScriptLexReturn:
		break;
	case kScriptLexBitNot:
		break;
	case kScriptLexNot:
		break;
	case kScriptLexUnaryMinus:
	case kScriptLexSub:
		BCPNEW(left, NOOP, INEG, LNEG, DNEG, FNEG);
		break;
	case kScriptLexUnaryPlus:
	case kScriptLexAdd:
		break;
	case kScriptLexNew:
		break;
	default:
		break;
	}
}

Void CodeTranslator::OnTernary(NodePtr node, Bool state) {

	ByteCodePtr bc = this->GetByteCode();

	if (state) {
		bc->New(ICONST1);
		bc->New(ICMP);
		bc->New(JNE)->Write(-1 & 0xffff);
	}
	else {
		bc->New(JUMP)->Write(-1 & 0xffff);
	}
}

Void CodeTranslator::OnCast(VariablePtr source, ClassPtr type) {

	if (source->GetClass()->IsInt()) {
		BCPNEW(type, NOOP, NOOP, ITL, ITD, ITF);
	}
	else if (source->GetClass()->IsLong()) {
		BCPNEW(type, NOOP, LTI, NOOP, LTD, LTF);
	}
	else if (source->GetClass()->IsFloat()) {
		BCPNEW(type, NOOP, FTI, FTL, FTD, NOOP);
	}
	else if (source->GetClass()->IsDouble()) {
		BCPNEW(type, NOOP, DTI, DTL, NOOP, DTF);
	}
	else if (source->GetClass()->IsObject()) {
		if (type->GetClass()->IsObject()) {
			// cast
		}
		else {
			// cast
		}
	}
}

Void CodeTranslator::OnLoad(VariablePtr var) {

	ByteCodePtr bc = this->GetByteCode();

	if (var->GetClass()->IsByte()) {
		bc->New(BIPUSH);
	}
	else if (
		var->GetClass()->IsShort() ||
		var->GetClass()->IsChar()
		) {
		bc->New(SIPUSH);
	}
	else {
		BCPNEW(var, RLOAD, ILOAD, LLOAD, DLOAD, FLOAD);
	}

	bc->Write(var->GetAddress());
}

Void CodeTranslator::OnStore(VariablePtr var) {

	BCPNEW(var, RSTORE, ISTORE, LSTORE, DSTORE, FSTORE)
		->Write(var->GetAddress());
}

Void CodeTranslator::OnReturn(ClassPtr var) {

	if (var) {
		BCPNEW(var, RRET, IRET, LRET, DRET, FRET);
	}
	else {
		this->GetByteCode()->New(LEAVE);
	}
}

Void CodeTranslator::OnInvoke(MethodPtr method) {

	this->GetByteCode()->New(CALL)
		->Write(method->GetPosition());
}

LAME_END2