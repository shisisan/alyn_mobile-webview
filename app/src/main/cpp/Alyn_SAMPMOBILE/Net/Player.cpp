#include "../Game/Game.h"
#include "NetGame.h"
#include "Player.h"

CPlayer::CPlayer(const char* szPlayerName, bool bIsNPC)
{
	m_bIsNPC = bIsNPC;
	m_iScore = 0;
	m_iPing = 0;
	strcpy(m_szPlayerName, szPlayerName);

	m_pRemotePlayer = new CRemotePlayer();
}

CPlayer::~CPlayer()
{
	if (m_pRemotePlayer) {
		delete m_pRemotePlayer;
		m_pRemotePlayer = nullptr;
	}
}