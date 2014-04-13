#include "Script.h"

LAME_BEGIN

Void ScriptThread::Launch(Void) {

	if (!this->machine_ || !this->builder_) {
		PostErrorMessage("Unable to start thread with NULL machine/builder", 1);
	}

	this->thread_.Create([](VoidP data) {
		ScriptThreadPtr thread = (ScriptThreadPtr)data;
		thread->machine_->Execute(&thread->performer_);
	}, this);
}

Void ScriptThread::Sleep(Uint32 delay) {
	this->thread_.GetCondition()->TimedWait(delay);
}

Void ScriptThread::Wait() {
	this->thread_.Wait();
}

LAME_END