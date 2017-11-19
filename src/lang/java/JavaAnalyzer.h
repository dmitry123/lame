#ifndef __LAME_LANG__JAVA_ANALYZER__
#define __LAME_LANG__JAVA_ANALYZER__

#include "../Analyzer.h"

LAME_LANG_BEGIN_WITH(java)

typedef class Analyzer : public ::lame::lang::Analyzer {
public:
	Analyzer(::lame::lang::ScopeBuilderPtr builder):
		::lame::lang::Analyzer(builder)
	{
	}
public:
	void analyze() override;
public:
	void analyze(const Vector<NodePtr>& vector);
} *AnalyzerPtr;

LAME_LANG_END_WITH

#endif // ~__LAME_LANG__JAVA_ANALYZER__