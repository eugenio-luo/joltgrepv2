#pragma once

#include <vector>
#include <span>
#include <meta>
#include <ranges>
#include <format>

struct ProgramOptions {
private:
    std::optional<int> findArg(const std::vector<std::string_view>& args, const std::string_view memStr) {
        const auto iter = std::ranges::find_if(args,
                    [&memStr](std::string_view arg) -> bool {
                        return arg.starts_with(memStr);
                    });

        return (iter == std::ranges::cend(args))
            ? std::nullopt
            : std::make_optional(std::ranges::distance(std::ranges::cbegin(args), iter));
    }

public:
    bool help = false;
    bool version = false;

    std::string versionString(void) {
        return "joltgrep 0.0.1\n";
    }

    std::string helpString(void) {
        constexpr auto ctx = std::meta::access_context::current();
        std::string out {"joltgrep usage:\n"};

        template for (constexpr auto member :
                      std::define_static_array(std::meta::nonstatic_data_members_of(^^ProgramOptions, ctx))) {
            const auto memStr = std::format("--{0}\n", std::meta::identifier_of(member));
            out += memStr;
        }

        return out;
    }

    ProgramOptions(int argc, char **argv) {
        constexpr auto ctx = std::meta::access_context::current();
        const auto args = std::span{argv, argv + argc}
            | std::views::drop(1)
            | std::views::transform([](const auto *arg) { return std::string_view{arg}; })
            | std::ranges::to<std::vector>();

        template for (constexpr auto member :
                      std::define_static_array(std::meta::nonstatic_data_members_of(^^ProgramOptions, ctx))) {
            const auto memStr = std::format("--{0}", std::meta::identifier_of(member));
            auto idx = findArg(args, memStr);

            if (idx) {
                this->[:member:] = true;
            }
        }
    }
};
