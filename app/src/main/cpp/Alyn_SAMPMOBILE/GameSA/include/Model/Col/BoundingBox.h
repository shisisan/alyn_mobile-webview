//
// Created by ALYN on 2024/8/17.
//
#pragma once

#include "../../SA.h"
#include "Box.h"
#include "Sphere.h"

namespace sa {
class CBoundingBox : public CBox {
public:
	constexpr CBoundingBox()
			: CBox(CVector{1.0f}, CVector{-1.0f}) { }
	constexpr CBoundingBox(CVector min, CVector max)
			: CBox(min, max) { }
	constexpr explicit CBoundingBox(const CBox& box)
			: CBox(box) { }
};
VALIDATE_SIZE(CBoundingBox, 0x18);
}
