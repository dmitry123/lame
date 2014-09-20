#ifndef __LAME_COMPILER_SEGMENT_BUILDER__
#define __LAME_COMPILER_SEGMENT_BUILDER__

#include "Segment.h"
#include "Object.h"
#include "Scope.h"

LAME_BEGIN2(Script)

class LAME_API SegmentBuilder {
public:
	SegmentBuilder() :
		dataSegment(NULL),
		codeSegment(NULL),
		textSegment(NULL)
	{
	}
public:
	~SegmentBuilder();
public:
	SegmentPtr BuildDataSegment(ScopePtr rootScope);
	SegmentPtr BuildCodeSegment(ScopePtr rootScope);
	SegmentPtr BuildTextSegment(ScopePtr rootScope);
private:
	typedef Void (*ForEachScopeObject)(
		SegmentPtr segment, VariablePtr object);
	typedef Void(*ForEachScopeMethod)(
		SegmentPtr segment, ObjectPtr object);
private:
	Void _ForEachScopeObject(ForEachScopeObject callback, SegmentPtr segment, ScopePtr scope);
	Void _ForEachScopeTemp(ForEachScopeObject callback, SegmentPtr segment, ScopePtr scope);
	Void _ForEachScopeVariable(ForEachScopeMethod callback, SegmentPtr segment, ScopePtr scope);
public:
	inline SegmentPtr GetDataSegment() {
		return this->dataSegment;
	}
	inline SegmentPtr GetCodeSegment() {
		return this->codeSegment;
	}
	inline SegmentPtr GetTextSegment() {
		return this->textSegment;
	}
private:
	SegmentPtr dataSegment;
	SegmentPtr codeSegment;
	SegmentPtr textSegment;
};

LAME_END2

#endif // ~__LAME_COMPILER_SEGMENT_BUILDER__