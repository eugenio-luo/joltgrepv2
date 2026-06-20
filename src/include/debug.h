#pragma once

#include <format>
#include <print>
#include <span>
#include <execinfo.h>
#include <unistd.h>

#include "params.h"

inline std::span<char*> getBacktraceSymbols(void) {
    void* frames[BACKTRACE_SIZE];
    size_t count = backtrace(frames, BACKTRACE_SIZE);
    return {backtrace_symbols(frames, count), count};
}

template <typename... Args>
void assert(bool condition, std::format_string<Args...> fmt, Args&&... args) {
    if constexpr (ASSERT_ENABLED) {
        if (!condition) {
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
        }
    }
}
