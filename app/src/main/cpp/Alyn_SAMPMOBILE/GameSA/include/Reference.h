#pragma once

namespace sa {
class CReference {
public:
	CReference* m_pNext;
	void** m_ppReferenceAddress;
};
VALIDATE_SIZE(CReference, (VER_x32 ? 0x8 : 0x10));
}
