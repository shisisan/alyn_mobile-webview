//
// Created by ALYN on 2024/8/15.
//

#include "Game.h"
#include "../Net/NetGame.h"
#include "../UI/UI.h"
#include "../Client.h"
#include "Entity.h"

extern UI* pUI;
extern Game* pGame;
extern NetGame* pNetGame;

void CEntity::Add()
{
	// spdlog::info("CEntity::add()");

	if (!m_entity || IsEntityPlaceable(m_entity)) {
		spdlog::error("CEntity::add: m_entity is nullptr");
		return;
	}

	if (!m_entity->m_pMovingList) {
		sa::CVector vec = {0.0f, 0.0f, 0.0f};

		m_entity->SetVelocity(vec);
		m_entity->SetTurnSpeed(vec);

		// CWorld::add
		Memory::callFunction<void>("_ZN6CWorld3AddEP7CEntity", m_entity);

		RwMatrix mat = m_entity->GetMatrix().ToRwMatrix();
		TeleportTo(mat.pos.x, mat.pos.y, mat.pos.z);
	}
}

void CEntity::Remove()
{
	// spdlog::info("CEntity::Remove()");

	if (!m_entity || IsEntityPlaceable(m_entity)) {
		spdlog::error("CEntity::Remove: m_entity is nullptr");
		return;
	}

	if (m_entity->m_pMovingList) {
		// CWorld::Remove
		Memory::callFunction<void>("_ZN6CWorld6RemoveEP7CEntity", m_entity);
	}
}

bool CEntity::IsAdded()
{
	// spdlog::info("CEntity::IsAdded()");

	if (m_entity) {
		if (IsEntityPlaceable(m_entity)) {
			return false;
		}

		if (m_entity->m_pMovingList) {
			return true;
		}
	}

	return false;
}

void CEntity::TeleportTo(float fX, float fY, float fZ)
{
	// spdlog::info("CEntity::TeleportTo({}, {}, {})", fX, fY, fZ);

	RwMatrix mat;

	if (m_entity && !IsEntityPlaceable(m_entity)) {
		uint16_t modelIndex = m_entity->m_nModelIndex;
		if (modelIndex != TRAIN_PASSENGER_LOCO &&
				modelIndex != TRAIN_FREIGHT_LOCO &&
				modelIndex != TRAIN_TRAM) {
			if (fX > 3000.0f || fX < -3000.0f || fY > 3000.0f || fY < -3000.0f) {
				mat = m_entity->GetMatrix().ToRwMatrix();
				mat.pos = sa::CVector{fX, fY, fZ};
				m_entity->SetMatrix((sa::CMatrix&) mat);
				UpdateRwMatrixAndFrame();
			}
			else {
				Memory::callFunction(*(void**) (*(uintptr_t*) m_entity + 15 * sizeof(void*)), m_entity, fX, fY, fZ, 0);
			}
		}
		else {
			ScriptCommand(&put_train_at, m_gtaId, fX, fY, fZ);
		}
	}
}

uint CEntity::GetModelIndex()
{
	// spdlog::info("CEntity::GetModelIndex()");

	if (!m_entity) {
		spdlog::error("CEntity::GetModelIndex: m_entity is nullptr");
		return 0;
	}

	return m_entity->m_nModelIndex;
}

void CEntity::SetModelIndex(uint uiModel)
{
	// spdlog::info("CEntity::SetModelIndex({})", uiModel);

	if (!m_entity) {
		spdlog::error("CEntity::SetModelIndex: m_entity is nullptr");
		return;
	}

	int iTryCount = 0;
	if (!pGame->IsModelLoaded(uiModel) && !GetModelRWObject(uiModel)) {
		pGame->RequestModel(uiModel);
		pGame->LoadRequestedModels();
		while (!pGame->IsModelLoaded(uiModel)) {
			sleep(1);

			if (iTryCount > 200) {
				pUI->chat()->addDebugMessage("Warning: Model %u wouldn't load in time!", uiModel);
				return;
			}

			iTryCount++;
		}
	}

	m_entity->DeleteRWObject();
	m_entity->m_nModelIndex = uiModel;
	m_entity->SetModelIndex(uiModel);
}

float CEntity::GetDistanceFromLocalPlayerPed()
{
	//spdlog::info("CEntity::GetDistanceFromLocalPlayerPed()");

	RwMatrix matFromPlayer;
	RwMatrix matThis;
	float fSX, fSY, fSZ;

	CPlayerPed* pLocalPlayerPed = pGame->FindPlayerPed();
	CLocalPlayer* pLocalPlayer = nullptr;

	if (!pLocalPlayerPed) {
		//spdlog::error("CEntity::GetDistanceFromLocalPlayerPed: pLocalPlayerPed is nullptr");
		return 10000.0f;
	}

	if (!m_entity) {
		//spdlog::error("CEntity::GetDistanceFromLocalPlayerPed: m_entity is nullptr");
		return 10000.0f;
	}

	matThis = m_entity->GetMatrix().ToRwMatrix();

	if (pNetGame) {
		pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
		if (pLocalPlayer && (pLocalPlayer->IsSpectating() || pLocalPlayer->IsInRCMode())) {
			pGame->GetCamera()->GetMatrix(&matFromPlayer);
		}
		else {
			matFromPlayer = pLocalPlayerPed->m_ped->GetMatrix().ToRwMatrix();
		}
	}
	else {
		matFromPlayer = pLocalPlayerPed->m_ped->GetMatrix().ToRwMatrix();
	}

	fSX = (matThis.pos.x - matFromPlayer.pos.x) * (matThis.pos.x - matFromPlayer.pos.x);
	fSY = (matThis.pos.y - matFromPlayer.pos.y) * (matThis.pos.y - matFromPlayer.pos.y);
	fSZ = (matThis.pos.z - matFromPlayer.pos.z) * (matThis.pos.z - matFromPlayer.pos.z);

	return (float) sqrt(fSX + fSY + fSZ);
}

float CEntity::GetDistanceFromCamera()
{
	//spdlog::info("CEntity::GetDistanceFromCamera()");

	if (!m_entity || IsEntityPlaceable(m_entity)) {
		spdlog::error("CEntity::GetDistanceFromCamera: m_entity is nullptr");
		return 100000.0f;
	}

	RwMatrix matEnt = m_entity->GetMatrix().ToRwMatrix();

	sa::CCamera& TheCamera = sa::getTheCamera();

	float tmpX = (matEnt.pos.x - TheCamera.m_mCameraMatrix.m_pos.x);
	float tmpY = (matEnt.pos.y - TheCamera.m_mCameraMatrix.m_pos.y);
	float tmpZ = (matEnt.pos.z - TheCamera.m_mCameraMatrix.m_pos.z);

	return sqrt(tmpX * tmpX + tmpY * tmpY + tmpZ * tmpZ);
}

float CEntity::GetDistanceFromPoint(sa::CVector Vector)
{
	//spdlog::info("CEntity::GetDistanceFromPoint({}, {}, {})", Vector.x, Vector.y, Vector.z);

	if (!m_entity) {
		spdlog::error("CEntity::GetDistanceFromPoint: m_entity is nullptr");
		return 10000.0f;
	}

	RwMatrix mat = m_entity->GetMatrix().ToRwMatrix();

	float tmpX = (mat.pos.x - Vector.x) * (mat.pos.x - Vector.x);
	float tmpY = (mat.pos.y - Vector.y) * (mat.pos.y - Vector.y);
	float tmpZ = (mat.pos.z - Vector.z) * (mat.pos.z - Vector.z);

	return (float) sqrt(tmpX + tmpY + tmpZ);
}

bool CEntity::GetCollisionChecking()
{
	// spdlog::info("CEntity::GetCollisionChecking()");

	if (m_entity && !IsEntityPlaceable(m_entity)) {
		return m_entity->m_nFlags & 1;
	}

	return true;
}

void CEntity::SetCollisionChecking(bool bCheck)
{
	// spdlog::info("CEntity::SetCollisionChecking({})", bCheck);

	if (m_entity && !IsEntityPlaceable(m_entity)) {
		if (bCheck) {
			m_entity->m_nFlags |= 1;
		}
		else {
			m_entity->m_nFlags &= 0xFFFFFFFE;
		}
	}
}

void CEntity::SetGravityProcessing(bool state)
{
	// spdlog::info("CEntity::SetGravityProcessing({})", state);

	if (m_entity && !IsEntityPlaceable(m_entity)) {
		if (state) {
			m_entity->m_nFlags &= 0x7FFFFFFD;
		}
		else {
			m_entity->m_nFlags |= 0x80000002;
		}
	}
}

void CEntity::UpdateMatrix(RwMatrix matrix)
{
	// spdlog::info("CEntity::UpdateMatrix()");

	if (m_entity && m_entity->m_matrix) {
		m_entity->Remove();
		m_entity->SetMatrix((sa::CMatrix&) matrix);
		UpdateRwMatrixAndFrame();
		m_entity->Add();
	}
}

void CEntity::UpdateRwMatrixAndFrame()
{
	// spdlog::info("CEntity::UpdateRwMatrixAndFrame()");

	if (m_entity && !IsEntityPlaceable(m_entity)) {
		if (m_entity->m_pRwObject) {
			if (m_entity->m_matrix) {
				RwMatrix* pRwMatrix = RwFrameGetMatrix(RwFrameGetParent(m_entity->m_pRwObject));

				// CMatrix::UpdateRwMatrix(const CMatrix *this, RwMatrix *pRwMatrix)
				Memory::callFunction<void>("_ZNK7CMatrix14UpdateRwMatrixEP11RwMatrixTag", m_entity->m_matrix, pRwMatrix);

				// CEntity::UpdateRwFrame(CEntity *this)
				Memory::callFunction<void>("_ZN7CEntity13UpdateRwFrameEv", m_entity);
			}
		}
	}
}

void CEntity::Render()
{
	// spdlog::info("CEntity::Render()");

	if (!m_entity) {
		spdlog::error("CEntity::Render: m_entity is nullptr");
		return;
	}

	int iModel = m_entity->m_nModelIndex;
	if (iModel >= 400 && iModel <= 611 && m_entity->m_pRwObject) {
		// CVisibilityPlugins::SetupVehicleVariables
		Memory::callFunction("_ZN18CVisibilityPlugins21SetupVehicleVariablesEP7RpClump", m_entity->m_pRwObject);
	}

	// CEntity::PreRender
	Memory::callFunction(g_saSym->GetVmtFunction(*(uintptr_t*) m_entity, 18), m_entity);

	if (m_entity->m_pRwObject) {
		// CRenderer::RenderOneNonRoad
		Memory::callFunction("_ZN9CRenderer16RenderOneNonRoadEP7CEntity", m_entity);
	}
}

bool CEntity::EnforceWorldBoundries(float fPX, float fZX, float fPY, float fNY)
{
	// spdlog::info("CEntity::EnforceWorldBoundries({}, {}, {}, {})", fPX, fZX, fPY, fNY);

	if (!m_entity) {
		spdlog::error("CEntity::EnforceWorldBoundries: m_entity is nullptr");
		return false;
	}

	RwMatrix matWorld = m_entity->GetMatrix().ToRwMatrix();
	sa::CVector vecMoveSpeed = m_entity->GetMoveSpeed();

	if (matWorld.pos.x > fPX) {
		if (vecMoveSpeed.x != 0.0f) {
			vecMoveSpeed.x = -0.2f;
			vecMoveSpeed.z = 0.1f;
		}

		m_entity->SetVelocity(vecMoveSpeed);
		matWorld.pos.z += 0.04f;
		m_entity->SetMatrix((sa::CMatrix&) matWorld);
		return true;
	}
	else if (matWorld.pos.x < fZX) {
		if (vecMoveSpeed.x != 0.0f) {
			vecMoveSpeed.x = 0.2f;
			vecMoveSpeed.z = 0.1f;
		}

		m_entity->SetVelocity(vecMoveSpeed);
		matWorld.pos.z += 0.04f;
		m_entity->SetMatrix((sa::CMatrix&) matWorld);
		return true;
	}
	else if (matWorld.pos.y > fPY) {
		if (vecMoveSpeed.y != 0.0f) {
			vecMoveSpeed.y = -0.2f;
			vecMoveSpeed.z = 0.1f;
		}

		m_entity->SetVelocity(vecMoveSpeed);
		matWorld.pos.z += 0.04f;
		m_entity->SetMatrix((sa::CMatrix&) matWorld);
		return true;
	}
	else if (matWorld.pos.y < fNY) {
		if (vecMoveSpeed.y != 0.0f) {
			vecMoveSpeed.y = 0.2f;
			vecMoveSpeed.z = 0.1f;
		}

		m_entity->SetVelocity(vecMoveSpeed);
		matWorld.pos.z += 0.04f;
		m_entity->SetMatrix((sa::CMatrix&) matWorld);
		return true;
	}

	return false;
}

bool CEntity::HasExceededWorldBoundries(float fPX, float fZX, float fPY, float fNY)
{
	// spdlog::info("CEntity::HasExceededWorldBoundries({}, {}, {}, {})", fPX, fZX, fPY, fNY);

	// TODO
	return false;
}
