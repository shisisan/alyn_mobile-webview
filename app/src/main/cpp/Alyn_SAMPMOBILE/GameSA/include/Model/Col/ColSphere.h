//
// Created by ALYN on 2024/8/17.
//
#pragma once

#include "../../SA.h"
#include "Sphere.h"
#include "ColBox.h"

namespace sa {
class CColSphere : public CSphere {
public:
	CColSurface m_Surface;

public:
	CColSphere() = default;

	CColSphere(const CSphere& sp, const CColSurface& surface = {})
			: // TODO: Make this explicit
			CSphere{sp}, m_Surface{surface} { }

	constexpr CColSphere(CSphere sp, eSurfaceType material, uint8 pieceType, tColLighting lighting = tColLighting(0xFF))
			: CSphere(sp)
	{
		m_Surface.m_nMaterial = material;
		m_Surface.m_nPiece = pieceType;
		m_Surface.m_nLighting = lighting;
	}

	[[deprecated]]
	CColSphere(float radius, const CVector& center)
			: CSphere(center, radius) { };

	CColSphere(const CVector& center, float radius)
			: CSphere(center, radius) { };

	auto GetSurfaceType() const { return m_Surface.m_nMaterial; }
};
VALIDATE_SIZE(CColSphere, 0x14);
}
