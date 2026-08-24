//
// Created by ALYN on 11-Jan-25.
//

#include "Game.h"
#include "../Client.h"

extern Game* pGame;

CActor::CActor(int iSkin, float fX, float fY, float fZ, float fAngle)
{
	spdlog::info("CActor::CActor({}, {}, {}, {}, {})", iSkin, fX, fY, fZ, fAngle);
	m_actor = nullptr;
	m_gtaId = 0;
	m_invulnerable = false;

	if (!IsValidPedModel(iSkin)) {
		spdlog::error("CActor::CActor: Invalid skin model: {}", iSkin);
		iSkin = 0;
	}

	if (!pGame->IsModelLoaded(iSkin)) {
		pGame->RequestModel(iSkin);
		pGame->LoadRequestedModels();
		while (!pGame->IsModelLoaded(iSkin)) usleep(1000);
	}

	uint32_t dwRet;
	ScriptCommand(&create_actor, 5, iSkin, fX, fY, fZ - 1.0f, &dwRet);
	ScriptCommand(&set_actor_z_angle, dwRet, fAngle);

	m_gtaId = dwRet;
	m_actor = GamePool_Ped_GetAt(m_gtaId);
	m_entity = m_actor;

	ScriptCommand(&set_actor_can_be_decapitated, m_gtaId, 0);
	ScriptCommand(&set_actor_decision_marker, m_gtaId, 0x10006);
}

CActor::~CActor()
{
	if (m_actor && GamePool_Ped_GetAt(m_gtaId) && !IsEntityPlaceable(m_actor)) {
		m_actor->Destructor();
		m_actor = nullptr;
		m_entity = nullptr;
	}
	else {
		m_gtaId = 0;
		m_actor = nullptr;
		m_entity = nullptr;
	}
}

void CActor::SetHealth(float fHealth)
{
	if (m_actor) {
		m_actor->m_fHealth = fHealth;

		if (m_actor->m_fHealth <= 0.0f) {
			ScriptCommand(&kill_actor, m_gtaId);
		}
	}
}

void CActor::SetInvulnerable(bool bInvulnerable)
{
	m_invulnerable = bInvulnerable;

	if (bInvulnerable) {
		ScriptCommand(&set_actor_immunities, m_gtaId, 1, 1, 1, 1, 1);
	}
	else {
		ScriptCommand(&set_actor_immunities, m_gtaId, 0, 0, 0, 1, 0);
	}
}

void CActor::ApplyAnimation(const char* szAnimName, const char* szAnimLib, float fDelta, int bLoop, int bLockX, int bLockY, int bFreeze, int iTime)
{
	if (!m_actor) {
		return;
	}

	if (!GamePool_Ped_GetAt(m_gtaId)) {
		return;
	}

	if (!szAnimLib || !strlen(szAnimLib)) {
		return;
	}

	if (!strcasecmp(szAnimLib, "SAMP") || !strcasecmp(szAnimLib, "SEX")) {
		return;
	}

	int iWaitAnimLoad = 0;

	if (!pGame->IsAnimationLoaded(szAnimLib)) {
		pGame->RequestAnimation(szAnimLib);
		while (!pGame->IsAnimationLoaded(szAnimLib)) {
			usleep(1000);
			iWaitAnimLoad++;
			if (iWaitAnimLoad > 15) {
				spdlog::error("Animation {} not loaded!", szAnimLib);
				return;
			}
		}
	}

	spdlog::info("Animation {} loaded!", szAnimLib);

	ScriptCommand(&apply_animation, m_gtaId, szAnimName, szAnimLib, fDelta, bLoop, bLockX, bLockY, bFreeze, iTime);
}

void CActor::ClearAnimation()
{
	if (m_actor) {
		// CPedIntelligence::FlushImmediately
		Memory::callFunction("_ZN16CPedIntelligence16FlushImmediatelyEb", m_actor->m_pIntelligence, true);
	}
}

void CActor::SetFacingAngle(float fAngle)
{
	if (m_actor && GamePool_Ped_GetAt(m_gtaId)) {
		m_actor->m_fAimingRotation = DegToRad(fAngle);
	}
}