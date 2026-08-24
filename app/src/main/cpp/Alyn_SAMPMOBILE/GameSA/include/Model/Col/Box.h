//
// Created by ALYN on 2024/8/17.
//
#pragma once

#include "../../SA.h"

namespace sa {
class CBox {
public:
	CVector m_vecMin{}, m_vecMax{};

	constexpr CBox() = default;
	constexpr CBox(CVector min, CVector max)
			: m_vecMin(min), m_vecMax(max) { }

	CVector GetSize() const { return m_vecMax - m_vecMin; }
	float GetWidth() const { return m_vecMax.x - m_vecMin.x; }
	float GetLength() const { return m_vecMax.y - m_vecMin.y; }
	float GetHeight() const { return m_vecMax.z - m_vecMin.z; }
	CVector GetCenter() const { return (m_vecMax + m_vecMin) / 2.f; }
};
VALIDATE_SIZE(CBox, 0x18);
}
