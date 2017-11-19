#ifndef __LAME_LANG__ANALYZER__
#define __LAME_LANG__ANALYZER__

#include "ScopeBuilder.h"

LAME_LANG_BEGIN

typedef class Analyzer {
public:
	typedef Vector<NodePtr>::iterator Iterator;
public:
	Analyzer(ScopeBuilderPtr builder):
		builder(builder)
	{
	}
public:
	virtual void analyze() = 0;
public:
	Vector<ObjectPtr> findClasses(ObjectPtr root);
	Vector<ObjectPtr> findMethods(ObjectPtr root);
protected:
public:
	inline ScopeBuilderPtr getBuilder() {
		return this->builder;
	}
private:
	ScopeBuilderPtr builder;
} *AnalyzerPtr;

LAME_LANG_END

#endif // ~__LAME_LANG__ANALYZER__