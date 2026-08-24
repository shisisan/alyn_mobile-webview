/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Base.h"
#include "Vector2D.h"

/* CRect class describes a rectangle.

    A(left;top)_____________________→
    |                               |
    |                               |
    |                               |
    |_________________B(right;bottom)
    ↓
*/

namespace sa {
class CRect {
public:
	// Init in flipped state
	float left = 1000000.0F; // x1
	float top = -1000000.0F; // y1
	float right = -1000000.0F; // x2
	float bottom = 1000000.0F; // y2

public:
	CRect() = default; // 0x4041C0 - TODO: Fix retarded argument order to be: left, top, right, bottom

	constexpr CRect(float left, float bottom, float right, float top)
			:
			left{left},
			bottom{bottom},
			right{right},
			top{top}
	{
		// assert(!IsFlipped());
	}

	constexpr CRect(const CVector2D& bottomLeft, const CVector2D& topRight)
			:
			CRect{bottomLeft.x, bottomLeft.y, topRight.x, topRight.y}
	{
	}

	/// A rect that can fit a circle of `radius` inside with `pos` being the center
	constexpr CRect(const CVector2D& pos, float radius)
			:
			CRect{pos.x - radius, pos.y - radius, pos.x + radius, pos.y + radius}
	{
	}

	[[nodiscard]] constexpr inline bool IsFlipped() const
	{ // 0x404190
		return left > right || bottom > top;
	}

	[[nodiscard]] inline CVector2D GetCenter() const { return {(right + left) * 0.5F, (bottom + top) * 0.5F}; }
	CVector2D GetTopLeft() const { return {left, top}; }
	CVector2D GetBottomRight() const { return {right, bottom}; }
};
VALIDATE_SIZE(CRect, 0x10);
}
