#ifndef __LAME_LANG__FOUNDATION__
#define __LAME_LANG__FOUNDATION__

#include "../core/Foundation.h"

#define LAME_LANG_BEGIN namespace lame { namespace lang {
#define LAME_LANG_END } }

#define LAME_LANG_BEGIN_WITH(_namespace) LAME_LANG_BEGIN namespace _namespace {
#define LAME_LANG_END_WITH LAME_LANG_END }

#endif // ~__LAME_LANG__FOUNDATION__