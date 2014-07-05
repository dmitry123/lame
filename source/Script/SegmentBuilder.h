#ifndef __LAME_SCRIPT_SEGMENT_BUILDER__
#define __LAME_SCRIPT_SEGMENT_BUILDER__

#include "Segment.h"

LAME_BEGIN2(Script)

class LAME_API SegmentBuilder {
public:
	SegmentBuilder(ScopeControllerPtr scopeController) :
		dataSegment(0),
		codeSegment(0),
		textSegment(0)
	{
		this->scopeController = scopeController;
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
	Void _ForEachScopeObject(ForEachScopeObject callback, SegmentPtr segment, ScopeControllerPtr scope);
	Void _ForEachScopeTemp(ForEachScopeObject callback, SegmentPtr segment, ScopeControllerPtr scope);
	Void _ForEachScopeMethod(ForEachScopeMethod callback, SegmentPtr segment, ScopeControllerPtr scope);
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
	ScopeControllerPtr scopeController;
};

LAME_END2

#endif // ~__LAME_SCRIPT_SEGMENT_BUILDER__