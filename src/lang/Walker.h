#ifndef __LAME_LANG__WALKER__
#define __LAME_LANG__WALKER__

#include "Node.h"

LAME_LANG_BEGIN

typedef class Walker {
	typedef Logic (*Filter)(NodePtr);
public:
	Walker(Filter filter):
		filter(filter)
	{
	}
public:
	const Vector<NodePtr>& walk(NodePtr root);
private:
	Filter filter;
	Vector<NodePtr> vector;
} *WalkerPtr;

LAME_LANG_END

#endif // ~__LAME_LANG__WALKER__