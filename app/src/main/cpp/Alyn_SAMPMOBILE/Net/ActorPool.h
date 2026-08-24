#pragma once

#include "../Game/Actor.h"

#pragma pack(push, 1)
typedef struct _NEW_ACTOR {
	PLAYERID ActorID;
	int iSkin;
	sa::CVector vecPos;
	float fAngle;
	float fHealth;
	bool bInvulnerable;
} NEW_ACTOR;
#pragma pack(pop)

class CActorPool {
public:
	CActorPool();
	~CActorPool();

	void New(NEW_ACTOR* pNewActor);
	void Delete(PLAYERID ActorID);

	CActor* GetAt(PLAYERID ActorID)
	{
		if (ActorID < MAX_ACTORS && m_bActorSlotState[ActorID]) {
			if (m_pActors[ActorID]) {
				return m_pActors[ActorID];
			}

			return nullptr;
		}
	}

	PLAYERID FindIDFromGtaPtr(sa::CPed* pPed)
	{
		for (PLAYERID i = 0; i < MAX_ACTORS; i++) {
			if (m_pGtaPed[i] == pPed) {
				return i;
			}
		}

		return INVALID_PLAYER_ID;
	}

private:
	CActor* m_pActors[MAX_ACTORS];
	bool m_bActorSlotState[MAX_ACTORS];
	sa::CPed* m_pGtaPed[MAX_ACTORS];
};