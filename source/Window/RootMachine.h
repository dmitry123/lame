#ifndef __LAME_WINDOW_SYSTEM__ROOT_MACHINE__
#define __LAME_WINDOW_SYSTEM__ROOT_MACHINE__

#include "StateMachine.h"

LAME_BEGIN

class LAME_API RootMachine {
public:
	inline StateMachinePtr operator -> () {
		return rootMachine;
	}
public:
	static StateMachinePtr getInstance() {
		return rootMachine;
	}
private:
	static StateMachinePtr rootMachine;
};

LAME_END

#endif // ~__LAME_WINDOW_SYSTEM__ROOT_MACHINE__