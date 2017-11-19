#ifndef __LAME_LANG__JAVA_SYNTAX_BUILDER__
#define __LAME_LANG__JAVA_SYNTAX_BUILDER__

#include "../SyntaxBuilder.h"
#include "JavaTokenManager.h"

LAME_LANG_BEGIN_WITH(java)

typedef class SyntaxBuilder : public ::lame::lang::SyntaxBuilder<TokenType, SyntaxBuilder> {
	typedef ::lame::lang::SyntaxBuilder<TokenType, SyntaxBuilder> Parent;
	typedef SyntaxBuilder* SyntaxBuilderPtr;
public:
	typedef Vector<NodePtr>::const_iterator Iterator;
public:
	SyntaxBuilder(FactoryPtr factory, ::lame::lang::ScriptParserPtr parser) :
		Parent(factory, parser), temp(nullptr)
	{
	}
protected:
	void matchBuilder() override;
protected:
	virtual SyntaxBuilderPtr buildCondition();
	virtual SyntaxBuilderPtr buildElse();
	virtual SyntaxBuilderPtr buildFor();
	virtual SyntaxBuilderPtr buildWhile();
	virtual SyntaxBuilderPtr buildDo();
	virtual SyntaxBuilderPtr buildClass();
	virtual SyntaxBuilderPtr buildInterface();
	virtual SyntaxBuilderPtr buildAnnotation();
	virtual SyntaxBuilderPtr buildMethod();
	virtual SyntaxBuilderPtr buildArguments();
	virtual SyntaxBuilderPtr buildBody();
	virtual SyntaxBuilderPtr buildImplements();
	virtual SyntaxBuilderPtr buildExtends();
	virtual SyntaxBuilderPtr buildThrows();
protected:
	TokenPtr getTemp() {
		if (this->temp == nullptr) {
			return (temp = this->getFactory()->findByToken(TokenType::_TEMP));
		}
		else {
			return temp;
		}
	}
private:
	TokenPtr temp;
} *SyntaxBuilderPtr;

LAME_LANG_END_WITH

#endif // ~__LAME_LANG__JAVA_SYNTAX_BUILDER__