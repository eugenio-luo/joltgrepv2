#pragma once

#include <stddef.h>

constexpr inline bool DEBUG_DEFINED =
#ifdef DEBUG
    true;
#else
    false;
#endif

constexpr inline bool ASSERT_ENABLED = DEBUG_DEFINED;
constexpr inline size_t BACKTRACE_SIZE = 64;
