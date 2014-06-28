#include "Key.h"

#include <pthread.h>

LAME_BEGIN2(Core)

Bool Key::Set(VoidP value) {
	return pthread_setspecific(*(pthread_key_t*)&this->handle_, value) == 0;
}

VoidP Key::Get(Void) {
	return pthread_getspecific(*(pthread_key_t*)&this->handle_);
}

Key::Key() {
	pthread_key_create((pthread_key_t*)&this->handle_, NULL);
}

Key::~Key() {
	pthread_key_delete(*(pthread_key_t*)&this->handle_);
}

Key::Key(KeyProc destructor) {
	pthread_key_create((pthread_key_t*)&this->handle_, destructor);
}

LAME_END2
