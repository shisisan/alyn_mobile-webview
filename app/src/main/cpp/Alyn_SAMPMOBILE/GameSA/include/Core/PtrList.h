#pragma once

#include "Base.h"
#include "PtrNode.h"

namespace sa {
class CPtrList {
public:
	CPtrNode* m_node;
};
VALIDATE_SIZE(CPtrList, (VER_x32 ? 0x4 : 0x8));
}
