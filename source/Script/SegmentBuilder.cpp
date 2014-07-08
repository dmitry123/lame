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

	memset(this->dataSegment->data, 0, this->dataSegment->size);

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

Void SegmentBuilder::_ForEachScopeObject(ForEachScopeObject callback, SegmentPtr segment, ScopeControllerPtr scope) {

	if (!scope) {
		scope = this->scopeController;
	}

	for (auto& i : scope->GetVarScope()->GetStringMap()) {
		if (i.second->CheckType(Class::Type::Variable) ||
			i.second->CheckType(Class::Type::Array)
		) {
			if (!i.second->CheckModificator(Class::Modificator::Register)) {
				callback(segment, i.second->GetVariable());
			}
		}
	}

	for (auto& i : scope->GetClassScope()->GetStringMap()) {
		for (auto& j : i.second->GetScopeController()->GetMethodScope()->GetStringMap()) {
			if (j.second->GetScopeController()) {
				this->_ForEachScopeObject(callback, segment, j.second->GetScopeController());
			}
		}
	}
}

Void SegmentBuilder::_ForEachScopeTemp(ForEachScopeObject callback, SegmentPtr segment, ScopeControllerPtr scope) {

	if (!scope) {
		scope = this->scopeController;
	}

	for (auto& i : scope->GetTempScope()->GetStringMap()) {
		if (i.second->CheckType(Class::Type::Variable) ||
			i.second->CheckType(Class::Type::Array)
		) {
			if (!i.second->CheckModificator(Class::Modificator::Register)) {
				callback(segment, i.second->GetVariable());
			}
		}
	}

	for (auto& i : scope->GetClassScope()->GetStringMap()) {
		for (auto& j : i.second->GetScopeController()->GetMethodScope()->GetStringMap()) {
			if (j.second->GetScopeController()) {
				this->_ForEachScopeTemp(callback, segment, j.second->GetScopeController());
			}
		}
	}
}

Void SegmentBuilder::_ForEachScopeMethod(ForEachScopeMethod callback, SegmentPtr segment, ScopeControllerPtr scope) {

	if (!scope) {
		scope = this->scopeController;
	}

	for (auto& i : scope->GetClassScope()->GetStringMap()) {
		for (auto& j : i.second->GetScopeController()->GetMethodScope()->GetStringMap()) {
			if (j.second->GetScopeController()) {
				if (!j.second->CheckModificator(Class::Modificator::Native) &&
					!j.second->CheckModificator(Class::Modificator::Abstract)
				) {
					callback(segment, j.second->GetMethod());
				}
			}
		}
		if (i.second->GetScopeController()) {
			_ForEachScopeMethod(callback, segment, i.second->GetScopeController());
		}
	}
}

LAME_END2