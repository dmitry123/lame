#ifndef __LAME_COMPILER_INTERNAL__
#define __LAME_COMPILER_INTERNAL__

#include "Define.h"

#include <VirtualMachine/VirtualMachine.h>

LAME_BEGIN2(Compiler)

#define REGISTERS_COUNT 10

using namespace Script;
using namespace VirtualMachine;

static Map<LexID, Class::Operator> operatorMap =
{
	{ kScriptLexDefault, Class::Operator::Unkown },
	{ kScriptLexAdd, Class::Operator::Add },
	{ kScriptLexAddSet, Class::Operator::Add },
	{ kScriptLexSub, Class::Operator::Sub },
	{ kScriptLexSubSet, Class::Operator::Sub },
	{ kScriptLexMul, Class::Operator::Mul },
	{ kScriptLexMulSet, Class::Operator::Mul },
	{ kScriptLexDiv, Class::Operator::Div },
	{ kScriptLexDivSet, Class::Operator::Div },
	{ kScriptLexMod, Class::Operator::Mod },
	{ kScriptLexModSet, Class::Operator::Mod },
	{ kScriptLexAbove, Class::Operator::Above },
	{ kScriptLexBellow, Class::Operator::Bellow },
	{ kScriptLexEqual, Class::Operator::Equal },
	{ kScriptLexNotEqual, Class::Operator::NotEqual },
	{ kScriptLexSet, Class::Operator::Move },
	{ kScriptLexBitShiftR, Class::Operator::ShiftR },
	{ kScriptLexBitShiftSetR, Class::Operator::ShiftR },
	{ kScriptLexBitShiftL, Class::Operator::ShiftL },
	{ kScriptLexBitShiftSetL, Class::Operator::ShiftL },
	{ kScriptLexBitAnd, Class::Operator::BitAnd },
	{ kScriptLexBitAndSet, Class::Operator::BitAnd },
	{ kScriptLexBitOr, Class::Operator::BitOr },
	{ kScriptLexBitOrSet, Class::Operator::BitOr },
	{ kScriptLexBitXor, Class::Operator::BitXor },
	{ kScriptLexBitXorSet, Class::Operator::BitXor },
	{ kScriptLexBellowEqual, Class::Operator::BellowE },
	{ kScriptLexAboveEqual, Class::Operator::AboveE },
	{ kScriptLexAnd, Class::Operator::And },
	{ kScriptLexOr, Class::Operator::Or },
	{ kScriptLexIncrement, Class::Operator::Inc },
	{ kScriptLexDecrement, Class::Operator::Dec },
	{ kScriptLexSizeof, Class::Operator::Sizeof },
	{ kScriptLexBitNot, Class::Operator::BitNot },
	{ kScriptLexNot, Class::Operator::Not },
	{ kScriptLexCast, Class::Operator::Cast }
};

typedef struct {
	VariablePtr registerVar;
	Bool isAvailable;
} AvailableRegister, *AvailableRegisterPtr;

static AvailableRegister isRegisterAvailableL[REGISTERS_COUNT] = {
		{ GlobalScope::r0, TRUE },
		{ GlobalScope::r1, TRUE },
		{ GlobalScope::r2, TRUE },
		{ GlobalScope::r3, TRUE },
		{ GlobalScope::r4, TRUE },
		{ GlobalScope::r5, TRUE },
		{ GlobalScope::r6, TRUE },
		{ GlobalScope::r7, TRUE },
		{ GlobalScope::r8, TRUE },
		{ GlobalScope::r9, TRUE }
};

static AvailableRegister isRegisterAvailableF[REGISTERS_COUNT] = {
		{ GlobalScope::f0, TRUE },
		{ GlobalScope::f1, TRUE },
		{ GlobalScope::f2, TRUE },
		{ GlobalScope::f3, TRUE },
		{ GlobalScope::f4, TRUE },
		{ GlobalScope::f5, TRUE },
		{ GlobalScope::f6, TRUE },
		{ GlobalScope::f7, TRUE },
		{ GlobalScope::f8, TRUE },
		{ GlobalScope::f9, TRUE }
};

static Void _ClearAvailableRegisters() {

	for (Uint32 i = 0; i < REGISTERS_COUNT; i++) {
		isRegisterAvailableL[i].isAvailable = TRUE;
		isRegisterAvailableF[i].isAvailable = TRUE;
	}
}

static VariablePtr _FindAvailableRegister(Bool isFloat = FALSE) {

	AvailableRegisterPtr registerList = isFloat ?
	isRegisterAvailableF :
						 isRegisterAvailableL;
	for (Uint32 i = 0; i < REGISTERS_COUNT; i++) {
		if (registerList[i].isAvailable) {
			registerList[i].isAvailable = FALSE;
			return registerList[i].registerVar;
		}
	}
	__asm int 3
}

static VariablePtr _FindAvailableRegisterF() {

	for (Uint32 i = 0; i < REGISTERS_COUNT; i++) {
		if (isRegisterAvailableF[i].isAvailable) {
			isRegisterAvailableF[i].isAvailable = FALSE;
			return isRegisterAvailableF[i].registerVar;
		}
	}
	__asm int 3
}

static Void _ReleaseRegister(VariablePtr var) {

	if (var->CheckModificator(Class::Modificator::Register)) {
		if (var->GetName()[0] == 'r') {
			isRegisterAvailableL[var->GetName()[1] - '0'].isAvailable = TRUE;
		}
		else if (var->GetName()[0] == 'f') {
			isRegisterAvailableF[var->GetName()[1] - '0'].isAvailable = TRUE;
		}
	}

	var->stringValue.clear();
	var->v.intValue = 0;
	if (var->objectValue && var->objectValue->DecRef()) {
		delete var->objectValue;
	}
	var->objectValue = 0;
}

static Void _ReleaseAllRegisters(Void) {

	for (Uint32 i = 0; i < REGISTERS_COUNT; i++) {
		if (isRegisterAvailableL[i].isAvailable) {
			_ReleaseRegister(isRegisterAvailableL[i].registerVar);
		}
		if (isRegisterAvailableF[i].isAvailable) {
			_ReleaseRegister(isRegisterAvailableF[i].registerVar);
		}
	}
}

LAME_END2

#endif // ~__LAME_COMPILER_INTERNAL__