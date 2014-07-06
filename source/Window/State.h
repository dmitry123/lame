#ifndef __LAME_WINDOW_SYSTEM__STATE__
#define __LAME_WINDOW_SYSTEM__STATE__

#include "Define.h"
#include "StateDelegate.h"

LAME_BEGIN

class LAME_API State : public StateDelegate, LockNew, LockDelete {
	friend class StateMachine;
protected:
	State(StringC name);
public:
	virtual ~State();
public:
	inline StateMachinePtr GetStateMachine() {
		return stateMachine;
	}
	inline StringC GetName() {
		return stateName;
	}
public:
	Void Next(Void);
	Void Prev(Void);
private:
	Void _SetStateMachine(StateMachinePtr);
	Void _RefreshState(Void);
private:
	StatePtr nextState;
	StatePtr prevState;
private:
	StringC stateName;
	StateMachinePtr stateMachine;
	Bool isAllocated;
};

LAME_END

#endif // ~__LAME_WINDOW_SYSTEM__STATE__