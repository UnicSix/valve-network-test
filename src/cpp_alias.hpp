#pragma once

#include <cstdint>

using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8  = uint8_t;
using i32 = int32_t;
using i16 = int16_t;
using i8  = int8_t;
using f32 = float;
using f64 = double;

template <typename T>
inline constexpr u64 U64(T num) {
    return static_cast<u64>(num);
}

template <typename T>
inline constexpr u32 U32(T num) {
    return static_cast<u32>(num);
}

template <typename T>
inline constexpr u16 U16(T num) {
    return static_cast<u16>(num);
}

template <typename T>
inline constexpr u8 U8(T num) {
    return static_cast<u8>(num);
}

template <typename T>
inline constexpr i32 I32(T num) {
    return static_cast<i32>(num);
}

template <typename T>
inline constexpr i16 I16(T num) {
    return static_cast<i16>(num);
}

template <typename T>
inline constexpr i8 I8(T num) {
    return static_cast<i8>(num);
}

template <typename T>
inline constexpr f32 F32(T num) {
    return static_cast<f32>(num);
}

template <typename T>
inline constexpr f64 F64(T num) {
    return static_cast<f64>(num);
}
