//
// Created by ALYN on 2024/8/28.
//

#include "Game.h"
#include "../Net/NetGame.h"
#include "../Client.h"

extern Game* pGame;
extern NetGame* pNetGame;
extern MaterialTextGenerator* pMaterialTextGenerator;

RwTexture* g_blankTexture;

CObject::CObject(int iModel, sa::CVector vecPos, sa::CVector vecRot, float fDrawDistance, uint8_t bAttached)
{
	spdlog::info("CObject::CObject({}, [{} {} {}], [{} {} {}], {}, {}).", iModel, vecPos.x, vecPos.y, vecPos.z, vecRot.x, vecRot.y, vecRot.z, fDrawDistance, bAttached);
	m_AttachedVehicleID = INVALID_VEHICLE_ID;
	m_AttachedObjectID = INVALID_OBJECT_ID;
	m_bAttachedToPed = bAttached;

	m_entity = nullptr;
	m_gtaId = 0;

	m_vecAttachedPos.x = 0.0f;
	m_vecAttachedPos.y = 0.0f;
	m_vecAttachedPos.z = 0.0f;
	m_vecAttachedRot.x = 0.0f;
	m_vecAttachedRot.y = 0.0f;
	m_vecAttachedRot.z = 0.0f;
	m_bSyncRotation = true;

	if (!IsValidModel(iModel)) {
		spdlog::info("Object model {} is invalid, using default model.", iModel);
		iModel = 18631;
	}

	uint32_t dwRetID;
	ScriptCommand(&create_object, iModel, vecPos.x, vecPos.y, vecPos.z, &dwRetID);
	spdlog::info("Object created with model {} at [{} {} {}].", iModel, vecPos.x, vecPos.y, vecPos.z);

	sa::CObject* pEntity = GamePool_Object_GetAt(dwRetID);

	if (dwRetID && pEntity) {
		m_entity = pEntity;
		m_gtaId = dwRetID;

		m_byteMoving = 0;
		m_fMoveSpeed = 0.0f;
		m_bNeedRotate = false;

		m_iModel = iModel;

		RwMatrix mat = m_entity->GetMatrix().ToRwMatrix();
		mat.pos = vecPos;
		m_entity->SetMatrix((sa::CMatrix&) mat);
		SetRotation(&vecRot);
	}
	else {
		spdlog::error("Failed to create object with model {} at [{} {} {}].", iModel, vecPos.x, vecPos.y, vecPos.z);
	}

	for (int i = 0; i < MAX_MATERIALS; i++) {
		memset(&m_Material[i], 0, sizeof(MATERIAL_OBJECT));
		m_pLastObjectMaterialTexture[i] = nullptr;
		m_dwLastObjectMaterialColor[i] = {};
	}

	m_bHasCustomObjectMaterial = false;
	m_bAllowChangeObjectMaterial = false;
	m_bObjectMaterialTextureChanged = false;
	m_bObjectMaterialColorChanged = false;
	m_uiLastObjectGeometry = 0;
	m_pLastObjectMaterialRwObject = nullptr;

	m_bForceRender = false;
}

CObject::~CObject()
{
	sa::CObject* pEntity = GamePool_Object_GetAt(m_gtaId);
	m_entity = pEntity;

	if (m_entity && !IsEntityPlaceable(m_entity)) {
		ScriptCommand(&destroy_object, m_gtaId);
		if (GetModelRefCounts(m_iModel) == 0) {
			pGame->RemoveModel(m_iModel, false);
		}
	}

	if (m_bHasCustomObjectMaterial) {
		for (int i = 0; i < MAX_MATERIALS; i++) {
			if (m_Material[i].pMaterialTexture && m_Material[i].pMaterialTexture != g_blankTexture) {
				RwTextureDestroy(m_Material[i].pMaterialTexture);
				m_Material[i].pMaterialTexture = 0;
			}

			memset(&m_Material[i], 0, sizeof(MATERIAL_OBJECT));
			m_pLastObjectMaterialTexture[i] = 0;
			m_dwLastObjectMaterialColor[i] = {};
		}

		m_bHasCustomObjectMaterial = false;
	}
}

/**
 * @param pVehicle
 */
void CObject::AttachToVehicle(CVehicle* pVehicle)
{
	if (!ScriptCommand(&is_object_attached, m_gtaId)) {
		ScriptCommand(&attach_object_to_car,
				m_gtaId,
				pVehicle->m_gtaId,
				m_vecAttachedPos.x,
				m_vecAttachedPos.y,
				m_vecAttachedPos.z,
				m_vecAttachedRot.x,
				m_vecAttachedRot.y,
				m_vecAttachedRot.z);
	}
}

/**
 * @param pObject
 */
void CObject::AttachToObject(CObject* pObject)
{
	if (!ScriptCommand(&is_object_attached, m_gtaId)) {
		ScriptCommand(&attach_object_to_object,
				m_gtaId,
				pObject->m_gtaId,
				m_vecAttachedPos.x,
				m_vecAttachedPos.y,
				m_vecAttachedPos.z,
				m_vecAttachedRot.x,
				m_vecAttachedRot.y,
				m_vecAttachedRot.z);
	}
}

/**
 * @param matPos
 * @return
 */
float CObject::DistanceRemaining(RwMatrix* matPos)
{
	float fSX, fSY, fSZ;
	fSX = (matPos->pos.x - m_matTarget.pos.x) * (matPos->pos.x - m_matTarget.pos.x);
	fSY = (matPos->pos.y - m_matTarget.pos.y) * (matPos->pos.y - m_matTarget.pos.y);
	fSZ = (matPos->pos.z - m_matTarget.pos.z) * (matPos->pos.z - m_matTarget.pos.z);
	return (float) sqrt(fSX + fSY + fSZ);
}

void CObject::ApplyMoveSpeed()
{
	if (m_entity) {
		float fTimeStep = *g_saSym->GetSymbol<float*>("_ZN6CTimer12ms_fTimeStepE");

		RwMatrix mat = m_entity->GetMatrix().ToRwMatrix();
		mat.pos.x += fTimeStep * m_entity->GetMoveSpeed().x;
		mat.pos.y += fTimeStep * m_entity->GetMoveSpeed().y;
		mat.pos.z += fTimeStep * m_entity->GetMoveSpeed().z;
		m_entity->SetMatrix((sa::CMatrix&) mat);
	}
}

/**
 * @param fElapsedTime
 */
void CObject::Process(float fElapsedTime)
{
	if (m_AttachedVehicleID != INVALID_VEHICLE_ID) {
		if (pNetGame) {
			CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
			if (pVehiclePool) {
				CVehicle* pVehicle = pVehiclePool->GetAt(m_AttachedVehicleID);
				if (pVehicle) {
					if (pVehicle->IsAdded()) {
						AttachToVehicle(pVehicle);
					}
				}
			}
		}
		return;
	}

	if (m_AttachedObjectID != INVALID_OBJECT_ID) {
		if (pNetGame) {
			CObjectPool* pObjectPool = pNetGame->GetObjectPool();
			if (pObjectPool) {
				CObject* pObject = pObjectPool->GetAt(m_AttachedObjectID);
				if (pObject) {
					AttachToObject(pObject);
				}
			}
		}

		return;
	}

	if (m_byteMoving & 1) {
		sa::CVector vecSpeed = {0.0f, 0.0f, 0.0f};

		RwMatrix matEnt = m_entity->GetMatrix().ToRwMatrix();

		float distance = fElapsedTime * m_fMoveSpeed;
		float remaining = DistanceRemaining(&matEnt);
		uint32_t dwThisTick = GetTickCount();

		float posX = matEnt.pos.x;
		float posY = matEnt.pos.y;
		float posZ = matEnt.pos.z;

		float f1 = ((float) (dwThisTick - m_dwMoveTick)) * 0.001f * m_fMoveSpeed;
		float f2 = m_fDistanceToTargetPoint - remaining;

		if (distance >= remaining) {
			m_entity->SetVelocity(vecSpeed);
			m_entity->SetTurnSpeed(vecSpeed);
			matEnt.pos.x = m_matTarget.pos.x;
			matEnt.pos.y = m_matTarget.pos.y;
			matEnt.pos.z = m_matTarget.pos.z;
			if (m_bNeedRotate) {
				m_quatTarget.GetMatrix(&matEnt);
			}
			UpdateMatrix(matEnt);
			StopMoving();
			return;
		}

		if (fElapsedTime <= 0.0f) {
			return;
		}

		float delta = 1.0f / (remaining / distance);
		matEnt.pos.x += ((m_matTarget.pos.x - matEnt.pos.x) * delta);
		matEnt.pos.y += ((m_matTarget.pos.y - matEnt.pos.y) * delta);
		matEnt.pos.z += ((m_matTarget.pos.z - matEnt.pos.z) * delta);

		distance = remaining / m_fDistanceToTargetPoint;
		float slerpDelta = 1.0f - distance;

		delta = 1.0f / fElapsedTime;
		vecSpeed.x = (matEnt.pos.x - posX) * delta * 0.02f;
		vecSpeed.y = (matEnt.pos.y - posY) * delta * 0.02f;
		vecSpeed.z = (matEnt.pos.z - posZ) * delta * 0.02f;

		if (FloatOffset(f1, f2) > 0.1f) {
			if (f1 > f2) {
				delta = (f1 - f2) * 0.1f + 1.0f;
				vecSpeed.x *= delta;
				vecSpeed.y *= delta;
				vecSpeed.z *= delta;
			}

			if (f2 > f1) {
				delta = 1.0f - (f2 - f1) * 0.1f;
				vecSpeed.x *= delta;
				vecSpeed.y *= delta;
				vecSpeed.z *= delta;
			}
		}

		m_entity->SetVelocity(vecSpeed);
		ApplyMoveSpeed();

		if (m_bNeedRotate) {
			float fx, fy, fz;
			GetRotation(&fx, &fy, &fz);
			distance = m_vecRotationTarget.z - distance * m_vecSubRotationTarget.z;
			vecSpeed.x = 0.0f;
			vecSpeed.y = 0.0f;
			vecSpeed.z = subAngle(remaining, distance) * 0.01f;
			if (vecSpeed.z <= 0.001f) {
				if (vecSpeed.z < -0.001f) {
					vecSpeed.z = -0.001f;
				}
			}
			else {
				vecSpeed.z = 0.001f;
			}

			m_entity->SetTurnSpeed(vecSpeed);
			matEnt = m_entity->GetMatrix().ToRwMatrix();
			sa::CQuaternion quat;
			quat.Slerp(&m_quatStart, &m_quatTarget, slerpDelta);
			quat.Normalize();
			quat.GetMatrix(&matEnt);
		}
		else {
			matEnt = m_entity->GetMatrix().ToRwMatrix();
		}

		UpdateMatrix(matEnt);
	}
}

/**
 * @param ObjectID
 * @param vecPos
 * @param vecRot
 * @param bSyncRotation
 */
void CObject::SetAttachedObject(uint16_t ObjectID, sa::CVector* vecPos, sa::CVector* vecRot, bool bSyncRotation)
{
	if (ObjectID == INVALID_OBJECT_ID) {
		m_AttachedObjectID = INVALID_OBJECT_ID;
		m_vecAttachedPos.x = 0.0f;
		m_vecAttachedPos.y = 0.0f;
		m_vecAttachedPos.z = 0.0f;
		m_vecAttachedRot.x = 0.0f;
		m_vecAttachedRot.y = 0.0f;
		m_vecAttachedRot.z = 0.0f;
		m_bSyncRotation = false;
	}
	else {
		m_AttachedObjectID = ObjectID;
		m_vecAttachedPos.x = vecPos->x;
		m_vecAttachedPos.y = vecPos->y;
		m_vecAttachedPos.z = vecPos->z;
		m_vecAttachedRot.x = vecRot->x;
		m_vecAttachedRot.y = vecRot->y;
		m_vecAttachedRot.z = vecRot->z;
		m_bSyncRotation = bSyncRotation;
	}
}

void CObject::SetScale(sa::CVector vecScale)
{
	// todo
}

sa::CVector CObject::GetScale()
{
	// todo
}

/**
 * @param vecRotation
 */
void CObject::SetRotation(sa::CVector* vecRotation)
{
	if (m_entity && GamePool_Object_GetAt(m_gtaId)) {
		ScriptCommand(&set_object_rotation, m_gtaId, vecRotation->x, vecRotation->y, vecRotation->z);
		m_vecRotation.x = vecRotation->x;
		m_vecRotation.y = vecRotation->y;
		m_vecRotation.z = vecRotation->z;
	}
}

/**
 * @param pfX
 * @param pfY
 * @param pfZ
 */
void CObject::GetRotation(float* pfX, float* pfY, float* pfZ)
{
	if (m_entity) {
		sa::CMatrix* mat = m_entity->m_matrix;

		if (mat) {
			// CMatrix::ConvertToEulerAngles
			Memory::callFunction("_ZN7CMatrix20ConvertToEulerAnglesERfS0_S0_i", mat, pfX, pfY, pfZ, 21);
		}

		*pfX = *pfX * 57.295776 * -1.0;
		*pfY = *pfY * 57.295776 * -1.0;
		*pfZ = *pfZ * 57.295776 * -1.0;
	}
}

/**
 * @param fX
 * @param fY
 * @param fZ
 */
void CObject::TeleportTo(float fX, float fY, float fZ)
{
	if (fX > 3000.0f || fX < -3000.0f || fY > 3000.0f || fY < -3000.0f) {
		m_bForceRender = true;
	}
	else {
		m_bForceRender = false;
	}

	CEntity::TeleportTo(fX, fY, fZ);
}

/**
 * @param VehicleID
 * @param vecPos
 * @param vecRot
 */
void CObject::SetAttachedVehicle(uint16_t VehicleID, sa::CVector* vecPos, sa::CVector* vecRot)
{
	if (VehicleID == INVALID_VEHICLE_ID) {
		m_AttachedVehicleID = INVALID_VEHICLE_ID;
		m_vecAttachedPos.x = 0.0f;
		m_vecAttachedPos.y = 0.0f;
		m_vecAttachedPos.z = 0.0f;
		m_vecAttachedRot.x = 0.0f;
		m_vecAttachedRot.y = 0.0f;
		m_vecAttachedRot.z = 0.0f;
	}
	else {
		m_AttachedVehicleID = VehicleID;
		m_vecAttachedPos.x = vecPos->x;
		m_vecAttachedPos.y = vecPos->y;
		m_vecAttachedPos.z = vecPos->z;
		m_vecAttachedRot.x = vecRot->x;
		m_vecAttachedRot.y = vecRot->y;
		m_vecAttachedRot.z = vecRot->z;
	}
}

/**
 * @param fX
 * @param fY
 * @param fZ
 * @param fSpeed
 * @param fRotX
 * @param fRotY
 * @param fRotZ
 */
void CObject::MoveTo(float fX, float fY, float fZ, float fSpeed, float fRotX, float fRotY, float fRotZ)
{
	RwMatrix mat = m_entity->GetMatrix().ToRwMatrix();

	if (m_byteMoving & 1) {
		StopMoving();
		mat.pos.x = m_matTarget.pos.x;
		mat.pos.y = m_matTarget.pos.y;
		mat.pos.z = m_matTarget.pos.z;

		if (m_bNeedRotate) {
			m_quatTarget.GetMatrix(&mat);
		}

		UpdateMatrix(mat);
	}

	m_dwMoveTick = GetTickCount();
	m_fMoveSpeed = fSpeed;
	m_matTarget.pos.x = fX;
	m_matTarget.pos.y = fY;
	m_matTarget.pos.z = fZ;
	m_byteMoving |= 1;

	if (fRotX <= -999.0f || fRotY <= -999.0f || fRotZ <= -999.0f) {
		m_bNeedRotate = false;
	}
	else {
		m_bNeedRotate = true;

		sa::CVector vecRot;
		RwMatrix matrix;
		GetRotation(&vecRot.x, &vecRot.y, &vecRot.z);
		m_vecRotationTarget.x = fixAngle(fRotX);
		m_vecRotationTarget.y = fixAngle(fRotY);
		m_vecRotationTarget.z = fixAngle(fRotZ);

		m_vecSubRotationTarget.x = subAngle(vecRot.x, fRotX);
		m_vecSubRotationTarget.y = subAngle(vecRot.y, fRotY);
		m_vecSubRotationTarget.z = subAngle(vecRot.z, fRotZ);

		RotateMatrix(sa::CVector{fRotX, fRotY, fRotZ});
		matrix = m_entity->GetMatrix().ToRwMatrix();
		m_quatStart.SetFromMatrix(&matrix);
		m_quatTarget.SetFromMatrix(&m_matTarget);
		m_quatStart.Normalize();
		m_quatTarget.Normalize();
	}

	m_fDistanceToTargetPoint = GetDistanceFromPoint(m_matTarget.pos);

	if (pNetGame) {
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		if (pPlayerPool) {
//			pPlayerPool->GetLocalPlayer()->UpdateSurfing();
		}
	}

	// sub_1009F070
	m_entity->m_nFlags &= 0xFFFFFFF7;
}

/**
 * @param vecRot
 */
void CObject::RotateMatrix(sa::CVector vecRot)
{
	m_vecRotation = vecRot;

	vecRot.x *= 0.017453292f; // x * pi/180
	vecRot.y *= 0.017453292f; // y * pi/180
	vecRot.z *= 0.017453292f; // z * pi/180

	float cosx = cos(vecRot.x);
	float sinx = sin(vecRot.x);

	float cosy = cos(vecRot.y);
	float siny = sin(vecRot.y);

	float cosz = cos(vecRot.z);
	float sinz = sin(vecRot.z);

	float sinzx = sinz * sinx;
	float coszx = cosz * sinx;

	m_matTarget.right.x = cosz * cosy - sinzx * siny;
	m_matTarget.right.y = coszx * siny + sinz * cosy;
	m_matTarget.right.z = -(siny * cosx);
	m_matTarget.up.x = -(sinz * cosx);
	m_matTarget.up.y = cosz * cosx;
	m_matTarget.up.z = sinx;
	m_matTarget.at.x = sinzx * cosy + cosz * siny;
	m_matTarget.at.y = sinz * siny - coszx * cosy;
	m_matTarget.at.z = cosy * cosx;
}

void CObject::StopMoving()
{
	sa::CVector vec = {0.0f, 0.0f, 0.0f};
	m_entity->SetVelocity(vec);
	m_entity->SetTurnSpeed(vec);

	m_byteMoving &= ~1;
}

/**
 * @return
 */
bool CObject::AttachedToMovingEntity()
{
	if (m_AttachedObjectID == INVALID_OBJECT_ID) {
		if (m_AttachedVehicleID != INVALID_VEHICLE_ID) {
			return true;
		}

		return (m_byteMoving & 1);
	}
	else {
		if (m_AttachedObjectID >= 0 && m_AttachedObjectID < MAX_OBJECTS) {
			if (pNetGame) {
				CObjectPool* pObjectPool = pNetGame->GetObjectPool();
				if (pObjectPool) {
					CObject* pObject = pObjectPool->GetAt(m_AttachedObjectID);
					if (pObject) return (pObject->m_byteMoving & 1);
				}
			}
		}
	}

	return false;
}

void CObject::SetMaterial(int iModel, uint8_t byteMaterialIndex, char* txdname, char* texturename, uint32_t dwColor)
{
	spdlog::info("CObject::SetMaterial({}, {}, {}, {}, 0x{:X}).", iModel, byteMaterialIndex, txdname, texturename, dwColor);

	if (byteMaterialIndex < MAX_MATERIALS) {
		if (!txdname || !strlen(txdname) || !texturename || !strlen(texturename)) {
			return;
		}

		if (m_Material[byteMaterialIndex].bTextureWasCreated && m_Material[byteMaterialIndex].pMaterialTexture) {
			RwTextureDestroy(m_Material[byteMaterialIndex].pMaterialTexture);
			m_Material[byteMaterialIndex].pMaterialTexture = nullptr;
		}

		if (iModel <= 0 || IsValidModel(iModel)) {
			if (iModel <= 0) {
				if (!iModel) {
					if (!g_blankTexture) {
						g_blankTexture = LoadTextureFromTxd("*", "blanktex");
					}

					m_Material[byteMaterialIndex].pMaterialTexture = g_blankTexture;
				}
			}
			else {
				int iWaitPreloaded;
				if (!pGame->IsModelLoaded(iModel)) {
					pGame->RequestModel(iModel);
					pGame->LoadRequestedModels();
					while (!pGame->IsModelLoaded(iModel)) {
						usleep(1);
						iWaitPreloaded++;
						if (iWaitPreloaded > 250) {
							m_Material[byteMaterialIndex].pMaterialTexture = 0;
							break;
						}
					}
				}

				if (pGame->IsModelLoaded(iModel)) {
					m_Material[byteMaterialIndex].pMaterialTexture = LoadTextureFromTxd(txdname, texturename);

					if (!ModelInfoGetReferenceCount(iModel)) {
						pGame->RemoveModel(iModel, false);
					}
				}
			}

			m_Material[byteMaterialIndex].dwMaterialColor = dwColor;
			m_Material[byteMaterialIndex].bTextureWasCreated = true;
			m_bHasCustomObjectMaterial = true;
		}
	}
}

void CObject::SetMaterialText(uint8_t byteMaterialIndex, uint8_t byteMaterialSize, const char* szFontName, uint8_t byteFontSize, uint8_t byteFontBold, uint32_t dwFontColor, uint32_t dwBackgroundColor, uint8_t byteAlign, const char* szText)
{
	spdlog::info("CObject::SetMaterialText({}, {}, {}, {}, {}, 0x{:X}, 0x{:X}, {}, {}).", byteMaterialIndex, byteMaterialSize, szFontName, byteFontSize, byteFontBold, dwFontColor, dwBackgroundColor, byteAlign, szText);

	if (byteMaterialIndex < MAX_MATERIALS) {
		if (!szText || !strlen(szText)) {
			return;
		}

		if (m_Material[byteMaterialIndex].bTextureWasCreated && m_Material[byteMaterialIndex].pMaterialTexture) {
			RwTextureDestroy(m_Material[byteMaterialIndex].pMaterialTexture);
			m_Material[byteMaterialIndex].pMaterialTexture = 0;
		}

		m_Material[byteMaterialIndex].TextInfo.byteMaterialSize = byteMaterialSize;

		memset(m_Material[byteMaterialIndex].TextInfo.szFontName, 0, 32);
		strncpy(m_Material[byteMaterialIndex].TextInfo.szFontName, szFontName, 32);
		m_Material[byteMaterialIndex].TextInfo.szFontName[32] = 0;

		m_Material[byteMaterialIndex].TextInfo.byteFontSize = byteFontSize;
		m_Material[byteMaterialIndex].TextInfo.byteFontBold = byteFontBold;
		m_Material[byteMaterialIndex].TextInfo.dwFontColor = dwFontColor;
		m_Material[byteMaterialIndex].TextInfo.dwBackgroundColor = dwBackgroundColor;
		m_Material[byteMaterialIndex].TextInfo.byteAlign = byteAlign;

		memset(m_Material[byteMaterialIndex].TextInfo.szText, 0, 2048);
		strncpy(m_Material[byteMaterialIndex].TextInfo.szText, szText, 2048);
		m_Material[byteMaterialIndex].TextInfo.szText[2048] = 0;

		m_Material[byteMaterialIndex].iType = MATERIAL_TYPE_TEXT;
		m_Material[byteMaterialIndex].dwMaterialColor = 0;
		m_Material[byteMaterialIndex].bTextureWasCreated = false;

		if (!m_bHasCustomObjectMaterial) m_bHasCustomObjectMaterial = true;
	}
}

void CObject::GetMaterialTextSize(int iMatSize, int* iSizeX, int* iSizeY)
{
	int sizes[14][2] = {
			{32, 32}, {64, 32}, {64, 64}, {128, 32}, {128, 64}, {128, 128}, {256, 32},
			{256, 64}, {256, 128}, {256, 256}, {512, 64}, {512, 128}, {512, 256}, {512, 512}
	};

	int del = (iMatSize / 10) - 1;
	*iSizeX = sizes[del][0];
	*iSizeY = sizes[del][1];
}

void CObject::ProcessMaterialText()
{
	for (int i = 0; i < MAX_MATERIALS; i++) {
		if (m_Material[i].bTextureWasCreated || m_Material[i].iType != MATERIAL_TYPE_TEXT) {
			return;
		}

		if (!m_Material[i].TextInfo.szText || !strlen(m_Material[i].TextInfo.szText)) {
			return;
		}

		if (m_Material[i].TextInfo.byteMaterialSize < MATERIAL_SIZE_32X32) {
			m_Material[i].TextInfo.byteMaterialSize = MATERIAL_SIZE_32X32;
		}
		else if (m_Material[i].TextInfo.byteMaterialSize > MATERIAL_SIZE_512X512) {
			m_Material[i].TextInfo.byteMaterialSize = MATERIAL_SIZE_512X512;
		}

		int iSizeX, iSizeY;
		GetMaterialTextSize(m_Material[i].TextInfo.byteMaterialSize, &iSizeX, &iSizeY);

		if (m_Material[i].TextInfo.byteAlign < MATERIAL_ALIGN_LEFT || m_Material[i].TextInfo.byteAlign > MATERIAL_ALIGN_RIGHT) {
			m_Material[i].TextInfo.byteAlign = MATERIAL_ALIGN_CENTER;
		}

		m_Material[i].TextInfo.byteFontSize *= 0.75;

		m_Material[i].pMaterialTexture = pMaterialTextGenerator->Create(iSizeX, iSizeY, m_Material[i].TextInfo.szFontName, m_Material[i].TextInfo.byteFontSize, m_Material[i].TextInfo.byteFontBold, m_Material[i].TextInfo.dwFontColor, m_Material[i].TextInfo.dwBackgroundColor, m_Material[i].TextInfo.byteAlign, m_Material[i].TextInfo.szText);

		m_Material[i].bTextureWasCreated = true;
	}
}

void CObject::TryChangeToCustomObjectMaterial()
{
	if (m_bHasCustomObjectMaterial && m_bAllowChangeObjectMaterial) {
		ChangeToCustomObjectMaterial();
	}

	m_bAllowChangeObjectMaterial = true;
}

RpAtomic* ChangeCustomMaterialObjectCB(RpAtomic* result, CObject* pObject)
{
	if (*(uint8_t*) result == 1) {
		RpGeometry* geometry = result->geometry;
		if (geometry) {
			int count = geometry->matList.numMaterials;
			if (count >= 1) {
				RpMaterial** materials = geometry->matList.materials;
				for (int i = 0; i < count; i++) {
					if (i >= MAX_MATERIALS) break;

					if (pObject->m_Material[i].pMaterialTexture) {
						materials[i]->texture = pObject->m_Material[i].pMaterialTexture;
						pObject->m_bObjectMaterialTextureChanged = true;
					}

					if (pObject->m_Material[i].dwMaterialColor) {
						geometry->flags |= rpGEOMETRYMODULATEMATERIALCOLOR;
						geometry->flags &= 0xFFFFFFF7;
						materials[i]->color = *(RwRGBA*) &pObject->m_Material[i].dwMaterialColor;
						materials[i]->surfaceProps.ambient = 1.0f;
						materials[i]->surfaceProps.specular = 0.0f;
						materials[i]->surfaceProps.diffuse = 1.0f;
						pObject->m_bObjectMaterialColorChanged = true;
					}
				}
			}
		}
	}

	return result;
}

RpAtomic* StoreOriginalMaterialObjectCB(RpAtomic* result, CObject* pObject)
{
	if (*(uint8_t*) result == 1) {
		RpGeometry* geometry = result->geometry;
		if (geometry) {
			pObject->m_uiLastObjectGeometry = geometry->flags;

			int count = geometry->matList.numMaterials;
			if (count >= 1) {
				RpMaterial** materials = geometry->matList.materials;
				for (int i = 0; i < count; i++) {
					if (i >= MAX_MATERIALS) break;
					pObject->m_pLastObjectMaterialTexture[i] = materials[i]->texture;
					pObject->m_dwLastObjectMaterialColor[i] = *(RwRGBA*) &materials[i]->color;
				}
			}
		}
	}

	return result;
}

RpAtomic* ChangeOriginalMaterialObjectCB(RpAtomic* result, CObject* pObject)
{
	if (*(uint8_t*) result == 1) {
		RpGeometry* geometry = result->geometry;
		if (geometry) {
			int count = geometry->matList.numMaterials;
			if (count >= 1) {
				geometry->flags = pObject->m_uiLastObjectGeometry;

				RpMaterial** materials = geometry->matList.materials;
				for (int i = 0; i < count; i++) {
					if (i >= MAX_MATERIALS) break;
					if (pObject->m_bObjectMaterialTextureChanged) {
						materials[i]->texture = pObject->m_pLastObjectMaterialTexture[i];
					}
					if (pObject->m_bObjectMaterialColorChanged) {
						materials[i]->color = *(RwRGBA*) &pObject->m_dwLastObjectMaterialColor[i];
					}
				}

				pObject->m_bObjectMaterialTextureChanged = false;
				pObject->m_bObjectMaterialColorChanged = false;
			}
		}
	}

	return result;
}

void CObject::ChangeToCustomObjectMaterial()
{
	if (m_bHasCustomObjectMaterial) {
		if (m_entity->m_pRwObject) {
			if (m_pLastObjectMaterialRwObject == m_entity->m_pRwObject) {
				auto parent = reinterpret_cast<RwFrame*>(m_entity->m_pRwObject->parent);
				if (parent) {
					RwFrameForAllObjects(parent, reinterpret_cast<RpAtomicCallBack*>(ChangeCustomMaterialObjectCB), this);
				}
			}
			else {
				auto parent = reinterpret_cast<RwFrame*>(m_entity->m_pRwObject->parent);
				if (parent) {
					RwFrameForAllObjects(parent, reinterpret_cast<RpAtomicCallBack*>(StoreOriginalMaterialObjectCB), this);
				}

				m_pLastObjectMaterialRwObject = m_entity->m_pRwObject;
				m_bAllowChangeObjectMaterial = false;
			}
		}
	}
}

void CObject::ChangeToOriginalObjectMaterial()
{
	if (m_bHasCustomObjectMaterial) {
		if (m_bObjectMaterialTextureChanged || m_bObjectMaterialColorChanged) {
			if (m_entity->m_pRwObject && m_pLastObjectMaterialRwObject == m_entity->m_pRwObject) {
				auto parent = reinterpret_cast<RwFrame*>(m_entity->m_pRwObject->parent);
				if (parent) {
					RwFrameForAllObjects(parent, reinterpret_cast<RpAtomicCallBack*>(ChangeOriginalMaterialObjectCB), this);
				}
			}
		}
	}
}
