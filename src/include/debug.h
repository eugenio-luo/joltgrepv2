#pragma once

#include <format>
#include <print>
#include <span>
#include <execinfo.h>
#include <stdexcept>
#include <unistd.h>
#include <utility>

#include "params.h"

struct TestError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

inline std::span<char*> getBacktraceSymbols(void) {
    void* frames[BACKTRACE_SIZE];
    size_t count = backtrace(frames, BACKTRACE_SIZE);
    return {backtrace_symbols(frames, count), count};
}

template <typename... Args>
void assert(bool condition, std::format_string<Args...> fmt, Args&&... args) {
    if constexpr (ASSERT_ENABLED) {
        if (!condition) {
            if constexpr (!TEST_DEFINED) {
                std::span<char *> symbols {getBacktraceSymbols()};
                std::string out {"assertion failed: "
                    + std::format(fmt, std::forward<Args>(args)...)
                    + "\nbacktrace:\n"};

                for (auto symbol : symbols) {
                    out += symbol;
                    out += "\n";
                }
                std::print("{0}", out);

                std::abort();
            } else {
                throw TestError(std::format(fmt, std::forward<Args>(args)...));
            }
        }
    }
}

/*
 * Assert that 0 <= x < right
 */
template <std::integral T, std::integral U, std::integral V>
void _assertBound(T left, U x, V right, std::string_view varName) {
    assert(std::cmp_greater_equal(x, left) && std::cmp_less(x, right),
           "should {0} <= {1} < {2}, but {1} = {3}", left, varName, right, x);
}

template <std::integral U, std::integral V>
void _assertAtLeast(U x, V least, std::string_view varName) {
    assert(std::cmp_greater_equal(x, least),
           "should {0} <= {1}, but {1} = {2}", least, varName, x);
}

#define assertBound(left, x, right) _assertBound(left, x, right, #x)
#define assertSpan(x, right) _assertBound(0, x, right, #x)
#define assertAtLeast(x, least) _assertAtLeast(x, least, #x)
