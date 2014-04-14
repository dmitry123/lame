#include "Script.h"

LAME_BEGIN

ScriptVirtualMachine& ScriptVirtualMachine::Execute(ScriptPerformerPtr performer) {

	ScriptNodePtr node = performer->root;

	while (node) {

		if (node->type == kScriptNodeThread) {
			break;
		}

		node = node->next;
	}

	if (!node) {
		PostSyntaxError(performer->root->object->line, "Unable to find entry point", 1);
	}

	node->Evaluate(performer);

	return *this;
}

LAME_END