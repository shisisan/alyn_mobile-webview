//
// Created by ALYN on 2024/8/18.
//
#pragma once

#include "../SA.h"
#include "AnimBlendSequence.h"

class CAnimBlendAssociation;

namespace sa {
class CAnimBlendNode {
public:
	float m_fTheta0; // angle between two quaternions
	float m_fTheta1; // 1 / sin(m_theta0), used in slerp calculation

	// indices into array in sequence
	int16 m_nNextKeyFrameId;
	int16 m_PreviousKeyFrameId;

	float m_fRemainingTime; // time until frames have to advance

	CAnimBlendSequence* m_pAnimSequence;
	CAnimBlendAssociation* m_pAnimBlendAssociation;
};
VALIDATE_SIZE(CAnimBlendNode, (VER_x32 ? 0x18 : 0x20));
}
