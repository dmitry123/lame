#ifndef __LAME_CORE__LIST__
#define __LAME_CORE__LIST__

#include "Define.h"
#include "Types.h"

#include <list>
#include <deque>
#include <map>

LAME_BEGIN

#if defined(LAME_CPP0X)
template <class T> using Vector = std::deque <T>;
template <class T> using List = std::list <T>;
template <class T, class K> using Map = std::map <T, K>;
#else
#  if defined(LAME_CLANG) && defined(LAME_DEBUG)
#    define Vector std::deque
#    define List std::list
#    define Map std::map
#  else
template <class T> class Vector : public std::deque <T> {};
template <class T> class List : public std::list <T> {};
template <class T, class K> class Map : public std::map <T, K> {};
#  endif
#endif

LAME_END

#endif // ~__LAME_CORE__LIST__