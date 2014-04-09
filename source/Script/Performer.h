#ifndef __LAME_SCRIPT__PERFORMER__
#define __LAME_SCRIPT__PERFORMER__

#include "Define.h"
#include "Exception.h"
#include "Parser.h"

LAME_BEGIN

typedef class ScriptPerformer ScriptPerformer, *ScriptPerformerPtr;
typedef class ScriptVariable ScriptVariable, *ScriptVariablePtr;

#if defined(LAME_DEBUG)
typedef ScriptPerformerPtr ScriptPerformerAny;
typedef ScriptVariablePtr ScriptVariableAny;
#else
typedef Any<ScriptPerformer> ScriptPerformerAny;
typedef Any<ScriptVariable> ScriptVariableAny;
#endif

class LAME_API ScriptPerformer {
	Void Set(ScriptVariableAny left, ScriptVariableAny right);
	Void Add(ScriptVariableAny left, ScriptVariableAny right);
	Void Sub(ScriptVariableAny left, ScriptVariableAny right);
	Void Mul(ScriptVariableAny left, ScriptVariableAny right);
	Void Div(ScriptVariableAny left, ScriptVariableAny right);
	Void Mod(ScriptVariableAny left, ScriptVariableAny right);
	Void BitAnd(ScriptVariableAny left, ScriptVariableAny right);
	Void BitOr(ScriptVariableAny left, ScriptVariableAny right);
	Void BitXor(ScriptVariableAny left, ScriptVariableAny right);
	Void BitShiftR(ScriptVariableAny left, ScriptVariableAny right);
	Void BitShiftL(ScriptVariableAny left, ScriptVariableAny right);
	Void And(ScriptVariableAny left, ScriptVariableAny right);
	Void Or(ScriptVariableAny left, ScriptVariableAny right);
	Void Above(ScriptVariableAny left, ScriptVariableAny right);
	Void Bellow(ScriptVariableAny left, ScriptVariableAny right);
	Void AboveEqual(ScriptVariableAny left, ScriptVariableAny right);
	Void BellowEqual(ScriptVariableAny left, ScriptVariableAny right);
	Void Equal(ScriptVariableAny left, ScriptVariableAny right);
	Void NotEqual(ScriptVariableAny left, ScriptVariableAny right);
	Void BitNot(ScriptVariableAny left);
	Void Not(ScriptVariableAny left);
	Void Inc(ScriptVariableAny left);
	Void Dec(ScriptVariableAny left);
	Void AsBool(ScriptVariableAny left);
public:
	ScriptPerformer& Evaluate(Void);
	ScriptPerformer& Trace(Void);
};

LAME_END

#endif // ~__LAME_SCRIPT__PERFORMER__