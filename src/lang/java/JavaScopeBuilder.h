#ifndef __LAME_LANG_JAVA__SCOPE_BUILDER__
#define __LAME_LANG_JAVA__SCOPE_BUILDER__

#include "../ScopeBuilder.h"

LAME_LANG_BEGIN_WITH(java)

typedef class ScopeBuilder : public ::lame::lang::ScopeBuilder {
public:
	ScopeBuilder(NodePtr root) :
		::lame::lang::ScopeBuilder(root),
		modifiers(0)
	{
	}
public:
	void matchBuilder(NodePtr node) override;
public:
	void buildConstruction(NodePtr node);
	void buildClass(NodePtr node);
	void buildExpression(NodePtr node);
	void buildMethod(NodePtr node);
protected:
	UInt32 setModifiers(UInt32 id);
	UInt32 getModifiers();
private:
	UInt32 modifiers;
} *ScopeBuilderPtr;

LAME_LANG_END_WITH

#endif // ~__LAME_LANG_JAVA__SCOPE_BUILDER__