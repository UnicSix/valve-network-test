#pragma once

#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <print>
#include <string_view>
#include "cpp_alias.hpp"

// very basic features that std doesn't provide
#ifdef __linux__
#include <unistd.h>
#elifdef _WIN32
#include <windows.h>
#endif

const inline std::filesystem::path get_ExecutableDirectory() {
#ifdef __linux__
    char path_buf[1024];
    auto len = readlink("/proc/self/exe", path_buf, sizeof(path_buf));
    if (len == -1) {
        std::println(stderr,
                     "errno: {}, Failed to get executable path",
                     errno);
        len = 0;  // return empty string on error
    }
    else if (len > sizeof(path_buf)) {
        std::println(stderr, "Path length > {} chars",
                     sizeof(path_buf));
        len = 0;
    }
#elif defined(_WIN32)
    char  path_buf[1024];
    DWORD len =
        GetModuleFileNameA(nullptr, path_buf, sizeof(path_buf));
    if (len == 0 || len >= sizeof(path_buf)) {
        std::println(stderr, "Err: {}, GetModuleFileNameA failed",
                     (u64)GetLastError());
        len = 0;
    }
#endif
    return std::filesystem::path(std::string_view(path_buf, len))
        .parent_path();
}

// copy and store string content in a consistent, immutable buffer
// guarantees null-termination
// Non-copyable, Non-Movable
class ConstString {
   public:
    ConstString(const ConstString& other) = delete;

    ConstString(
        std::initializer_list<std::string_view> strs) noexcept {
        this->len = 0;
        for (auto& str : strs) {
            this->len += str.length();
        }
        data    = static_cast<char*>(malloc(this->len + 1));
        int loc = 0;
        for (auto& str : strs) {
            std::memcpy(data + loc, str.data(), str.length());
            loc += str.length();
        }
        data[len] = '\0';
    }
    ~ConstString() {
        std::free(data);
        data = nullptr;
        len  = 0;
    }

    constexpr int   Size() const { return len; }
    constexpr char* Cstr() const { return data; }

   private:
    char* data = nullptr;
    int   len  = 0;
};
