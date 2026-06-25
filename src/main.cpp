#include "args.hpp"

#include <print>

void version(ProgramOptions& opts) {
    std::print("{}", opts.versionString());
}

void help(ProgramOptions& opts) {
    std::print("{}", opts.helpString());
}

int main(int argc, char **argv)
{
    ProgramOptions opts(argc, argv);

    if (opts.help) {
        help(opts);
    } else if (opts.version) {
        version(opts);
    }
}
