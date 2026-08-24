#include "../Client.h"
#include "Game.h"
#include "../Net/NetGame.h"

extern Game* pGame;
extern NetGame* pNetGame;

#define EVENT_TYPE_PAINTJOB            1
#define EVENT_TYPE_CARCOMPONENT        2
#define EVENT_TYPE_CARCOLOR            3
#define EVENT_ENTEREXIT_MODSHOP        4

extern bool bFirstSpawn;

void ProcessIncommingEvent(PLAYERID playerId, int iEventType, uint32_t dwParam1, uint32_t dwParam2, uint32_t dwParam3)
{
	if (pNetGame == nullptr) return;
	if (bFirstSpawn) return;

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();

	CVehicle* pVehicle = nullptr;
	CRemotePlayer* pPlayer = nullptr;

	switch (iEventType) {
		case EVENT_TYPE_PAINTJOB: pVehicle = pVehiclePool->GetAt(dwParam1);
			if (pVehicle) {
				pVehicle->SetPaintJob(dwParam2);
			}
			break;

		case EVENT_TYPE_CARCOMPONENT: pVehicle = pVehiclePool->GetAt(dwParam1);
			if (pVehicle) {
				pVehicle->AddComponent(dwParam2);
			}
			break;

		case EVENT_TYPE_CARCOLOR: pVehicle = pVehiclePool->GetAt(dwParam1);
			if (pVehicle) {
				pVehicle->SetColor(dwParam2, dwParam3);
			}
			break;

		case EVENT_ENTEREXIT_MODSHOP: if (playerId > MAX_PLAYERS) return;

			pVehicle = pVehiclePool->GetAt(dwParam1);
			pPlayer = pPlayerPool->GetAt(playerId);

			if (pPlayer && pVehicle) {
				CPlayerPed* pPlayerPed = pPlayer->GetPlayerPed();
				CPlayerPed* pLocalPlayerPed = pPlayerPool->GetLocalPlayer()->GetPlayerPed();

				if (pPlayerPed && pLocalPlayerPed) {
					if (pPlayerPed->IsInVehicle()) {
						if (pLocalPlayerPed->IsInVehicle()) {
							if (!pPlayerPed->IsAPassenger()) {
								if (pLocalPlayerPed->IsAPassenger()) {
									VEHICLEID VehicleID = pPlayerPed->GetCurrentVehicleID();
									if (VehicleID == pLocalPlayerPed->GetCurrentVehicleID()) {
										CVehicle* pCurrentVehicle = pPlayer->m_pCurrentVehicle;

										if (dwParam2) {
											if (pCurrentVehicle) {
												pCurrentVehicle->LinkToInterior(dwParam3);
											}
											pLocalPlayerPed->SetInterior(dwParam3, false);
											pLocalPlayerPed->TogglePlayerControllable(false);
										}
										else {
											if (pCurrentVehicle) {
												pCurrentVehicle->LinkToInterior(0);
											}
											pLocalPlayerPed->SetInterior(0, false);
											pLocalPlayerPed->TogglePlayerControllable(true);
										}
									}
								}
							}
						}
					}
				}
			}
			break;
	}
}