#include "CodeTranslator.h"
#include "LowLevelStack.h"
#include "Assembler.h"

LAME_BEGIN2(Compiler)

#define RILFD(s, r, i, l, f, d) \
    (s->GetClassType()->IsInt() ? i : s->GetClassType()->IsLong() ? l : s->GetClassType()->IsFloat() ? f : s->GetClassType()->IsDouble() ? d : r)

#define BCPNEW(s, r, i, l, d, f) \
    this->byteCodePrinter->New(RILFD(s, r, i, l, f, d))

Void CodeTranslator::OnBinary(VariablePtr left, VariablePtr right) {
    
    Sint64 intValue;
    
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
            if (left->GetClassType()->IsInt()) {
                this->byteCodePrinter->New(ICMPJNB)
                    ->Write(this->byteCodePrinter->GetPosition() + 11);
                this->byteCodePrinter->New(ICONST1);
                this->byteCodePrinter->New(JUMP)
                    ->Write(this->byteCodePrinter->GetPosition() + 6);
                this->byteCodePrinter->New(ICONST0);
            }
            else if (left->GetClassType()->IsLong()) {
                this->byteCodePrinter->New(LCMP);
                this->byteCodePrinter->New(JNB)
                    ->Write(this->byteCodePrinter->GetPosition() + 11);
                this->byteCodePrinter->New(LCONST1);
                this->byteCodePrinter->New(JUMP)
                    ->Write(this->byteCodePrinter->GetPosition() + 6);
                this->byteCodePrinter->New(LCONST0);
            }
            else if (left->GetClassType()->IsFloat()) {
                this->byteCodePrinter->New(FCMP);
                this->byteCodePrinter->New(JNB)
                    ->Write(this->byteCodePrinter->GetPosition() + 11);
                this->byteCodePrinter->New(FCONST1);
                this->byteCodePrinter->New(JUMP)
                    ->Write(this->byteCodePrinter->GetPosition() + 6);
                this->byteCodePrinter->New(FCONST0);
            }
            else if (left->GetClassType()->IsDouble()) {
                this->byteCodePrinter->New(DCMP);
                this->byteCodePrinter->New(JNB)
                    ->Write(this->byteCodePrinter->GetPosition() + 11);
                this->byteCodePrinter->New(DCONST1);
                this->byteCodePrinter->New(JUMP)
                    ->Write(this->byteCodePrinter->GetPosition() + 6);
                this->byteCodePrinter->New(DCONST0);
            }
            break;
        case kScriptLexAbove:
            if (left->GetClassType()->IsInt()) {
                this->byteCodePrinter->New(ICMPJNA)
                    ->Write(this->byteCodePrinter->GetPosition() + 11);
                this->byteCodePrinter->New(ICONST1);
                this->byteCodePrinter->New(JUMP)
                    ->Write(this->byteCodePrinter->GetPosition() + 6);
                this->byteCodePrinter->New(ICONST0);
            }
            else if (left->GetClassType()->IsLong()) {
                this->byteCodePrinter->New(LCMP);
                this->byteCodePrinter->New(JNA)
                    ->Write(this->byteCodePrinter->GetPosition() + 11);
                this->byteCodePrinter->New(LCONST1);
                this->byteCodePrinter->New(JUMP)
                    ->Write(this->byteCodePrinter->GetPosition() + 6);
                this->byteCodePrinter->New(LCONST0);
            }
            else if (left->GetClassType()->IsFloat()) {
                this->byteCodePrinter->New(FCMP);
                this->byteCodePrinter->New(JNA)
                    ->Write(this->byteCodePrinter->GetPosition() + 11);
                this->byteCodePrinter->New(FCONST1);
                this->byteCodePrinter->New(JUMP)
                    ->Write(this->byteCodePrinter->GetPosition() + 6);
                this->byteCodePrinter->New(FCONST0);
            }
            else if (left->GetClassType()->IsDouble()) {
                this->byteCodePrinter->New(DCMP);
                this->byteCodePrinter->New(JNA)
                    ->Write(this->byteCodePrinter->GetPosition() + 11);
                this->byteCodePrinter->New(DCONST1);
                this->byteCodePrinter->New(JUMP)
                    ->Write(this->byteCodePrinter->GetPosition() + 6);
                this->byteCodePrinter->New(DCONST0);
            }
            break;
        case kScriptLexBellowEqual:
            if (left->GetClassType()->IsInt()) {
                this->byteCodePrinter->New(ICMPJNE)
                    ->Write(this->byteCodePrinter->GetPosition() + 11);
                this->byteCodePrinter->New(ICONST1);
                this->byteCodePrinter->New(JUMP)
                    ->Write(this->byteCodePrinter->GetPosition() + 6);
                this->byteCodePrinter->New(ICONST0);
            }
            else if (left->GetClassType()->IsLong()) {
                this->byteCodePrinter->New(LCMP);
                this->byteCodePrinter->New(JNE)
                    ->Write(this->byteCodePrinter->GetPosition() + 11);
                this->byteCodePrinter->New(LCONST1);
                this->byteCodePrinter->New(JUMP)
                    ->Write(this->byteCodePrinter->GetPosition() + 6);
                this->byteCodePrinter->New(LCONST0);
            }
            else if (left->GetClassType()->IsFloat()) {
                this->byteCodePrinter->New(FCMP);
                this->byteCodePrinter->New(JNE)
                    ->Write(this->byteCodePrinter->GetPosition() + 11);
                this->byteCodePrinter->New(FCONST1);
                this->byteCodePrinter->New(JUMP)
                    ->Write(this->byteCodePrinter->GetPosition() + 6);
                this->byteCodePrinter->New(FCONST0);
            }
            else if (left->GetClassType()->IsDouble()) {
                this->byteCodePrinter->New(DCMP);
                this->byteCodePrinter->New(JNE)
                    ->Write(this->byteCodePrinter->GetPosition() + 11);
                this->byteCodePrinter->New(DCONST1);
                this->byteCodePrinter->New(JUMP)
                    ->Write(this->byteCodePrinter->GetPosition() + 6);
                this->byteCodePrinter->New(DCONST0);
            }
            break;
        case kScriptLexAboveEqual:
            if (left->GetClassType()->IsInt()) {
                this->byteCodePrinter->New(ICMPJNAE)
                    ->Write(this->byteCodePrinter->GetPosition() + 11);
                this->byteCodePrinter->New(ICONST1);
                this->byteCodePrinter->New(JUMP)
                    ->Write(this->byteCodePrinter->GetPosition() + 6);
                this->byteCodePrinter->New(ICONST0);
            }
            else if (left->GetClassType()->IsLong()) {
                this->byteCodePrinter->New(LCMP);
                this->byteCodePrinter->New(JNAE)
                    ->Write(this->byteCodePrinter->GetPosition() + 11);
                this->byteCodePrinter->New(LCONST1);
                this->byteCodePrinter->New(JUMP)
                    ->Write(this->byteCodePrinter->GetPosition() + 6);
                this->byteCodePrinter->New(LCONST0);
            }
            else if (left->GetClassType()->IsFloat()) {
                this->byteCodePrinter->New(FCMP);
                this->byteCodePrinter->New(JNAE)
                    ->Write(this->byteCodePrinter->GetPosition() + 11);
                this->byteCodePrinter->New(FCONST1);
                this->byteCodePrinter->New(JUMP)
                    ->Write(this->byteCodePrinter->GetPosition() + 6);
                this->byteCodePrinter->New(FCONST0);
            }
            else if (left->GetClassType()->IsDouble()) {
                this->byteCodePrinter->New(DCMP);
                this->byteCodePrinter->New(JNAE)
                    ->Write(this->byteCodePrinter->GetPosition() + 11);
                this->byteCodePrinter->New(DCONST1);
                this->byteCodePrinter->New(JUMP)
                    ->Write(this->byteCodePrinter->GetPosition() + 6);
                this->byteCodePrinter->New(DCONST0);
            }
            break;
        case kScriptLexEqual:
            if (left->GetClassType()->IsInt()) {
                this->byteCodePrinter->New(ICMPJNE)
                    ->Write(this->byteCodePrinter->GetPosition() + 11);
                this->byteCodePrinter->New(ICONST1);
                this->byteCodePrinter->New(JUMP)
                    ->Write(this->byteCodePrinter->GetPosition() + 6);
                this->byteCodePrinter->New(ICONST0);
            }
            else if (left->GetClassType()->IsLong()) {
                this->byteCodePrinter->New(LCMP);
                this->byteCodePrinter->New(JNE)
                    ->Write(this->byteCodePrinter->GetPosition() + 11);
                this->byteCodePrinter->New(LCONST1);
                this->byteCodePrinter->New(JUMP)
                    ->Write(this->byteCodePrinter->GetPosition() + 6);
                this->byteCodePrinter->New(LCONST0);
            }
            else if (left->GetClassType()->IsFloat()) {
                this->byteCodePrinter->New(FCMP);
                this->byteCodePrinter->New(JNE)
                    ->Write(this->byteCodePrinter->GetPosition() + 11);
                this->byteCodePrinter->New(FCONST1);
                this->byteCodePrinter->New(JUMP)
                    ->Write(this->byteCodePrinter->GetPosition() + 6);
                this->byteCodePrinter->New(FCONST0);
            }
            else if (left->GetClassType()->IsDouble()) {
                this->byteCodePrinter->New(DCMP);
                this->byteCodePrinter->New(JNE)
                    ->Write(this->byteCodePrinter->GetPosition() + 11);
                this->byteCodePrinter->New(DCONST1);
                this->byteCodePrinter->New(JUMP)
                    ->Write(this->byteCodePrinter->GetPosition() + 6);
                this->byteCodePrinter->New(DCONST0);
            }
            break;
        case kScriptLexNotEqual:
            if (left->GetClassType()->IsInt()) {
                this->byteCodePrinter->New(ICMPJE)
                    ->Write(this->byteCodePrinter->GetPosition() + 11);
                this->byteCodePrinter->New(ICONST1);
                this->byteCodePrinter->New(JUMP)
                    ->Write(this->byteCodePrinter->GetPosition() + 6);
                this->byteCodePrinter->New(ICONST0);
            }
            else if (left->GetClassType()->IsLong()) {
                this->byteCodePrinter->New(LCMP);
                this->byteCodePrinter->New(JE)
                    ->Write(this->byteCodePrinter->GetPosition() + 11);
                this->byteCodePrinter->New(LCONST1);
                this->byteCodePrinter->New(JUMP)
                    ->Write(this->byteCodePrinter->GetPosition() + 6);
                this->byteCodePrinter->New(LCONST0);
            }
            else if (left->GetClassType()->IsFloat()) {
                this->byteCodePrinter->New(FCMP);
                this->byteCodePrinter->New(JE)
                    ->Write(this->byteCodePrinter->GetPosition() + 11);
                this->byteCodePrinter->New(FCONST1);
                this->byteCodePrinter->New(JUMP)
                    ->Write(this->byteCodePrinter->GetPosition() + 6);
                this->byteCodePrinter->New(FCONST0);
            }
            else if (left->GetClassType()->IsDouble()) {
                this->byteCodePrinter->New(DCMP);
                this->byteCodePrinter->New(JE)
                    ->Write(this->byteCodePrinter->GetPosition() + 11);
                this->byteCodePrinter->New(DCONST1);
                this->byteCodePrinter->New(JUMP)
                    ->Write(this->byteCodePrinter->GetPosition() + 6);
                this->byteCodePrinter->New(DCONST0);
            }
            break;
        case kScriptLexAnd:
            BCPNEW(left, NOOP, IAND, LAND, NOOP, NOOP);
            break;
        case kScriptLexOr:
            BCPNEW(left, NOOP, IOR, LOR, NOOP, NOOP);
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

Void CodeTranslator::OnCast(VariablePtr source, VariablePtr left) {
    
	if (left->GetClassType()->IsInt()) {
		BCPNEW(source, NOOP, NOOP, ITL, ITD, ITF);
	}
	else if (left->GetClassType()->IsLong()) {
		BCPNEW(source, NOOP, LTI, NOOP, LTD, LTF);
	}
	else if (left->GetClassType()->IsFloat()) {
		BCPNEW(source, NOOP, FTI, FTL, FTD, NOOP);
	}
	else if (left->GetClassType()->IsDouble()) {
		BCPNEW(source, NOOP, DTI, DTL, NOOP, DTF);
	}
	else if (left->GetClassType()->IsObject()) {
		if (source->GetClassType()->IsObject()) {
            // cast
		}
		else {
            // cast
		}
	}
}

Void CodeTranslator::OnLoad(VariablePtr var) {
    
	if (var->GetClassType()->IsInt8()) {
		this->byteCodePrinter->New(BIPUSH);
	}
	else if (var->GetClassType()->IsInt16()) {
		this->byteCodePrinter->New(SIPUSH);
	}
	else {
		BCPNEW(var, RLOAD, ILOAD, LLOAD, DLOAD, FLOAD);
	}
    
	this->byteCodePrinter->Write(var->GetAddress());
}

Void CodeTranslator::OnStore(VariablePtr var) {
	
	BCPNEW(var, RSTORE, ISTORE, LSTORE, DSTORE, FSTORE)
        ->Write(var->GetAddress());
}

Void CodeTranslator::OnReturn(VariablePtr var) {
    
    if (var) {
        BCPNEW(var, RRET, IRET, LRET, DRET, FRET);
    } else {
        this->byteCodePrinter->New(LEAVE);
    }
}

Void CodeTranslator::OnInvoke(MethodPtr method) {

	this->byteCodePrinter->New(CALL)
		->Write(method->segmentCodePosition);
}

LAME_END2