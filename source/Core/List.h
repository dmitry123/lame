#ifndef __LAME_CORE__LIST__
#define __LAME_CORE__LIST__

#include "Types.h"

#include <utility>
#include <list>
#include <deque>
#include <map>
#include <stack>
#include <queue>
#include <set>

LAME_BEGIN2(Core)

template <class T> using Vector = std::vector <T>;
template <class T> using Deque = std::deque <T>;
template <class T> using List = std::list <T>;
template <class T> using Stack = std::stack <T>;
template <class T> using Queue = std::queue <T>;
template <class T> using Set = std::set <T>;
template <class K, class T> using Map = std::map <K, T>;
template <class L, class R> using Pair = std::pair <L, R>;

LAME_END2

#endif // ~__LAME_CORE__LIST__