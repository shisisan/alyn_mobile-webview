//
// Created by ALYN on 2024/8/17.
//
#pragma once

#include "../../SA.h"

namespace sa {
class CSphere {
public:
	CVector m_vecCenter{};
	float m_fRadius{};

	constexpr CSphere() = default;
	constexpr CSphere(CVector center, float radius)
			: m_vecCenter(center), m_fRadius(radius) { }
	[[deprecated]]
	constexpr CSphere(float radius, CVector center)
			: m_vecCenter(center), m_fRadius(radius) { }
};
VALIDATE_SIZE(CSphere, 0x10);
}
