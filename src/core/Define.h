#ifndef __LAME_CORE_DEFINE__
#define __LAME_CORE_DEFINE__

#define LAME_PLATFORM_UNKNOWN 0x0000
#define LAME_PLATFORM_WINDOWS 0x0001
#define LAME_PLATFORM_OSX     0x0002
#define LAME_PLATFORM_LINUX   0x0003

#if defined(_WIN32) || defined(_WIN64)
#  define LAME_PLATFORM LAME_PLATFORM_WINDOWS
#elif defined(__APPLE__)
#  define LAME_PLATFORM LAME_PLATFORM_OSX
#else
#  define LAME_PLATFORM LAME_PLATFORM_LINUX
#endif

#if !defined(LAME_PLATFORM)
#  define LAME_PLATFORM LAME_PLATFORM_UNKNOWN
#endif

#define LAME_ARCH_UNKNOWN 0x0000
#define LAME_ARCH_ARM     0x0001
#define LAME_ARCH_AMD64   0x0002
#define LAME_ARCH_IA86    0x0003

#if LAME_PLATFORM == LAME_PLATFORM_WINDOWS
#  if defined(_WIN64)
#    define LAME_ARCH LAME_ARCH_AMD64
#  else
#    define LAME_ARCH LAME_ARCH_IA86
#  endif
#else
#  if defined(__amd64__)
#    define LAME_ARCH LAME_ARCH_AMD64
#  else
#    define LAME_ARCH LAME_ARCH_IA86
#  endif
#endif

#if !defined(LAME_ARCH)
#  define LAME_ARCH LAME_ARCH_UNKNOWN
#endif

#define LAME_COMPILER_UNKNOWN 0x0000
#define LAME_COMPILER_MSVC    0x0001
#define LAME_COMPILER_CLANG   0x0002

#if defined(_MSC_VER)
#  define LAME_COMPILER LAME_COMPILER_MSVC
#elif defined(__clang__)
#  define LAME_COMPILER LAME_COMPILER_CLANG
#endif

#if !defined(LAME_COMPILER)
#  define LAME_COMPILER LAME_COMPILER_UNKNOWN
#endif

#if !defined(LAME_TRUE)
#  define LAME_TRUE 1
#endif

#if !defined(LAME_FALSE)
#  define LAME_FALSE 0
#endif

#define LAME_TARGET_UNKNOWN 0x0000
#define LAME_TARGET_DEBUG   0x0001
#define LAME_TARGET_RELEASE 0x0002

#if defined(_DEBUG) || defined(DEBUG) || defined(NDEBUG)
#  define LAME_TARGET LAME_TARGET_DEBUG
#else
#  define LAME_TARGET LAME_TARGET_RELEASE
#endif

#if !defined(LAME_TARGET)
#  define LAME_TARGET LAME_TARGET_UNKNOWN
#endif

#if LAME_COMPILER == LAME_COMPILER_MSVC
#  pragma warning(disable: 4150)
#  pragma warning(disable: 4996)
#  pragma warning(disable: 4251)
#endif

#if LAME_PLATFORM == LAME_PLATFORM_WINDOWS
#  if !defined(WIN32_LEAN_AND_MEAN)
#    define WIN32_LEAN_AND_MEAN
#  endif
#  if !defined(_CRT_SECURE_NO_WARNINGS)
#    define _CRT_SECURE_NO_WARNINGS
#  endif
#  if !defined(_USE_MATH_DEFINES)
#    define _USE_MATH_DEFINES
#  endif
#endif

#if LAME_PLATFORM == LAME_PLATFORM_WINDOWS
#  define LAME_SEPARATOR '\\'
#else
#  define LAME_SEPARATOR '/'
#endif

#if !defined(FALSE) || !defined(TRUE)
#  define FALSE 0
#  define TRUE  1
#endif

#define LAME_ERROR_BUFFER 1024

#endif // ~__LAME_CORE_DEFINE__