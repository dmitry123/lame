class Class {

};

class Register {
public:
	enum {
		DEFAULT,
		BYTE,
		SHORT,
		INTEGER,
		LONG,
		FLOAT,
		DOUBLE
	}
public:
	// register flags
	const Uin32 flags;
	// register offset for <v>
	const Uint8 offset;
public:
	enum {
		TYPES = 3
	};
public:
	struct {
		Sint64  i; // 0
		Float64 f; // 1
		Uint32  c; // 2
	} v;
private:

};

enum {
	ADD,
	SUB,
	DIV,
	MUL,
	MOD,
	MOV,
	SHR,
	SHL,
	CMP,
	PUSH,
	POP,
	CALL,
	OFFSET
};

class RegisterPerformer {
	typedef RegisterPtr R;
public:
	typedef void (*CommandFunction)(R, R);
private:
	static CommandFunction add[Register::TYPES] = { AddI, AddF, NULL };
	static CommandFunction sub[Register::TYPES] = { SubI, SubF, NULL };
	static CommandFunction mul[Register::TYPES] = { MulI, MulF, NULL };
	static CommandFunction div[Register::TYPES] = { DivI, DivF, NULL };
	static CommandFunction mod[Register::TYPES] = { ModI, ModF, NULL };
	static CommandFunction mov[Register::TYPES] = { MovI, MovF, MovC };
private:
	static CommandFunction* cmd[] = {
		add,
		sub,
		mul,
		div,
		mov
	};
public:
	static void Apply(R left, R right, Uint32 command) {
		cmd[command][left->offset](left, right);
	}
private:
	static void AddI(R left, R right) { left->v.l += right->v.l; }
	static void AddF(R left, R right) { left->v.f += right->v.f; }
	static void SubI(R left, R right) { left->v.i -= right->v.i; }
	static void SubF(R left, R right) { left->v.f -= right->v.f; }
	static void MulI(R left, R right) { left->v.i *= right->v.i; }
	static void MulF(R left, R right) { left->v.f *= right->v.f; }
	static void DivI(R left, R right) { left->v.i /= right->v.i; }
	static void DivF(R left, R right) { left->v.f /= right->v.f; }
	static void ModI(R left, R right) { left->v.i %= right->v.i; }
	static void ModF(R left, R right) { left->v.f %= right->v.f; }
	static void MovI(R left, R right) { left->v.i  = right->v.i; }
	static void MovF(R left, R right) { left->v.f  = right->v.f; }
	static void MovC(R left, R right) { left->v.c  = right->v.c; }
};