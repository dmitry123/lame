#ifndef __LAME_SCRIPT_SEQUENCE_MATCHER__
#define __LAME_SCRIPT_SEQUENCE_MATCHER__

#include "LexNode.h"

LAME_BEGIN2(Script)

class LAME_API SequenceMatcher {
public:
	typedef Deque<LexNodePtr>::iterator
		Iterator;
	typedef Deque<NodePtr> NodeList;
public:
	Bool Declare(Uint32 id, Vector<LexID> lexList);
	Bool Match(Uint32 id, Iterator i, Iterator end);
private:
	Map<Uint32, Vector<LexID>> lexMap_;
};

LAME_END2

#endif // ~__LAME_SCRIPT_SEQUENCE_MATCHER__s