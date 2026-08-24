//
// Created by ALYN on 2024/8/18.
//
#pragma once

#include "../SA.h"
#include "TaskSimpleAnim.h"

namespace sa {
class CTaskSimpleRunNamedAnim : public CTaskSimpleAnim {
public:
	char m_animName[24];
	char m_animGroupName[16];
	float m_fBlendDelta;
	CAnimBlendHierarchy* m_pAnimHierarchy;
	uint32 m_Time;
	CTaskTimer m_Timer;
	CVector m_vecOffsetAtEnd;
	uint32 m_animFlags;
	int16 m_nAnimId;
};
VALIDATE_SIZE(CTaskSimpleRunNamedAnim, (VER_x32 ? 0x64 : 0x78));
}
