#ifndef __LAME_VIRTUAL_MACHINE__
#define __LAME_VIRTUAL_MACHINE__

#include <Core/Define.h>

#undef LAME_API

#if defined(LAME_MSVC)
#  if defined(VIRTUALMACHINE_EXPORTS)
#    define LAME_API __declspec(dllexport)
#  else
#    define LAME_API __declspec(dllimport)
#  endif
#else
#  define LAME_API
#endif

#define LAME_VM_MOV
#define LAME_VM_ADD
#define LAME_VM_SUB
#define LAME_VM_MUL
#define LAME_VM_DIV
#define LAME_VM_MOD
#define LAME_VM_SHR
#define LAME_VM_SHL
#define LAME_VM_CMP
#define LAME_VM_XCHG
#define LAME_VM_JA
#define LAME_VM_JAE
#define LAME_VM_JB
#define LAME_VM_JBE
#define LAME_VM_JC
#define LAME_VM_JE
#define LAME_VM_JG
#define LAME_VM_JGE
#define LAME_VM_JL
#define LAME_VM_JLE
#define LAME_VM_JNA
#define LAME_VM_JNAE
#define LAME_VM_JNB
#define LAME_VM_JNBE
#define LAME_VM_JNC
#define LAME_VM_JNE
#define LAME_VM_JNG
#define LAME_VM_JNGE
#define LAME_VM_JNL
#define LAME_VM_JNLE
#define LAME_VM_JNO
#define LAME_VM_JNP
#define LAME_VM_JNS
#define LAME_VM_JNZ
#define LAME_VM_JO
#define LAME_VM_JP
#define LAME_VM_JS
#define LAME_VM_JZ
#define LAME_VM_JCXZ
#define LAME_VM_JECXZ
#define LAME_VM_JRCXZ

#endif // ~__LAME_VIRTUAL_MACHINE__