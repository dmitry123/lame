#include "Assembler.h"

LAME_BEGIN2(Compiler)

static Bool isVirtualMachineInitialized = 0;

enum {
	RSTACK_SIZE = 1024,
	FSTACK_SIZE = 1024
};

static AsmInfo asmArray[0xff];

static Void _SetAsm(Uint8 command, StringC name, Uint8 arguments) {

	asmArray[command].command = command;
	asmArray[command].name = name;
	asmArray[command].arguments = arguments;
}

static Void _InitializeAssembler() {

	isVirtualMachineInitialized = TRUE;

	_SetAsm(NOOP, "noop", 0);
	_SetAsm(RCONST0, "rconst0", 0);
	_SetAsm(ICONST0, "iconst0", 0);
	_SetAsm(ICONST1, "iconst1", 0);
	_SetAsm(LCONST0, "lconst0", 0);
	_SetAsm(LCONST1, "lconst1", 0);
	_SetAsm(FCONST0, "fconst0", 0);
	_SetAsm(FCONST1, "fconst1", 0);
	_SetAsm(DCONST0, "dconst0", 0);
	_SetAsm(DCONST1, "dconst1", 0);
	_SetAsm(IALOAD, "iaload", 0);
	_SetAsm(LALOAD, "laload", 0);
	_SetAsm(FALOAD, "faload", 0);
	_SetAsm(DALOAD, "daload", 0);
	_SetAsm(RALOAD, "raload", 0);
	_SetAsm(BALOAD, "baload", 0);
	_SetAsm(CALOAD, "caload", 0);
	_SetAsm(SALOAD, "saload", 0);
	_SetAsm(IASTORE, "iastore", 0);
	_SetAsm(LASTORE, "lastore", 0);
	_SetAsm(FASTORE, "fastore", 0);
	_SetAsm(DASTORE, "dastore", 0);
	_SetAsm(RASTORE, "rastore", 0);
	_SetAsm(BASTORE, "bastore", 0);
	_SetAsm(CASTORE, "castore", 0);
	_SetAsm(SASTORE, "sastore", 0);
	_SetAsm(POP, "pop", 0);
	_SetAsm(POP2, "pop2", 0);
	_SetAsm(DUP, "dup", 0);
	_SetAsm(DUP2, "dup2", 0);
	_SetAsm(RDUP, "rdup", 0);
	_SetAsm(XCHG, "xchg", 0);
	_SetAsm(IADD, "iadd", 0);
	_SetAsm(LADD, "ladd", 0);
	_SetAsm(FADD, "fadd", 0);
	_SetAsm(DADD, "dadd", 0);
	_SetAsm(ISUB, "isub", 0);
	_SetAsm(LSUB, "lsub", 0);
	_SetAsm(FSUB, "fsub", 0);
	_SetAsm(DSUB, "dsub", 0);
	_SetAsm(IMUL, "imul", 0);
	_SetAsm(LMUL, "lmul", 0);
	_SetAsm(FMUL, "fmul", 0);
	_SetAsm(DMUL, "dmul", 0);
	_SetAsm(IDIV, "idiv", 0);
	_SetAsm(LDIV, "ldiv", 0);
	_SetAsm(FDIV, "fdiv", 0);
	_SetAsm(DDIV, "ddiv", 0);
	_SetAsm(IMOD, "imod", 0);
	_SetAsm(LMOD, "lmod", 0);
	_SetAsm(FMOD, "fmod", 0);
	_SetAsm(DMOD, "dmod", 0);
	_SetAsm(INEG, "ineg", 0);
	_SetAsm(LNEG, "lneg", 0);
	_SetAsm(FNEG, "fneg", 0);
	_SetAsm(DNEG, "dneg", 0);
	_SetAsm(ISHL, "ishl", 0);
	_SetAsm(LSHL, "lshl", 0);
	_SetAsm(ISHR, "ishr", 0);
	_SetAsm(LSHR, "lshr", 0);
	_SetAsm(IUSHR, "uishr", 0);
	_SetAsm(LUSHR, "lushr", 0);
	_SetAsm(IAND, "iand", 0);
	_SetAsm(LAND, "land", 0);
	_SetAsm(IOR, "ior", 0);
	_SetAsm(LOR, "lor", 0);
	_SetAsm(IXOR, "ixor", 0);
	_SetAsm(LXOR, "ixor", 0);
	_SetAsm(ITL, "itl", 0);
	_SetAsm(ITF, "itf", 0);
	_SetAsm(ITD, "itd", 0);
	_SetAsm(LTI, "lti", 0);
	_SetAsm(LTF, "ltf", 0);
	_SetAsm(LTD, "ltd", 0);
	_SetAsm(FTI, "fti", 0);
	_SetAsm(FTL, "ftl", 0);
	_SetAsm(FTD, "ftd", 0);
	_SetAsm(DTI, "dti", 0);
	_SetAsm(DTL, "dtl", 0);
	_SetAsm(DTF, "dtf", 0);
	_SetAsm(ITB, "itb", 0);
	_SetAsm(ITC, "itc", 0);
	_SetAsm(ITS, "its", 0);
	_SetAsm(ICMP, "icmp", 0);
	_SetAsm(LCMP, "lcmp", 0);
	_SetAsm(FCMP, "fcmp", 0);
	_SetAsm(DCMP, "dcmp", 0);
	_SetAsm(IRET, "iret", 0);
	_SetAsm(LRET, "lret", 0);
	_SetAsm(FRET, "fret", 0);
	_SetAsm(DRET, "dret", 0);
	_SetAsm(RRET, "rret", 0);
	_SetAsm(LEAVE, "leave", 0);
	_SetAsm(ALENGTH, "alength", 0);
	_SetAsm(THROW, "throw", 0);
	_SetAsm(SYNCE, "synce", 0);
	_SetAsm(SYNCQ, "syncq", 0);
	_SetAsm(DBG, "dbg", 0);
	_SetAsm(ANEW, "anew", 1);
	_SetAsm(BIPUSH, "bipush", 1);
	_SetAsm(LDC, "ldc", 1);
	_SetAsm(LDC16, "ldc16", 1);
	_SetAsm(LDC32, "ldc32", 1);
	_SetAsm(RLOAD, "rload", 1);
	_SetAsm(ILOAD, "iload", 1);
	_SetAsm(LLOAD, "lload", 1);
	_SetAsm(FLOAD, "fload", 1);
	_SetAsm(DLOAD, "dload", 1);
	_SetAsm(ALOAD, "aload", 1);
	_SetAsm(ISTORE, "istore", 1);
	_SetAsm(LSTORE, "lstore", 1);
	_SetAsm(FSTORE, "fstore", 1);
	_SetAsm(DSTORE, "dstore", 1);
	_SetAsm(RSTORE, "rstore", 1);
	_SetAsm(JE, "je", 1);
	_SetAsm(JNE, "jne", 1);
	_SetAsm(JB, "jb", 1);
	_SetAsm(JNB, "jnb", 1);
	_SetAsm(JNBE, "jnbe", 1);
	_SetAsm(JAE, "jae", 1);
	_SetAsm(JNAE, "jnae", 1);
	_SetAsm(JA, "ja", 1);
	_SetAsm(JNA, "jna", 1);
	_SetAsm(ICMPJE, "icmpje", 1);
	_SetAsm(ICMPJNE, "icmpjne", 1);
	_SetAsm(ICMPJB, "impcjb", 1);
	_SetAsm(ICMPJNB, "icmpjnb", 1);
	_SetAsm(ICMPJAE, "icmpjae", 1);
	_SetAsm(ICMPJNAE, "icmpjnae", 1);
	_SetAsm(ICMPJA, "icmpja", 1);
	_SetAsm(ICMPJNA, "icmpjna", 1);
	_SetAsm(RCMPJE, "rcmpje", 1);
	_SetAsm(RCMPJNE, "rcmpjne", 1);
	_SetAsm(JUMP, "jump", 1);
	_SetAsm(JZ, "jz", 1);
	_SetAsm(JNZ, "jnz", 1);
	_SetAsm(SIPUSH, "sipush", 1);
	_SetAsm(IINC, "iinc", 1);
	_SetAsm(IDEC, "idec", 1);
	_SetAsm(IINC, "linc", 1);
	_SetAsm(IDEC, "ldec", 1);
	_SetAsm(RNEW, "rnew", 1);
	_SetAsm(ARNEW, "arnew", 1);
	_SetAsm(INSTANCEOF, "instanceof", 1);
	_SetAsm(CALL, "call", 1);
	_SetAsm(IRLOAD, "irload", 1);
	_SetAsm(LRLOAD, "lrload", 1);
	_SetAsm(FRLOAD, "frload", 1);
	_SetAsm(DRLOAD, "drload", 1);
	_SetAsm(RRLOAD, "rrload", 1);
	_SetAsm(BRLOAD, "brload", 1);
	_SetAsm(CRLOAD, "crload", 1);
	_SetAsm(SRLOAD, "srload", 1);
	_SetAsm(IRSTORE, "irstore", 1);
	_SetAsm(LRSTORE, "lrstore", 1);
	_SetAsm(FRSTORE, "frstore", 1);
	_SetAsm(DRSTORE, "drstore", 1);
	_SetAsm(RRSTORE, "rrstore", 1);
	_SetAsm(BRSTORE, "brstore", 1);
	_SetAsm(CRSTORE, "crstore", 1);
	_SetAsm(SRSTORE, "srstore", 1);
	_SetAsm(CLEAR, "clear", 1);
}

AsmInfoPtr Assembler::GetAsmInfo(Uint8 command) {

	if (!isVirtualMachineInitialized) {
		_InitializeAssembler();
	}

	return &asmArray[command];
}

LAME_END2