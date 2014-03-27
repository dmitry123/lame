#ifndef __LAME_CORE__LIST__
#define __LAME_CORE__LIST__

#include "Define.h"
#include "Types.h"

#include <list>
#include <deque>
#include <map>

LAME_BEGIN

template <class T> class Vector : public std::deque <T> {};
template <class T> class List : public std::list <T> {};
template <class T, class K> class Map : public std::map <T, K> {};

LAME_END

#endif // ~__LAME_CORE__LIST__