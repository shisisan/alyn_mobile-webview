//
// Created by ALYN on 2024/8/18.
//

#pragma once

#include "../SA.h"
#include "TaskSimpleRunNamedAnim.h"

namespace sa {
class CTaskSimpleSlideToCoord : public CTaskSimpleRunNamedAnim {
public:
	CVector m_SlideToPos;
	float m_fAimingRotation; // Heading
	float m_fSpeed;
	bool m_bFirstTime : 1;
	bool m_bRunningAnim : 1;
	int32 m_Timer;
};
VALIDATE_SIZE(CTaskSimpleSlideToCoord, (VER_x32 ? 0x80 : 0x90));
}
