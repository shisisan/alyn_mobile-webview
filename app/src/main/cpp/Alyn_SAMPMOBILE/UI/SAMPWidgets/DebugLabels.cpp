#include "../UI.h"
#include "../../Client.h"
#include "../../game/Game.h"
#include "../../net/NetGame.h"

extern UI* pUI;
extern Game* pGame;
extern NetGame* pNetGame;

/* DebugLables */

void DebugLabels::draw(ImGuiRenderer* renderer)
{
	if (pNetGame) {
		CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
		if (pVehiclePool) {
			for (VEHICLEID x = 0; x < MAX_VEHICLES; x++) {
				CVehicle* pVehicle = pVehiclePool->GetAt(x);
				if (pVehicle) {
					if (pVehicle->GetDistanceFromLocalPlayerPed() <= 20.0f) {
						RwMatrix matVehicle = pVehicle->m_pVehicle->GetMatrix().ToRwMatrix();

						RwV3d vecPos;
						vecPos.x = matVehicle.pos.x;
						vecPos.y = matVehicle.pos.y;
						vecPos.z = matVehicle.pos.z;

						RwV3d vecOut;
						CalcScreenCoors(&vecPos, &vecOut, 0.0f, 0.0f, false, false);

						if (vecOut.z < 1.0f) {
							break;
						}

						char szTextLabel[256];
						sprintf(szTextLabel, "[ID: %d | Model: %d | Subtype: %d | Health: %.1f | Preloaded: %d]\nDistance: %.2fm\nPassengerSeats: %d\ncPos: %.3f, %.3f, %.3f\nsPos: %.3f, %.3f, %.3f",
								x, pVehicle->m_pVehicle->GetModelId(), pVehicle->GetVehicleSubtype(),
								pVehicle->GetHealth(), pVehicle->m_bPreloaded,
								pVehicle->GetDistanceFromLocalPlayerPed(), pVehicle->m_pVehicle->m_nMaxPassengers,
								matVehicle.pos.x, matVehicle.pos.y, matVehicle.pos.z,
								pVehiclePool->GetVehiclePos(x).x, pVehiclePool->GetVehiclePos(x).y, pVehiclePool->GetVehiclePos(x).z
						);

						std::stringstream ss_data(szTextLabel);
						std::string s_row;
						while (std::getline(ss_data, s_row, '\n')) {
							ImVec2 sz = renderer->calculateTextSize(s_row, UISettings::fontSize() / 2);
							renderer->drawText(ImVec2(vecOut.x - (sz.x / 2), vecOut.y), __builtin_bswap32((0x358BD4FF) | (0x000000FF)), s_row, true, UISettings::fontSize() / 2);
							vecOut.y += UISettings::fontSize() / 2;
						}
					}
				}
			}
		}
	}
}
