//
// Created by ALYN on 2024/8/17.
//
#pragma once

#include "../../SA.h"
#include "Box.h"
#include "Enums/SurfaceType.h"
#include "Collision/ColPoint.h"

namespace sa {
struct CColSurface {
	eSurfaceType m_nMaterial{};
	uint8 m_nPiece{}; // ePedPieceTypes, eCarPiece, etc
	tColLighting m_nLighting{};
	uint8 m_nLight{};
};
VALIDATE_SIZE(CColSurface, 0x4);

class CColBox : public CBox {
public:
	CColSurface m_Surface;

public:
	constexpr CColBox() = default;
	constexpr CColBox(const CVector& min, const CVector& max)
			: CBox(min, max) { }
	constexpr CColBox(const CBox& box)
			: CBox(box) { }
	constexpr CColBox(const CBox& box, eSurfaceType material, uint8 pieceType, tColLighting lightning)
			: CBox(box)
	{
		m_Surface.m_nMaterial = material;
		m_Surface.m_nPiece = pieceType;
		m_Surface.m_nLighting = lightning;
	}

	auto GetSurfaceType() const { return m_Surface.m_nMaterial; }
};
VALIDATE_SIZE(CColBox, 0x1C);
}
