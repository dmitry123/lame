#include "SegmentBuilder.h"
#include "Method.h"
#include "Variable.h"

LAME_BEGIN2(Script)

SegmentPtr SegmentBuilder::BuildDataSegment(Void) {

	if (this->dataSegment) {
		delete this->dataSegment;
	}

	this->dataSegment
		= new Segment("data");

	this->dataSegment->Allocate();

	_ForEachScopeObject([](SegmentPtr segment, VariablePtr object) {
		object->SetSegment(segment, segment->Write(object));
	}, this->dataSegment, NULL);

	this->dataSegment->Flush();

	return this->dataSegment;
}

SegmentPtr SegmentBuilder::BuildCodeSegment(Void) {

	if (this->codeSegment) {
		delete this->codeSegment;
	}

	this->codeSegment
		= new Segment("code");

	this->codeSegment->Allocate();

	_ForEachScopeMethod([](SegmentPtr segment, MethodPtr object) {
		object->SetSegment(segment, segment->Write(object));
	}, this->codeSegment, NULL);

	this->codeSegment->Flush();

	return this->codeSegment;
}

SegmentPtr SegmentBuilder::BuildTextSegment(Void) {

	if (this->textSegment) {
		delete this->textSegment;
	}

	this->textSegment
		= new Segment("text");

	this->textSegment->Allocate();

	_ForEachScopeTemp([](SegmentPtr segment, VariablePtr object) {
		object->SetSegment(segment, segment->Write(object));
	}, this->textSegment, NULL);

	this->textSegment->Flush();

	return this->textSegment;
}

Void SegmentBuilder::_ForEachScopeObject(ForEachScopeObject callback, SegmentPtr segment, ScopePtr scope) {

	if (!scope) {
		scope = this->rootScope;
	}

	for (ObjectPtr i : scope->GetVariableSet()) {
		if (i->CheckType(Class::Type::Variable)) {
			if (!i->CheckModificator(Class::Modificator::Constant)) {
				callback(segment, i->GetVariable());
			}
		}
	}

	for (ObjectPtr i : scope->GetClassSet()) {
		for (ObjectPtr j : i->GetMethodSet()) {
			this->_ForEachScopeObject(callback, segment, j);
		}
	}

	for (ObjectPtr i : scope->GetMethodSet()) {
		this->_ForEachScopeObject(callback, segment, i);
	}

	for (ObjectPtr i : scope->GetConditionSet()) {
		this->_ForEachScopeObject(callback, segment, i);
	}
}

Void SegmentBuilder::_ForEachScopeTemp(ForEachScopeObject callback, SegmentPtr segment, ScopePtr scope) {

	if (!scope) {
		scope = this->rootScope;
	}

	for (ObjectPtr i : scope->GetVariableSet()) {
		if (i->CheckType(Class::Type::Variable)) {
			if (i->CheckModificator(Class::Modificator::Constant)) {
				callback(segment, i->GetVariable());
			}
		}
	}

	for (ObjectPtr i : scope->GetClassSet()) {
		for (ObjectPtr j : i->GetMethodSet()) {
			this->_ForEachScopeTemp(callback, segment, j);
		}
	}
}

Void SegmentBuilder::_ForEachScopeMethod(ForEachScopeMethod callback, SegmentPtr segment, ScopePtr scope) {

	if (!scope) {
		scope = this->rootScope;
	}

	for (ObjectPtr i : scope->GetClassSet()) {
		for (ObjectPtr j : i->GetMethodSet()) {
			if (!j->CheckModificator(Class::Modificator::Native) &&
				!j->CheckModificator(Class::Modificator::Abstract)
			) {
				callback(segment, j->GetMethod());
			}
		}
		_ForEachScopeMethod(callback, segment, i);
	}
}

LAME_END2