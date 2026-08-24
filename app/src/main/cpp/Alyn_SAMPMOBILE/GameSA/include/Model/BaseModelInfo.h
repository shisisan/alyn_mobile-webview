//
// Created by ALYN on 2024/8/17.
//
#pragma once

#include "../SA.h"
#include "Col/ColModel.h"

namespace sa {
class CBaseModelInfo {
public:
	uintptr_t vtable;
	uint32 m_hashKey;
	uint8 m_modelName[21];
	uint8 pad[1];
	int16 m_numRefs;
	int16 m_txdIndex;
	uint8 m_alpha;
	uint8 m_num2dEffects;
	int16 m_n2dEffects;
	int16 m_dynamicIndex;
	uint16 m_flags;
	uint8 pad2[2];
	CColModel* m_pColModel;
	float m_lodDistance;
#if !VER_x32
	uint8 pad3[4];
#endif
	RwObject* m_pRwObject;
};
VALIDATE_SIZE(CBaseModelInfo, (VER_x32 ? 0x38 : 0x48));
}
