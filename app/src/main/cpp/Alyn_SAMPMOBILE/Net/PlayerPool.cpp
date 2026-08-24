#include "../Game/Game.h"
#include "NetGame.h"

CPlayerPool::CPlayerPool()
{
	for (PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++) {
		m_bPlayerSlotState[playerId] = false;
	}

	m_pLocalPlayer = new CLocalPlayer();
	m_iLocalPlayerScore = 0;
	m_iLocalPlayerPing = 0;
	m_LocalPlayerID = 0;
	m_iLastPlayerID = 0;
}

CPlayerPool::~CPlayerPool()
{
	if (m_pLocalPlayer) {
		delete m_pLocalPlayer;
		m_pLocalPlayer = nullptr;
	}

	for (PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++) {
		Delete(playerId, 0);
	}
}

void CPlayerPool::Process()
{
	for (PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++) {
		if (m_bPlayerSlotState[playerId]) {
			GetAt(playerId)->Process();
		}
	}

	if (m_pLocalPlayer) {
		m_pLocalPlayer->Process();
	}
}

bool CPlayerPool::New(PLAYERID playerId, const char* szPlayerName, bool bIsNPC)
{
	CPlayer* pPlayer = new CPlayer(szPlayerName, bIsNPC);

	if (playerId > MAX_PLAYERS) {
		return false;
	}

	m_pPlayers[playerId] = pPlayer;
	if (!pPlayer) {
		return false;
	}

	pPlayer->m_pRemotePlayer->SetID(playerId);

	if (bIsNPC) {
		pPlayer->m_pRemotePlayer->SetNPC();
	}

	m_bPlayerSlotState[playerId] = true;

	FindLastPlayerID();

	return true;
}

bool CPlayerPool::Delete(PLAYERID playerId, uint8_t byteReason)
{
	if (playerId >= MAX_PLAYERS || !m_bPlayerSlotState[playerId] || !m_pPlayers[playerId]) {
		return false;
	}

	if (m_pLocalPlayer && m_pLocalPlayer->IsSpectating() && m_pLocalPlayer->m_SpectateID == playerId) {
		m_pLocalPlayer->ToggleSpectating(false);
	}
	m_bPlayerSlotState[playerId] = false;

	if (m_pPlayers[playerId]) {
		delete m_pPlayers[playerId];
	}
	m_pPlayers[playerId] = nullptr;
	FindLastPlayerID();

	return true;
}

void CPlayerPool::FindLastPlayerID()
{
	int iLastPlayerID = 0;

	for (PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++) {
		if (m_bPlayerSlotState[playerId]) {
			iLastPlayerID = playerId;
		}
	}

	m_iLastPlayerID = iLastPlayerID;
}

char* CPlayerPool::GetPlayerName(PLAYERID playerId)
{
	if (playerId == m_LocalPlayerID) {
		return m_szLocalPlayerName;
	}
	else {
		if (playerId <= MAX_PLAYERS && m_pPlayers[playerId] != nullptr) {
			return m_pPlayers[playerId]->m_szPlayerName;
		}
	}

	return nullptr;
}

int CPlayerPool::GetPlayerScore(PLAYERID playerId)
{
	if (playerId > MAX_PLAYERS) {
		return false;
	}

	CPlayer* pPlayer = m_pPlayers[playerId];
	if (pPlayer) {
		return pPlayer->m_iScore;
	}
}

int CPlayerPool::GetPlayerPing(PLAYERID playerId)
{
	if (playerId > MAX_PLAYERS) {
		return false;
	}

	CPlayer* pPlayer = m_pPlayers[playerId];
	if (pPlayer) {
		return pPlayer->m_iPing;
	}
}

bool CPlayerPool::IsPlayerNPC(PLAYERID playerId)
{
	if (playerId > MAX_PLAYERS) {
		return false;
	}

	CPlayer* pPlayer = m_pPlayers[playerId];
	if (pPlayer) {
		return pPlayer->m_bIsNPC;
	}

	return false;
}

int CPlayerPool::GetTotalPlayers()
{
	int iTotal = 1;

	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (GetSlotState(i)) iTotal++;
	}

	return iTotal;
}

void CPlayerPool::UpdatePlayerScore(PLAYERID playerId, int iScore)
{
	if (playerId == m_LocalPlayerID) {
		m_iLocalPlayerScore = iScore;
	}
	else {
		if (playerId < MAX_PLAYERS) {
			CPlayer* pPlayer = m_pPlayers[playerId];
			if (pPlayer) {
				pPlayer->m_iScore = iScore;
			}
		}
	}
}

void CPlayerPool::UpdatePlayerPing(PLAYERID playerId, int iPing)
{
	if (playerId == m_LocalPlayerID) {
		m_iLocalPlayerPing = iPing;
	}
	else {
		if (playerId < MAX_PLAYERS) {
			CPlayer* pPlayer = m_pPlayers[playerId];
			if (pPlayer) {
				pPlayer->m_iPing = iPing;
			}
		}
	}
}

PLAYERID CPlayerPool::FindRemotePlayerIDFromGtaPtr(sa::CPed* pActor)
{
	for (int i = 0; i < MAX_PLAYERS; i++) {
		CRemotePlayer* pRemotePlayer = GetAt(i);
		if (pRemotePlayer) {
			CPlayerPed* pPlayerPed = pRemotePlayer->GetPlayerPed();
			if (pPlayerPed) {
				if (pPlayerPed->m_ped) {
					if (pPlayerPed->m_ped == pActor) {
						return pRemotePlayer->GetID();
					}
				}
			}
		}
	}

	return INVALID_PLAYER_ID;
}

void CPlayerPool::DeactivateAll()
{
	m_pLocalPlayer->m_bIsActive = false;
	m_pLocalPlayer->m_iSelectedClass = 0;

	for (PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++) {
		CRemotePlayer* pRemotePlayer = GetAt(playerId);
		if (pRemotePlayer) {
			pRemotePlayer->SetState(PLAYER_STATE_NONE);
			pRemotePlayer->m_bShowNameTag = true;
		}
	}
}

void CPlayerPool::ProcessAttachedObjects()
{
	if (m_pLocalPlayer) {
		CPlayerPed* pPlayerPed = m_pLocalPlayer->GetPlayerPed();
		if (pPlayerPed) {
			pPlayerPed->ProcessCuff();
			pPlayerPed->ProcessAttachedObjects();
		}
	}

	for (PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++) {
		CRemotePlayer* pRemotePlayer = GetAt(playerId);
		if (pRemotePlayer) {
			CPlayerPed* pPlayerPed = pRemotePlayer->GetPlayerPed();
			if (pPlayerPed) {
				pPlayerPed->ProcessCuff();
				pPlayerPed->ProcessAttachedObjects();
			}
		}
	}
}

void CPlayerPool::ApplyCollisionChecking()
{
	CRemotePlayer* pRemotePlayer = nullptr;
	CPlayerPed* pPlayerPed = nullptr;

	for (PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++) {
		pRemotePlayer = GetAt(playerId);
		if (pRemotePlayer) {
			pPlayerPed = pRemotePlayer->GetPlayerPed();
			if (pPlayerPed) {
				if (!pPlayerPed->IsInVehicle()) {
					m_bCollisionChecking[playerId] = pPlayerPed->GetCollisionChecking();
					pPlayerPed->SetCollisionChecking(true);
				}
			}
		}
	}
}

void CPlayerPool::ResetCollisionChecking()
{
	CRemotePlayer* pRemotePlayer = nullptr;
	CPlayerPed* pPlayerPed = nullptr;

	for (PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++) {
		pRemotePlayer = GetAt(playerId);
		if (pRemotePlayer) {
			pPlayerPed = pRemotePlayer->GetPlayerPed();
			if (pPlayerPed) {
				if (!pPlayerPed->IsInVehicle()) {
					pPlayerPed->SetCollisionChecking(m_bCollisionChecking[playerId] != 0);
				}
			}
		}
	}
}