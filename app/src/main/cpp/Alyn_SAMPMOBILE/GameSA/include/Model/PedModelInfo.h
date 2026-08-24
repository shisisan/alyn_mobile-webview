//
// Created by ALYN on 17-Jan-25.
//

#ifndef SA_MP_MOBILE_PEDMODELINFO_H
#define SA_MP_MOBILE_PEDMODELINFO_H

#include "../SA.h"
#include "ClumpModelInfo.h"
#include "Entity/Ped/Ped.h"

namespace sa {
class CPedModelInfo : public CClumpModelInfo {
public:
	void Init()
	{
		Memory::callFunction(*(void**) (vtable + 7 * sizeof(void*)), this);
	}

public:
	AssocGroupId m_motionAnimGroup;
	ePedType m_defaultPedType;
	uint32_t m_defaultPedStats;
	uint16 m_drivesWhichCars;
	uint16 m_flags;
	CColModel* m_pHitColModel;
	int8 m_radio1;
	int8 m_radio2;
	uint8 m_Race;
	uint8 pad[1];
	int16 m_AudioPedType;
	int16 m_FirstVoice;
	int16 m_LastVoice;
	int16 m_NextVoice;
#if !VER_x32
	uint8 pad2[4];
#endif
};
VALIDATE_SIZE(CPedModelInfo, (VER_x32 ? 0x5C : 0x78));
}

#endif //SA_MP_MOBILE_PEDMODELINFO_H
