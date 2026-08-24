#include "../Game/Game.h"
#include "NetGame.h"
#include "../UI/UI.h"

extern NetGame* pNetGame;

CPlayerBubblePool::CPlayerBubblePool()
{
	for (int i = 0; i < MAX_PLAYERS; i++) {
		m_pPlayerBubble[i] = 0;
		m_bSlotState[i] = false;
	}
}

CPlayerBubblePool::~CPlayerBubblePool()
{
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (m_pPlayerBubble[i] && m_bSlotState[i]) {
			delete m_pPlayerBubble[i];
		}

		m_pPlayerBubble[i] = 0;
		m_bSlotState[i] = false;
	}
}

PlayerBubbleStruct* CPlayerBubblePool::New(PLAYERID playerId, const char* text, uint32_t color, float distance, uint32_t time)
{
	if (m_bSlotState[playerId] && m_pPlayerBubble[playerId]) {
		delete m_pPlayerBubble[playerId];
	}

	m_pPlayerBubble[playerId] = new PlayerBubbleStruct;
	const char* GText = Encoding::cp2utf(text).c_str();
	strcpy(m_pPlayerBubble[playerId]->szText, GText);

	m_pPlayerBubble[playerId]->fDistance = distance;
	m_pPlayerBubble[playerId]->uiExpireTime = GetTickCount() + time;
	m_pPlayerBubble[playerId]->uiColor = color;
	m_bSlotState[playerId] = true;

	return m_pPlayerBubble[playerId];
}

void CPlayerBubblePool::Delete(PLAYERID playerId)
{
	if (m_bSlotState[playerId] && m_pPlayerBubble[playerId]) {
		delete m_pPlayerBubble[playerId];
		m_bSlotState[playerId] = false;
	}
}

void CPlayerBubblePool::Render(ImGuiRenderer* renderer)
{
	for (PLAYERID i = 0; i < MAX_PLAYERS; i++) {
		if (!m_pPlayerBubble[i]->szText || !strlen(m_pPlayerBubble[i]->szText)) {
			continue;
		}

		if (m_bSlotState[i] && m_pPlayerBubble[i]) {
			if (GetTickCount() >= m_pPlayerBubble[i]->uiExpireTime) {
				Delete(i);
				continue;
			}

			CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
			if (pPlayerPool->GetSlotState(i)) {
				CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(i);
				if (pRemotePlayer && pRemotePlayer->IsActive()) {
					CPlayerPed* pPlayerPed = pRemotePlayer->GetPlayerPed();
					if (pPlayerPed && pPlayerPed->IsAdded()) {
						if (pPlayerPed->GetDistanceFromCamera() <= m_pPlayerBubble[i]->fDistance) {
							sa::CVector VecPos;
							VecPos.x = 0.0f;
							VecPos.y = 0.0f;
							VecPos.z = 0.0f;

							pPlayerPed->m_ped->GetBonePosition(&VecPos, 8, false);

							RwV3d TagPos;
							TagPos.x = VecPos.x;
							TagPos.y = VecPos.y;
							TagPos.z = VecPos.z;

							TagPos.z += 0.25f + (pPlayerPed->GetDistanceFromCamera() * 0.0475f);

							RwV3d Out;

							CalcScreenCoors(&TagPos, &Out, 0.0f, 0.0f, false, false);

							if (Out.z < 1.0f) {
								return;
							}

							ImVec2 pos = ImVec2(Out.x, Out.y - (UISettings::fontSize() * 1.0));
							pos.x -= renderer->calculateTextSize(m_pPlayerBubble[i]->szText, UISettings::fontSize() / 2).x / 2;
							renderer->drawText(pos, __builtin_bswap32(m_pPlayerBubble[i]->uiColor | (0x000000FF)), m_pPlayerBubble[i]->szText, true, UISettings::fontSize() / 2);
						}
					}
				}
			}
		}
	}
}