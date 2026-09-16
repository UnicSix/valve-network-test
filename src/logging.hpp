#pragma once

#include <print>

#define InfoLog(__fmt, ...)                                     \
    do {                                                        \
        std::print(stdout,                                      \
                   "\033[34m[INFO ][{} -- line: {}]\033[0m ",   \
                   __FILE_NAME__, __LINE__);                    \
        std::println(stdout, __fmt __VA_OPT__(, ) __VA_ARGS__); \
    } while (0)

#ifndef NDEBUG
#define DebugLog(__fmt, ...)                                        \
    do {                                                            \
        std::print(stderr,                                          \
                   "\033[38;5;208m[DEBUG][{} -- line: {}]\033[0m ", \
                   __FILE_NAME__, __LINE__);                        \
        std::println(stderr, __fmt __VA_OPT__(, ) __VA_ARGS__);     \
    } while (0)
#else
#define DebugLog(__fmt, ...) \
    do {                     \
    } while (0)
#endif

#define ErrorLog(__fmt, ...)                                    \
    do {                                                        \
        std::print(stderr,                                      \
                   "\033[31m[ERROR][{} -- line: {}]\033[0m ",   \
                   __FILE_NAME__, __LINE__);                    \
        std::println(stderr, __fmt __VA_OPT__(, ) __VA_ARGS__); \
    } while (0)
