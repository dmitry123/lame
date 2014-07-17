#ifndef __LAME_SCRIPT_SEGMENT_BUILDER__
#define __LAME_SCRIPT_SEGMENT_BUILDER__

#include "Segment.h"

LAME_BEGIN2(Script)

class LAME_API SegmentBuilder {
public:
	SegmentBuilder(ScopePtr rootScope) :
		dataSegment(0),
		codeSegment(0),
		textSegment(0)
	{
		this->rootScope = rootScope;
	}
	~SegmentBuilder() {
		delete this->dataSegment;
		delete this->codeSegment;
		delete this->textSegment;
	}
public:
	SegmentPtr BuildDataSegment(Void);
	SegmentPtr BuildCodeSegment(Void);
	SegmentPtr BuildTextSegment(Void);
private:
	typedef Void (*ForEachScopeObject)(
		SegmentPtr segment, VariablePtr object);
	typedef Void(*ForEachScopeMethod)(
		SegmentPtr segment, MethodPtr object);
private:
	Void _ForEachScopeObject(ForEachScopeObject callback, SegmentPtr segment, ScopePtr scope);
	Void _ForEachScopeTemp(ForEachScopeObject callback, SegmentPtr segment, ScopePtr scope);
	Void _ForEachScopeMethod(ForEachScopeMethod callback, SegmentPtr segment, ScopePtr scope);
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
private:
	ScopePtr rootScope;
};

LAME_END2

#endif // ~__LAME_SCRIPT_SEGMENT_BUILDER__