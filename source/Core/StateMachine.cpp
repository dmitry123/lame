#include "State.h"

LAME_BEGIN

Void StateMachine::AttachState(StatePtr state) {
    
    if (this->first == NULL) {
        this->first = this->last = state;
    }
    else {
        this->last->nextState = state;
        state->prevState = this->last;
        this->last = state;
    }
    
    if (current == NULL) {
        current = state;
    }
    
    state->SetStateMachine(this);
    state->onStateAttach();
    
    this->onStateMachineAttach(state);
}

Void StateMachine::DetachState(StatePtr state) {
    
    if (IsExist(state)) {
        
        if (state == this->first) {
            this->first = this->first->nextState;
        }
        
        if (state == this->last) {
            this->last = state->prevState;
        }
        
        if (state->prevState) {
            state->prevState->nextState = state->nextState;
        }
        
        if (state->nextState) {
            state->nextState->prevState = state->prevState;
        }
        
        state->onStateDetach();
        this->onStateMachineDetach(state);
        
        delete state;
    }
}

Void StateMachine::ChangeState(StatePtr state) {
    
    if (IsExist(state) && current != state) {
        
        current->onStateUnLoad();
        current->onStateChange(state);
        
        this->onStateMachineChange(state);
        
        current = state;
        current->onStateLoad();
    }
}

Void StateMachine::NextState(Void) {
    
    if (current->nextState) {
        this->ChangeState(current->nextState);
    }
}

Void StateMachine::PrevState(Void) {
    
    if (current->prevState) {
        this->ChangeState(current->prevState);
    }
}

Bool StateMachine::IsExist(StatePtr with) {
    
    StatePtr state = this->first;
    
    while (state != NULL) {
        
        if (state == with) {
            return TRUE;
        }
        
        state = state->nextState;
    }
    
    return FALSE;
}

Void StateMachine::RenderState(Void) {
    
    if (current) {
        this->onStateMachineRender(current);
        current->onStateRender();
    }
}

StatePtr StateMachine::FindState(StringC name) {
    
    StatePtr first = this->first;
    
    while (first) {
        
        if (strcmp(first->stateName, name) == 0) {
            return first;
        }
        
        first = first->nextState;
    }
    
    return NULL;
}

LAME_END