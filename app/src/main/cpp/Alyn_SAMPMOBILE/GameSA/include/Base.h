/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <cstdint>

namespace sa {
// Basic types for structures describing
typedef int8_t int8;
typedef int8_t Int8;
typedef int8_t __int8;
typedef int16_t int16;
typedef int16_t Int16;
typedef int16_t __int16;
typedef int32_t int32;
typedef int32_t Int32;
typedef int32_t __int32;
typedef int64_t int64;
typedef int64_t Int64;
typedef int64_t __int64;
typedef uint8_t uint8;
typedef uint8_t UInt8;
typedef uint8_t UInt8;
typedef uint16_t uint16;
typedef uint16_t UInt16;
typedef uint32_t uint32;
typedef uint32_t UInt32;
typedef uint64_t uint64;
typedef uint64_t UInt64;
typedef intptr_t intptr;
typedef uintptr_t uintptr;

typedef uint8 bool8;
typedef uint8 Bool8;
typedef uint16 bool16;
typedef uint16 Bool16;
typedef uint32 bool32;
typedef uint32 Bool32;

typedef uint16 Char16;

#define VALIDATE_SIZE(struc, size) static_assert(sizeof(struc) == size, "Invalid structure size of " #struc)

VALIDATE_SIZE(bool, 1);
VALIDATE_SIZE(char, 1);
VALIDATE_SIZE(short, 2);
VALIDATE_SIZE(int, 4);
VALIDATE_SIZE(float, 4);
VALIDATE_SIZE(long long, 8);
}
