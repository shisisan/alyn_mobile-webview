//
// Created by ALYN on 2024/8/17.
//
#pragma once

#include "../../SA.h"

namespace sa {
class CColTriangle {
public:
	CColTriangle() = default;
	CColTriangle(int32 a, int32 b, int32 c, eSurfaceType material, tColLighting light)
			:
			vA(a),
			vB(b),
			vC(c),
			m_nMaterial(material),
			m_nLight(light)
	{
	}

	auto GetSurfaceType() const { return m_nMaterial; }
public:
	union {
		struct {
			int32 vA; // vertex index in vertices array
			int32 vB; // vertex index in vertices array
			int32 vC; // vertex index in vertices array
		};
		int32 m_vertIndices[3];
	};
	eSurfaceType m_nMaterial;
	tColLighting m_nLight;
};
VALIDATE_SIZE(CColTriangle, 0x10);
}
