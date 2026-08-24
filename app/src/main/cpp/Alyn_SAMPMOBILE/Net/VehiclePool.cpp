#include "../Game/Game.h"
#include "../UI/UI.h"
#include "NetGame.h"
#include "../Client.h"

extern UI* pUI;
extern Game* pGame;
extern NetGame* pNetGame;

CVehiclePool::CVehiclePool()
{
	for (int i = 0; i < MAX_VEHICLES; i++) {
		m_pVehicles[i] = nullptr;
		m_bVehicleSlotState[i] = false;
		m_pGTAVehicles[i] = nullptr;
		m_bIsActive[i] = false;
		m_bIsWasted[i] = false;
		m_dwWastedTime[i] = 0;
	}
}

CVehiclePool::~CVehiclePool()
{
	for (int i = 0; i < MAX_VEHICLES; i++) {
		Delete(i);
	}
}

void CVehiclePool::Process()
{
	uint32_t dwThisTick = GetTickCount();

	for (VEHICLEID VehicleID = 0; VehicleID < MAX_VEHICLES; VehicleID++) {
		if (m_bVehicleSlotState[VehicleID] && m_bIsActive[VehicleID]) {
			if (m_bIsWasted[VehicleID] && (dwThisTick - m_dwWastedTime[VehicleID]) > 15000) {
				Delete(VehicleID);
			}
			else {
				CVehicle* pVehicle = m_pVehicles[VehicleID];

				if (pVehicle->GetDistanceFromLocalPlayerPed() < 200.0f) {
					if (!pVehicle->IsAdded()) {
						spdlog::info("Vehicle {} is added", VehicleID);
						pVehicle->Add();
						pVehicle->SetColor(pVehicle->m_byteColor1, pVehicle->m_byteColor2);
					}

					CVehicle* pTrailer = pVehicle->GetTrailer();
					if (pTrailer && !pTrailer->IsAdded()) {
						RwMatrix matPos = pVehicle->m_pVehicle->GetMatrix().ToRwMatrix();
						pTrailer->TeleportTo(matPos.pos.x, matPos.pos.y, matPos.pos.z);
						pTrailer->Add();
					}
				}
				else {
					if (pVehicle->IsAdded()) {
						spdlog::info("Vehicle {} is removed", VehicleID);
						pVehicle->Remove();
					}
				}

				if (pVehicle->IsDriverLocalPlayer()) {
					pVehicle->SetInvulnerable(false);
				}
				else {
					pVehicle->SetInvulnerable(true);
				}

				if (pVehicle->GetHealth() == 0.0f) {
					NotifyVehicleDeath(VehicleID);
					if (!m_bIsWasted[VehicleID]) {
						m_bIsWasted[VehicleID] = true;
						m_dwWastedTime[VehicleID] = GetTickCount();
					}
				}
				else {
					float fDistance = pVehicle->GetDistanceFromLocalPlayerPed();
					if (pVehicle->GetVehicleSubtype() != VEHICLE_SUBTYPE_BOAT && fDistance < 200.0f && pVehicle->HasSunk()) {
						NotifyVehicleDeath(VehicleID);
					}
					else {
						// UnoccupiedProcess TODO
						// ...

						if (pNetGame->m_netSet->manualVehicleEngineAndLight) {
							if (pVehicle->GetEngineState()) {
								pVehicle->SetEngineState(true);
							}
							else {
								pVehicle->SetEngineState(false);
							}

							if (pVehicle->GetLightState()) {
								pVehicle->SetLightState(true);
							}
							else {
								pVehicle->SetLightState(false);
							}
						}
						else {
							if (!pVehicle->GetEngineState()) {
								if (!pVehicle->HasADriver()) {
									pVehicle->SetEngineState(false);
								}
								else {
									pVehicle->SetEngineState(true);
								}
							}
							else if (pVehicle->GetEngineState()) {
								pVehicle->SetEngineState(true);
							}
							else {
								pVehicle->SetEngineState(false);
							}

							if (!pVehicle->GetLightState()) {
								pVehicle->SetLightState(false);
							}
							else if (pVehicle->GetLightState()) {
								pVehicle->SetLightState(true);
							}
						}

						if (pVehicle->m_pVehicle != m_pGTAVehicles[VehicleID]) {
							m_pGTAVehicles[VehicleID] = pVehicle->m_pVehicle;
						}

						//ProcessColors();
						pVehicle->ProcessMarkers();
					}
				}
			}
		}
	}
}

bool CVehiclePool::New(NEW_VEHICLE* new_veh)
{
	if (m_pVehicles[new_veh->VehicleID]) {
		if (pUI) pUI->chat()->addDebugMessage("Warning: vehicle %u was not deleted", new_veh->VehicleID);
		Delete(new_veh->VehicleID);
	}

	CVehicle* pNewVehicle = pGame->NewVehicle(
			new_veh->iVehicleType,
			new_veh->vecPos.x,
			new_veh->vecPos.y,
			new_veh->vecPos.z,
			new_veh->fRotation,
			new_veh->byteAddSiren);

	if (!pNewVehicle) return false;

	m_pVehicles[new_veh->VehicleID] = pNewVehicle;

	if (new_veh->byteInteriorColor1 != 0xFF || new_veh->byteInreriorColor2 != 0xFF) {
		pNewVehicle->SetColor(new_veh->byteInteriorColor1, new_veh->byteInreriorColor2);
	}

	pNewVehicle->SetHealth(new_veh->fHealth);
	m_pGTAVehicles[new_veh->VehicleID] = pNewVehicle->m_pVehicle;
	m_bVehicleSlotState[new_veh->VehicleID] = true;

	m_vecPos[new_veh->VehicleID].x = new_veh->vecPos.x;
	m_vecPos[new_veh->VehicleID].y = new_veh->vecPos.y;
	m_vecPos[new_veh->VehicleID].z = new_veh->vecPos.z;

	if (new_veh->byteInterior) {
		pNewVehicle->LinkToInterior(new_veh->byteInterior);
	}

	if (new_veh->dwPanelDamageStatus || new_veh->dwDoorDamageStatus || new_veh->byteLightDamageStatus) {
		pNewVehicle->SetDamageStatus(new_veh->dwPanelDamageStatus, new_veh->dwDoorDamageStatus, new_veh->byteLightDamageStatus);
	}

	pNewVehicle->SetTireDamageStatus(new_veh->byteTireDamageStatus);

	m_bIsActive[new_veh->VehicleID] = true;
	m_bIsWasted[new_veh->VehicleID] = false;
	m_dwWastedTime[new_veh->VehicleID] = 0;

	m_pVehicles[new_veh->VehicleID]->SetModelIndex(new_veh->iVehicleType);

	//CountVehicles();

	return true;
}

bool CVehiclePool::Delete(VEHICLEID VehicleID)
{
	CVehicle* pVehicle = GetAt(VehicleID);
	if (!pVehicle) return false;

	// pVehicle->sub_100B7950()

	delete m_pVehicles[VehicleID];
	m_pVehicles[VehicleID] = nullptr;

	m_bVehicleSlotState[VehicleID] = false;
	m_pVehicles[VehicleID] = nullptr;
	m_pGTAVehicles[VehicleID] = nullptr;

	//CountVehicles();

	return true;
}

uint32_t CVehiclePool::FindGtaIDFromID(VEHICLEID VehicleID)
{
	if (m_pGTAVehicles[VehicleID]) {
		return GamePool_Vehicle_GetIndex(m_pGTAVehicles[VehicleID]);
	}

	return INVALID_VEHICLE_ID;
}

/**
 * @param VehicleID
 */
void CVehiclePool::NotifyVehicleDeath(VEHICLEID VehicleID)
{
	if (pNetGame->GetPlayerPool()->GetLocalPlayer()->m_LastVehicle == VehicleID) {
		RakNet::BitStream bsVehicleDeath;
		bsVehicleDeath.Write(VehicleID);
		pNetGame->GetRakClient()->RPC(&RPC_VehicleDestroyed, &bsVehicleDeath, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
	}
}

/**
 * @return
 */
VEHICLEID CVehiclePool::FindNearestToLocalPlayerPed()
{
	float fLeastDistance = 10000.0f;
	float fThisDistance;
	VEHICLEID ClosestSoFar = INVALID_VEHICLE_ID;

	VEHICLEID x = 0;
	while (x < MAX_VEHICLES) {
		if (m_bVehicleSlotState[x] && m_bIsActive[x]) {
			fThisDistance = m_pVehicles[x]->GetDistanceFromLocalPlayerPed();
			if (fThisDistance < fLeastDistance) {
				fLeastDistance = fThisDistance;
				ClosestSoFar = x;
			}
		}
		x++;
	}

	return ClosestSoFar;
}

/**
 * @param VehicleID
 * @param byteObjective
 * @param byteDoorsLocked
 */
void CVehiclePool::AssignSpecialParamsToVehicle(VEHICLEID VehicleID, uint8_t byteObjective, uint8_t byteDoorsLocked)
{
	if (VehicleID < MAX_VEHICLES && m_bVehicleSlotState[VehicleID]) {
		CVehicle* pVehicle = m_pVehicles[VehicleID];
		if (pVehicle && m_bIsActive[VehicleID]) {
			if (byteObjective) {
				pVehicle->m_byteObjectiveVehicle = true;
				pVehicle->m_bSpecialMarkerEnabled = false;
			}

			pVehicle->SetDoorState(byteDoorsLocked);
		}
	}
}

void CVehiclePool::LinkToInterior(VEHICLEID VehicleID, uint8_t byteInterior)
{
	if (m_bVehicleSlotState[VehicleID] && m_pVehicles[VehicleID]) {
		m_pVehicles[VehicleID]->LinkToInterior(byteInterior);
	}
}
