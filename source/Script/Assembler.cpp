#include "Assembler.h"

LAME_BEGIN2(Script)

Void Assembler::_Set(Instruction code, StringC name, Uint8 arguments) {

	this->asmArray[Uint32(code)].code = code;
	this->asmArray[Uint32(code)].name = name;
	this->asmArray[Uint32(code)].args = arguments;
}

Void Assembler::_Defaults() {

	/* Special */
	this->_Set(Instruction::NOOP,    "noop",    0);
	this->_Set(Instruction::POP,     "pop",     0);
	this->_Set(Instruction::POP2,    "pop2",    0);
	this->_Set(Instruction::DUP,     "dup",     0);
	this->_Set(Instruction::DUP2,    "dup2",    0);
	this->_Set(Instruction::SWAP,    "swap",    0);
	this->_Set(Instruction::SYNCE,   "synce",   1);
	this->_Set(Instruction::SYNCQ,   "syncq",   1);
	this->_Set(Instruction::ANEW,    "anew",    1);
	this->_Set(Instruction::RNEW,    "rnew",    1);
	this->_Set(Instruction::ALEN,    "alen",    1);
	this->_Set(Instruction::THROW,   "throw",   1);
	this->_Set(Instruction::DBG,     "dbg",     0);
	this->_Set(Instruction::JUMP,    "jump",    1);
	this->_Set(Instruction::VCALL,   "vcall",   1);
	this->_Set(Instruction::CALL,    "call",    1);
	this->_Set(Instruction::INSTOF,  "instof",  1);
	/* Null Constant */
	this->_Set(Instruction::RCONST0, "rconst0", 0);
	this->_Set(Instruction::ICONST0, "iconst0", 0);
	this->_Set(Instruction::LCONST0, "lconst0", 0);
	this->_Set(Instruction::FCONST0, "fconst0", 0);
	this->_Set(Instruction::DCONST0, "dconst0", 0);
	this->_Set(Instruction::ACONST0, "aconst0", 0);
	/* Constant 1 */
	this->_Set(Instruction::ICONST1, "iconst1", 0);
	this->_Set(Instruction::LCONST1, "lconst1", 0);
	this->_Set(Instruction::FCONST1, "fconst1", 0);
	this->_Set(Instruction::DCONST1, "dconst1", 0);
	/* Array Load */
	this->_Set(Instruction::BALOAD,  "baload",  1);
	this->_Set(Instruction::SALOAD,  "saload",  1);
	this->_Set(Instruction::IALOAD,  "iaload",  1);
	this->_Set(Instruction::LALOAD,  "laload",  1);
	this->_Set(Instruction::FALOAD,  "faload",  1);
	this->_Set(Instruction::DALOAD,  "daload",  1);
	this->_Set(Instruction::RALOAD,  "raload",  1);
	this->_Set(Instruction::AALOAD,  "aaload",  1);
	/* Array Store */
	this->_Set(Instruction::BASTORE, "bastore", 1);
	this->_Set(Instruction::SASTORE, "sastore", 1);
	this->_Set(Instruction::IASTORE, "iastore", 1);
	this->_Set(Instruction::LASTORE, "lastore", 1);
	this->_Set(Instruction::FASTORE, "fastore", 1);
	this->_Set(Instruction::DASTORE, "dastore", 1);
	this->_Set(Instruction::RASTORE, "rastore", 1);
	this->_Set(Instruction::AASTORE, "aastore", 1);
	/* Reference Load */
	this->_Set(Instruction::BRLOAD,  "brload",  1);
	this->_Set(Instruction::SRLOAD,  "srload",  1);
	this->_Set(Instruction::IRLOAD,  "irload",  1);
	this->_Set(Instruction::LRLOAD,  "lrload",  1);
	this->_Set(Instruction::FRLOAD,  "frload",  1);
	this->_Set(Instruction::DRLOAD,  "drload",  1);
	this->_Set(Instruction::RRLOAD,  "rrload",  1);
	this->_Set(Instruction::ARLOAD,  "arload",  1);
	/* Reference Store */
	this->_Set(Instruction::BRSTORE, "brstore", 1);
	this->_Set(Instruction::SRSTORE, "srstore", 1);
	this->_Set(Instruction::IRSTORE, "irstore", 1);
	this->_Set(Instruction::LRSTORE, "lrstore", 1);
	this->_Set(Instruction::FRSTORE, "frstore", 1);
	this->_Set(Instruction::DRSTORE, "drstore", 1);
	this->_Set(Instruction::RRSTORE, "rrstore", 1);
	this->_Set(Instruction::ARSTORE, "arstore", 1);
	/* Math */
	this->_Set(Instruction::IADD,    "iadd",    0);
	this->_Set(Instruction::LADD,    "ladd",    0);
	this->_Set(Instruction::FADD,    "fadd",    0);
	this->_Set(Instruction::DADD,    "dadd",    0);
	this->_Set(Instruction::ISUB,    "isub",    0);
	this->_Set(Instruction::LSUB,    "lsub",    0);
	this->_Set(Instruction::FSUB,    "fsub",    0);
	this->_Set(Instruction::DSUB,    "dsub",    0);
	this->_Set(Instruction::IMUL,    "imul",    0);
	this->_Set(Instruction::LMUL,    "lmul",    0);
	this->_Set(Instruction::FMUL,    "fmul",    0);
	this->_Set(Instruction::DMUL,    "dmul",    0);
	this->_Set(Instruction::IDIV,    "idiv",    0);
	this->_Set(Instruction::LDIV,    "ldiv",    0);
	this->_Set(Instruction::FDIV,    "fdiv",    0);
	this->_Set(Instruction::DDIV,    "ddiv",    0);
	this->_Set(Instruction::IMOD,    "imod",    0);
	this->_Set(Instruction::LMOD,    "lmod",    0);
	this->_Set(Instruction::FMOD,    "fmod",    0);
	this->_Set(Instruction::DMOD,    "dmod",    0);
	this->_Set(Instruction::INEG,    "ineg",    0);
	this->_Set(Instruction::LNEG,    "lneg",    0);
	this->_Set(Instruction::FNEG,    "fneg",    0);
	this->_Set(Instruction::DNEG,    "dneg",    0);
	this->_Set(Instruction::ISHL,    "ishl",    0);
	this->_Set(Instruction::LSHL,    "lshl",    0);
	this->_Set(Instruction::ISHR,    "ishr",    0);
	this->_Set(Instruction::LSHR,    "lshr",    0);
	this->_Set(Instruction::IUSHR,   "uishr",   0);
	this->_Set(Instruction::LUSHR,   "lushr",   0);
	this->_Set(Instruction::IAND,    "iand",    0);
	this->_Set(Instruction::LAND,    "land",    0);
	this->_Set(Instruction::IOR,     "ior",     0);
	this->_Set(Instruction::LOR,     "lor",     0);
	this->_Set(Instruction::IXOR,    "ixor",    0);
	this->_Set(Instruction::LXOR,    "lxor",    0);
	this->_Set(Instruction::IINC,    "iinc",    1);
	this->_Set(Instruction::IDEC,    "idec",    1);
	this->_Set(Instruction::LINC,    "linc",    1);
	this->_Set(Instruction::LDEC,    "ldec",    1);
	/* Cast */
	this->_Set(Instruction::ITL,     "itl",     0);
	this->_Set(Instruction::ITF,     "itf",     0);
	this->_Set(Instruction::ITD,     "itd",     0);
	this->_Set(Instruction::LTI,     "lti",     0);
	this->_Set(Instruction::LTF,     "ltf",     0);
	this->_Set(Instruction::LTD,     "ltd",     0);
	this->_Set(Instruction::FTI,     "fti",     0);
	this->_Set(Instruction::FTL,     "ftl",     0);
	this->_Set(Instruction::FTD,     "ftd",     0);
	this->_Set(Instruction::DTI,     "dti",     0);
	this->_Set(Instruction::DTL,     "dtl",     0);
	this->_Set(Instruction::DTF,     "dtf",     0);
	this->_Set(Instruction::ITB,     "itb",     0);
	this->_Set(Instruction::ITC,     "itc",     0);
	this->_Set(Instruction::ITS,     "its",     0);
	/* Load */
	this->_Set(Instruction::ILOAD,   "iload",   1);
	this->_Set(Instruction::LLOAD,   "lload",   1);
	this->_Set(Instruction::FLOAD,   "fload",   1);
	this->_Set(Instruction::DLOAD,   "dload",   1);
	this->_Set(Instruction::RLOAD,   "rload",   1);
	this->_Set(Instruction::ALOAD,   "aload",   1);
	/* Store */
	this->_Set(Instruction::ISTORE,  "istore",  1);
	this->_Set(Instruction::LSTORE,  "lstore",  1);
	this->_Set(Instruction::FSTORE,  "fstore",  1);
	this->_Set(Instruction::DSTORE,  "dstore",  1);
	this->_Set(Instruction::RSTORE,  "rstore",  1);
	this->_Set(Instruction::ASTORE,  "astore",  1);
	/* Constant Load */
	this->_Set(Instruction::BIPUSH,  "bipush",  1);
	this->_Set(Instruction::SIPUSH,  "sipush",  2);
	/* Return */
	this->_Set(Instruction::IRET,    "iret",    0);
	this->_Set(Instruction::LRET,    "lret",    0);
	this->_Set(Instruction::FRET,    "fret",    0);
	this->_Set(Instruction::DRET,    "dret",    0);
	this->_Set(Instruction::RRET,    "rret",    0);
	this->_Set(Instruction::ARET,    "aret",    0);
	this->_Set(Instruction::LEAVE,   "leave",   0);
	/* Condition */
	this->_Set(Instruction::IJE,     "ije",     2);
	this->_Set(Instruction::IJNE,    "ijne",    2);
	this->_Set(Instruction::IJB,     "ijb",     2);
	this->_Set(Instruction::IJNB,    "ijnb",    2);
	this->_Set(Instruction::IJBE,    "ijbe",    2);
	this->_Set(Instruction::IJNBE,   "ijnbe",   2);
	this->_Set(Instruction::IJA,     "ija",     2);
	this->_Set(Instruction::IJNA,    "ijna",    2);
	this->_Set(Instruction::IJAE,    "ijae",    2);
	this->_Set(Instruction::IJNAE,   "ijnae",   2);
	this->_Set(Instruction::LJE,     "lje",     2);
	this->_Set(Instruction::LJNE,    "ljne",    2);
	this->_Set(Instruction::LJB,     "ljb",     2);
	this->_Set(Instruction::LJNB,    "ljnb",    2);
	this->_Set(Instruction::LJBE,    "ljbe",    2);
	this->_Set(Instruction::LJNBE,   "ljnbe",   2);
	this->_Set(Instruction::LJA,     "lja",     2);
	this->_Set(Instruction::LJNA,    "ljna",    2);
	this->_Set(Instruction::LJAE,    "ljae",    2);
	this->_Set(Instruction::LJNAE,   "ljnae",   2);
	this->_Set(Instruction::FJE,     "fje",     2);
	this->_Set(Instruction::FJNE,    "fjne",    2);
	this->_Set(Instruction::FJB,     "fjb",     2);
	this->_Set(Instruction::FJNB,    "fjnb",    2);
	this->_Set(Instruction::FJBE,    "fjbe",    2);
	this->_Set(Instruction::FJNBE,   "fjnbe",   2);
	this->_Set(Instruction::FJA,     "fja",     2);
	this->_Set(Instruction::FJNA,    "fjna",    2);
	this->_Set(Instruction::FJAE,    "fjae",    2);
	this->_Set(Instruction::FJNAE,   "fjnae",   2);
	this->_Set(Instruction::DJE,     "dje",     2);
	this->_Set(Instruction::DJNE,    "djne",    2);
	this->_Set(Instruction::DJB,     "djb",     2);
	this->_Set(Instruction::DJNB,    "djnb",    2);
	this->_Set(Instruction::DJBE,    "djbe",    2);
	this->_Set(Instruction::DJNBE,   "djnbe",   2);
	this->_Set(Instruction::DJA,     "dja",     2);
	this->_Set(Instruction::DJNA,    "djna",    2);
	this->_Set(Instruction::DJAE,    "djae",    2);
	this->_Set(Instruction::DJNAE,   "djnae",   2);
	/* Zero Condition */
	this->_Set(Instruction::JIFZ,    "jifz",    2);
	this->_Set(Instruction::JIFNZ,   "jifnz",   2);
}

AssemblerPtr Assembler::instance
	= new Assembler();

LAME_END2