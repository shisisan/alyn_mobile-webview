#pragma once

#include "Task.h"

namespace sa {
class CTaskSimple : public CTask {
	// ~
};
VALIDATE_SIZE(CTaskSimple, (VER_x32 ? 0x8 : 0x10));
}
