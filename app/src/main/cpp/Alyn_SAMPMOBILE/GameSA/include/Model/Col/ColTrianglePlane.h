//
// Created by ALYN on 2024/8/17.
//
#pragma once

#include "../../SA.h"

namespace sa {
typedef CVector CollisionVector;

class CColTriangle;

class CColTrianglePlane {
public:
	enum class Orientation : uint8 {
		POS_X,
		NEG_X,
		POS_Y,
		NEG_Y,
		POS_Z,
		NEG_Z,
	};
public:
	CollisionVector m_normal{4096};       //< Surface normal
	float m_normalOffset{128}; //< vA.Dot(m_normal)
	Orientation m_orientation{};  //< The planes orientation (Calculated from the normal)
	uint8 pad0[3];
};
VALIDATE_SIZE(CColTrianglePlane, 0x14);
}
