//
// Created by ALYN on 13-Feb-25.
//

#include "SA.h"
#include "../../../Game/Game.h"
#include "../../../Net/NetGame.h"

extern Game* pGame;
extern NetGame* pNetGame;

namespace sa {
CEntity::CEntity()
		: CPlaceable()
{
	m_nStatus = STATUS_ABANDONED;
	m_nType = ENTITY_TYPE_NOTHING;

	m_nFlags = 0;
	m_bIsVisible = true;
	m_bBackfaceCulled = true;

	m_nScanCode = 0;
	m_nAreaCode = eAreaCodes::AREA_CODE_NORMAL_WORLD;
	m_nModelIndex = 0xFFFF;
	m_pRwObject = nullptr;
	m_nIplIndex = 0;
	m_nRandomSeed = rand();
	m_pReferences = nullptr;
	m_pStreamingLink = nullptr;
	m_nNumLodChildren = 0;
	m_nNumLodChildrenRendered = 0;
	m_pLod = nullptr;
}

CEntity::~CEntity()
{
	if (m_pLod) {
		m_pLod->m_nNumLodChildren--;
	}

	CEntity::DeleteRWObject();
	CEntity::ResolveReferences();
}

void CEntity::ResolveReferences()
{
	Memory::callFunction("_ZN7CEntity17ResolveReferencesEv", this);
}

void CEntity::UpdateRwFrame()
{
	if (!m_pRwObject) {
		return;
	}

	//RwFrameUpdateObjects(static_cast<RwFrame*>(rwObjectGetParent(m_pRwObject)));
	Memory::callFunction("_Z20RwFrameUpdateObjectsP7RwFrame", static_cast<RwFrame*>(rwObjectGetParent(m_pRwObject)));
}

void CEntity::UpdateRW()
{
	if (!m_pRwObject) {
		return;
	}

	auto parentMatrix = GetModellingMatrix();
	if (m_matrix) {
		m_matrix->UpdateRwMatrix(parentMatrix);
	}
	else {
		m_placement.UpdateRwMatrix(parentMatrix);
	}
}

RwMatrix* CEntity::GetModellingMatrix()
{
	if (!m_pRwObject) {
		return nullptr;
	}

	return RwFrameGetMatrix(RwFrameGetParent(m_pRwObject));
}

void CEntity::RegisterReference(CEntity** entity)
{
	Memory::callFunction("_ZN7CEntity17RegisterReferenceEPPS_", this, entity);
}

void CEntity::PruneReferences()
{
	Memory::callFunction("_ZN7CEntity15PruneReferencesEv", this);
}

void CEntity::CleanUpOldReference(CEntity** entity)
{
	Memory::callFunction("_ZN7CEntity19CleanUpOldReferenceEPPS_", this, entity);
}
}
