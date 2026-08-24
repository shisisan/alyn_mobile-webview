//
// Created by ALYN on 2024/8/28.
//

#ifndef SA_MP_MOBILE_ATOMICMODELINFO_H
#define SA_MP_MOBILE_ATOMICMODELINFO_H

#include "../SA.h"

namespace sa {
class CAtomicModelInfo : public CBaseModelInfo {
public:
	void Init()
	{
		Memory::callFunction(*(void**) (vtable + 7 * sizeof(void*)), this);
	}
};
}

#endif //SA_MP_MOBILE_ATOMICMODELINFO_H
