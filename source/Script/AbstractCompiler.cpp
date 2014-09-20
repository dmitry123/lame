#include "AbstractCompiler.h"

LAME_BEGIN2(Script)

Void AbstractCompiler::Run(
	CodeBuilderPtr   codeBuilder,
	SyntaxBuilderPtr syntaxBuilder,
	ScopePtr         rootScope,
	SegmentPtr       codeSegment
) {
	Segment temporaryCodeSegment;

	temporaryCodeSegment.SetOffset(
		codeSegment->GetPosition());

	_CompileMethods(codeBuilder, &temporaryCodeSegment);
	_CompileMethods(codeBuilder, codeSegment);
}

Void AbstractCompiler::_CompileMethods(CodeBuilderPtr codeBuilder, SegmentPtr codeSegment) {

	for (ObjectPtr m : codeBuilder->GetMethodList()) {

		NodePtr n = m->GetMethod()->GetRootNode();

		this->currentNode = n;
		this->currentMethod = m;

		for (NodePtr n2 : n->argList) {
			this->invoker.onStore(n2->var);
		}

		ObjectPtr thisVar = NULL;

		if (!m->CheckModificator(Object::Modificator::Static)) {
			if (m->GetName() == "<init>" || m->GetName() == m->GetOwner()->GetName()) {
				this->invoker.onClone(ObjectPtr(m->GetOwner()));
			}
			if ((thisVar = m->Find("this", FALSE, Uint32(Object::Type::Variable)))) {
				this->invoker.onStore(thisVar);
			}
		}

		this->_Run(m->GetMethod()->GetRootNode()->blockList);

		if (m->GetMethod()->GetReturnType()->IsVoid()) {
			this->invoker.onReturn(m->GetMethod()->returnVar);
		}
	}
}

Void AbstractCompiler::_Run(NodeListRef nodeList, Bool backupStack) {

	for (NodePtr n : nodeList) {


	}
}

Void AbstractCompiler::_Push(SegmentPtr segment) {

	if (!segment) {
		throw ScriptException("Lost object's segment, can't push NULL object");
	}

	if (!this->byteCode) {
		this->byteCode = new ByteCode(segment);
	}
	else {
		this->GetByteCode()->SetSegment(segment);
	}

	if (this->segmentList.empty()) {
		this->segmentOffset = segment->GetOffset();
	}

	if (!segment->GetOffset()) {
		segment->SetOffset(this->byteCode->GetPosition());
	}

	this->segmentList.push_back(segment);
	this->segmentStack.push(segment);
}

SegmentPtr AbstractCompiler::_Pop(Void) {

	SegmentPtr segment;
	SegmentPtr next;

	if (this->segmentStack.empty() || !this->byteCode) {
		throw ScriptException("Unable to pop segment from stack, stack is empty");
	}

	segment = this->segmentStack.top();
	this->segmentStack.pop();
	next = this->segmentStack.top();
	this->byteCode->SetSegment(next);

	return segment;
}

LAME_END2