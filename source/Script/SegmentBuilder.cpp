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

SegmentPtr SegmentBuilder::BuildDataSegment(ScopePtr rootScope) {

	if (this->dataSegment) {
		delete this->dataSegment;
	}

	this->dataSegment
		= new Segment("data");

	this->dataSegment->Allocate();

	_ForEachScopeObject([](SegmentPtr segment, VariablePtr object) {
		if (object->GetName() == "super") {
			object->SetAddress(object->GetParent()->Find("this", FALSE)->GetAddress());
		}
		else {
			segment->Write(object);
			object->SetAddress(segment->GetSize());
			object->SetSize(segment->GetLastSize());
		}
	}, this->dataSegment, rootScope);

	memset(this->dataSegment->GetBlockAt(0), 0,
           this->dataSegment->GetSize());

	this->dataSegment->Flush();

	return this->dataSegment;
}

SegmentPtr SegmentBuilder::BuildCodeSegment(ScopePtr rootScope) {

	if (this->codeSegment) {
		delete this->codeSegment;
	}

	this->codeSegment
		= new Segment("code");

	this->codeSegment->Allocate();

	_ForEachScopeVariable([](SegmentPtr segment, ObjectPtr object) {
		if (!object->GetSegment() && !object->CheckType(Object::Type::Variable)) {
			object->SetSegment(new Segment(object->GetName()));
		}
	}, this->codeSegment, rootScope);

	this->codeSegment->Flush();

	return this->codeSegment;
}

SegmentPtr SegmentBuilder::BuildTextSegment(ScopePtr rootScope) {

	if (this->textSegment) {
		delete this->textSegment;
	}

	this->textSegment
		= new Segment("text");

	this->textSegment->Allocate();

	_ForEachScopeTemp([](SegmentPtr segment, VariablePtr object) {
		segment->Write(object);
		object->SetAddress(segment->GetSize());
		object->SetSize(segment->GetLastSize());
	}, this->textSegment, rootScope);

	this->textSegment->Flush();

	return this->textSegment;
}

Void SegmentBuilder::_ForEachScopeObject(ForEachScopeObject callback, SegmentPtr segment, ScopePtr scope) {

	static ScopePtr rootScope = scope;

	if (!scope) {
		scope = rootScope;
	}

	for (auto i : scope->GetHashMap()) {
		if (i.second->CheckType(Object::Type::Variable)) {
			if (!i.second->CheckModificator(Object::Modificator::Constant) &&
				!i.second->CheckModificator(Object::Modificator::Internal)
			) {
				callback(segment, i.second->GetVariable());
			}
		}
		this->_ForEachScopeObject(callback, segment, i.second);
	}
}

Void SegmentBuilder::_ForEachScopeTemp(ForEachScopeObject callback, SegmentPtr segment, ScopePtr scope) {

	static ScopePtr rootScope = scope;

	if (!scope) {
		scope = rootScope;
	}

	for (auto i : scope->GetHashMap()) {
		if (i.second->CheckType(Class::Type::Variable) &&
			i.second->CheckModificator(Class::Modificator::Constant)
		) {
			callback(segment, i.second->GetVariable());
		}
		this->_ForEachScopeTemp(callback, segment, i.second);
	}
}

Void SegmentBuilder::_ForEachScopeVariable(ForEachScopeMethod callback, SegmentPtr segment, ScopePtr scope) {

	static ScopePtr rootScope = scope;

	if (!scope) {
		scope = rootScope;
	}

	callback(segment, ObjectPtr(scope));

	for (auto i : scope->GetHashMap()) {
		_ForEachScopeVariable(callback, segment, i.second);
	}
}

LAME_END2