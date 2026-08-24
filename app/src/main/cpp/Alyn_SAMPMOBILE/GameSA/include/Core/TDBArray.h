#pragma once

#include "../Base.h"

namespace sa {
template<class A>
struct TDBArray {
	unsigned int numAlloced;
	unsigned int numEntries;
	A* dataPtr;
};
VALIDATE_SIZE(TDBArray<void*>, (VER_x32 ? 0xC : 0x10));
}
