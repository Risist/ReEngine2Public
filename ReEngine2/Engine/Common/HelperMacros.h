#pragma once
#include <assert.h>


#define RE_FLAG(flag) 1 >> (flag)
#define RE_STRINGIFY(String) #String

#define RE_ASSERT(x) assert((x))
#define RE_CHECK(x) assert((x) != nullptr); (x)