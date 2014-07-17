#ifndef __LAME_CORE__DEFINE__
#define __LAME_CORE__DEFINE__

#if defined(_WIN32) || defined(_WIN64)
#  define LAME_WINDOWS
#  if defined(_WIN64)
#    define LAME_X64
#  else
#    define LAME_X86
#  endif
#elif defined(__APPLE__) || defined(__APPLE_CC__)
#  define LAME_APPLE
#  if defined(__amd64__)
#    define LAME_X64
#  else
#    define LAME_X86
#  endif
#else
#  error "Unsupported platform!"
#endif

#if defined(_MSC_VER)
#  if _MSC_VER == 1800
#    define LAME_MSVC_120
#    define LAME_MSVC 120
#  elif _MSC_VER == 1700
#    define LAME_MSVC_110
#    define LAME_MSVC 110
#  elif _MSC_VER == 1600
#    define LAME_MSVC_100
#    define LAME_MSVC 100
#  elif _MSC_VER == 1500
#    define LAME_MSVC_090
#    define LAME_MSVC 090
#  elif _MSC_VER == 1400
#    define LAME_MSVC_080
#    define LAME_MSVC 080
#  elif _MSC_VER == 1300
#    define LAME_MSVC_071
#    define LAME_MSVC 071
#  elif _MSC_VER == 1300
#    define LAME_MSVC_070
#    define LAME_MSVC 070
#  elif _MSC_VER == 1200
#    define LAME_MSVC_060
#    define LAME_MSVC 060
#  elif _MSC_VER == 1100
#    define LAME_MSVC_050
#    define LAME_MSVC 050
#  endif
#elif defined(__clang__)
#  define LAME_CLANG
#  if defined(__llvm__)
#    define LAME_LLVM
#  endif
#else
#  error "Unsupported compiler!"
#endif

#if LAME_MSVC < 120
#  error "Lame Script requires MSVC12 compiler with C++0x support!"
#endif

#if defined(LAME_MSVC) || defined(LAME_CLANG)
#  define LAME_IASM_INTEL
#endif

#if defined(LAME_CLANG)
#  define LAME_IASM_ATNT
#endif

#if !defined(LAME_NULL)
#  define LAME_NULL 0
#endif

#if !defined(LAME_TRUE)
#  define LAME_TRUE (::LAME_NAMESPACE::Bool)1
#endif

#if !defined(LAME_FALSE)
#  define LAME_FALSE (::LAME_NAMESPACE::Bool)0
#endif

#if !defined(TRUE)
#  define TRUE 1
#endif

#if !defined(FALSE)
#  define FALSE 0
#endif

#if !defined(NULL)
#  define NULL 0
#endif

#if defined(_DEBUG) || defined(DEBUG) || defined(NDEBUG)
#  define LAME_DEBUG
#else
#  define LAME_RELEASE
#endif

#if defined(__cplusplus)
#  define LAME_CPP
#endif

#if defined(LAME_MSVC)
#  if LAME_MSVC >= 120
#    define LAME_CPP0X
#  endif
#endif

#undef LAME_BEGIN
#undef LAME_END
#undef LAME_BEGIN2
#undef LAME_END2

#if defined(LAME_CPP)
#  define LAME_NAMESPACE Lame
#  define LAME_BEGIN namespace LAME_NAMESPACE {
#  define LAME_END }
#  define LAME_BEGIN2(_ns) LAME_BEGIN namespace _ns {
#  define LAME_END2 LAME_END }
#else
#  define LAME_NAMESPACE
#  define LAME_BEGIN
#  define LAME_END
#  define LAME_BEGIN2
#  define LAME_END2
#endif

#if defined(LAME_CPP)
#  define LAME_EXTERN extern "C"
#else
#  define LAME_EXTERN extern
#endif

#if defined(LAME_MSVC) && defined(_USRDLL)
#  define LAME_DLL
#endif

#if defined(LAME_WINDOWS)
#  if defined(CORE_EXPORTS)
#    define LAME_API __declspec(dllexport)
#  else
#    define LAME_API __declspec(dllimport)
#  endif
#else
#  define LAME_API
#endif

#if defined(LAME_WINDOWS)
#  if !defined(WIN32_LEAN_AND_MEAN)
#    define WIN32_LEAN_AND_MEAN
#  endif
#  if defined(LAME_MSVC) && !defined(_CRT_SECURE_NO_WARNINGS)
#    define _CRT_SECURE_NO_WARNINGS
#  endif
#  if defined(LAME_MSVC) && !defined(_USE_MATH_DEFINES)
#    define _USE_MATH_DEFINES
#  endif
#endif

#if defined(LAME_MSVC)
#  pragma warning(disable : 4150)
#  pragma warning(disable : 4996)
#  pragma warning(disable : 4251)
#  pragma warning(disable : 4275)
#endif

#define LAME_INFINITE 0xffffffff

#if defined(LAME_WINDOWS)
#  define LAME_SLASH '\\'
#  define LAME_SLASH2 "\\"
#else
#  define LAME_SLASH '/'
#  define LAME_SLASH2 "/"
#endif

#define LAME_DECLARE_HANDLE(_name) \
	typedef struct _##_name { int _unused; } *_name;

#define LAME_SIGNATURE_MAX 10
#define LAME_FILENAME_MAX 260
#define LAME_FILEPATH_MAX 260
#define LAME_FILEEXTENSION_MAX 20
#define LAME_ERRORBUFFER_MAX 2048
#define LAME_TIMEBUFFER_MAX 64

#define LAME_TIME_SECOND 1000
#define LAME_TIME_MINUTE 60000
#define LAME_TIME_HOUR   3600000
#define LAME_TIME_DAY    86400000
#define LAME_TIME_WEEK   604800000
#define LAME_TIME_MONTH  2419200000
#define LAME_TIME_YEAR   29030400000

#define LAME_DECLARE_TYPE(_type, _name) \
	typedef _type _name, *_name##P;

#define __LAME_STRINGISE_IMPL(x) #x
#define __LAME_STRINGISE(x) __LAME_STRINGISE_IMPL(x)
#define __LAME_FLL __FILE__ "(" __LAME_STRINGISE(__LINE__) ") : "

#if defined(LAME_MSVC)
#  define LAME_WARNING(_message) (__LAME_FLL "WARNING : " _message)
#  define LAME_TODO(_message) (__LAME_FLL "TODO : "_message)
#else
#  define LAME_WARNING(_message) (_message)
#  define LAME_TODO(_message) (_message)
#endif

#define LAME_SCOPE

#if defined(LAME_WINDOWS)
#  if defined(LAME_MSVC)
#    pragma comment(lib, "pthreadvc2.lib")
#  else
#    error "Unsupported compiler";
#  endif
#endif

#endif // ~__LAME_CORE__DEFINE__