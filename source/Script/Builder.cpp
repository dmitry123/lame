#include "Script.h"

LAME_BEGIN

ScriptBuilder& ScriptBuilder::Analyze(ScriptParserPtr parser, ScriptPerformerPtr performer) {

	this->parser = parser;
	this->performer = performer;

	if (!performer->root) {
		performer->root = new ScriptNode();
	}

	this->AnalizeConstruction(performer->root, this->parser->objectList.begin());

	return *this;
}

Void ScriptBuilder::AnalizeConstruction(ScriptNodePtr node, Iterator& i) {

	ScriptNodePtr prevNode = 0;

	while (i != this->parser->objectList.end()) {

		ScriptNodePtr next = new ScriptNode();

		if (!prevNode) {
			node->next = next;
		}
		else {
			prevNode->next = next;
		}

		next->parent = node;
		next->object = *i;

		switch ((*i)->object) {
		case kScriptObjectClass:
			this->BuildConstruction(next, i);
			break;
		case kScriptObjectIf:
		case kScriptObjectElse:
		case kScriptObjectWhile:
		case kScriptObjectDo:
		case kScriptObjectFor:
			this->BuildCondition(next, i);
			break;
		default:
			continue;
		}

		prevNode = next; ++i;
	}
}

Void ScriptBuilder::BuildConstruction(ScriptNodePtr node, Iterator& i) {
	
	Vector<ScriptNodePtr> nodeStack;
	Vector<ScriptNodePtr>* lastStack = 0;
	ScriptNodePtr prevNode = 0;
	ScriptNodePtr lastParent = 0;
	Uint32 extraParentheses = 0;
	StringC typeName = 0;
	Uint32 modificators = 0;

	node->object->type.name = (*++i)->word;

	if (node->object->type == kScriptTypeDefault) {
		node->type = kScriptNodeClass;
	}
	else {
		node->type = kScriptNodeFunction;
	}

	while (++i != this->parser->objectList.end()) {

		ScriptNodePtr next = new ScriptNode();

		next->parent = node;
		next->object = *i;

		if (!next->var) {
			next->var = new ScriptVariable(next->object);
		}

		if (next->object->object == kScriptObjectDefault) {
			typeName = next->object->word.data();
			next->object->type.Parse(&typeName);
			if (next->object->type == kScriptTypeDefault) {
				ScriptNodePtr typeNode = this->performer->FindType(typeName);
				if (typeNode) {
					next = typeNode;
					next->object->object = kScriptObjectType;
					next->object->arguments = 1;
				}
			}
			else {
				next->object->object = kScriptObjectType;
				next->object->arguments = 1;
			}
		}

		if (next->object->IsModificator()) {
			switch (next->object->object) {
				case kScriptObjectPublic:
					modificators |= kScriptModificatorPublic;
					break;
				case kScriptObjectPrivate:
					modificators |= kScriptModificatorPrivate;
					break;
				case kScriptObjectProtected:
					modificators |= kScriptModificatorProtected;
					break;
				case kScriptObjectFinal:
					modificators |= kScriptModificatorFinal;
					break;
				case kScriptObjectOverride:
					modificators |= kScriptModificatorOverride;
					break;
				case kScriptObjectAbstract:
					modificators |= kScriptModificatorAbstract;
					break;
				case kScriptObjectStatic:
					modificators |= kScriptModificatorStatic;
					break;
			}
			continue;
		}
		else {
			next->var->modificators = modificators;
			modificators = 0;
		}

		switch ((*i)->object) {
			case kScriptObjectBraceL:
				if (extraParentheses) {
					PostSyntaxError(next->object->line, "Parentheses mismatched", 1);
				}
				lastStack = &node->block;
				break;
			case kScriptObjectBraceR:
				if (!lastStack) {
					PostSyntaxError(next->object->line, "Braces mismatched", 1);
				}
				if (extraParentheses) {
					PostSyntaxError(next->object->line, "Parentheses mismatched", 1);
				}
				goto __ExitLoop;
				break;
			case kScriptObjectParentheseL:
				if (lastStack != &node->condition) {
					lastStack = &node->condition;
				}
				else {
					++extraParentheses; goto __SaveNode;
				}
				break;
			case kScriptObjectParentheseR:
				if (!lastStack) {
					PostSyntaxError(next->object->line, "Parentheses mismatched", 1);
				}
				if (!extraParentheses) {
					lastStack = 0;
				}
				else {
					--extraParentheses; goto __SaveNode;
				}
				break;
			case kScriptObjectType:
				if (node->type == kScriptNodeClass) {
					this->BuildConstruction(next, i);
				}
				goto __SaveNode;
			default:
			__SaveNode:
				if (lastStack) {
					lastStack->push_back(next);
					if (next->object->IsCondition()) {
						this->BuildCondition(next, i);
					}
				}
				break;
		}
		prevNode = next;
	}
	PostSyntaxError(this->parser->objectList.back()->line, "Braces mismatched", 1);
__ExitLoop:

	// register current class as type
	this->performer->RegisterType(node);

	// order node's condition and block
	node->Order(this->performer);

	// seeking for constants in block or condition and registering its variable
	for (ScriptNodePtr node : node->condition) {
		switch (node->object->object) {
		case kScriptObjectInt:
			node->var = new ScriptVariable(node->object);
			node->object->type = kScriptTypeInt;
			node->var->intValue = ScriptObject::ParseIntValue(node->object->word);
			break;
		case kScriptObjectFloat:
			node->var = new ScriptVariable(node->object);
			node->object->type = kScriptTypeFloat;
			node->var->floatValue = ScriptObject::ParseFloatValue(node->object->word);
			break;
		case kScriptObjectString:
			node->var = new ScriptVariable(node->object);
			node->object->type = kScriptTypeString;
			node->var->stringValue = ScriptObject::ParseStringValue(node->object->word);
			break;
		case kScriptObjectBool:
			node->var = new ScriptVariable(node->object);
			node->object->type = kScriptTypeBool;
			node->var->boolValue = (ScriptNativeBool)ScriptObject::ParseIntValue(node->object->word);
			break;
		}
	}
	for (ScriptNodePtr node : node->block) {
		switch (node->object->object) {
		case kScriptObjectInt:
			node->var = new ScriptVariable(node->object);
			node->object->type = kScriptTypeInt;
			node->var->intValue = ScriptObject::ParseIntValue(node->object->word);
			break;
		case kScriptObjectFloat:
			node->var = new ScriptVariable(node->object);
			node->object->type = kScriptTypeFloat;
			node->var->floatValue = ScriptObject::ParseFloatValue(node->object->word);
			break;
		case kScriptObjectString:
			node->var = new ScriptVariable(node->object);
			node->object->type = kScriptTypeString;
			node->var->stringValue = ScriptObject::ParseStringValue(node->object->word);
			break;
		case kScriptObjectBool:
			node->var = new ScriptVariable(node->object);
			node->object->type = kScriptTypeBool;
			node->var->boolValue = (ScriptNativeBool)ScriptObject::ParseIntValue(node->object->word);
			break;
		}
	}
}

Void ScriptBuilder::BuildCondition(ScriptNodePtr node, Iterator& i) {

	Vector<ScriptNodePtr> nodeStack;
	Vector<ScriptNodePtr>* lastStack = 0;
	ScriptNodePtr prevNode = 0;
	ScriptNodePtr lastParent = 0;
	Uint32 extraParentheses = 0;

	node->type = kScriptNodeCondition;

	while (++i != this->parser->objectList.end()) {

		ScriptNodePtr next = new ScriptNode();

		next->parent = node;
		next->object = *i;

		switch ((*i)->object) {
			case kScriptObjectBraceL:
				if (extraParentheses) {
					PostSyntaxError(next->object->line, "Parentheses mismatched", 1);
				}
				lastStack = &node->block;
				break;
			case kScriptObjectBraceR:
				if (!lastStack) {
					PostSyntaxError(next->object->line, "Braces mismatched", 1);
				}
				if (extraParentheses) {
					PostSyntaxError(next->object->line, "Parentheses mismatched", 1);
				}
				goto __ExitLoop;
				break;
			case kScriptObjectParentheseL:
				if (prevNode && !prevNode->object->IsCondition()) {
					++extraParentheses; goto __SaveNode;
				}
				if (lastStack != &node->condition) {
					lastStack = &node->condition;
				}
				else {
					++extraParentheses; goto __SaveNode;
				}
				break;
			case kScriptObjectParentheseR:
				if (!lastStack) {
					PostSyntaxError(next->object->line, "Parentheses mismatched", 1);
				}
				if (!extraParentheses) {
					lastStack = 0;
				}
				else {
					--extraParentheses; goto __SaveNode;
				}
				break;
			default:
			__SaveNode:
				if (lastStack) {
					lastStack->push_back(next);
					if (next->object->IsCondition()) {
						this->BuildCondition(next, i);
					}
				}
				break;
		}
		prevNode = next;
	}
	PostSyntaxError(this->parser->objectList.back()->line, "Braces mismatched", 1);
__ExitLoop:

	// order node's condition and block
	node->Order(this->performer);

	// seeking for constants in block or condition and registering its variable
	for (ScriptNodePtr node : node->condition) {
		switch (node->object->object) {
			case kScriptObjectInt:
				node->var = new ScriptVariable(node->object);
				node->object->type = kScriptTypeInt;
				node->var->intValue = ScriptObject::ParseIntValue(node->object->word);
				break;
			case kScriptObjectFloat:
				node->var = new ScriptVariable(node->object);
				node->object->type = kScriptTypeFloat;
				node->var->floatValue = ScriptObject::ParseFloatValue(node->object->word);
				break;
			case kScriptObjectString:
				node->var = new ScriptVariable(node->object);
				node->object->type = kScriptTypeString;
				node->var->stringValue = ScriptObject::ParseStringValue(node->object->word);
				break;
			case kScriptObjectBool:
				node->var = new ScriptVariable(node->object);
				node->object->type = kScriptTypeBool;
				node->var->boolValue = (ScriptNativeBool)ScriptObject::ParseIntValue(node->object->word);
				break;
		}
	}
	for (ScriptNodePtr node : node->block) {
		switch (node->object->object) {
		case kScriptObjectInt:
			node->var = new ScriptVariable(node->object);
			node->object->type = kScriptTypeInt;
			node->var->intValue = ScriptObject::ParseIntValue(node->object->word);
			break;
		case kScriptObjectFloat:
			node->var = new ScriptVariable(node->object);
			node->object->type = kScriptTypeFloat;
			node->var->floatValue = ScriptObject::ParseFloatValue(node->object->word);
			break;
		case kScriptObjectString:
			node->var = new ScriptVariable(node->object);
			node->object->type = kScriptTypeString;
			node->var->stringValue = ScriptObject::ParseStringValue(node->object->word);
			break;
		case kScriptObjectBool:
			node->var = new ScriptVariable(node->object);
			node->object->type = kScriptTypeBool;
			node->var->boolValue = (ScriptNativeBool)ScriptObject::ParseIntValue(node->object->word);
			break;
		}
	}
}

LAME_END