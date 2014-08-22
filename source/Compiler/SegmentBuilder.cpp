#include "SegmentBuilder.h"

LAME_BEGIN2(Script)

SegmentBuilder::~SegmentBuilder() {

	delete this->dataSegment;
	delete this->textSegment;

	if (this->codeSegment) {

		_ForEachScopeVariable([](SegmentPtr segment, ObjectPtr object) {
			delete object->GetSegment(); object->SetSegment(NULL);
		}, this->codeSegment, NULL);

		delete this->codeSegment;
	}
}

SegmentPtr SegmentBuilder::BuildDataSegment(Void) {

	if (this->dataSegment) {
		delete this->dataSegment;
	}

	this->dataSegment
		= new Segment("data");

	this->dataSegment->Allocate();

	_ForEachScopeObject([](SegmentPtr segment, VariablePtr object) {
		object->SetAddress(*segment->Write(object));
		object->SetSize(segment->GetLastSize());
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

	this->codeSegment->isCode = TRUE;
	this->codeSegment->Allocate();

	_ForEachScopeVariable([](SegmentPtr segment, ObjectPtr object) {
		if (!object->GetSegment() && !object->CheckType(Object::Type::Variable)) {
			object->SetSegment(new Segment(object->GetName()));
		}
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
		object->SetAddress(*segment->Write(object));
		object->SetSize(segment->GetLastSize());
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

Void SegmentBuilder::_ForEachScopeVariable(ForEachScopeMethod callback, SegmentPtr segment, ScopePtr scope) {

	if (!scope) {
		scope = this->rootScope;
	}

	callback(segment, ObjectPtr(scope));

	for (auto i : scope->GetHashMap()) {
		_ForEachScopeVariable(callback, segment, i.second);
	}
}

LAME_END2