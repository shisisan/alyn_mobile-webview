#include "../Game/Game.h"
#include "../UI/UI.h"
#include "NetGame.h"
#include "../Client.h"

extern Game* pGame;
extern NetGame* pNetGame;

CTextLabelPool::CTextLabelPool()
{
	for (int i = 0; i < MAX_TEXT_LABELS; i++) {
		strcpy(m_TextLabels[i].text, "");
		m_TextLabels[i].dwColor = 0;
		m_TextLabels[i].vecPos.x = 0.0f;
		m_TextLabels[i].vecPos.y = 0.0f;
		m_TextLabels[i].vecPos.z = 0.0f;
		m_TextLabels[i].fDistance = 0.0f;
		m_TextLabels[i].bTestLOS = false;
		m_TextLabels[i].playerId = 0;
		m_TextLabels[i].vehicleId = 0;

		m_bSlotUsed[i] = false;
	}
}

CTextLabelPool::~CTextLabelPool()
{
	for (int i = 0; i < MAX_TEXT_LABELS; i++) {
		if (m_bSlotUsed[i]) {
			ClearLabel(i);
		}
	}
}

/**
 * @param wLabelId
 * @param pLabel
 */
void CTextLabelPool::NewLabel(uint16_t wLabelId, TEXT_LABEL* pLabel)
{

	if (wLabelId < MAX_TEXT_LABELS) {
		if (m_bSlotUsed[wLabelId]) {
			TEXT_LABEL* pTextLabel = &m_TextLabels[wLabelId];
			strcpy(pTextLabel->text, "");
			pTextLabel->dwColor = 0;
			pTextLabel->vecPos.x = 0.0f;
			pTextLabel->vecPos.y = 0.0f;
			pTextLabel->vecPos.z = 0.0f;
			pTextLabel->fDistance = 0.0f;
			pTextLabel->bTestLOS = false;
			pTextLabel->playerId = 0;
			pTextLabel->vehicleId = 0;
		}

		TEXT_LABEL* pTextLabel = &m_TextLabels[wLabelId];
		strcpy(pTextLabel->text, Encoding::cp2utf(pLabel->text).c_str());

		pTextLabel->dwColor = pLabel->dwColor;
		pTextLabel->vecPos.x = pLabel->vecPos.x;
		pTextLabel->vecPos.y = pLabel->vecPos.y;
		pTextLabel->vecPos.z = pLabel->vecPos.z;
		pTextLabel->fDistance = pLabel->fDistance;
		pTextLabel->bTestLOS = pLabel->bTestLOS;
		pTextLabel->playerId = pLabel->playerId;
		pTextLabel->vehicleId = pLabel->vehicleId;

		if (pLabel->vehicleId != INVALID_VEHICLE_ID || pLabel->playerId != INVALID_PLAYER_ID) {
			pTextLabel->vecOffsetCoords = pLabel->vecPos;
		}

		m_bSlotUsed[wLabelId] = true;
	}
}

/**
 * @param wLabelId
 */
void CTextLabelPool::ClearLabel(uint16_t wLabelId)
{
	if (wLabelId < MAX_TEXT_LABELS && m_bSlotUsed[wLabelId]) {

		TEXT_LABEL* label = &m_TextLabels[wLabelId];
		strcpy(label->text, "");
		label->dwColor = 0;
		label->vecPos.x = 0.0f;
		label->vecPos.y = 0.0f;
		label->vecPos.z = 0.0f;
		label->fDistance = 0.0f;
		label->bTestLOS = false;
		label->playerId = 0;
		label->vehicleId = 0;

		m_bSlotUsed[wLabelId] = false;
	}
}

/**
 * @param renderer
 */
void CTextLabelPool::Render(ImGuiRenderer* renderer)
{
	CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
	if (pPlayerPed && pNetGame) {
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();

		for (int i = 0; i < MAX_TEXT_LABELS; i++) {
			if (m_bSlotUsed[i]) {
				TEXT_LABEL* pTextLabel = &m_TextLabels[i];

				sa::CVector textPos;
				if (pTextLabel->playerId != INVALID_PLAYER_ID) {
					if (pTextLabel->playerId == pPlayerPool->GetLocalPlayerID()) {
						continue;
					}

					if (pPlayerPool && pPlayerPool->GetSlotState(pTextLabel->playerId)) {
						CRemotePlayer* pPlayer = pPlayerPool->GetAt(pTextLabel->playerId);
						if (!pPlayer) continue;

						CPlayerPed* pPlayerPed = pPlayer->GetPlayerPed();
						if (!pPlayerPed) continue;

						sa::CVector matPlayer;
						pPlayerPed->m_ped->GetBonePosition(&matPlayer, 8, false);

						textPos.x = matPlayer.x + pTextLabel->vecOffsetCoords.x;
						textPos.y = matPlayer.y + pTextLabel->vecOffsetCoords.y;
						textPos.z = matPlayer.z + 0.23 + pTextLabel->vecOffsetCoords.z;
					}
				}
				else if (pTextLabel->vehicleId != INVALID_VEHICLE_ID) {
					if (pVehiclePool && pVehiclePool->GetSlotState(pTextLabel->vehicleId)) {
						CVehicle* pVehicle = pVehiclePool->GetAt(pTextLabel->vehicleId);
						if (!pVehicle) continue;

						RwMatrix matVehicle = pVehicle->m_pVehicle->GetMatrix().ToRwMatrix();

						textPos.x = matVehicle.pos.x + pTextLabel->vecOffsetCoords.x;
						textPos.y = matVehicle.pos.y + pTextLabel->vecOffsetCoords.y;
						textPos.z = matVehicle.pos.z + pTextLabel->vecOffsetCoords.z;
					}
				}
				else {
					textPos.x = pTextLabel->vecPos.x;
					textPos.y = pTextLabel->vecPos.y;
					textPos.z = pTextLabel->vecPos.z;
				}

				int hitEntity = 0;

				CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
				if (!pPlayerPed) continue;

				if (pTextLabel->bTestLOS) {
					CAMERA_AIM* pCam = GameGetInternalAim();
					RwMatrix mat;
					sa::CVector playerPosition;

					mat = pPlayerPed->m_ped->GetMatrix().ToRwMatrix();

					playerPosition.x = mat.pos.x;
					playerPosition.y = mat.pos.y;
					playerPosition.z = mat.pos.z;

					if (pTextLabel->bTestLOS) {
						hitEntity = ScriptCommand(&get_line_of_sight, playerPosition.x, playerPosition.y, playerPosition.z, pCam->pos1x, pCam->pos1y, pCam->pos1z, 1, 0, 0, 0, 0);
					}
				}

				pTextLabel->vecPos.x = textPos.x;
				pTextLabel->vecPos.y = textPos.y;
				pTextLabel->vecPos.z = textPos.z;

				if (!pTextLabel->bTestLOS || hitEntity) {
					if (pPlayerPed->GetDistanceFromPoint(pTextLabel->vecPos) > pTextLabel->fDistance) continue;
					Draw(renderer, textPos, pTextLabel->text, pTextLabel->dwColor);
				}
			}
		}
	}
}

/**
 * @param renderer
 * @param vecPos
 * @param text
 * @param dwColor
 */
void CTextLabelPool::Draw(ImGuiRenderer* renderer, sa::CVector vecPos, const char* text, uint32_t dwColor)
{
	RwV3d vPos = vecPos;
	RwV3d vecOut;

	CalcScreenCoors(&vPos, &vecOut, 0.0f, 0.0f, false, false);
	if (vecOut.z < 1.0f) return;

	std::stringstream ss_data(text);
	std::string s_row;
	while (std::getline(ss_data, s_row, '\n')) {
		ImVec2 sz = renderer->calculateTextSize(s_row, UISettings::fontSize() / 2);
		renderer->drawText(ImVec2(vecOut.x - (sz.x / 2), vecOut.y), __builtin_bswap32(dwColor | (0x000000FF)), s_row, true, UISettings::fontSize() / 2);
		vecOut.y += UISettings::fontSize() / 2;
	}
}
