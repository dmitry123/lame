#ifndef __LAME_WINDOW_SYSTEM__DEFINE__
#define __LAME_WINDOW_SYSTEM__DEFINE__

#include <Core\Core.h>

#undef LAME_API

#if defined(LAME_MSVC)
#  if defined(WINDOWSYSTEM_EXPORTS)
#    define LAME_API __declspec(dllexport)
#  else
#    define LAME_API __declspec(dllimport)
#  endif
#else
#  define LAME_API
#endif

LAME_BEGIN

typedef class Window Window, *WindowPtr;
typedef class State State, *StatePtr;
typedef class StateMachine StateMachine, *StateMachinePtr;
typedef class Display Display, *DisplayPtr;
typedef class StateDelegate StateDelegate, *StateDelegatePtr;
typedef class RootMachine RootMachine, *RootMachinePtr;

LAME_END

#endif // ~__LAME_WINDOW_SYSTEM__DEFINE__