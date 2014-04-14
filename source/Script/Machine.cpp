#include "Script.h"

LAME_BEGIN

ScriptVirtualMachine& ScriptVirtualMachine::Execute(ScriptPerformerPtr performer) {

	ScriptNodePtr node = performer->root;

	while (node && node->object->object != kScriptObjectThread) {
		node = node->next;
	}

	if (!node) {
		PostSyntaxError(performer->root->object->line, "Unable to find entry point", 1);
	}

	node->Evaluate(performer);

	return *this;
}

LAME_END