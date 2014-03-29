#include "Script.h"

LAME_BEGIN

inline Bool IsDigit(Sint8 symbol) {
	return symbol >= '0' && symbol <= '9';
}

inline Bool IsLetter(Sint8 symbol) {
	return
		(symbol >= 'a' && symbol <= 'z') ||
		(symbol >= 'A' && symbol <= 'Z');
}

inline Bool IsIntValue(StringC string) {

	Uint32 length = strlen(string);

	for (Uint32 i = 0; i < length; i++) {
		if (!IsDigit(string[i])) {
			return LAME_FALSE;
		}
	}
	return LAME_TRUE;
}

inline Bool IsStringValue(StringC string) {

	Uint32 length = strlen(string);

	return
		(string[0] == '\"' && string[length - 1] == '\"') ||
		(string[0] == '\'' && string[length - 1] == '\'');
}

inline Bool IsIntValue(BufferRefC string) {

	for (char s : string) {
		if (!IsDigit(s)) {
			return LAME_FALSE;
		}
	}
	return LAME_TRUE;
}

inline Bool IsFloatValue(StringC string) {

	Sint8P finish;

	if (!string) {
		return LAME_FALSE;
	}

	strtof(string, &finish);

	if ((*finish == 'f' && *(finish + 1) == '\0') || !*finish) {
		return LAME_TRUE;
	}

	return LAME_FALSE;
}

inline Bool IsFloatValue(BufferRefC string) {

	if (!string.length()) {
		return LAME_FALSE;
	} else {
		return IsFloatValue(string.data());
	}
}

inline Bool IsValue(BufferRefC string) {
	return IsIntValue(string) || IsFloatValue(string);
}

inline Sint32 ParseIntValue(BufferRefC string) {
	return atoi(string.data());
}

inline Float32 ParseFloatValue(BufferRefC string) {
	return strtof(string.data(), NULL);
}

inline Buffer ParseStringValue(StringC string) {
	Buffer result = string + 1;
	result[result.length() - 1] = '\0';
	return result;
}

static StringC allowedTokens = "'!~*/%+-&^|=\"\n\r\t ;,.:-{}()[]";
static StringC compareTokens = "<>=!";
static StringC unaryTokens = "+-";
static StringC logicTokens = "&|^";
static StringC separateTokens = ";{},";

KeyWord& KeyWord::Parse(Uint32 line, StringC word) {

	this->line = line;
	this->word = word;

	if (strchr(separateTokens, *word)) {
		this->key = kKeyWordSeparator;
        this->priority.Parse(word);
		goto __Return;
	} else if (IsIntValue(word)) {
		this->key = kKeyWordInt;
		this->priority = kPriorityVariable;
		this->type = kScriptTypeInt;
		this->intValue = ParseIntValue(word);
		goto __Return;
	} else if (IsFloatValue(word)) {
		this->key = kKeyWordFloat;
		this->priority = kPriorityVariable;
		this->type = kScriptTypeFloat;
		this->floatValue = ParseFloatValue(word);
		goto __Return;
	} else if (IsStringValue(word)) {
		this->key = kKeyWordString;
		this->priority = kPriorityVariable;
		this->type = kScriptTypeString;
		this->stringValue = ParseStringValue(word);
		goto __Return;
	} else if ((this->language.Parse(word) != kScriptLanguageDefault)) {
		this->key = kKeyWordPriority;
		this->priority = kPriorityLanguage;
		goto __Return;
	} else if (this->type.Parse(word) != kScriptTypeDefault) {
		this->key = kKeyWordPriority;
		this->priority = kPriorityType;
		goto __Return;
	} else if ((this->priority.Parse(word)) != kPriorityDefault) {
		this->key = kKeyWordPriority;
		if (this->priority == kPriorityAnd) {
			this->priority = kPriorityBitAnd;
		} else if (this->priority == kPriorityOr) {
			this->priority = kPriorityBitOr;
		}
		goto __Return;
	} else {
		this->key = kKeyWordPriority;
		this->priority = kPriorityVariable;
		goto __Return;
	}

__Return:
	return *this;
}

ScriptParser& ScriptParser::Load(StringC filename) {

    File f;
    Buffer s;
    
    f.Open(filename, "rt");
    s.resize(f.GetSize());
    f.Read((String)s.data(), f.GetSize());
    f.Close();
    
    this->Parse(s);
    
    return *this;
}

ScriptParser& ScriptParser::Parse(StringC script) {

	Uint32 line = 1;
	Sint8 token = 0;
	Uint32 save = 0;
	Uint32 prevSaved = 0;
	Buffer word;
	StringC last = script;

	Bool isToken = 0;
	Bool isParseToken = 0;
	Bool isQuoteLock = 0;
	Bool isApostropheLock = 0;
	Bool isCommentLock = 0;
    
    KeyWordPtr previous = 0;

#ifdef LAME_DEBUG
	printf("%s", script);
#endif

	while (*script) {

		token = *script;

		isToken = 0;
		isParseToken = 0;
		save = 1;
        
		if (*(script + 1)) {
			if (!isCommentLock) {
				if (*script == '/' && *(script + 1) == '*') {
					isCommentLock = 1; ++script;  goto __Continue;
				}
			} else {
				if (*script == '*' && *(script + 1) == '/') {
					isCommentLock = 0; ++script;  goto __Continue;
				}
			}
		}

		if (token == '\"') {
			if (!isApostropheLock) {
				isQuoteLock = !isQuoteLock; goto __Skip;
			}
		} else if (token == '\'') {
			if (!isQuoteLock) {
				isApostropheLock = !isApostropheLock; goto __Skip;
			}
		} else {
			if (isQuoteLock || isApostropheLock || isCommentLock) {
				goto __Skip;
			}
		}

		if (token == '\n' || token == '\r') {
			if (token == '\n') {
				line++;
			}
			token = ' ';
		} else if (token == '\t') {
			token = ' ';
		}

		if (strchr(compareTokens, token)) {
			if (token == '<' && token == '>') {
				save = 1;
			} else if (*(script + 1) == '=') {
				save = 2;
			}
			goto __SaveNonSpaceToken;
		} else if (strchr(logicTokens, token)) {
			switch (*(script + 1)) {
				case '=':
				case '&':
					save = 2; break;
				default:
					save = 1; break;
			}
			goto __SaveNonSpaceToken;
		} else if (strchr(unaryTokens, token)) {
			switch (*(script + 1)) {
				case '+':
				case '-':
					save = 2; break;
				default:
					save = 1; break;
			}
			goto __SaveNonSpaceToken;
		} else if (!strchr(allowedTokens, token)) {
			if (token != ' ') {
				goto __Skip;
			}
		} else {
		__SaveNonSpaceToken:
			if (token != ' ') {
				isToken = 1;
			}
		}

		if (isToken) {
			if (script - last > 0) {
				isParseToken = 1;
				isToken = 0;
				if (save > 1) {
					prevSaved = save;
				}
				if (!save) {
					save = 1;
				}
				save = 1;
			}
		}

		if (isToken || script - last > 0) {
		__ParseToken:
			word.clear();
			if (isToken) {
				word.append(last, script - last, save);
			} else {
				word.append(last, script - last);
			}
			this->keyQueue.push_back(
				KeyWord().Parse(line, word)
			);
            if (this->keyQueue.back().priority == kPriorityType) {
                if (!previous || previous->priority != kPriorityParenthesesL) {
                    this->keyQueue.back().priority = kPriorityDeclare;
                }
            }
			if (this->keyQueue.back().priority == kPriorityLanguage) {
				if (this->keyQueue.back().language == kScriptLanguageElse) {
					this->keyQueue.push_back(
						KeyWord().Parse(line, "(")
					);
					this->keyQueue.push_back(
						KeyWord().Parse(line, ")")
					);
				}
			}
            previous = &this->keyQueue.back();
			script += save - 1;
		}

		if (isParseToken) {
			isParseToken = 0;
			isToken = 1;
			save = prevSaved;
			prevSaved = 0;
			if (!save) {
				save = 1;
			}
			goto __ParseToken;
		}

	__Continue:
		last = script + 1;
	__Skip:
		++script;
	}

	return *this;
}

ScriptParser& ScriptParser::Trace(Bool flag) {

	if (this->yardQueue.size()) {
		puts("\n------------------------");
	}
	Uint32 position = 0;
	for (auto k : this->yardQueue) {
		if (flag) {
			printf("%d: ", position);
		}
        if (!k->word.length()) {
            continue;
        }
		if (k->key == kKeyWordPriority) {
			printf("%s ", k->word.data());
		} else if (k->jump != 0) {
			printf("[%d] ", k->jump);
		} else if (k->key == kKeyWordSeparator) {
			printf("# ");
		} else {
			switch (k->type) {
			case kScriptTypeInt:
				printf("%d ", k->intValue);
				break;
			case kScriptTypeFloat:
				printf("%.2f ", k->floatValue);
				break;
			case kScriptTypeString:
				printf("\"%s\" ", k->stringValue.data());
				break;
			default:
				printf("%s ", k->word.data());
				break;
			}
		}
		if (flag) {
			puts("");
		}
		++position;
	}

	return *this;
}

ScriptParser& ScriptParser::Sort(Void) {

	List <KeyWordPtr> stack;
	List <KeyWordPtr> result;

	Bool found = 0;
	KeyWordPtr back = 0;
    
	for (KeyWord& kw : this->keyQueue) {

		if (kw.priority == kPriorityDeclare) {
			continue;
		}

		if (kw.key == kKeyWordInt ||
			kw.key == kKeyWordFloat ||
			kw.key == kKeyWordDefault ||
			kw.key == kKeyWordString
		) {
			result.push_back(&kw);
		} else if (kw.key == kKeyWordPriority) {
			if (kw.priority == kPriorityVariable) {
				result.push_back(&kw);
			} else if (kw.priority == kPriorityParenthesesL) {
				stack.push_back(&kw);
			} else if (kw.priority == kPriorityParenthesesR) {
				found = 0;
				while (stack.size()) {
					back = stack.back();
					if (back->priority == kPriorityParenthesesL) {
						found = 1; break;
					} else {
						result.push_back(back);
						stack.pop_back();
					}
				}
				if (!found) {
					PostSyntaxError(kw.line, "Parentheses mismatched", 1);
				} else {
					stack.pop_back();
					if (stack.size()) {
						back = stack.back();
						if (back->priority == kPriorityLanguage ||
							back->priority == kPriorityVariable
						) {
							result.push_back(back);
							stack.pop_back();
						}
					}
				}
			} else {
				while (stack.size()) {
					back = stack.back();
					if (back->key == kKeyWordPriority) {
						if (((kw.priority.IsLeftAssociated() && kw.priority.Priority() >= back->priority.Priority())) ||
							(!kw.priority.IsLeftAssociated() && kw.priority.Priority() > back->priority.Priority()))
						{
							result.push_back(back);
							stack.pop_back();
						} else {
							break;
						}
					}
				}
				stack.push_back(&kw);
			}
		} else if (kw.key == kKeyWordSeparator) {
			while (stack.size()) {
				back = stack.back();
				if (back->key == kKeyWordPriority) {
					if (back->priority == kPriorityParenthesesL ||
						back->priority == kPriorityParenthesesR
					) {
						PostSyntaxError(back->line, "Parentheses mismatched", 1);
					} else {
						result.push_back(back);
						stack.pop_back();
					}
				}
			}
			while (result.size()) {
				this->yardQueue.insert(this->yardQueue.end(), result.front());
				result.pop_front();
			}
			this->yardQueue.push_back(&kw);
		}
	}

	if (stack.size() || result.size()) {
		PostSyntaxError(stack.back()->line, "Semicolon missed", 1);
	}

	return *this;
}

LAME_END