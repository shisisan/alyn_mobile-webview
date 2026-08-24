//
// Created by ALYN on 2024/8/17.
//
#pragma once

#include "../../SA.h"
#include "ColSphere.h"

namespace sa {
class CColDisk : public CColSphere {
public:
	CVector m_vThickness{};
	float m_fThickness{};

public:
	// 0x40FD50
	/*
	void Set(float startRadius, const CVector& start, const CVector& end, float endRadius, eSurfaceType material, uint8 pieceType, tColLighting lighting) {
		m_fStartRadius = startRadius;
		m_vecStart = start;
		m_vecEnd = end;
		m_fEndRadius = endRadius;
		m_Surface.m_nMaterial = material;
		m_Surface.m_nPiece = pieceType;
		m_Surface.ligthing = lighting;
	}
	*/
};
VALIDATE_SIZE(CColDisk, 0x24);
}
