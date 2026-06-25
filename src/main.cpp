#include "args.hpp"

#include <span>
#include <print>

int main(int argc, char **argv)
{
    const auto args = std::span{argv, argv + argc}
        | std::views::drop(1)
        | std::views::transform([](const auto *arg) { return std::string_view{arg}; })
        | std::ranges::to<std::vector>();
    ProgramOptions opts(args);

    std::print("help: {0}, version: {1}\n", opts.help, opts.version);
}
