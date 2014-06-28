#ifndef __LAME_CORE__LIST__
#define __LAME_CORE__LIST__

#include "Define.h"
#include "Types.h"

#include <list>
#include <deque>
#include <map>

LAME_BEGIN2(Core)

#if defined(LAME_CPP0X)
template <class T> using Vector = std::deque <T>;
template <class T> using List = std::list <T>;
template <class K, class T> using Map = std::map <K, T>;
template <class L, class R> using Pair = std::pair <L, R>;
#else
template <class T> class Vector : public std::deque <T> {};
template <class T> class List : public std::list <T> {};
template <class T, class K> class Map : public std::map <T, K> {};
template <class T, class K> class Pair : public std::pair <T, K> {};
#endif

LAME_END2

#endif // ~__LAME_CORE__LIST__