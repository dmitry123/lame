#ifndef __LAME_CORE__STATE__
#define __LAME_CORE__STATE__

#include "Define.h"
#include "Utility.h"

LAME_BEGIN

typedef class State* StatePtr;
typedef class StateMachine* StateMachinePtr;

class StateDelegate {
public:
    virtual void onStateAttach() {}
    virtual void onStateDetach() {}
    virtual void onStateRender() {}
    virtual void onStateChange(StatePtr state) {}
    virtual void onStateLoad() {}
    virtual void onStateUnLoad() {}
};

class LAME_API State : public StateDelegate {
    friend class StateMachine;
public:
    State(StringC name) :
        stateName(name),
        stateMachine(NULL)
    {
        this->nextState = NULL;
        this->prevState = NULL;
    }
public:
    virtual ~State();
public:
    virtual void Set(StatePtr left, StatePtr right) {}
private:
    void SetStateMachine(StateMachinePtr stateMachine) {
        this->stateMachine = stateMachine;
    }
public:
    StateMachinePtr GetStateMachine() {
        return stateMachine;
    }
    StringC GetName() {
        return stateName;
    }
private:
    StatePtr nextState;
    StatePtr prevState;
private:
    StringC stateName;
    StateMachinePtr stateMachine;
};

class StateMachineDelegate {
public:
    virtual void onStateMachineAttach(StatePtr state) {}
    virtual void onStateMachineDetach(StatePtr state) {}
    virtual void onStateMachineChange(StatePtr state) {}
    virtual void onStateMachineRender(StatePtr state) {}
};

class LAME_API StateMachine : public StateMachineDelegate {
public:
    StateMachine() {
        this->current = NULL;
        this->first = NULL;
        this->last = NULL;
    }
public:
    ~StateMachine() {
        while (this->first) {
            this->DetachState(this->first);
        }
        this->current = NULL;
        this->first = NULL;
        this->last = NULL;
    }
public:
    Void AttachState(StatePtr state);
    Void DetachState(StatePtr state);
    Void ChangeState(StatePtr state);
    Void NextState(Void);
    Void PrevState(Void);
    Bool IsExist(StatePtr state);
    Void RenderState(Void);
    StatePtr FindState(StringC name);
private:
    StatePtr current;
    StatePtr first;
    StatePtr last;
};

LAME_END

#endif // ~__LAME_CORE__STATE__