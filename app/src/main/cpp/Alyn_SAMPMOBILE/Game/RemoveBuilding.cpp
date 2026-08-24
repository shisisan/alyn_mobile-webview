//
// Created by ALYN on 16-Jan-25.
//

#include "Game.h"
#include "RemoveBuilding.h"

void RemoveBuilding::add(uint32_t model, sa::CVector pos, float radius)
{
	spdlog::info("RemoveBuilding::add {}, ({}, {}, {}), {}", model, pos.x, pos.y, pos.z, radius);

	if (model == 19300) {
		return;
	}

	if (m_buildingToRemoveCount >= 1000) {
		return;
	}

	removeObjectInRange(model, pos, radius);

	m_buildingToRemove[m_buildingToRemoveCount].model = model;
	m_buildingToRemove[m_buildingToRemoveCount].pos = pos;
	m_buildingToRemove[m_buildingToRemoveCount].radius = radius;
	m_buildingToRemoveCount++;
}

void RemoveBuilding::removeObjectInRange(uint32_t model, sa::CVector pos, float radius)
{
	removeOccludersInRadius(pos, 20000.0f);

	typedef sa::CPool<sa::CEntity, sa::CEntity> CPool;

	// CPools::ms_pBuildingPool
	CPool* pBuildingPool = *g_saSym->GetSymbol<CPool**>("_ZN6CPools16ms_pBuildingPoolE");
	if (!pBuildingPool) {
		spdlog::error("RemoveBuilding::removeObjectInRange: ms_pBuildingPool is null");
		return;
	}

	for (int i = 0; i < pBuildingPool->m_nSize; i++) {
		sa::CEntity* pEntity = pBuildingPool->GetAt(i);
		if (pEntity) {
			if (model == -1 || pEntity->m_nModelIndex == model) {
				float fDistance = sa::DistanceBetweenPoints(pEntity->GetPosition(), pos);
				if (fDistance <= radius) {
					if (&pEntity->m_placement) {
						pEntity->m_placement.m_vPosn.z -= 2000.0;
					}
				}
			}
		}
	}

	// CPools::ms_pDummyPool
	CPool* pDummyPool = *g_saSym->GetSymbol<CPool**>("_ZN6CPools13ms_pDummyPoolE");
	if (!pDummyPool) {
		spdlog::error("RemoveBuilding::removeObjectInRange: ms_pDummyPool is null");
		return;
	}

	for (int i = 0; i < pDummyPool->m_nSize; i++) {
		sa::CEntity* pEntity = pDummyPool->GetAt(i);
		if (pEntity) {
			if (model == -1 || pEntity->m_nModelIndex == model) {
				float fDistance = sa::DistanceBetweenPoints(pEntity->GetPosition(), pos);
				if (fDistance <= radius) {
					if (&pEntity->m_placement) {
						pEntity->m_placement.m_vPosn.z -= 2000.0;
					}
				}
			}
		}
	}

	// CPools::ms_pObjectPool
	CPool* pObjectPool = *g_saSym->GetSymbol<CPool**>("_ZN6CPools14ms_pObjectPoolE");
	if (!pObjectPool) {
		spdlog::error("RemoveBuilding::removeObjectInRange: ms_pObjectPool is null");
		return;
	}

	for (int i = 0; i < pObjectPool->m_nSize; i++) {
		sa::CEntity* pEntity = pObjectPool->GetAt(i);
		if (pEntity) {
			if (model == -1 || pEntity->m_nModelIndex == model) {
				float fDistance = sa::DistanceBetweenPoints(pEntity->GetPosition(), pos);
				if (fDistance <= radius) {
					if (&pEntity->m_placement) {
						pEntity->m_placement.m_vPosn.z -= 2000.0;
					}
				}
			}
		}
	}
}

class COccluder {
public:
	unsigned short fMidX;
	unsigned short fMidY;
	unsigned short fMidZ;
	unsigned short fWidthX;
	unsigned short fWidthY;
	unsigned short fHeight;
	char cRotZ;
	char cRotY;
	char cRotX;
	char cPad;
	unsigned short nFlags;
};
VALIDATE_SIZE(COccluder, 0x12);

COccluder* aOccluders = nullptr;
int32_t NumOccludersOnMap = 0;

void RemoveBuilding::removeOccludersInRadius(sa::CVector pos, float radius)
{
	if (NumOccludersOnMap >= 1) {
		for (int i = 0; i <= NumOccludersOnMap; i++) {
			sa::CVector vecOccluderPos;
			vecOccluderPos.x = (float) (aOccluders[i].fMidX * 0.25);
			vecOccluderPos.y = (float) (aOccluders[i].fMidY * 0.25);
			vecOccluderPos.z = (float) (aOccluders[i].fMidZ * 0.25);

			float fDistance = sa::DistanceBetweenPoints(vecOccluderPos, pos);
			if (fDistance <= radius) {
				aOccluders[i].fMidX = 0;
				aOccluders[i].fMidY = 0;
				aOccluders[i].fMidZ = 0;
				aOccluders[i].fWidthX = 0;
				aOccluders[i].fWidthY = 0;
				aOccluders[i].fHeight = 0;
			}
		}
	}
}

bool RemoveBuilding::needsRemove(uint32_t model, sa::CVector pos)
{
	if (m_buildingToRemoveCount == 0) {
		return false;
	}

	for (int i = 0; i < m_buildingToRemoveCount; i++) {
		if (m_buildingToRemove[i].model == -1 || m_buildingToRemove[i].model == model) {
			if (sa::DistanceBetweenPoints(pos, m_buildingToRemove[i].pos) <= m_buildingToRemove[i].radius) {
				return true;
			}
		}
	}

	return false;
}

DECL_HOOK(void, CFileLoader_LoadObjectInstance, sa::CFileObjectInstance* instance, char* name)
{
	sa::CVector* pos = &instance->m_vecPosition;
	int* model = &instance->m_nModelId;

	if (instance && RemoveBuilding::needsRemove(*model, pos)) {
		spdlog::info("RemoveBuilding hook: model {}", *model);
		*model = 19300;
	}

	CFileLoader_LoadObjectInstance(instance, name);
}

void RemoveBuilding::installHooks()
{
	spdlog::info("Installing remove building hooks...");

	aOccluders = *g_saSym->GetSymbol<COccluder**>("_ZN10COcclusion10aOccludersE");
	NumOccludersOnMap = *g_saSym->GetSymbol<int32_t*>("_ZN10COcclusion17NumOccludersOnMapE");

	HOOK("_ZN11CFileLoader18LoadObjectInstanceEP19CFileObjectInstancePKc", CFileLoader_LoadObjectInstance);
}
