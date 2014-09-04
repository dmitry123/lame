#ifndef __LAME_COMPILER_ABSTRACT_TRANSLATOR__
#define __LAME_COMPILER_ABSTRACT_TRANSLATOR__

#include "Define.h"

LAME_BEGIN2(Compiler)

class LAME_API AbstractTranslator {
public:
	enum class Type : Uint32 {
		Default,
		Construction,
		Class,
		Method,
		Math,
		Logic,
		Boolean
	};
public:
	AbstractTranslator(TranslateEnginePtr engine, Type type) :
		translateEngine(engine),
		compilationType(type)
	{
	}
public:
	virtual ~AbstractTranslator() {
	}
public:
	virtual Void onTranslate(NodePtr node) = 0;
public:
	inline Bool CheckCompilationType(Type type) {
		return this->compilationType == type;
	}
public:
	inline TranslateEnginePtr GetEngine() { return this->translateEngine; }
	inline Type GetCompilationType() { return this->compilationType; }
private:
	TranslateEnginePtr translateEngine;
	Type compilationType;
};

LAME_END2

#endif // ~__LAME_COMPILER_ABSTRACT_TRANSLATOR__