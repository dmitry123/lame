#include "Assembler.h"

LAME_BEGIN2(Compiler)

static Bool isVirtualMachineInitialized = 0;

enum {
	RSTACK_SIZE = 1024,
	FSTACK_SIZE = 1024
};

static R r0 = 0;
static R r1 = 0;
static R r2 = 0;
static R r3 = 0;
static R r4 = 0;
static R r5 = 0;
static R r6 = 0;
static R r7 = 0;
static R r8 = 0;
static R r9 = 0;
static R rc = 0;
static R rt = 0;
static R rn = 0;
static R rp = 0;

static R cr0 = 0;
static R cr1 = 0;
static F cf0 = 0;
static F cf1 = 0;

static F f0 = 0;
static F f1 = 0;
static F f2 = 0;
static F f3 = 0;
static F f4 = 0;
static F f5 = 0;
static F f6 = 0;
static F f7 = 0;
static F f8 = 0;
static F f9 = 0;

static R rStack[RSTACK_SIZE];
static F fStack[FSTACK_SIZE];

static Uint32 rStackDepth = 0;
static Uint32 fStackDepth = 0;

static Uint32 cmpFlags = 0;

#define F(_operator) \
	!(cmpFlags & FF) && cr0 _operator cr1 || \
	 (cmpFlags & FF) && cf0 _operator cf1

enum {
	CF = 0x0001, /* Carry Flag */
	PF = 0x0002, /* Parity Flag */
	AF = 0x0004, /* Parity Flag */
	ZF = 0x0008, /* Zero Flag */
	SF = 0x0010, /* Sign Flag */
	TF = 0x0020, /* Trap Flag */
	IF = 0x0040, /* Interrupt Flag */
	DF = 0x0080, /* Direction Flag */
	OF = 0x0100, /* Overflow Flag */
	NT = 0x0200, /* Nested Task */
	RF = 0x0400, /* Resume Flag */
	VM = 0x0800, /* Virtual-8086 Mode */
	AC = 0x1000, /* Alignment Check */
	FF = 0x2000  /* Float Flag */
};

static AsmInfo asmArray[0xff];

static Void _SetAsm(Uint8 command,
	StringC name,
	AsmProc callback,
	Uint8 arguments) {

	asmArray[command].command = command;
	asmArray[command].name = name;
	asmArray[command].callback = callback;
	asmArray[command].arguments = arguments;
}

static Void _Nop(VoidP a0, VoidP a1, VoidP right) {
	// noop
}

static Void _Mov(VoidP a0, VoidP a1, VoidP a2) {
	*RP(a0) = *RP(a1);
}

static Void _Add(VoidP a0, VoidP a1, VoidP a2) {
	*RP(a0) = *RP(a1) + *RP(a2);
}

static Void _Sub(VoidP a0, VoidP a1, VoidP a2) {
	*RP(a0) = *RP(a1) - *RP(a2);
}

static Void _Mul(VoidP a0, VoidP a1, VoidP a2) {
	*RP(a0) = *RP(a1) * *RP(a2);
}

static Void _Div(VoidP a0, VoidP a1, VoidP a2) {
	*RP(a0) = *RP(a1) / *RP(a2);
}

static Void _Mod(VoidP a0, VoidP a1, VoidP a2) {
	*RP(a0) = *RP(a1) % *RP(a2);
}

static Void _Shl(VoidP a0, VoidP a1, VoidP a2) {
	*RP(a0) = *RP(a1) << *RP(a2);
}

static Void _Shr(VoidP a0, VoidP a1, VoidP a2) {
	*RP(a0) = *RP(a1) >> *RP(a2);
}

static Void _Cmp(VoidP a0, VoidP a1, VoidP a2) {
	cr0 = *RP(a0); cr1 = *RP(a1); cmpFlags &= ~FF;
}

static Void _Ja(VoidP a0, VoidP a1, VoidP a2) {
	if (F(>)) { rp = *RP(a0); }
}

static Void _Jae(VoidP a0, VoidP a1, VoidP a2) {
	if (F(>= )) { rp = *RP(a0); }
}

static Void _Jb(VoidP a0, VoidP a1, VoidP a2) {
	if (F(<)) { rp = *RP(a0); }
}

static Void _Jbe(VoidP a0, VoidP a1, VoidP a2) {
	if (F(<= )) { rp = *RP(a0); }
}

static Void _Jc(VoidP a0, VoidP a1, VoidP a2) {
	if ((cmpFlags & CF) != 0) { rp = *RP(a0); }
}

static Void _Je(VoidP a0, VoidP a1, VoidP a2) {
	if (F(== )) { rp = *RP(a0); }
}

static Void _Jg(VoidP a0, VoidP a1, VoidP a2) {
	if (F(>) && (cmpFlags & SF) != 0) { rp = *RP(a0); }
}

static Void _Jge(VoidP a0, VoidP a1, VoidP a2) {
	if (F(>= ) && (cmpFlags & SF) != 0) { rp = *RP(a0); }
}

static Void _Jl(VoidP a0, VoidP a1, VoidP a2) {
	if (F(<) && (cmpFlags & SF) == 0) { rp = *RP(a0); }
}

static Void _Jle(VoidP a0, VoidP a1, VoidP a2) {
	if (F(<= ) && (cmpFlags & SF) == 0) { rp = *RP(a0); }
}

static Void _Jna(VoidP a0, VoidP a1, VoidP a2) {
	if (cr0 <= cr1) { rp = *RP(a0); }
}

static Void _Jnae(VoidP a0, VoidP a1, VoidP a2) {
	if (F(<)) { rp = *RP(a0); }
}

static Void _Jnb(VoidP a0, VoidP a1, VoidP a2) {
	if (F(>= )) { rp = *RP(a0); }
}

static Void _Jnbe(VoidP a0, VoidP a1, VoidP a2) {
	if (F(>)) { rp = *RP(a0); }
}

static Void _Jnc(VoidP a0, VoidP a1, VoidP a2) {
	if ((cmpFlags & CF) == 0) { rp = *RP(a0); }
}

static Void _Jne(VoidP a0, VoidP a1, VoidP a2) {
	if (F(!= )) { rp = *RP(a0); }
}

static Void _Jng(VoidP a0, VoidP a1, VoidP a2) {
	if (F(>) && (cmpFlags & SF) == 0) { rp = *RP(a0); }
}

static Void _Jnge(VoidP a0, VoidP a1, VoidP a2) {
	if (F(>= ) && (cmpFlags & SF) == 0) { rp = *RP(a0); }
}

static Void _Jnl(VoidP a0, VoidP a1, VoidP a2) {
	if (F(<) && (cmpFlags & SF) != 0) { rp = *RP(a0); }
}

static Void _Jnle(VoidP a0, VoidP a1, VoidP a2) {
	if (F(<= ) && (cmpFlags & SF) != 0) { rp = *RP(a0); }
}

static Void _Jno(VoidP a0, VoidP a1, VoidP a2) {
	if ((cmpFlags & OF) == 0) { rp = *RP(a0); }
}

static Void _Jnp(VoidP a0, VoidP a1, VoidP a2) {
	if ((cmpFlags & PF) != 0) { rp = *RP(a0); }
}

static Void _Jns(VoidP a0, VoidP a1, VoidP a2) {
	if ((cmpFlags & SF) == 0) { rp = *RP(a0); }
}

static Void _Jnz(VoidP a0, VoidP a1, VoidP a2) {
	if ((cmpFlags & ZF) == 0) { rp = *RP(a0); }
}

static Void _Jo(VoidP a0, VoidP a1, VoidP a2) {
	if ((cmpFlags & OF) != 0) { rp = *RP(a0); }
}

static Void _Jp(VoidP a0, VoidP a1, VoidP a2) {
	if ((cmpFlags & PF) == 0) { rp = *RP(a0); }
}

static Void _Js(VoidP a0, VoidP a1, VoidP a2) {
	if ((cmpFlags & SF) != 0) { rp = *RP(a0); }
}

static Void _Jz(VoidP a0, VoidP a1, VoidP a2) {
	if ((cmpFlags & ZF) != 0) { rp = *RP(a0); }
}

static Void _Jcxz(VoidP a0, VoidP a1, VoidP a2) {
	if (!rc) { rp = *RP(a0); }
}

static Void _Xchg(VoidP a0, VoidP a1, VoidP a2) {
	*RP(a0) ^= *RP(a1) ^= *RP(a0) ^= *RP(a1);
}

static Void _And(VoidP a0, VoidP a1, VoidP a2) {
	*RP(a0) = *RP(a1) & *RP(a2);
}

static Void _Or(VoidP a0, VoidP a1, VoidP a2) {
	*RP(a0) = *RP(a1) & *RP(a2);
}

static Void _Xor(VoidP a0, VoidP a1, VoidP a2) {
	*RP(a0) = *RP(a1) ^ *RP(a2);
}

static Void _Inc(VoidP a0, VoidP a1, VoidP a2) {
	++*RP(a0);
}

static Void _Dec(VoidP a0, VoidP a1, VoidP a2) {
	--*RP(a0);
}

static Void _Lnot(VoidP a0, VoidP a1, VoidP a2) {
	*RP(a0) = ~*RP(a0);
}

static Void _Not(VoidP a0, VoidP a1, VoidP a2) {
	*RP(a0) = ~*RP(a0);
}

static Void _Push(VoidP a0, VoidP a1, VoidP a2) {
	rStack[rStackDepth++] = *RP(a0);
}

static Void _Pop(VoidP a0, VoidP a1, VoidP a2) {
	*RP(a0) = rStack[--rStackDepth];
}

static Void _Loop(VoidP a0, VoidP a1, VoidP a2) {
	if (rc > 0) { rp = *RP(a0); }
}

static Void _Jmp(VoidP a0, VoidP a1, VoidP a2) {
	rp = *RP(a0);
}

static Void _New(VoidP a0, VoidP a1, VoidP a2) {
	__asm int 3
}

static Void _Free(VoidP a0, VoidP a1, VoidP a2) {
	__asm int 3
}

static Void _Fmov(VoidP a0, VoidP a1, VoidP a2) {
	*FP(a0) = *FP(a1);
}

static Void _Fadd(VoidP a0, VoidP a1, VoidP a2) {
	*FP(a0) = *FP(a1) + *FP(a2);
}

static Void _Fsub(VoidP a0, VoidP a1, VoidP a2) {
	*FP(a0) = *FP(a1) - *FP(a2);
}

static Void _Fmul(VoidP a0, VoidP a1, VoidP a2) {
	*FP(a0) = *FP(a1) * *FP(a2);
}

static Void _Fdiv(VoidP a0, VoidP a1, VoidP a2) {
	*FP(a0) = *FP(a1) / *FP(a2);
}

static Void _Fcmp(VoidP a0, VoidP a1, VoidP a2) {
	*FP(a0) = *FP(a1) + *FP(a2); cmpFlags |= FF;
}

static Void _Fpush(VoidP a0, VoidP a1, VoidP a2) {
	fStack[fStackDepth++] = *FP(a0);
}

static Void _Fpop(VoidP a0, VoidP a1, VoidP a2) {
	*FP(a0) = fStack[--fStackDepth];
}

static Void _Fti(VoidP a0, VoidP a1, VoidP a2) {
	*RP(a0) = (R)*FP(a1);
}

static Void _Itf(VoidP a0, VoidP a1, VoidP a2) {
	*FP(a0) = (F)*RP(a1);
}

static Void _Frnd(VoidP a0, VoidP a1, VoidP a2) {
	*RP(a0) = (R)*FP(a1);
}

static Void _Test(VoidP a0, VoidP a1, VoidP a2) {
	__asm int 3
}

static Void _Ftst(VoidP a0, VoidP a1, VoidP a2) {
	__asm int 3
}

static Void _Int(VoidP a0, VoidP a1, VoidP a2) {
	__asm int 3
}

static Void _Jcmp(VoidP a0, VoidP a1, VoidP a2) {
	__asm int 3
}

static Void _InitializeVirtualMachine() {

	isVirtualMachineInitialized = TRUE;

	_SetAsm(NOP, "nop", _Nop, 0);
	_SetAsm(MOV, "mov", _Mov, 2);
	_SetAsm(ADD, "add", _Add, 3);
	_SetAsm(SUB, "sub", _Sub, 3);
	_SetAsm(MUL, "mul", _Mul, 3);
	_SetAsm(DIV, "div", _Div, 3);
	_SetAsm(MOD, "mod", _Mod, 3);
	_SetAsm(SHL, "shl", _Shl, 3);
	_SetAsm(SHR, "shr", _Shr, 3);
	_SetAsm(CMP, "cmp", _Cmp, 2);
	_SetAsm(JA, "ja", _Ja, 1);
	_SetAsm(JAE, "jae", _Jae, 1);
	_SetAsm(JB, "jb", _Jb, 1);
	_SetAsm(JBE, "jbe", _Jbe, 1);
	_SetAsm(JC, "jc", _Jc, 1);
	_SetAsm(JE, "je", _Je, 1);
	_SetAsm(JG, "jg", _Jg, 1);
	_SetAsm(JGE, "jge", _Jge, 1);
	_SetAsm(JL, "jl", _Jl, 1);
	_SetAsm(JLE, "jle", _Jle, 1);
	_SetAsm(JNA, "jna", _Jna, 1);
	_SetAsm(JNAE, "jnae", _Jnae, 1);
	_SetAsm(JNB, "jnb", _Jnb, 1);
	_SetAsm(JNBE, "jnbe", _Jnbe, 1);
	_SetAsm(JNC, "jnc", _Jnc, 1);
	_SetAsm(JNE, "jne", _Jne, 1);
	_SetAsm(JNG, "jng", _Jng, 1);
	_SetAsm(JNGE, "jnge", _Jnge, 1);
	_SetAsm(JNL, "jnl", _Jnl, 1);
	_SetAsm(JNLE, "jnle", _Jnle, 1);
	_SetAsm(JNO, "jno", _Jno, 1);
	_SetAsm(JNP, "jnp", _Jnp, 1);
	_SetAsm(JNS, "jns", _Jns, 1);
	_SetAsm(JNZ, "jnz", _Jnz, 1);
	_SetAsm(JO, "jo", _Jo, 1);
	_SetAsm(JP, "jp", _Jp, 1);
	_SetAsm(JS, "js", _Js, 1);
	_SetAsm(JZ, "jz", _Jz, 1);
	_SetAsm(XCHG, "xchg", _Xchg, 2);
	_SetAsm(AND, "and", _And, 3);
	_SetAsm(OR, "or", _Or, 3);
	_SetAsm(XOR, "xor", _Xor, 3);
	_SetAsm(INC, "inc", _Inc, 1);
	_SetAsm(DEC, "dec", _Dec, 1);
	_SetAsm(LNOT, "lnot", _Lnot, 1);
	_SetAsm(NOT, "not", _Not, 1);
	_SetAsm(PUSH, "push", _Push, 1);
	_SetAsm(POP, "pop", _Pop, 1);
	_SetAsm(LOOP, "loop", _Loop, 1);
	_SetAsm(JMP, "jmp", _Jmp, 1);
	_SetAsm(NEW, "new", _New, 2);
	_SetAsm(FREE, "free", _Free, 1);
	_SetAsm(FMOV, "fmov", _Fmov, 2);
	_SetAsm(FADD, "fadd", _Fadd, 3);
	_SetAsm(FSUB, "fsub", _Fsub, 3);
	_SetAsm(FMUL, "fmul", _Fmul, 3);
	_SetAsm(FDIV, "fdiv", _Fdiv, 3);
	_SetAsm(FCMP, "fcmp", _Fcmp, 2);
	_SetAsm(FPUSH, "fpush", _Fpush, 1);
	_SetAsm(FPOP, "fpop", _Fpop, 1);
	_SetAsm(FTI, "fti", _Fti, 2);
	_SetAsm(ITF, "itf", _Itf, 2);
	_SetAsm(FRND, "frnd", _Frnd, 2);
	_SetAsm(TEST, "test", _Test, 2);
	_SetAsm(FTST, "ftst", _Ftst, 2);
	_SetAsm(INT, "int", _Int, 1);
	_SetAsm(JCMP, "jcmp", _Jcmp, 2);
}

Assembler::Assembler() {

	if (!isVirtualMachineInitialized) {
		_InitializeVirtualMachine();
	}
}

AsmInfoPtr Assembler::GetAsmInfo(Uint8 command) {
	return &asmArray[command];
}

LAME_END2