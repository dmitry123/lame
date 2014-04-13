#include "Script.h"

LAME_BEGIN

ScriptVirtualMachine& ScriptVirtualMachine::Execute(ScriptPerformerPtr performer) {

	ScriptNodePtr node = performer->root;

	node->Evaluate(performer);

	return *this;
}

LAME_END