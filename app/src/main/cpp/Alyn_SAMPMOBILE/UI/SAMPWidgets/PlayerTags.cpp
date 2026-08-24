#include "../UI.h"
#include "../../Game/Game.h"
#include "../../Net/NetGame.h"

extern Game* pGame;
extern NetGame* pNetGame;

PlayerTags::PlayerTags()
{
	spdlog::info("Loading AFK icon..");
	m_afkTexture = LoadTextureFromTxd("samp", "afk_icon");
}

void PlayerTags::Render(ImGuiRenderer* renderer)
{
	sa::CVector vecPos;
	RwMatrix matLocal, matPlayer;
	int hitEntity = 0;
	char szNickBuf[64];

	if (pNetGame && pNetGame->m_netSet->showNameTags) {
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		matLocal = pGame->FindPlayerPed()->m_ped->GetMatrix().ToRwMatrix();

		for (PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++) {
			if (pPlayerPool->GetSlotState(playerId)) {
				CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(playerId);

				if (pRemotePlayer && pRemotePlayer->IsActive() && pRemotePlayer->m_bShowNameTag) {
					CPlayerPed* pPlayerPed = pRemotePlayer->GetPlayerPed();

					if (pPlayerPed->GetDistanceFromCamera() <= pNetGame->m_netSet->nameTagDrawDistance) {
						if (pRemotePlayer->GetState() == PLAYER_STATE_DRIVER && pRemotePlayer->m_pCurrentVehicle && pRemotePlayer->m_pCurrentVehicle->IsRCVehicle()) {
							matPlayer = pRemotePlayer->m_pCurrentVehicle->m_pVehicle->GetMatrix().ToRwMatrix();
							vecPos.x = matPlayer.pos.x;
							vecPos.y = matPlayer.pos.y;
							vecPos.z = matPlayer.pos.z;
						}
						else {
							if (!pPlayerPed->IsAdded()) continue;
							vecPos.x = 0.0f;
							vecPos.y = 0.0f;
							vecPos.z = 0.0f;
							pPlayerPed->m_ped->GetBonePosition(&vecPos, 8, false);
						}

						CAMERA_AIM* pCam = GameGetInternalAim();

						if (pNetGame->m_netSet->nameTagLOS) {
							hitEntity = ScriptCommand(&get_line_of_sight, vecPos.x, vecPos.y, vecPos.z, pCam->pos1x, pCam->pos1y, pCam->pos1z, 1, 0, 0, 1, 0);
						}

						if (!pNetGame->m_netSet->nameTagLOS || hitEntity && !pRemotePlayer->IsNPC()) {
							sprintf(szNickBuf, "%s (%d)", Encoding::cp2utf(pPlayerPool->GetPlayerName(playerId)).c_str(), playerId);
							drawTag(renderer, &vecPos, szNickBuf, pRemotePlayer->GetPlayerColor(), pPlayerPed->GetDistanceFromCamera(), pRemotePlayer->m_fReportedHealth, pRemotePlayer->m_fReportedArmour, pRemotePlayer->m_bIsAFK);
						}
					}
				}
			}
		}
	}
}

void PlayerTags::drawTag(ImGuiRenderer* renderer, RwV3d* vec, const char* szNick, uint32_t dwColor, float fDist, float fHealth, float fArmour, bool bAfk)
{
	RwV3d vecTagPos = *vec;
	vecTagPos.z += 0.3f + (fDist * 0.0475f);

	RwV3d vecOut;
	CalcScreenCoors(&vecTagPos, &vecOut, 0.0f, 0.0f, false, false);
	if (vecOut.z < 1.0f) return;

	// name (id)
	ImVec2 pos = ImVec2(vecOut.x, vecOut.y);
	pos.x -= renderer->calculateTextSize(szNick, UISettings::fontSize() / 2).x / 2;
	renderer->drawText(pos, __builtin_bswap32(dwColor | (0x000000FF)), szNick, true, UISettings::fontSize() / 2);

	// Health bar
	if (fHealth < 0.0f) return;
	vecOut.x = (float) ((int) vecOut.x);
	vecOut.y = (float) ((int) vecOut.y);

	ImColor HealthBarBDRColor = ImColor(0x00, 0x00, 0x00, 0xFF);
	ImColor HealthBarColor = ImColor(0xB9, 0x22, 0x28, 0xFF);
	ImColor HealthBarBGColor = ImColor(0x4B, 0x0B, 0x14, 0xFF);

	float fOutline = UISettings::outlineSize();

	ImVec2 HealthBarBDR1;
	ImVec2 HealthBarBDR2;
	ImVec2 HealthBarBG1;
	ImVec2 HealthBarBG2;
	ImVec2 HealthBar1;
	ImVec2 HealthBar2;

	// top left
	HealthBarBDR1.x = vecOut.x - ((UISettings::nametagBarSize().x / 2) + fOutline);
	HealthBarBDR1.y = vecOut.y + ((UISettings::fontSize() / 2) * 1.2f);

	// bottom right
	HealthBarBDR2.x = vecOut.x + ((UISettings::nametagBarSize().x / 2) + fOutline);
	HealthBarBDR2.y = vecOut.y + ((UISettings::fontSize() / 2) * 1.2f) + UISettings::nametagBarSize().y;

	// top left
	HealthBarBG1.x = HealthBarBDR1.x + fOutline;
	HealthBarBG1.y = HealthBarBDR1.y + fOutline;

	// bottom right
	HealthBarBG2.x = HealthBarBDR2.x - fOutline;
	HealthBarBG2.y = HealthBarBDR2.y - fOutline;

	// top left
	HealthBar1.x = HealthBarBG1.x;
	HealthBar1.y = HealthBarBG1.y;

	// bottom right
	HealthBar2.y = HealthBarBG2.y;

	if (fHealth > 100.0f) {
		fHealth = 100.0f;
	}

	fHealth *= UISettings::nametagBarSize().x / 100.0f;
	fHealth -= (UISettings::nametagBarSize().x / 2);
	HealthBar2.x = vecOut.x + fHealth;

	float offsetY = 13.0f;

	if (fArmour > 0.0f) {
		HealthBarBDR1.y += offsetY;
		HealthBarBDR2.y += offsetY;
		HealthBarBG1.y += offsetY;
		HealthBarBG2.y += offsetY;
		HealthBar1.y += offsetY;
		HealthBar2.y += offsetY;
	}

	renderer->drawRect(HealthBarBDR1, HealthBarBDR2, HealthBarBDRColor, true);
	renderer->drawRect(HealthBarBG1, HealthBarBG2, HealthBarBGColor, true);
	renderer->drawRect(HealthBar1, HealthBar2, HealthBarColor, true);

	// Armour Bar
	if (fArmour > 0.0f) {
		HealthBarBDR1.y -= offsetY;
		HealthBarBDR2.y -= offsetY;
		HealthBarBG1.y -= offsetY;
		HealthBarBG2.y -= offsetY;
		HealthBar1.y -= offsetY;
		HealthBar2.y -= offsetY;

		HealthBarColor = ImColor(200, 200, 200, 255);
		HealthBarBGColor = ImColor(40, 40, 40, 255);

		if (fArmour > 100.0f) {
			fArmour = 100.0f;
		}

		fArmour *= UISettings::nametagBarSize().x / 100.0f;
		fArmour -= (UISettings::nametagBarSize().x / 2);
		HealthBar2.x = vecOut.x + fArmour;

		renderer->drawRect(HealthBarBDR1, HealthBarBDR2, HealthBarBDRColor, true);
		renderer->drawRect(HealthBarBG1, HealthBarBG2, HealthBarBGColor, true);
		renderer->drawRect(HealthBar1, HealthBar2, HealthBarColor, true);
	}

	// AFK icon
	if (bAfk) {
		ImVec2 a = ImVec2(HealthBarBDR1.x - ((UISettings::fontSize() / 2) * 1.4f), HealthBarBDR1.y);
		ImVec2 b = ImVec2(a.x + ((UISettings::fontSize() / 2) * 1.3f), a.y + ((UISettings::fontSize() / 2) * 1.3f));
		renderer->drawImage(a, b, m_afkTexture->raster);
	}
}