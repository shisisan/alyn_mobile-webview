//
// Created by ALYN on 18-Feb-25.
//

#include "Game.h"
#include "Pickups.h"
#include "../Net/NetGame.h"

extern NetGame* pNetGame;

DECL_HOOK(void, CPickups_Update)
{
	static std::array<sa::CPickup, sa::MAX_NUM_PICKUPS>& aPickUps = *(std::array<sa::CPickup, sa::MAX_NUM_PICKUPS>*) (g_saSym->Abs(VER_x32 ? 0x7AFD70 : 0x991AB8));

	auto start = 620 * (*g_saSym->GetSymbol<uint32_t*>("_ZN6CTimer14m_FrameCounterE") % 32) / 32;
	auto end = 620 * (*g_saSym->GetSymbol<uint32_t*>("_ZN6CTimer14m_FrameCounterE") % 32 + 1) / 32;
	for (auto i = start; i < end; i++) {
		auto& pickup = aPickUps[i];
		if (pickup.m_nPickupType == sa::CPickup::PICKUP_NONE) {
			continue;
		}

		if (pickup.m_nFlags.bVisible = pickup.IsVisible()) {
			if (!pickup.m_nFlags.bDisabled && !pickup.m_pObject) {
				pickup.GiveUsAPickUpObject(&pickup.m_pObject, -1);

				if (auto& obj = pickup.m_pObject; obj) {
					// CWorld::Add(obj);
					Memory::callFunction("_ZN6CWorld3AddEP7CEntity", obj);
				}
			}
			if (pickup.Update()) {
				CPickupPool* pPickups = pNetGame->GetPickupPool();
				pPickups->PickedUp(i);
			}
		}
		else {
			pickup.GetRidOfObjects();
		}
	}
}

void Pickups::installHooks()
{
	spdlog::info("Installing pickup hooks...");
	HOOK("_ZN8CPickups6UpdateEv", CPickups_Update);
}

bool sa::CPickup::Update()
{
	if (!pNetGame || !pNetGame->GetPlayerPool()) {
		return false;
	}

	auto ped = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->m_ped;
	auto oldState = m_bIsPicked;

	auto checkPickup = [&]() {
		if (m_nPickupType == PICKUP_2P) {
			if (!ped->IsInVehicle()) {
				return false;
			}

			if (DistanceBetweenPoints(GetPosn(), ped->pVehicle->GetPosition()) < 1.0f) {
				Remove();
				return true;
			}
			else {
				return false;
			}
		}
			// todo check if its jetpack and return pos < 5.0f maybe?

		else {
			return DistanceBetweenPoints(GetPosn(), ped->GetPosition()) < 0.9f;
		}
	};

	m_bIsPicked = checkPickup();

	if (oldState == false && m_bIsPicked == true) {
		return true;
	}

	return false;
}
