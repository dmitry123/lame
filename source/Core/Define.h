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
#  error "Unknown platform!"
#endif

#if defined(_MSC_VER)
#  define LAME_MSVC
#elif defined(__clang__)
#  define LAME_CLANG
#else
#  error "Unknown compiler!"
#endif

#if defined(LAME_CLANG)
#  if defined(__llvm__)
#    define LAME_LLVM
#  endif
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
#  define LAME_TRUE (LAME_NAMESPACE::Uint8)1
#endif

#if !defined(LAME_FALSE)
#  define LAME_FALSE (LAME_NAMESPACE::Uint8)0
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
#  if _MSC_VER >= 1800
#    define LAME_CPP0X
#  endif
#endif

#undef LAME_BEGIN
#undef LAME_END

#if defined(LAME_CPP)
#  define LAME_NAMESPACE lame
#  define LAME_BEGIN namespace LAME_NAMESPACE {
#  define LAME_END   }
#else
#  define LAME_NAMESPACE
#  define LAME_BEGIN
#  define LAME_END
#endif

#if defined(LAME_CPP)
#  define LAME_EXTERN extern "C"
#else
#  define LAME_EXTERN extern
#endif

#if defined(LAME_MSVC) && defined(_USRDLL)
#  define LAME_DLL
#endif

#if defined(LAME_DLL)
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
#  pragma warning(disable: 4150)
#  pragma warning(disable: 4996)
#  pragma warning(disable: 4251)
#endif

#define LAME_INFINITE 0xffffffff

#if defined(LAME_WINDOWS)
#  define LAME_SLASH '\\'
#  define LAME_SLASH_STR "\\"
#else
#  define LAME_SLASH '/'
#  define LAME_SLASH_STR "/"
#endif

#define LAME_DECLARE_HANDLE(_name) \
typedef struct _##_name { int _unused; } *_name;

#define LAME_SIGNATURE_MAX 10
#define LAME_FILENAME_MAX 260
#define LAME_FILEPATH_MAX 260
#define LAME_FILEEXTENSION_MAX 20
#define LAME_ERRORBUFFER_MAX 1024
#define LAME_TIMEBUFFER_MAX 64

#define LAME_MS_SECOND 1000
#define LAME_MS_MINUTE 60000
#define LAME_MS_HOUR 3600000
#define LAME_MS_DAY 86400000
#define LAME_MS_WEEK 604800000
#define LAME_MS_MONTH 2419200000
#define LAME_MS_YEAR 29030400000

#define LAME_DECLARE_TYPE(_type, _name) \
	typedef _type _name, *_name##P;

#endif // ~__LAME_CORE__DEFINE__