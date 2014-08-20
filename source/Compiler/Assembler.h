#ifndef __LAME_COMPILER_ASSEMBLER__
#define __LAME_COMPILER_ASSEMBLER__

#include "Define.h"

LAME_BEGIN2(Compiler)

enum Asm {
	NOOP,
	RCONST0,
	RCONSTF,
	ICONSTM,
	ICONST0,
	ICONST1,
	ICONSTF,
	LCONST0,
	LCONST1,
	LCONSTF,
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
	NEW,
	ARNEW,
	INSTANCEOF,
	CALL
};

typedef Void(*AsmProc0)(Void);
typedef Void(*AsmProc1)(VoidP a0);
typedef Void(*AsmProc2)(VoidP a0, VoidP a1);
typedef Void(*AsmProc3)(VoidP a0, VoidP a1, VoidP a2);

typedef Void(*AsmProc)(
	VoidP a0, VoidP a1, VoidP a2);

typedef struct {
	StringC name;
	Uint8 command;
	VoidP callback;
	Uint8 arguments;
} AsmInfo, *AsmInfoPtr;

typedef Uint64  R, *RP;
typedef Float64 F, *FP;

class Assembler {
public:
	Assembler();
public:
	AsmInfoPtr GetAsmInfo(Uint8 command);
};

LAME_END2

#endif // ~__LAME_COMPILER_ASSEMBLER__