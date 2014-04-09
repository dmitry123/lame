#include "Script.h"

LAME_BEGIN

ScriptVirtualMachine& ScriptVirtualMachine::Execute(ScriptPerformerPtr performer) {

	ScriptNodePtr node = performer->root->next;

	while (node) {

		if (node->type == kScriptNodeCondition) {
			node->Evaluate(performer);
		}

		node = node->next;
	}

	return *this;
}

LAME_END