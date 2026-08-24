#pragma once

#include "Base.h"

namespace sa {
class CPtrNode {
public:
	void* m_item;
	CPtrNode* m_next;
};
VALIDATE_SIZE(CPtrNode, (VER_x32 ? 0x8 : 0x10));

class CPtrNodeSingleLink {
public:
	void* m_item;
	CPtrNodeSingleLink* m_next;
};
VALIDATE_SIZE(CPtrNodeSingleLink, (VER_x32 ? 0x8 : 0x10));

class CPtrNodeDoubleLink {
public:
	void* m_item;
	CPtrNodeDoubleLink* m_next;
	CPtrNodeDoubleLink* m_prev;
};
VALIDATE_SIZE(CPtrNodeDoubleLink, (VER_x32 ? 0xC : 0x18));
}
