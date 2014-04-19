#ifndef __LAME_SCRIPT__PERFORMER__
#define __LAME_SCRIPT__PERFORMER__

#include "Define.h"
#include "Exception.h"
#include "Builder.h"

LAME_BEGIN

class LAME_API ScriptPerformer {
public:
	Void Evaluate(ScriptBuilderPtr builder);
	Void Trace(Void);
private:
	Void _Trace(ScriptNodePtr node);
	Void _Declare(ScriptNodePtr node);
	ScriptVarPtr _Find(BufferRefC name, ScriptNodePtr node);
	Void _RegisterConstant(ScriptNodePtr node);
	Void _Evaluate(Vector<ScriptNodePtr>* list);
	Void _Evaluate(Vector<ScriptNodePtr>* list, Vector<ScriptNodePtr>* result);
	Void _EvaluateArgs(ScriptNodePtr node);
	Void _EvaluateBlock(ScriptNodePtr node);
	ScriptVarPtr _CreateTemp(ScriptVarPtr var);
	ScriptNodePtr _CreateTemp(ScriptNodePtr node);
	Void _EvaluateCondition(ScriptNodePtr node);
private:
	typedef struct {
		ScriptNode node;
		ScriptVar var;
	} Temp;
private:
	Vector<Buffer> cmdList_;
	List<Temp> tempList_;
	ScriptBuilderPtr builder_;
	ScriptManager manager_;
	Uint32 traceDepth_ = 0;
};

LAME_END

#endif // ~__LAME_SCRIPT__PERFORMER__