#ifndef __LAME_SCRIPT__PRIORITY__
#define __LAME_SCRIPT__PRIORITY__

#include "Define.h"
#include "Type.h"
#include "Language.h"

LAME_BEGIN

typedef enum {
	kPriorityDefault = -1,
	kPriorityPostfixIncrement,
	kPriorityPostfixDecrement,
	kPriorityParenthesesL,
	kPriorityParenthesesR,
	kPriorityBracketL,
	kPriorityBracketR,
	kPriorityDot,
	kPriorityPrefixIncrement,
	kPriorityPrefixDecrement,
	kPriorityNot,
	kPriorityBitNot,
	kPriorityType,
	kPriorityMul,
	kPriorityDiv,
	kPriorityMod,
	kPriorityAdd,
	kPrioritySub,
	kPriorityShiftL,
	kPriorityShiftR,
	kPriorityBellow,
	kPriorityBellowEqual,
	kPriorityAbove,
	kPriorityAboveEqual,
	kPriorityEqual,
	kPriorityNotEqual,
	kPriorityBitAnd,
	kPriorityBitXor,
	kPriorityBitOr,
	kPriorityAnd,
	kPriorityOr,
	kPrioritySet,
	kPriorityAddSet,
	kPrioritySubSet,
	kPriorityMulSet,
	kPriorityDivSet,
	kPriorityModSet,
	kPriorityShiftSetL,
	kPriorityShiftSetR,
	kPriorityBitAndSet,
	kPriorityBitXorSet,
	kPriorityBitOrSet,
	kPriorityComa,
	kPriorityLanguage,
	kPriorityVariable,
	kPriorityAmount
} ScriptPriorities;

static StringC languagePriority[kPriorityAmount] = {
	"++", // 1
	"--", // 1
	"(", // 0
	")", // 0
	"[", // 0
	"]", // 0
	".", // 2
	"++", // 1
	"--", // 1
	"!", // 1
	"~", // 1
	"${type}", // 1
	"*", // 2
	"/", // 2
	"%", // 2
	"+", // 2
	"-", // 2
	"<<", // 2
	">>", // 2
	"<", // 2
	"<=", // 2
	">", // 2
	">=", // 2
	"==", // 2
	"!=", // 2
	"&", // 2
	"^", // 2
	"|", // 2
	"&&", // 2
	"||", // 2
	"=", // 2
	"+=", // 2
	"-=", // 2
	"*=", // 2
	"/=", // 2
	"%=", // 2
	"<<=", // 2
	">>=", // 2
	"&=", // 2
	"^=", // 2
	"|=", // 2
	",", // 2
	"${language}", // 0
	"${variable}" // 0
};

inline Bool IsLanguagePriority(StringC word) {
	for (Uint32 i = 0; i < kPriorityAmount; i++) {
		if (!strncmp(word, languagePriority[i], strlen(languagePriority[i]))) {
			return LAME_FALSE;
		}
	}
	return LAME_FALSE;
}

inline Bool IsPriorityType(ScriptPriorities priority) {
	return priority == kPriorityType;
}

inline Bool IsLanguagePriority(BufferRefC word) {
	return IsLanguagePriority(word.data());
}

inline ScriptPriorities ParseLanguagePriority(StringC word) {
	for (Uint32 i = 0; i < kPriorityAmount; i++) {
		if (!languagePriority[i]) {
			break;
		}
		if (!strncmp(word, languagePriority[i], strlen(languagePriority[i]))) {
			return (ScriptPriorities)i;
		}
	}
	if (ScriptType().Parse(word) != kScriptTypeDefault) {
		return kPriorityType;
	}
	if (ScriptLanguage().Parse(word) != kScriptLanguageDefault) {
		return kPriorityLanguage;
	}
	return kPriorityDefault;
}

inline ScriptPriorities ParseLanguagePriority(BufferRefC word) {
	return ParseLanguagePriority(word.data());
}

inline Bool IsPriorityLeftAssociated(ScriptPriorities priority) {
	return priority >= kPriorityMul && priority <= kPriorityBitOrSet;
}

inline Uint32 GetPriority(ScriptPriorities priority) {
	return (Uint32)priority;
}

inline Uint32 GetPriorityCountOfArguments(ScriptPriorities priority) {
	if ((priority >= kPriorityMul && priority <= kPriorityComa) || priority == kPriorityDot) {
		return 2;
	}
	if (IsPriorityType(priority)) {
		return 1;
	}
	switch (priority) {
		case kPriorityPostfixIncrement:
		case kPriorityPostfixDecrement:
		case kPriorityPrefixIncrement:
		case kPriorityPrefixDecrement:
		case kPriorityNot:
		case kPriorityBitNot:
		case kPriorityLanguage:
			return 1;
	}
	return 0;
}

LAME_END

#endif // ~__LAME_SCRIPT__PRIORITY__