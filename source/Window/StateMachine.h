#ifndef __LAME_WIDOW_SYSTEM__STATE_MACHINE__
#define __LAME_WIDOW_SYSTEM__STATE_MACHINE__

#include "Define.h"
#include "State.h"

LAME_BEGIN

class LAME_API StateMachine {
public:
	 StateMachine();
	~StateMachine();
public:
	template <class T, typename... Args> inline void New(Args... args) {
		this->_Attach(new T(args...), TRUE);
	}
public:
	inline StatePtr GetCurrentState(Void) {
		return this->current;
	}
public:
	Void Attach(StatePtr state);
	Bool Detach(StatePtr state);
	Bool Detach(StringC name);
	Bool Change(StatePtr state);
	Bool Change(StringC name);
	StatePtr GetLast(Void);
	StatePtr PopLast(Void);
	Bool Next(Void);
	Bool Prev(Void);
	Bool IsExist(StatePtr state);
	Bool IsExist(StringC name);
	Void Render(Void);
	StatePtr Find(StringC name);
private:
	inline Void _Attach(StatePtr state, Bool isAllocated) {
		state->isAllocated = isAllocated; this->Attach(state);
	}
private:
	StatePtr current;
	StatePtr first;
	StatePtr last;
private:
	Vector<StatePtr> history;
	Map<StringC, StatePtr> hash;
};

LAME_END

#endif // ~__LAME_WIDOW_SYSTEM__STATE_MACHINE__