#pragma once

import test;

#define TEST_FN(name) \
    static void name(); \
    TestRegister register_##name(#name, name); \
    static void name()
