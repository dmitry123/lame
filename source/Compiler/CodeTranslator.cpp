#include "CodeTranslator.h"
#include "Assembler.h"

LAME_BEGIN2(Compiler)

#define RILFD(s, r, i, l, f, d) \
    (s->GetClass()->IsInt() ? i : s->GetClass()->IsLong() ? l : s->GetClass()->IsFloat() ? f : s->GetClass()->IsDouble() ? d : r)

#define BCPNEW(s, r, i, l, d, f) \
    this->GetByteCode()->New(RILFD(s, r, i, l, f, d))

Void _CompileBool(ByteCodePtr bc, Script::VariablePtr left, LexID lexID) {

	Asm command = NOOP;

	switch (lexID) {
		case kScriptLexAbove:       command = JNA;  break;
		case kScriptLexBellow:      command = JNB;  break;
		case kScriptLexAboveEqual:  command = JNAE; break;
		case kScriptLexBellowEqual: command = JNBE; break;
		case kScriptLexEqual:       command = JNE;  break;
		case kScriptLexNotEqual:    command = JE;   break;
    default:
        break;
	}

	if (command == NOOP) {
		__asm int 3
	}

	if (left->GetClass()->IsInt()) {
		bc->New(ICMP);
		bc->New(command)
			->Write(bc->GetPosition() + 11);
		bc->New(ICONST1);
		bc->New(JUMP)
			->Write(bc->GetPosition() + 6);
		bc->New(ICONST0);
	}
	else if (left->GetClass()->IsLong()) {
		bc->New(LCMP);
		bc->New(command)
			->Write(bc->GetPosition() + 11);
		bc->New(LCONST1);
		bc->New(JUMP)
			->Write(bc->GetPosition() + 6);
		bc->New(LCONST0);
	}
	else if (left->GetClass()->IsFloat()) {
		bc->New(FCMP);
		bc->New(command)
			->Write(bc->GetPosition() + 11);
		bc->New(FCONST1);
		bc->New(JUMP)
			->Write(bc->GetPosition() + 6);
		bc->New(FCONST0);
	}
	else if (left->GetClass()->IsDouble()) {
		bc->New(DCMP);
		bc->New(command)
			->Write(bc->GetPosition() + 11);
		bc->New(DCONST1);
		bc->New(JUMP)
			->Write(bc->GetPosition() + 6);
		bc->New(DCONST0);
	}
}

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
		this->OnStore(left);
		break;
	case kScriptLexBellow:
	case kScriptLexAbove:
	case kScriptLexBellowEqual:
	case kScriptLexAboveEqual:
	case kScriptLexEqual:
	case kScriptLexNotEqual:
		_CompileBool(this->GetByteCode(), left, this->currentNode->lex->lex->id);
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
    default:
        break;
	}
}

Void CodeTranslator::OnUnary(VariablePtr left) {

	switch (this->currentNode->lex->lex->id) {
	case kScriptLexIncrement:
	case kScriptLexPrefixIncrement:
		if (left->GetClass()->IsLong()) {
			this->GetByteCode()->New(LINC)
				->Write(left->GetAddress());
		}
		else {
			this->GetByteCode()->New(IINC)
				->Write(left->GetAddress());
		}
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
		if (!type->IsIntegerLike() || type->IsLong()) {
			BCPNEW(type, NOOP, NOOP, ITL, ITD, ITF);
		}
	}
	else if (source->GetClass()->IsLong()) {
		if (!type->IsLong()) {
			BCPNEW(type, NOOP, LTI, NOOP, LTD, LTF);
		}
	}
	else if (source->GetClass()->IsFloat()) {
		if (!type->IsFloat()) {
			BCPNEW(type, NOOP, FTI, FTL, FTD, NOOP);
		}
	}
	else if (source->GetClass()->IsDouble()) {
		if (!type->IsDouble()) {
			BCPNEW(type, NOOP, DTI, DTL, NOOP, DTF);
		}
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
    ObjectPtr thisVar = NULL;
    
	if (!var->GetThis()) {

		if (var->GetName() == "true") {
			bc->New(ICONST1);
		}
		else if (var->GetName() == "false") {
			bc->New(ICONST0);
		}
		else if (var->GetName() == "null") {
			bc->New(RCONST0);
		}
		else {
			if (var->CheckModificator(Object::Modificator::Constant)) {
				if (var->GetClass()->IsIntegerLike() ||
					var->GetClass()->IsBooleanLike()
				) {
					if (var->v.intValue == 0) {
						bc->New(ICONST0);
					}
					else if (var->v.intValue == 1) {
						bc->New(ICONST1);
					}
					else if (var->v.intValue == -1) {
						bc->New(ICONST1)->New(INEG);
					}
					else {
						goto _LoadVar;
					}
				}
				else if (var->GetClass()->IsFloatLike()) {
					if (var->v.floatValue == 0) {
						bc->New(FCONST0);
					}
					else if (var->v.floatValue == 1) {
						bc->New(FCONST1);
					}
					else if (var->v.floatValue == -1) {
						bc->New(FCONST1)->New(FNEG);
					}
					else {
						goto _LoadVar;
					}
				}
			}
			else {
			_LoadVar:
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
		}
	}
	else {
		if (!(thisVar = this->GetCurrentMethod()->Find("this", FALSE))) {
			thisVar = var->GetThis();
		}
		if (thisVar) {
			this->OnLoad(VariablePtr(thisVar));
		}
		BCPNEW(var, RRLOAD, IRLOAD, LRLOAD, DRLOAD, FRLOAD)
			->Write(var->GetFieldID());
	}
}

Void CodeTranslator::OnStore(VariablePtr var) {

	ObjectPtr thisVar = NULL;

	if (var->GetThis()) {
		if (!(thisVar = this->GetCurrentMethod()->Find("this", FALSE))) {
			thisVar = var->GetThis();
		}
		if (thisVar) {
			this->OnLoad(VariablePtr(thisVar));
		}
		BCPNEW(var, RRSTORE, IRSTORE, LRSTORE, DRSTORE, FRSTORE)
			->Write(var->GetFieldID());
	}
	else if (var->GetVarType() == Variable::Var::Array) {
		BCPNEW(var, RASTORE, IASTORE, LASTORE, DASTORE, FASTORE)
			->Write(var->GetAddress());
	}
	else {
		BCPNEW(var, RSTORE, ISTORE, LSTORE, DSTORE, FSTORE)
			->Write(var->GetAddress());
	}
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

Void CodeTranslator::OnDup(ObjectPtr var) {
	BCPNEW(var, RDUP, DUP, DUP2, DUP2, DUP);
}

LAME_END2