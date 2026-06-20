module;

#include "debug.h"

#include <vector>
#include <print>
#include <functional>

export module test;

struct TestCase {
    std::string_view name;
    std::function<void()> fn;
};

struct TestFramework {
    std::vector<TestCase> tests;
};
static TestFramework tf;

export struct TestRegister {
    TestRegister(std::string_view name, std::function<void()> fn) {
        tf.tests.push_back({name, fn});
    }
};

export void runTests(void) {
    for (auto& test : tf.tests) {
        std::print("Test \"{0}\": Running\n", test.name);
        try {
            test.fn();
        } catch (const TestError& err) {
            std::print("Test FAILED: {0}\n", err.what());
            return;
        }
        std::print("Test \"{0}\": Successful\n", test.name);
    }
}
