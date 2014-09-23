#ifndef __LAME_SCRIPT_ASSEMBLER__
#define __LAME_SCRIPT_ASSEMBLER__

#include "Define.h"

LAME_BEGIN2(Script)

enum class Instruction : Uint8 {
	/* Special */
	NOOP,
	POP,
	POP2,
	DUP,
	DUP2,
	SWAP,
	SYNCE,
	SYNCQ,
	ANEW,
	RNEW,
	ALEN,
	THROW,
	DBG,
	JUMP,
	VCALL,
	CALL,
	INSTOF,
	/* Null Constant */
	RCONST0,
	ICONST0,
	LCONST0,
	FCONST0,
	DCONST0,
	ACONST0,
	/* Constant 1 */
	ICONST1,
	LCONST1,
	FCONST1,
	DCONST1,
	/* Array Load */
	BALOAD,
	SALOAD,
	IALOAD,
	LALOAD,
	FALOAD,
	DALOAD,
	RALOAD,
	AALOAD,
	/* Array Store */
	BASTORE,
	SASTORE,
	IASTORE,
	LASTORE,
	FASTORE,
	DASTORE,
	RASTORE,
	AASTORE,
	/* Reference Load */
	BRLOAD,
	SRLOAD,
	IRLOAD,
	LRLOAD,
	FRLOAD,
	DRLOAD,
	RRLOAD,
	ARLOAD,
	/* Reference Store */
	BRSTORE,
	SRSTORE,
	IRSTORE,
	LRSTORE,
	FRSTORE,
	DRSTORE,
	RRSTORE,
	ARSTORE,
	/* Math */
	IADD,
	LADD,
	FADD,
	DADD,
	ISUB,
	LSUB,
	FSUB,
	DSUB,
	IMUL,
	LMUL,
	FMUL,
	DMUL,
	IDIV,
	LDIV,
	FDIV,
	DDIV,
	IMOD,
	LMOD,
	FMOD,
	DMOD,
	INEG,
	LNEG,
	FNEG,
	DNEG,
	ISHL,
	LSHL,
	ISHR,
	LSHR,
	IUSHR,
	LUSHR,
	IAND,
	LAND,
	IOR,
	LOR,
	IXOR,
	LXOR,
	IINC,
	IDEC,
	LINC,
	LDEC,
	/* Cast */
	ITL,
	ITF,
	ITD,
	LTI,
	LTF,
	LTD,
	FTI,
	FTL,
	FTD,
	DTI,
	DTL,
	DTF,
	ITB,
	ITC,
	ITS,
	/* Load */
	ILOAD,
	LLOAD,
	FLOAD,
	DLOAD,
	RLOAD,
	ALOAD,
	/* Store */
	ISTORE,
	LSTORE,
	FSTORE,
	DSTORE,
	RSTORE,
	ASTORE,
	/* Constant Load */
	BIPUSH,
	SIPUSH,
	/* Return */
	IRET,
	LRET,
	FRET,
	DRET,
	RRET,
	ARET,
	LEAVE,
	/* Condition */
	IJE,
	IJNE,
	IJB,
	IJNB,
	IJBE,
	IJNBE,
	IJA,
	IJNA,
	IJAE,
	IJNAE,
	LJE,
	LJNE,
	LJB,
	LJNB,
	LJBE,
	LJNBE,
	LJA,
	LJNA,
	LJAE,
	LJNAE,
	FJE,
	FJNE,
	FJB,
	FJNB,
	FJBE,
	FJNBE,
	FJA,
	FJNA,
	FJAE,
	FJNAE,
	DJE,
	DJNE,
	DJB,
	DJNB,
	DJBE,
	DJNBE,
	DJA,
	DJNA,
	DJAE,
	DJNAE,
	/* Zero Condition */
	JIFZ,
	JIFNZ
};

class LAME_API Assembler {
public:
	typedef struct {
		StringC name;
		Instruction code;
		Uint8 args;
	} AsmInfo, 
		*      AsmInfoPtr,
		*const AsmInfoPtrC;
public:
	inline AsmInfoPtrC GetInfo(Instruction command) {
		if (Uint32(command) >= 0 && Uint32(command) <= 0xff) {
			return &this->asmArray[Uint32(command)];
		}
		return NULL;
	}
private:
	Void _Set(Instruction code, StringC name, Uint8 arguments);
	Void _Defaults();
private:
	AsmInfo asmArray[0xff];
public:
	static inline AssemblerPtr GetInstance() {
		return instance;
	}
private:
	Assembler() {
		this->_Defaults();
	}
private:
	static AssemblerPtr instance;
};

LAME_END2

#endif // ~__LAME_SCRIPT_ASSEMBLER__