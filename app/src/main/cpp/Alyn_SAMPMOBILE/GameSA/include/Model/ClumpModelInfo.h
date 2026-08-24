//
// Created by ALYN on 17-Jan-25.
//

#ifndef SA_MP_MOBILE_CLUMPMODELINFO_H
#define SA_MP_MOBILE_CLUMPMODELINFO_H

#include "../SA.h"

namespace sa {
class CClumpModelInfo : public CBaseModelInfo {
public:
	uint32_t m_animFileIndex;
};
VALIDATE_SIZE(CClumpModelInfo, (VER_x32 ? 0x3C : 0x50));
}

#endif //SA_MP_MOBILE_CLUMPMODELINFO_H
