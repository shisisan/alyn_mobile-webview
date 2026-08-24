//
// Created by ALYN on 2024/8/15.
//

#include "Game.h"
#include "../Client.h"
#include "../UI/UI.h"
#include "../Net/NetGame.h"

extern UI* pUI;
extern Game* pGame;
extern NetGame* pNetGame;

[[maybe_unused]] bool bInProcessDetachTrailer = false;

CVehicle::CVehicle(int iType, float fX, float fY, float fZ, float fRotation, bool bPreloaded, bool bSiren)
{
	spdlog::info("CVehicle::CVehicle({}, {}, {}, {}, {}, {}, {})", iType, fX, fY, fZ, fRotation, bPreloaded, bSiren);
	uint32_t dwRetID = 0;
	RwMatrix mat;
	static sa::CVehicle* pCreatedTrain = nullptr;

	m_pVehicle = nullptr;
	m_gtaId = 0;
	m_pTrailer = nullptr;

	if ((iType != TRAIN_PASSENGER_LOCO) &&
			(iType != TRAIN_FREIGHT_LOCO) &&
			(iType != TRAIN_PASSENGER) &&
			(iType != TRAIN_FREIGHT) &&
			(iType != TRAIN_TRAM)) {

		if (!pGame->IsModelLoaded(iType)) {
			pGame->RequestModel(iType);
			pGame->LoadRequestedModels();
			while (!pGame->IsModelLoaded(iType)) usleep(100);
		}

		ScriptCommand(&create_car, iType, fX, fY, fZ, &dwRetID);
		ScriptCommand(&set_car_z_angle, dwRetID, fRotation);
		ScriptCommand(&car_gas_tank_explosion, dwRetID, 0);
		ScriptCommand(&set_car_hydraulics, dwRetID, 0);
		ScriptCommand(&toggle_car_tires_vulnerable, dwRetID, 0);

		spdlog::info("vehicle ret id: {}", dwRetID);

		m_pVehicle = GamePool_Vehicle_GetAt(dwRetID);
		m_entity = m_pVehicle;
		m_gtaId = dwRetID;

		if (m_pVehicle) {
			m_pVehicle->m_nDoorLock = sa::CARLOCK_NOT_USED;
			m_pVehicle->m_fDirtLevel = 0.0f;
			m_bIsLocked = false;

			mat = m_pVehicle->GetMatrix().ToRwMatrix();
			mat.pos.x = fX;
			mat.pos.y = fY;
			mat.pos.z = fZ;

			if (!GetVehicleSubtype() == VEHICLE_SUBTYPE_BIKE && !GetVehicleSubtype() == VEHICLE_SUBTYPE_PUSHBIKE) {
				mat.pos.z += 0.25f;
			}

			m_pVehicle->SetMatrix((sa::CMatrix&) mat);
			m_bPreloaded = bPreloaded;
		}
	}
	else if ((iType == TRAIN_PASSENGER_LOCO) ||
			(iType == TRAIN_FREIGHT_LOCO) ||
			(iType == TRAIN_TRAM)) {
		if (iType == TRAIN_PASSENGER_LOCO) { iType = 5; }
		else if (iType == TRAIN_FREIGHT_LOCO) { iType = 3; }
		else if (iType == TRAIN_TRAM) iType = 9;

		uint32_t dwDirection = 0;
		if (fRotation > 180.0f) {
			dwDirection = 1;
		}

		pGame->RequestModel(TRAIN_PASSENGER_LOCO);
		pGame->RequestModel(TRAIN_PASSENGER);
		pGame->RequestModel(TRAIN_FREIGHT_LOCO);
		pGame->RequestModel(TRAIN_FREIGHT);
		pGame->RequestModel(TRAIN_TRAM);
		pGame->LoadRequestedModels();

		while (!pGame->IsModelLoaded(TRAIN_PASSENGER_LOCO)) usleep(1000);
		while (!pGame->IsModelLoaded(TRAIN_PASSENGER)) usleep(1000);
		while (!pGame->IsModelLoaded(TRAIN_FREIGHT_LOCO)) usleep(1000);
		while (!pGame->IsModelLoaded(TRAIN_FREIGHT)) usleep(1000);
		while (!pGame->IsModelLoaded(TRAIN_TRAM)) usleep(1000);

		ScriptCommand(&create_train, iType, fX, fY, fZ, dwDirection, &dwRetID);
		m_pVehicle = GamePool_Vehicle_GetAt(dwRetID);
		m_entity = m_pVehicle;
		m_gtaId = dwRetID;

		pCreatedTrain = m_pVehicle;

		GamePrepareTrain(m_pVehicle);
	}
	else if ((iType == TRAIN_PASSENGER) ||
			(iType == TRAIN_FREIGHT)) {
		if (!pCreatedTrain) {
			m_entity = nullptr;
			m_pVehicle = nullptr;
			return;
		}

		m_pVehicle = ((sa::CTrain*) pCreatedTrain)->pLinkedToForward; // todo check this?
		if (!m_pVehicle) {
			if (pUI) pUI->chat()->addDebugMessage("Warning: bad train carriages");
			m_entity = nullptr;
			m_pVehicle = nullptr;
			pCreatedTrain = nullptr;
			return;
		}

		m_gtaId = GamePool_Vehicle_GetIndex(m_pVehicle);
		m_entity = m_pVehicle;
		pCreatedTrain = m_pVehicle;
	}

	m_bIsInvulnerable = false;
	m_byteObjectiveVehicle = 0;
	m_bSpecialMarkerEnabled = false;
	m_dwMarkerID = 0;
	//_this->field_6D = 0;
	//_this->field_69 = GetTickCount();
	m_bLightState = false;
	m_bDoorsLocked = false;
	m_bHaveColor = false;
	//_this->field_71 = 0;
	//_this->field_75 = 0;
	//_this->field_8F = 0;
	memset(m_szPlateText, 0, sizeof(m_szPlateText));
}

CVehicle::~CVehicle()
{
	m_pVehicle = GamePool_Vehicle_GetAt(m_gtaId);

	if (m_pVehicle) {
		if (m_dwMarkerID) {
			spdlog::info("remove vehicle radar marker: {}", m_dwMarkerID);
			pGame->DisableMarker(m_dwMarkerID);
			m_dwMarkerID = 0;
		}

		RemoveEveryoneFromVehicle();

		if (m_pTrailer) {
			DetachTrailer();
			m_pTrailer = nullptr;
		}

		int iModel = m_pVehicle->m_nModelIndex;
		if (iModel == 538 || iModel == 537) {
			ScriptCommand(&destroy_train, m_gtaId);
		}
		else {
			ScriptCommand(&destroy_car, m_gtaId);
			int iRefs = GetModelRefCounts(iModel);
			spdlog::info("Destroy car({}): Refs: {}", iModel, iRefs);
			if (!iRefs && !m_bPreloaded) {
				pGame->RemoveModel(iModel, true);
			}
		}
	}
}

void CVehicle::RemoveEveryoneFromVehicle()
{
	if (!m_pVehicle) return;
	if (!GamePool_Vehicle_GetAt(m_gtaId)) return;

	float fPosX = m_pVehicle->m_matrix->m_pos.x;
	float fPosY = m_pVehicle->m_matrix->m_pos.y;
	float fPosZ = m_pVehicle->m_matrix->m_pos.z;

	int iPlayerID = 0;
	if (m_pVehicle->pDriver) {
		iPlayerID = GamePool_Ped_GetIndex(m_pVehicle->pDriver);
		ScriptCommand(&remove_actor_from_car_and_put_at, iPlayerID, fPosX, fPosY, fPosZ + 2);
	}

	for (int i = 0; i < 7; i++) {
		if (m_pVehicle->m_apPassengers[i] != NULL) {
			iPlayerID = GamePool_Ped_GetIndex(m_pVehicle->m_apPassengers[i]);
			ScriptCommand(&remove_actor_from_car_and_put_at, iPlayerID, fPosX, fPosY, fPosZ + 2);
		}
	}
}

float CVehicle::GetHealth()
{
	if (m_pVehicle) {
		return m_pVehicle->fHealth;
	}

	return 0.0f;
}

/**
 * @param byteColor1
 * @param byteColor2
 */
void CVehicle::SetColor(uint8_t byteColor1, uint8_t byteColor2)
{
	if (m_pVehicle && GamePool_Vehicle_GetAt(m_gtaId)) {
		m_pVehicle->m_nPrimaryColor = byteColor1;
		m_pVehicle->m_nSecondaryColor = byteColor2;
	}

	m_byteColor1 = byteColor1;
	m_byteColor2 = byteColor2;
	m_bHaveColor = true;
}

/**
 * @param iInterior
 */
void CVehicle::LinkToInterior(int iInterior)
{
	if (GamePool_Vehicle_GetAt(m_gtaId)) {
		ScriptCommand(&link_vehicle_to_interior, m_gtaId, iInterior);
	}
}

/**
 * @param byteTireDamageStatus
 */
void CVehicle::SetTireDamageStatus(uint8_t byteTireDamageStatus)
{
	if (m_pVehicle) {
		if (GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR) {
			((sa::CAutomobile*) m_pVehicle)->Damage.m_Wheel[0] = byteTireDamageStatus & 1;
			((sa::CAutomobile*) m_pVehicle)->Damage.m_Wheel[1] = (byteTireDamageStatus >> 1) & 1;
			((sa::CAutomobile*) m_pVehicle)->Damage.m_Wheel[2] = (byteTireDamageStatus >> 2) & 1;
			((sa::CAutomobile*) m_pVehicle)->Damage.m_Wheel[3] = (byteTireDamageStatus >> 3) & 1;
		}
		else if (GetVehicleSubtype() == VEHICLE_SUBTYPE_BIKE) {
			((sa::CBike*) m_pVehicle)->m_nWheelStatus[0] = byteTireDamageStatus & 1;
			((sa::CBike*) m_pVehicle)->m_nWheelStatus[1] = (byteTireDamageStatus >> 1) & 1;
		}
	}
}

/**
 * @param dwPanelDamageStatus
 * @param dwDoorDamageStatus
 * @param byteLightDamageStatus
 */
void CVehicle::SetDamageStatus(uint32_t dwPanelDamageStatus, uint32_t dwDoorDamageStatus, uint8_t byteLightDamageStatus)
{
	if (m_pVehicle) {
		if (GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR) {
			if (!dwPanelDamageStatus && !dwDoorDamageStatus && !byteLightDamageStatus) {
				uint32_t dwDoorStatus = (((sa::CAutomobile*) m_pVehicle)->Damage.m_Door[0] | (((sa::CAutomobile*) m_pVehicle)->Damage.m_Door[1] << 8) | (((sa::CAutomobile*) m_pVehicle)->Damage.m_Door[2] << 16) | (((sa::CAutomobile*) m_pVehicle)->Damage.m_Door[3] << 24));
				if (((sa::CAutomobile*) m_pVehicle)->Damage.m_Panels || dwDoorStatus || ((sa::CAutomobile*) m_pVehicle)->Damage.m_Lights) {
					// The ingame car is damaged in some way although the update
					// says the car should be repaired. So repair it and exit.
					// CAutoMobile::Fix
					Memory::callFunction("_ZN11CAutomobile3FixEv", m_pVehicle);
					return;
				}
			}

			// panel
			((sa::CAutomobile*) m_pVehicle)->Damage.m_Panels = dwPanelDamageStatus;

			// door
			((sa::CAutomobile*) m_pVehicle)->Damage.m_Door[0] = dwDoorDamageStatus & 7;
			((sa::CAutomobile*) m_pVehicle)->Damage.m_Door[1] = (dwDoorDamageStatus >> 8) & 7;
			((sa::CAutomobile*) m_pVehicle)->Damage.m_Door[2] = (dwDoorDamageStatus >> 16) & 7;
			((sa::CAutomobile*) m_pVehicle)->Damage.m_Door[3] = (dwDoorDamageStatus >> 24) & 7;

			// light
			((sa::CAutomobile*) m_pVehicle)->Damage.m_Lights = (uint32_t) byteLightDamageStatus;

			// CAutomobile::SetupDamageAfterLoad
			Memory::callFunction("_ZN11CAutomobile20SetupDamageAfterLoadEv", m_pVehicle);

		}
	}
}

void CVehicle::SetHealth(float fHealth)
{
	if (m_pVehicle) {
		m_pVehicle->fHealth = fHealth;
	}
}

/**
 * @return
 */
CVehicle* CVehicle::GetTrailer()
{
	if (m_pVehicle) {
		sa::CVehicle* pTrailer = m_pVehicle->m_pTrailer;
		if (pTrailer) {
			if (pNetGame) {
				CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
				if (pVehiclePool) {
					VEHICLEID TrailerID = pVehiclePool->FindIDFromGtaPtr(pTrailer);
					if (TrailerID != INVALID_VEHICLE_ID) {
						return pVehiclePool->GetAt(TrailerID);
					}
				}
			}
		}
	}

	return nullptr;
}

/**
 * @param bytePaintJobID
 */
void CVehicle::SetPaintJob(uint8_t bytePaintJobID)
{
	if (m_pVehicle && GamePool_Vehicle_GetAt(m_gtaId)) {
		if (GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR) {
			if (bytePaintJobID <= 3) {
				ScriptCommand(&change_car_skin, m_gtaId, bytePaintJobID);
			}
		}
	}
}

/**
 * @param iComponentID
 */
void CVehicle::AddComponent(int iComponentID)
{
	if (!m_pVehicle || !GamePool_Vehicle_GetAt(m_gtaId)) return;
	if (GetVehicleSubtype() != VEHICLE_SUBTYPE_CAR) return;

	if (iComponentID < 1000 || iComponentID > 1193) {
		spdlog::error("CVehicle::AddComponent: Invalid component ID {}", iComponentID);
		return;
	}

	/*if (!ScriptCommand(&is_component_available, iComponentID)) {
		pGame->RequestModel(iComponentID);
		pGame->LoadRequestedModels();
		ScriptCommand(&request_car_component, iComponentID);

		int dwTime = 0;
		while (dwTime < 20) {
			if (ScriptCommand(&is_component_available, iComponentID)) {
				break;
			}
			usleep(1000);
			dwTime++;
		}
	}

	if (!ScriptCommand(&is_component_available, iComponentID)) {
		return;
	}

	uint32_t dwRet;
	ScriptCommand(&add_car_component, m_gtaId, iComponentID, &dwRet);*/

	int waitPreloaded;

	// CStreaming::HasVehicleUpgradeLoaded
	bool HasVehicleUpgradeLoaded = Memory::callFunction<bool>("_ZN10CStreaming23HasVehicleUpgradeLoadedEi", iComponentID);

	if (!HasVehicleUpgradeLoaded) {
		// CStreaming::RequestVehicleUpgrade
		Memory::callFunction<void>("_ZN10CStreaming21RequestVehicleUpgradeEii", iComponentID, 10);
		pGame->LoadRequestedModels();

		while (!HasVehicleUpgradeLoaded) {
			usleep(1);
			waitPreloaded++;
			if (waitPreloaded > 200) {
				//pUI->chat()->addDebugMessage("Warning: component %u wouldn't load in time.", iComponentID);
				return;
			}
		}
	}

	uint32_t dwRetID;
	ScriptCommand(&add_car_component, m_gtaId, iComponentID, &dwRetID);
}

void CVehicle::RemoveComponent(int iComponentID)
{
	if (!m_pVehicle || !GamePool_Vehicle_GetAt(m_gtaId)) {
		return;
	}

	ScriptCommand(&remove_component, m_gtaId, iComponentID);
}

/**
 * @return
 */
CVehicle* CVehicle::GetTractor()
{
	if (!m_pVehicle) return nullptr;

	int iSubType = ::GetVehicleSubtype(m_pVehicle);
	if (iSubType == VEHICLE_SUBTYPE_BIKE
			|| iSubType == VEHICLE_SUBTYPE_BOAT
			|| iSubType == VEHICLE_SUBTYPE_TRAIN
			|| iSubType == VEHICLE_SUBTYPE_PUSHBIKE) {
		return nullptr;
	}

	sa::CVehicle* pTractor = m_pVehicle->m_pTowingVehicle;
	if (pTractor) {
		if (pNetGame) {
			CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
			if (pVehiclePool) {
				VEHICLEID TrailerID = pVehiclePool->FindIDFromGtaPtr(pTractor);
				if (TrailerID != INVALID_VEHICLE_ID) {
					return pVehiclePool->GetAt(TrailerID);
				}
			}
		}
	}

	return nullptr;
}

/**
 * @param pTrailer
 */
void CVehicle::SetTrailer(CVehicle* pTrailer)
{
	m_pTrailer = pTrailer;
}

/**
 * @return
 */
float CVehicle::GetTrainSpeed()
{
	if (m_pVehicle) {
		return ((sa::CTrain*) m_pVehicle)->LinearSpeed;
	}
	return 0.0f;
}

/**
 * @return
 */
uint16_t CVehicle::GetHydraThrusters()
{
	if (m_pVehicle) {
		return ((sa::CAutomobile*) m_pVehicle)->m_nSuspensionHydraulics;
	}

	return 0.0f;
}

/**
 * @return
 */
float CVehicle::GetBikeLean()
{
	if (m_pVehicle && GamePool_Vehicle_GetAt(m_gtaId)) {
		if (GetVehicleSubtype() == VEHICLE_SUBTYPE_BIKE) {
//			return ((sa::CBike*) m_pVehicle)->m_LeanMatrix.pos.X;
			// todo
		}
	}

	return 0.0f;
}

int CVehicle::GetVehicleSubtype()
{
	if (m_pVehicle) {
		return ::GetVehicleSubtype(m_pVehicle);
	}
	return 0;
}

/**
 * @return
 */
bool CVehicle::IsRCVehicle()
{
	if (m_pVehicle) {
		if (GamePool_Vehicle_GetAt(m_gtaId)) {
			int iModel = m_pVehicle->m_nModelIndex;
			if (iModel == 441
					|| iModel == 464
					|| iModel == 465
					|| iModel == 594
					|| iModel == 501
					|| iModel == 564) {
				return true;
			}
		}
	}

	return false;
}

/**
 * @return
 */
bool CVehicle::VerifyInstance()
{
	return GamePool_Vehicle_GetAt(m_gtaId) != nullptr;
}

void CVehicle::DetachTrailer()
{
	bInProcessDetachTrailer = true;
	if (m_pTrailer) {
		if (m_gtaId && GamePool_Vehicle_GetAt(m_gtaId)) {
			if (m_pTrailer->m_pVehicle) {
				ScriptCommand(&detach_trailer_from_cab, m_pTrailer->m_gtaId, m_gtaId);
			}
		}
	}
}

void CVehicle::SetHydraThrusters(int state)
{
	((sa::CAutomobile*) m_pVehicle)->m_nSuspensionHydraulics = state;
}

/**
 * @param iDoor
 * @param iComponent
 * @param fDoorOpenRatio
 */
void CVehicle::SetComponentOpenState(int iDoor, int iComponent, float fDoorOpenRatio)
{
	if (!m_pVehicle || GetVehicleSubtype() != VEHICLE_SUBTYPE_CAR) return;

	ScriptCommand(&open_car_door_a_bit, m_gtaId, iDoor, iComponent, fDoorOpenRatio);
}

/**
 *  todo
 * @param component
 */
void CVehicle::OpenWindow(uint8_t component)
{
	if (!m_pVehicle) return;
}

/**
 * todo
 * @param component
 */
void CVehicle::CloseWindow(uint8_t component)
{
	if (!m_pVehicle) return;
}

/**
 * @return
 */
bool CVehicle::IsDriverLocalPlayer()
{
	if (m_pVehicle && m_pVehicle->pDriver == GamePool_FindPlayerPed()) {
		return true;
	}

	return false;
}

/**
 * @param bInv
 */
void CVehicle::SetInvulnerable(bool bInv)
{
	if (m_pVehicle && GamePool_Vehicle_GetAt(m_gtaId) && !IsEntityPlaceable(m_pVehicle)) {
		if (bInv) {
			ScriptCommand(&set_car_immunities, m_gtaId, 1, 1, 1, 1, 1);
			ScriptCommand(&toggle_car_tires_vulnerable, m_gtaId, 0);
			m_bIsInvulnerable = true;
		}
		else {
			ScriptCommand(&set_car_immunities, m_gtaId, 0, 0, 0, 0, 0);
			ScriptCommand(&toggle_car_tires_vulnerable, m_gtaId, 1);
			m_bIsInvulnerable = false;
		}
	}
}

/**
 * @return
 */
bool CVehicle::HasSunk()
{
	if (m_pVehicle) {
		return ScriptCommand(&has_car_sunk, m_gtaId);
	}

	return false;
}

/**
 * @return
 */
bool CVehicle::HasADriver()
{
	if (m_pVehicle && GamePool_Vehicle_GetAt(m_gtaId)) {
		if (m_pVehicle->pDriver) {
			if (m_pVehicle->pDriver->IsInVehicle() && m_pVehicle->pDriver->m_nPedType == 0) {
				return true;
			}
		}
	}

	return false;
}

/**
 * @param fAngle
 */
void CVehicle::SetZAngle(float fAngle)
{
	if (GamePool_Vehicle_GetAt(m_gtaId)) {
		ScriptCommand(&set_car_z_angle, m_gtaId, fAngle);
	}
}

/**
 */
void CVehicle::ProcessMarkers()
{
	if (!m_pVehicle || m_pVehicle->pDriver) return;

	if (m_byteObjectiveVehicle) {
		if (!m_bSpecialMarkerEnabled) {
			if (m_dwMarkerID) {
				pGame->DisableMarker(m_dwMarkerID);
				m_dwMarkerID = 0;
			}

			ScriptCommand(&tie_marker_to_car, m_gtaId, 1, 3, &m_dwMarkerID);
			ScriptCommand(&set_marker_color, m_dwMarkerID, 1006);
			ScriptCommand(&show_on_radar, m_dwMarkerID, 3);
			m_bSpecialMarkerEnabled = true;
		}
	}
	else {
		if (m_bSpecialMarkerEnabled && m_dwMarkerID) {
			pGame->DisableMarker(m_dwMarkerID);
			m_bSpecialMarkerEnabled = false;
			m_dwMarkerID = 0;
		}

		if (GetDistanceFromLocalPlayerPed() < 200.0f && !IsOccupied()) {
			// show
			if (!m_dwMarkerID) {
				ScriptCommand(&tie_marker_to_car, m_gtaId, 1, 2, &m_dwMarkerID);
				ScriptCommand(&set_marker_color, m_dwMarkerID, 1004);
			}
		}
		else {
			// remove
			if (m_dwMarkerID) {
				pGame->DisableMarker(m_dwMarkerID);
				m_dwMarkerID = 0;
			}
		}
	}
}

/**
 * @return
 */
bool CVehicle::IsOccupied()
{
	if (m_pVehicle) {
		if (m_pVehicle &&
				(m_pVehicle->m_apPassengers[0] ||
						m_pVehicle->m_apPassengers[1] ||
						m_pVehicle->m_apPassengers[2] ||
						m_pVehicle->m_apPassengers[3] ||
						m_pVehicle->m_apPassengers[4] ||
						m_pVehicle->m_apPassengers[5] ||
						m_pVehicle->m_apPassengers[6])) {
			return true;
		}
	}

	return false;
}

/**
 * @return
 */
bool CVehicle::IsATrainPart()
{
	if (m_pVehicle) {
		int iModelIndex = m_pVehicle->m_nModelIndex;
		if (iModelIndex == 538 || iModelIndex == 570 || iModelIndex == 537 ||
				iModelIndex == 569 || iModelIndex == 449) {
			return true;
		}
	}

	return false;
}

void CVehicle::AttachTrailer()
{
	if (m_pTrailer) {
		ScriptCommand(&put_trailer_on_cab, m_pTrailer->m_gtaId, m_gtaId);
	}
}

void CVehicle::SetLightState(bool state)
{
	//m_pVehicle->SetLightsStatus(state);
	m_bLightState = state;
	if (m_pVehicle && GamePool_Vehicle_GetAt(m_gtaId)) {
		ScriptCommand(&force_car_lights, m_gtaId, state ? 2 : 1);
	}
}

/**
 * @return
 */
bool CVehicle::GetDoorState()
{
	if (m_pVehicle) {
		return m_pVehicle->m_nDoorLock == sa::CARLOCK_LOCKED;
	}

	return false;
}

/**
 * @param state
 */
void CVehicle::SetDoorState(int state)
{
	if (state) {
		m_pVehicle->m_nDoorLock = sa::CARLOCK_LOCKED;
		m_bDoorsLocked = true;
	}
	else {
		m_pVehicle->m_nDoorLock = sa::CARLOCK_NOT_USED;
		m_bDoorsLocked = false;
	}
}
