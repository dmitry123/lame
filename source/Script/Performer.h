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
    Void _EvaluateNew(ScriptNodePtr node, ScriptNodePtr left, ScriptVarPtr var);
    Void _EvaluateSingle(ScriptNodePtr node, ScriptNodePtr left);
    Void _EvaluateReturn(ScriptNodePtr node, ScriptNodePtr left);
	ScriptNodePtr _InvokeMethod(ScriptNodePtr node, ScriptNodePtr expNode, ScriptNodePtr invokeNode, ScriptNodePtr left);
	ScriptNodePtr _CreateTemp(ScriptVarPtr var);
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
	Vector<ScriptVarPtr> varList_;
	Vector<ScriptVarPtr> newList_;
public:
	~ScriptPerformer();
};

LAME_END

#endif // ~__LAME_SCRIPT__PERFORMER__