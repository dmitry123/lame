#ifndef __LAME_SCRIPT_ASSEMBLER__
#define __LAME_SCRIPT_ASSEMBLER__

#include "Define.h"

LAME_BEGIN2(Script)

enum Asm {
	NOOP,
	RCONST0,
	ICONST0,
	ICONST1,
	LCONSTN,
	LCONST0,
	LCONST1,
	FCONST0,
	FCONST1,
	DCONST0,
	DCONST1,
	IALOAD,
	LALOAD,
	FALOAD,
	DALOAD,
	RALOAD,
	BALOAD,
	CALOAD,
	SALOAD,
	IASTORE,
	LASTORE,
	FASTORE,
	DASTORE,
	RASTORE,
	BASTORE,
	CASTORE,
	SASTORE,
	POP,
	POP2,
	DUP,
	DUP2,
	RDUP,
	XCHG,
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
	ICLD,
	ICMP,
	LCMP,
	FCMP,
	DCMP,
	IRET,
	LRET,
	FRET,
	DRET,
	RRET,
	LEAVE,
	ALENGTH,
	THROW,
	SYNCE,
	SYNCQ,
	DBG,
	ANEW,
	BIPUSH,
	LDC,
	LDC16,
	LDC32,
	RLOAD,
	ILOAD,
	LLOAD,
	FLOAD,
	DLOAD,
	ALOAD,
	ISTORE,
	LSTORE,
	FSTORE,
	DSTORE,
	RSTORE,
	JE,
	JNE,
	JB,
	JNB,
	JNBE,
	JAE,
	JNAE,
	JA,
	JNA,
	ICMPJE,
	ICMPJNE,
	ICMPJB,
	ICMPJNB,
	ICMPJAE,
	ICMPJNAE,
	ICMPJA,
	ICMPJNA,
	RCMPJE,
	RCMPJNE,
	JUMP,
	JZ,
	JNZ,
	SIPUSH,
	IINC,
	IDEC,
    LINC,
    LDEC,
	RNEW,
	ARNEW,
	INSTANCEOF,
	CALL,
	IRLOAD,
	LRLOAD,
	FRLOAD,
	DRLOAD,
	RRLOAD,
	BRLOAD,
	CRLOAD,
	SRLOAD,
	IRSTORE,
	LRSTORE,
	FRSTORE,
	DRSTORE,
	RRSTORE,
	BRSTORE,
	CRSTORE,
	SRSTORE
};

class LAME_API Assembler {
public:
	typedef struct {
		StringC name;
		Uint8   code;
		Uint8   args;
	} AsmInfo, 
		*      AsmInfoPtr,
		*const AsmInfoPtrC;
public:
	inline AsmInfoPtrC GetInfo(Uint8 command) {
		if (command >= 0 && command <= 0xff) {
			return &this->asmArray[command];
		}
		return NULL;
	}
private:
	Void _Set(Uint8 code, StringC name, Uint8 arguments);
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