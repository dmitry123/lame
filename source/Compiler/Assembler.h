#ifndef __LAME_VIRTUAL_MACHINE_ASSEMBLER__
#define __LAME_VIRTUAL_MACHINE_ASSEMBLER__

#include "Define.h"

LAME_BEGIN2(VirtualMachine)

enum Asm {
	NOP = 0x00,
	MOV = 0x01,
	ADD = 0x02,
	SUB = 0x03,
	MUL = 0x04,
	DIV = 0x05,
	MOD = 0x06,
	SHL = 0x07,
	SHR = 0x08,
	CMP = 0x09,
	JA = 0x0A,
	JAE = 0x0B,
	JB = 0x0C,
	JBE = 0x0D,
	JC = 0x0E,
	JE = 0x0F,
	JG = 0x10,
	JGE = 0x11,
	JL = 0x12,
	JLE = 0x13,
	JNA = 0x14,
	JNAE = 0x15,
	JNB = 0x16,
	JNBE = 0x17,
	JNC = 0x18,
	JNE = 0x19,
	JNG = 0x1A,
	JNGE = 0x1B,
	JNL = 0x1C,
	JNLE = 0x1D,
	JNO = 0x1E,
	JNP = 0x1F,
	JNS = 0x20,
	JNZ = 0x21,
	JO = 0x22,
	JP = 0x23,
	JS = 0x24,
	JZ = 0x25,
	JCXZ = 0x26,
	XCHG = 0x29,
	AND = 0x2A,
	OR = 0x2B,
	XOR = 0x2C,
	INC = 0x2D,
	DEC = 0x2E,
	LNOT = 0x2F,
	NOT = 0x30,
	PUSH = 0x31,
	POP = 0x32,
	LOOP = 0x33,
	JMP = 0x34,
	NEW = 0x35,
	FREE = 0x36,
	FMOV = 0x37,
	FADD = 0x38,
	FSUB = 0x39,
	FMUL = 0x3A,
	FDIV = 0x3B,
	FCMP = 0x3C,
	FPUSH = 0x3D,
	FPOP = 0x3E,
	FTI = 0x3F,
	ITF = 0x40,
	FRND = 0x41,
	TEST = 0x42,
	FTST = 0x43,
	INT = 0x44,
	JCMP = 0x45
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

#endif // ~__LAME_VIRTUAL_MACHINE_ASSEMBLER__