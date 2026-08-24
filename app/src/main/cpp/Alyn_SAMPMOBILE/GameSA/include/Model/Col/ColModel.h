//
// Created by ALYN on 2024/8/17.
//
#pragma once

#include "../../SA.h"
#include "CollisionData.h"
#include "BoundingBox.h"

namespace sa {
class CColModel {
public:
	CBoundingBox m_boundBox;
	CSphere m_boundSphere;
	uint8 m_nColSlot;
	union {
		struct {
			uint8 m_bHasCollisionVolumes : 1; // AKA `m_bNotEmpty`
			uint8 m_bIsSingleColDataAlloc : 1;
			uint8 m_bIsActive : 1;
		};
		uint8 m_nFlags;
	};
	uint8 pad0[2];
	CCollisionData* m_pColData;

public:
	// HELPERS
	[[nodiscard]] auto GetTriCount() const noexcept { return m_pColData ? m_pColData->m_nNumTriangles : 0u; }
	[[nodiscard]] float GetBoundRadius() const noexcept { return m_boundSphere.m_fRadius; }
	auto& GetBoundCenter() { return m_boundSphere.m_vecCenter; }
	auto& GetBoundingBox() { return m_boundBox; }
	auto& GetBoundingSphere() { return m_boundSphere; }
	CCollisionData* GetData() const { return m_pColData; }
};
VALIDATE_SIZE(CColModel, (VER_x32 ? 0x30 : 0x38));
}
