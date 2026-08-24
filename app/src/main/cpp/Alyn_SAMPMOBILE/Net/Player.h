#pragma once

class CRemotePlayer;
class CPlayer {
public:
	CPlayer(const char* szPlayerName, bool bIsNPC);
	~CPlayer();

	void SetPlayerName(const char* szPlayerName)
	{
		strcpy(m_szPlayerName, szPlayerName);
	}

public:
	CRemotePlayer* m_pRemotePlayer;
	char m_szPlayerName[MAX_PLAYER_NAME + 1];
	bool m_bIsNPC;
	int m_iScore;
	int m_iPing;
};