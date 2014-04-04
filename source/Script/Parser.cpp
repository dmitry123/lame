#include "Script.h"

LAME_BEGIN

static inline Bool IsDigit(Sint8 symbol) {
    
	return
        symbol >= '0' &&
        symbol <= '9';
}

static inline Bool IsLetter(Sint8 symbol) {
    
	return
        (symbol >= 'a' && symbol <= 'z') ||
        (symbol >= 'A' && symbol <= 'Z');
}

static inline Bool IsIntValue(StringC string) {
    
	String result;

	strtol(string, &result, 10);

	if (string != result) {
		return LAME_TRUE;
	}

	return LAME_FALSE;
}

static inline Bool IsStringValue(StringC string) {
    
	Uint32 length = (Uint32)strlen(string);
    
	if (length <= 1) {
		return LAME_FALSE;
	}
    
	return
        (string[0] == '\"' && string[length - 1] == '\"') ||
        (string[0] == '\'' && string[length - 1] == '\'');
}

static inline Bool IsIntValue(const Buffer& string) {
    
	return IsIntValue(string.data());
}

static inline Bool IsFloatValue(StringC string) {
    
	Sint8P finish;
    
	if (!string) {
		return LAME_FALSE;
	}
    
	strtof(string, &finish);
    
	if (finish != string && (*finish == 'f' || !*finish)) {
		return LAME_TRUE;
	}
    
	return LAME_FALSE;
}

static inline Bool IsFloatValue(const Buffer& string) {
    
	if (!string.length()) {
		return LAME_FALSE;
	}
	else {
		return IsFloatValue(string.data());
	}
}

static inline Bool IsValue(const Buffer& string) {
    
	return
        IsIntValue(string) ||
        IsFloatValue(string);
}

static inline Sint32 ParseIntValue(const Buffer& string) {
    
	return atoi(string.data());
}

static inline Float32 ParseFloatValue(StringC string) {
    
	return strtof(string, NULL);
}

static inline Float32 ParseFloatValue(const Buffer& string) {
    
	return strtof(string.data(), NULL);
}

static inline Buffer ParseStringValue(StringC string) {
    
	Buffer result = string + 1;
	result[result.length() - 1] = '\0';
	return result;
}

ScriptParser& ScriptParser::Parse(StringC script) {
    
	Bool isCommentLock = 0;
	Buffer constString;
	Uint32 line = 1;

	while (*script) {

		ScriptObject key = ScriptObject().Parse(&script);

		while (
			*(script) == ' ' ||
			*(script) == '\t' ||
			*(script) == '\r' ||
			*(script) == '\n'
		) {
			if (!*script) {
				goto __ExitLoop;
			}
			if (*script == '\n' || *script == '\r') {
				++line;
			}
			script++;
		}

		key.line = line;

		if (key.flag == kScriptQuote || key.flag == kScriptApostrophe) {
			constString.clear();
			while (*script && *script != *key.GetString()) {
				constString.append(1, *script++);
			} ++script;
			key.flag = kScriptString;
			key.word = constString;
            key.associativity = kScriptAssociativityRight;
		}

		if (key.flag == kScriptCommentL) {
			isCommentLock = LAME_TRUE; continue;
		} else if (key.flag == kScriptCommentR) {
			isCommentLock = LAME_FALSE; continue;
		}

		if (key.flag == kScriptCommentLine) {
			while (*script && *script != '\n') {
				++script;
			}
			continue;
		}

		if (isCommentLock) {
			continue;
		}

		if (key.word.length()) {
			this->keyList.push_back(key);
		}
	}
__ExitLoop:

    this->RegisterConstants();

	return *this;
}

ScriptParser& ScriptParser::Build(ScriptPerformerPtr performer) {

	List<ScriptNodePtr> nodeStack;
	Vector<ScriptNodePtr>* lastStack = 0;
	ScriptNodePtr prevNode = 0;
	ScriptNodePtr lastNode = 0;
	ScriptNodePtr lastParent = 0;
	ScriptNode node;
	Uint32 extraParentheses = 0;
	Bool isSingleCmdCondition = 0;

	for (ScriptObject& key : this->keyList) {

		node.object = &key;
		node.parent = lastNode;

		if (node.object->var) {
			node.object->var->object = &key;
		}

		this->nodeList.push_back(node);

		if (key.IsArgsBegin()) {
			if (!prevNode) {
				PostSyntaxError(key.line, "Empty block in unknown place without parent", 1);
			}
			if (!prevNode->object->IsCondition()) {
				if (!lastNode) {
					PostSyntaxError(key.line, "Empty block in unknown place without parent", 1);
				}
			}
			if (lastNode != prevNode) {
				++extraParentheses; goto __SaveAsNode;
			}
		}
		else if (key.IsBlockBegin()) {
		__BeginBlock:
			if (extraParentheses) {
				PostSyntaxError(key.line, "Parentheses mismatched", 1);
			}
			if (!prevNode) {
				PostSyntaxError(key.line, "Empty block in unknown place without parent", 1);
			}
			if (!prevNode->object->IsCondition()) {
				if (!lastNode) {
					PostSyntaxError(key.line, "Empty block in unknown place without parent", 1);
				}
			}
			lastStack = &lastNode->block;
		}
		else if (key.IsArgsEnd()) {
			if (!lastStack) {
				PostSyntaxError(key.line, "Parentheses mismatched", 1);
			}
			if (!extraParentheses) {
				if (lastStack->size()) {
					lastStack = LAME_NULL;
				}
			}
			else {
				if (extraParentheses) {
					extraParentheses--;
					goto __SaveAsNode;
				}
			}
		}
		else if (key.IsBlockEnd()) {
		__EndBlock:
			if (extraParentheses) {
				PostSyntaxError(key.line, "Parentheses mismatched", 1);
			}
			if (!lastStack || !nodeStack.size()) {
				PostSyntaxError(key.line, "Braces mismatched", 1);
			}
			if (lastStack->size()) {
				lastStack = LAME_NULL;
			}
			lastNode->Order();
			lastParent = nodeStack.back();
			nodeStack.pop_back();
			if (nodeStack.size()) {
				lastNode = nodeStack.back();
				lastStack = &lastNode->block;
			}
			else {
				lastNode = LAME_NULL;
				lastStack = LAME_NULL;
			}
		}
		else {
		__SaveAsNode:

			if (!lastStack && lastNode) {
				isSingleCmdCondition = LAME_TRUE;
				lastNode->block.push_back(&this->nodeList.back());
				goto __BeginBlock;
			}

			if (node.object->flag == kScriptSemicolon) {
				if (isSingleCmdCondition) {
					isSingleCmdCondition = LAME_FALSE;
					goto __EndBlock;
				}
			}

			if (lastStack) {
				lastStack->push_back(&this->nodeList.back());
			}
			else {
				performer->nodeTree.push_back(&this->nodeList.back());
			}
		}

		prevNode = &this->nodeList.back();

		if (prevNode->object->IsCondition()) {
			lastNode = prevNode;
			lastNode->parent = lastParent;
			nodeStack.push_back(lastNode);
			lastStack = &lastNode->condition;
		}
	}

	return *this;
}

ScriptParser& ScriptParser::Load(StringC file) {

	File handle;
	Buffer script;

	handle.Open(file, "rt");
	script.resize(handle.GetSize());
	handle.Read((String)script.data(), handle.GetSize());
	handle.Close();

	this->Parse(script.data());

	return *this;
}

Void ScriptParser::RegisterConstants(Void) {
    
    ScriptVariable var;
    
    for (ScriptObject& key : this->keyList) {
        
        var.Reset();
        
        switch (key.flag) {
            case kScriptInt:
                var.intValue = ParseIntValue(key.word);
                var.type = kScriptTypeInt;
                break;
            case kScriptFloat:
                var.floatValue = ParseFloatValue(key.word);
                var.type = kScriptTypeFloat;
                break;
            case kScriptString:
                var.stringValue = key.word;
                var.type = kScriptTypeString;
                break;
            default:
                continue;
        }
        
        this->varList.push_back(var);
        key.var = &this->varList.back();
        key.var->object = &key;
    }
}

LAME_END