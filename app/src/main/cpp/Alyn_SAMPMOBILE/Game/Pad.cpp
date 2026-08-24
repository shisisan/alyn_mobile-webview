#include "Game.h"
#include "../UI/UI.h"
#include "../Net/NetGame.h"
#include "../Client.h"

extern UI* pUI;
extern Game* pGame;
extern NetGame* pNetGame;

extern CPlayerPed* g_pCurrentFiredPed;
extern BULLET_DATA* g_pCurrentBulletData;
extern int g_iLagCompensationMode;

PAD_KEYS LocalPlayerKeys;
PAD_KEYS RemotePlayerKeys[PLAYER_PED_SLOTS];

sa::CPed* dwCurPlayerActor = nullptr;
uint8_t byteInternalPlayer = 0;
uint8_t byteCurPlayer = 0;

uint8_t byteSavedCameraMode;
uint16_t wSavedCameraMode2;

void* ghook_CWorld_ProcessLineOfSight = nullptr;

DECL_HOOK(void, CPed_ProcessControl, sa::CPed* _this)
{
	dwCurPlayerActor = _this;
	byteInternalPlayer = *pbyteCurrentPlayer;
	byteCurPlayer = FindPlayerNumFromPedPtr(_this);

	if (dwCurPlayerActor && (byteCurPlayer != 0) && byteInternalPlayer == 0) {
		// save the internal cammode, apply the context.
		byteSavedCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);

		// save the second internal cammode, apply the context.
		wSavedCameraMode2 = *wCameraMode2;
		*wCameraMode2 = GameGetPlayerCameraMode(byteCurPlayer);
		if (*wCameraMode2 == 4) *wCameraMode2 = 0;

		// save the camera zoom factor, apply the context.
		GameStoreLocalPlayerCameraExtZoomAndAspect();
		GameSetRemotePlayerCameraExtZoomAndAspect(byteCurPlayer);

		// aim switching.
		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(byteCurPlayer);

		// weapon skills.
		//GameStoreLocalPlayerSkills();
		//GameSetRemotePlayerSkills(byteCurPlayer);

		// set the internal player to the passed actor.
		*pbyteCurrentPlayer = byteCurPlayer;

		// CPed::UpdatePosition nulled from CPed::ProcessControl
		Memory::nop(g_saSym->Abs<uintptr_t>(addr::CPed_UpdatePosition_BranchBL1), 2);
		Memory::ret(g_saSym->Abs<uintptr_t>(addr::CWidget_setEnabled));

		// call orig
		CPed_ProcessControl(_this);

		// restore
		Memory::restoreNop(g_saSym->Abs<uintptr_t>(addr::CPed_UpdatePosition_BranchBL1));
		Memory::restoreRet(g_saSym->Abs<uintptr_t>(addr::CWidget_setEnabled));

		// restore weapon skills.
		//GameSetLocalPlayerSkills();

		// restore the camera modes.
		*pbyteCameraMode = byteSavedCameraMode;
		*wCameraMode2 = wSavedCameraMode2;

		// remote the local player's camera zoom factor
		GameSetLocalPlayerCameraExtZoomAndAspect();

		// restore the local player's internal ID.
		*pbyteCurrentPlayer = 0;

		// restore aim switching.
		GameSetLocalPlayerAim();
	}
	else {
		// Apply the original code to set ped rot from Cam
		Memory::restoreNop(g_saSym->Abs<uintptr_t>(addr::pedRots));

		// call orig
		CPed_ProcessControl(_this);

		// Reapply the no ped rots from Cam patch
		Memory::nop(g_saSym->Abs<uintptr_t>(addr::pedRots), 2);
	}
}

DECL_HOOK(uint16_t, CPad_GetPedWalkLeftRight, uintptr_t _this)
{
	if (*pbyteCurrentPlayer) {
		uint16_t dwResult = RemotePlayerKeys[byteCurPlayer].wKeyLR;
		if ((dwResult == 0xFF80 || dwResult == 0x80) && RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_WALK]) {
			dwResult = 0x40;
		}
		return dwResult;
	}
	else {
		LocalPlayerKeys.wKeyLR = CPad_GetPedWalkLeftRight(_this);
		return LocalPlayerKeys.wKeyLR;
	}
}
DECL_HOOK(uint16_t, CPad_GetPedWalkUpDown, uintptr_t _this)
{
	if (*pbyteCurrentPlayer) {
		uint16_t dwResult = RemotePlayerKeys[byteCurPlayer].wKeyUD;
		if ((dwResult == 0xFF80 || dwResult == 0x80) && RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_WALK]) {
			dwResult = 0x40;
		}
		return dwResult;
	}
	else {
		LocalPlayerKeys.wKeyUD = CPad_GetPedWalkUpDown(_this);
		return LocalPlayerKeys.wKeyUD;
	}
}

DECL_HOOK(uint16_t, CPad_JumpJustDown, uintptr_t _this)
{
	if (*pbyteCurrentPlayer) {
		if (!RemotePlayerKeys[byteCurPlayer].bIgnoreJump && RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP] && !RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE]) {
			RemotePlayerKeys[byteCurPlayer].bIgnoreJump = true;
			return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
		}

		return 0;
	}
	else {
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad_JumpJustDown(_this);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}
DECL_HOOK(uint16_t, CPad_GetJump, uintptr_t _this)
{
	if (*pbyteCurrentPlayer) {
		if (RemotePlayerKeys[byteCurPlayer].bIgnoreJump) return 0;
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
	}
	else {
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad_JumpJustDown(_this);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

DECL_HOOK(uint16_t, CPad_GetDuck, uintptr_t _this)
{
	if (*pbyteCurrentPlayer) {
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_CROUCH];
	}
	else {
		LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH] = CPad_GetDuck(_this);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH];
	}
}

DECL_HOOK(uint16_t, CPad_MeleeAttackJustDown, uintptr_t _this)
{
	if (*pbyteCurrentPlayer) {
		if (RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE] && RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SECONDARY_ATTACK]) {
			return 2;
		}

		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE];
	}
	else {
		uint32_t dwResult = CPad_MeleeAttackJustDown(_this);
		/*if (dwResult == 2)
		{
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE] = true;
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = true;
			return dwResult;
		}*/
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = dwResult;
		return dwResult;
	}
}

DECL_HOOK(uint16_t, CPad_DuckJustDown, uintptr_t _this, int unk)
{
	if (*pbyteCurrentPlayer) {
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_CROUCH];
	}
	else {
		LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH] = CPad_DuckJustDown(_this, unk);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH];
	}
}

DECL_HOOK(uint32_t, CPad_GetSprint, uintptr_t _this, uint32_t unk)
{
	if (*pbyteCurrentPlayer) {
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SPRINT];
	}
	else {
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT] = CPad_GetSprint(_this, unk);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT];
	}
}

DECL_HOOK(uint32_t, CPad_GetAutoClimb, uintptr_t _this)
{
	if (*pbyteCurrentPlayer) {
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
	}
	else {
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad_GetAutoClimb(_this);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

DECL_HOOK(uint32_t, CPad_GetAbortClimb, uintptr_t _this)
{
	if (*pbyteCurrentPlayer) {
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SECONDARY_ATTACK];
	}
	else {
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = CPad_GetAbortClimb(_this);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK];
	}
}

DECL_HOOK(uint32_t, CPad_DiveJustDown, uintptr_t _this)
{
	if (*pbyteCurrentPlayer) {
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE];
	}
	else {
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = CPad_DiveJustDown(_this);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE];
	}
}

DECL_HOOK(uint32_t, CPad_SwimJumpJustDown, uintptr_t _this)
{
	if (*pbyteCurrentPlayer) {
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
	}
	else {
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad_SwimJumpJustDown(_this);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

DECL_HOOK(uint32_t, CPad_GetBlock, uintptr_t _this)
{
	if (*pbyteCurrentPlayer) {
		return (RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP] && RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE]);
	}
	else {
		return CPad_GetBlock(_this);
	}
}

DECL_HOOK(int16_t, CPad_GetSteeringLeftRight, uintptr_t _this)
{
	if (*pbyteCurrentPlayer) {
		return (int16_t) RemotePlayerKeys[byteCurPlayer].wKeyLR;
	}
	else {
		LocalPlayerKeys.wKeyLR = CPad_GetSteeringLeftRight(_this);
		return LocalPlayerKeys.wKeyLR;
	}
}

DECL_HOOK(uint16_t, CPad_GetSteeringUpDown, uintptr_t _this)
{
	if (*pbyteCurrentPlayer) {
		return RemotePlayerKeys[byteCurPlayer].wKeyUD;
	}
	else {
		LocalPlayerKeys.wKeyUD = CPad_GetSteeringUpDown(_this);
		return LocalPlayerKeys.wKeyUD;
	}
}

DECL_HOOK(uint16_t, CPad_GetAccelerate, uintptr_t _this)
{
	if (*pbyteCurrentPlayer) {
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SPRINT] ? 0xFF : 0x00;
	}
	else {
		sa::CPed* local = GamePool_FindPlayerPed();
		sa::CVehicle* pVehicle = local->pVehicle;
		if (pVehicle) {
			bool engineOn = pVehicle->m_nVehicleFlags.bEngineOn;
			if (pVehicle->pDriver == local && !engineOn) return 0;
		}
		uint16_t wResult = CPad_GetAccelerate(_this);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT] = wResult;
		return wResult;
	}
}

DECL_HOOK(uint16_t, CPad_GetBrake, uintptr_t _this)
{
	if (*pbyteCurrentPlayer) {
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP] ? 0xFF : 0x00;
	}
	else {
		sa::CPed* local = GamePool_FindPlayerPed();
		sa::CVehicle* pVehicle = local->pVehicle;
		if (pVehicle) {
			bool engineOn = pVehicle->m_nVehicleFlags.bEngineOn;
			if (pVehicle->pDriver == local && !engineOn) return 0;
		}

		uint16_t wResult = CPad_GetBrake(_this);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = wResult;
		return wResult;
	}
}

DECL_HOOK(uint32_t, CPad_GetHandBrake, uintptr_t _this)
{
	if (*pbyteCurrentPlayer) {
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE] ? 0xFF : 0x00;
	}
	else {
		sa::CPed* local = GamePool_FindPlayerPed();
		sa::CVehicle* pVehicle = local->pVehicle;
		if (pVehicle) {
			bool engineOn = pVehicle->m_nVehicleFlags.bEngineOn;
			if (pVehicle->pDriver == local && !engineOn) return 0;
		}

		uint32_t dwResult = CPad_GetHandBrake(_this);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = dwResult;
		return dwResult;
	}
}

DECL_HOOK(uint32_t, CPad_GetHorn, uintptr_t _this)
{
	if (*pbyteCurrentPlayer) {
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_CROUCH];
	}
	else {
		uint32_t dwResult = CPad_GetHorn(_this);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH] = dwResult;
		return dwResult;
	}
}

DECL_HOOK(uint32_t, CPad_ExitVehicleJustDown, uintptr_t _this, int a2, uintptr_t vehicle, int a4, uintptr_t vec)
{
	static uint32_t dwPassengerEnterExit = GetTickCount();
	if (GetTickCount() - dwPassengerEnterExit < 1000) {
		return 0;
	}

	if (pNetGame) {
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		if (pPlayerPool) {
			CLocalPlayer* pLocalPlayer = pPlayerPool->GetLocalPlayer();
			if (pLocalPlayer) {
				if (pLocalPlayer->HandlePassengerEntry()) {
					dwPassengerEnterExit = GetTickCount();
					return 0;
				}
			}
		}
	}

	return CPad_ExitVehicleJustDown(_this, a2, vehicle, a4, vec);
}

DECL_HOOK(int, CPad_GetTurretLeft, uintptr_t _this)
{
	if (*pbyteCurrentPlayer) {
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_LOOK_LEFT];
	}
	else {
		LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_LEFT] = CPad_GetTurretLeft(_this);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_LEFT];
	}
}

DECL_HOOK(int, CPad_GetTurretRight, uintptr_t _this)
{
	if (*pbyteCurrentPlayer) {
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_LOOK_RIGHT];
	}
	else {
		LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_RIGHT] = CPad_GetTurretRight(_this);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_RIGHT];
	}
}

DECL_HOOK(int, CPad_GetNitroFired, uintptr_t _this)
{
	if (*pbyteCurrentPlayer) {
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE];
	}
	else {
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = CPad_GetNitroFired(_this);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE];
	}
}

DECL_HOOK(int, CPad_GetHydraulicJump, uintptr_t _this)
{
	if (*pbyteCurrentPlayer) {
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_CROUCH];
	}
	else {
		LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH] = CPad_GetHydraulicJump(_this);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH];
	}
}

uint8_t savedControlFlags = 0;

DECL_HOOK(void, CAutomobile_ProcessControl, sa::CVehicle* _this)
{
	sa::CVehicle* pVehicle = _this;
	byteInternalPlayer = *pbyteCurrentPlayer;

	if (pVehicle && pVehicle->pDriver && pVehicle->pDriver->m_nPedType == 0 && pVehicle->pDriver != GamePool_FindPlayerPed() && byteInternalPlayer == 0) {
		byteCurPlayer = FindPlayerNumFromPedPtr(pVehicle->pDriver);

		// save the internal cammode, apply the context.
		byteSavedCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);

		// save the second internal cammode, apply the context.
		wSavedCameraMode2 = *wCameraMode2;
		*wCameraMode2 = GameGetPlayerCameraMode(byteCurPlayer);
		if (*wCameraMode2 == 4) *wCameraMode2 = 0;

		// aim switching.
		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(byteCurPlayer);

		// Tyre burst fix
		if (pVehicle->pDriver) {
			if (pVehicle->m_nVehicleFlags.bTyresDontBurst) {
				pVehicle->m_nVehicleFlags.bTyresDontBurst = 0;
			}
			if (!pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = true;
		}
		else {
			if (!pVehicle->m_nVehicleFlags.bTyresDontBurst) {
				pVehicle->m_nVehicleFlags.bTyresDontBurst = 1;
			}
			if (pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = false;
		}

		*pbyteCurrentPlayer = 0;

		pVehicle->pDriver->m_nPedType = sa::PEDTYPE_CIVMALE; // So CPed::IsPlayer returns FALSE
		savedControlFlags = pVehicle->nControlFlags;
		pVehicle->nControlFlags = 26;

		// CAEVehicleAudioEntity::Service
		Memory::callFunction<void>("_ZN21CAEVehicleAudioEntity7ServiceEv", &pVehicle->m_VehicleAudioEntity);

		pVehicle->nControlFlags = savedControlFlags;
		pVehicle->pDriver->m_nPedType = sa::PEDTYPE_PLAYER1;

		// set the internal player to the passed actor.
		*pbyteCurrentPlayer = byteCurPlayer;

		CAutomobile_ProcessControl(pVehicle);

		// restore the local player's internal ID.
		*pbyteCurrentPlayer = 0;

		// restore the camera modes.
		*pbyteCameraMode = byteSavedCameraMode;
		*wCameraMode2 = wSavedCameraMode2;

		// restore aim switching.
		GameSetLocalPlayerAim();
	}
	else {
		if (pVehicle && pVehicle->pDriver && pVehicle->pDriver->m_nPedType == 0 && GamePool_FindPlayerPed() == pVehicle->pDriver) {
			// Tyre burst fix
			if (pVehicle->pDriver) {
				if (pVehicle->m_nVehicleFlags.bTyresDontBurst) {
					pVehicle->m_nVehicleFlags.bTyresDontBurst = 0;
				}
				if (!pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = true;
			}
			else {
				if (!pVehicle->m_nVehicleFlags.bTyresDontBurst) {
					pVehicle->m_nVehicleFlags.bTyresDontBurst = 1;
				}
				if (pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = false;
			}
		}

		// CAEVehicleAudioEntity::Service
		Memory::callFunction<void>("_ZN21CAEVehicleAudioEntity7ServiceEv", &pVehicle->m_VehicleAudioEntity);

		CAutomobile_ProcessControl(pVehicle);
	}
}

DECL_HOOK(void, CBoat_ProcessControl, sa::CVehicle* _this)
{
	sa::CVehicle* pVehicle = _this;
	byteInternalPlayer = *pbyteCurrentPlayer;

	if (pVehicle && pVehicle->pDriver && pVehicle->pDriver->m_nPedType == 0 && pVehicle->pDriver != GamePool_FindPlayerPed() && byteInternalPlayer == 0) {
		byteCurPlayer = FindPlayerNumFromPedPtr(pVehicle->pDriver);

		// save the internal cammode, apply the context.
		byteSavedCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);

		// save the second internal cammode, apply the context.
		wSavedCameraMode2 = *wCameraMode2;
		*wCameraMode2 = GameGetPlayerCameraMode(byteCurPlayer);
		if (*wCameraMode2 == 4) *wCameraMode2 = 0;

		// aim switching.
		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(byteCurPlayer);

		// Tyre burst fix
		if (pVehicle->pDriver) {
			if (pVehicle->m_nVehicleFlags.bTyresDontBurst) {
				pVehicle->m_nVehicleFlags.bTyresDontBurst = 0;
			}
			if (!pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = true;
		}
		else {
			if (!pVehicle->m_nVehicleFlags.bTyresDontBurst) {
				pVehicle->m_nVehicleFlags.bTyresDontBurst = 1;
			}
			if (pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = false;
		}

		*pbyteCurrentPlayer = 0;

		pVehicle->pDriver->m_nPedType = sa::PEDTYPE_CIVMALE; // So CPed::IsPlayer returns FALSE
		savedControlFlags = pVehicle->nControlFlags;
		pVehicle->nControlFlags = 26;

		// CAEVehicleAudioEntity::Service
		Memory::callFunction<void>("_ZN21CAEVehicleAudioEntity7ServiceEv", &pVehicle->m_VehicleAudioEntity);

		pVehicle->nControlFlags = savedControlFlags;
		pVehicle->pDriver->m_nPedType = sa::PEDTYPE_PLAYER1;

		// set the internal player to the passed actor.
		*pbyteCurrentPlayer = byteCurPlayer;

		CBoat_ProcessControl(pVehicle);

		// restore the local player's internal ID.
		*pbyteCurrentPlayer = 0;

		// restore the camera modes.
		*pbyteCameraMode = byteSavedCameraMode;
		*wCameraMode2 = wSavedCameraMode2;

		// restore aim switching.
		GameSetLocalPlayerAim();
	}
	else {
		if (pVehicle && pVehicle->pDriver && pVehicle->pDriver->m_nPedType == 0 && GamePool_FindPlayerPed() == pVehicle->pDriver) {
			// Tyre burst fix
			if (pVehicle->pDriver) {
				if (pVehicle->m_nVehicleFlags.bTyresDontBurst) {
					pVehicle->m_nVehicleFlags.bTyresDontBurst = 0;
				}
				if (!pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = true;
			}
			else {
				if (!pVehicle->m_nVehicleFlags.bTyresDontBurst) {
					pVehicle->m_nVehicleFlags.bTyresDontBurst = 1;
				}
				if (pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = false;
			}
		}

		// CAEVehicleAudioEntity::Service
		Memory::callFunction<void>("_ZN21CAEVehicleAudioEntity7ServiceEv", &pVehicle->m_VehicleAudioEntity);

		CBoat_ProcessControl(pVehicle);
	}
}

DECL_HOOK(void, CBike_ProcessControl, sa::CVehicle* _this)
{
	sa::CVehicle* pVehicle = _this;
	byteInternalPlayer = *pbyteCurrentPlayer;

	if (pVehicle && pVehicle->pDriver && pVehicle->pDriver->m_nPedType == 0 && pVehicle->pDriver != GamePool_FindPlayerPed() && byteInternalPlayer == 0) {
		byteCurPlayer = FindPlayerNumFromPedPtr(pVehicle->pDriver);

		// save the internal cammode, apply the context.
		byteSavedCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);

		// save the second internal cammode, apply the context.
		wSavedCameraMode2 = *wCameraMode2;
		*wCameraMode2 = GameGetPlayerCameraMode(byteCurPlayer);
		if (*wCameraMode2 == 4) *wCameraMode2 = 0;

		// aim switching.
		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(byteCurPlayer);

		// Tyre burst fix
		if (pVehicle->pDriver) {
			if (pVehicle->m_nVehicleFlags.bTyresDontBurst) {
				pVehicle->m_nVehicleFlags.bTyresDontBurst = 0;
			}
			if (!pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = true;
		}
		else {
			if (!pVehicle->m_nVehicleFlags.bTyresDontBurst) {
				pVehicle->m_nVehicleFlags.bTyresDontBurst = 1;
			}
			if (pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = false;
		}

		*pbyteCurrentPlayer = 0;

		pVehicle->pDriver->m_nPedType = sa::PEDTYPE_CIVMALE; // So CPed::IsPlayer returns FALSE
		savedControlFlags = reinterpret_cast<uint8_t&>(pVehicle->m_nVehicleFlags);
		reinterpret_cast<uint8_t&>(pVehicle->m_nVehicleFlags) = 26;

		// CAEVehicleAudioEntity::Service
		Memory::callFunction<void>("_ZN21CAEVehicleAudioEntity7ServiceEv", &pVehicle->m_VehicleAudioEntity);

		reinterpret_cast<uint8_t&>(pVehicle->m_nVehicleFlags) = savedControlFlags;
		pVehicle->pDriver->m_nPedType = sa::PEDTYPE_PLAYER1;

		// set the internal player to the passed actor.
		*pbyteCurrentPlayer = byteCurPlayer;

		CBike_ProcessControl(pVehicle);

		// restore the local player's internal ID.
		*pbyteCurrentPlayer = 0;

		// restore the camera modes.
		*pbyteCameraMode = byteSavedCameraMode;
		*wCameraMode2 = wSavedCameraMode2;

		// restore aim switching.
		GameSetLocalPlayerAim();
	}
	else {
		if (pVehicle && pVehicle->pDriver && pVehicle->pDriver->m_nPedType == 0 && GamePool_FindPlayerPed() == pVehicle->pDriver) {
			// Tyre burst fix
			if (pVehicle->pDriver) {
				if (pVehicle->m_nVehicleFlags.bTyresDontBurst) {
					pVehicle->m_nVehicleFlags.bTyresDontBurst = 0;
				}
				if (!pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = true;
			}
			else {
				if (!pVehicle->m_nVehicleFlags.bTyresDontBurst) {
					pVehicle->m_nVehicleFlags.bTyresDontBurst = 1;
				}
				if (pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = false;
			}
		}

		// CAEVehicleAudioEntity::Service
		Memory::callFunction<void>("_ZN21CAEVehicleAudioEntity7ServiceEv", &pVehicle->m_VehicleAudioEntity);

		CBike_ProcessControl(pVehicle);
	}
}

DECL_HOOK(void, CHeli_ProcessControl, sa::CVehicle* _this)
{
	sa::CVehicle* pVehicle = _this;
	byteInternalPlayer = *pbyteCurrentPlayer;

	if (pVehicle && pVehicle->pDriver && pVehicle->pDriver->m_nPedType == 0 && pVehicle->pDriver != GamePool_FindPlayerPed() && byteInternalPlayer == 0) {
		byteCurPlayer = FindPlayerNumFromPedPtr(pVehicle->pDriver);

		// save the internal cammode, apply the context.
		byteSavedCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);

		// save the second internal cammode, apply the context.
		wSavedCameraMode2 = *wCameraMode2;
		*wCameraMode2 = GameGetPlayerCameraMode(byteCurPlayer);
		if (*wCameraMode2 == 4) *wCameraMode2 = 0;

		// aim switching.
		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(byteCurPlayer);

		// Tyre burst fix
		if (pVehicle->pDriver) {
			if (pVehicle->m_nVehicleFlags.bTyresDontBurst) {
				pVehicle->m_nVehicleFlags.bTyresDontBurst = 0;
			}
			if (!pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = true;
		}
		else {
			if (!pVehicle->m_nVehicleFlags.bTyresDontBurst) {
				pVehicle->m_nVehicleFlags.bTyresDontBurst = 1;
			}
			if (pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = false;
		}

		*pbyteCurrentPlayer = 0;

		pVehicle->pDriver->m_nPedType = sa::PEDTYPE_CIVMALE; // So CPed::IsPlayer returns FALSE
		savedControlFlags = pVehicle->nControlFlags;
		pVehicle->nControlFlags = 26;

		// CAEVehicleAudioEntity::Service
		// Memory::callFunction<void>("_ZN21CAEVehicleAudioEntity7ServiceEv", &pVehicle->m_VehicleAudioEntity);

		pVehicle->nControlFlags = savedControlFlags;
		pVehicle->pDriver->m_nPedType = sa::PEDTYPE_PLAYER1;

		// set the internal player to the passed actor.
		*pbyteCurrentPlayer = byteCurPlayer;

		CHeli_ProcessControl(pVehicle);

		// restore the local player's internal ID.
		*pbyteCurrentPlayer = 0;

		// restore the camera modes.
		*pbyteCameraMode = byteSavedCameraMode;
		*wCameraMode2 = wSavedCameraMode2;

		// restore aim switching.
		GameSetLocalPlayerAim();
	}
	else {
		if (pVehicle && pVehicle->pDriver && pVehicle->pDriver->m_nPedType == 0 && GamePool_FindPlayerPed() == pVehicle->pDriver) {
			// Tyre burst fix
			if (pVehicle->pDriver) {
				if (pVehicle->m_nVehicleFlags.bTyresDontBurst) {
					pVehicle->m_nVehicleFlags.bTyresDontBurst = 0;
				}
				if (!pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = true;
			}
			else {
				if (!pVehicle->m_nVehicleFlags.bTyresDontBurst) {
					pVehicle->m_nVehicleFlags.bTyresDontBurst = 1;
				}
				if (pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = false;
			}
		}

		// CAEVehicleAudioEntity::Service
		// Memory::callFunction<void>("_ZN21CAEVehicleAudioEntity7ServiceEv", &pVehicle->m_VehicleAudioEntity);

		CHeli_ProcessControl(pVehicle);
	}
}

DECL_HOOK(void, CPlane_ProcessControl, sa::CVehicle* _this)
{
	sa::CVehicle* pVehicle = _this;
	byteInternalPlayer = *pbyteCurrentPlayer;

	if (pVehicle && pVehicle->pDriver && pVehicle->pDriver->m_nPedType == 0 && pVehicle->pDriver != GamePool_FindPlayerPed() && byteInternalPlayer == 0) {
		byteCurPlayer = FindPlayerNumFromPedPtr(pVehicle->pDriver);

		// save the internal cammode, apply the context.
		byteSavedCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);

		// save the second internal cammode, apply the context.
		wSavedCameraMode2 = *wCameraMode2;
		*wCameraMode2 = GameGetPlayerCameraMode(byteCurPlayer);
		if (*wCameraMode2 == 4) *wCameraMode2 = 0;

		// aim switching.
		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(byteCurPlayer);

		// Tyre burst fix
		if (pVehicle->pDriver) {
			if (pVehicle->m_nVehicleFlags.bTyresDontBurst) {
				pVehicle->m_nVehicleFlags.bTyresDontBurst = 0;
			}
			if (!pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = true;
		}
		else {
			if (!pVehicle->m_nVehicleFlags.bTyresDontBurst) {
				pVehicle->m_nVehicleFlags.bTyresDontBurst = 1;
			}
			if (pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = false;
		}

		*pbyteCurrentPlayer = 0;

		pVehicle->pDriver->m_nPedType = sa::PEDTYPE_CIVMALE; // So CPed::IsPlayer returns FALSE
		savedControlFlags = pVehicle->nControlFlags;
		pVehicle->nControlFlags = 26;

		// CAEVehicleAudioEntity::Service
		// Memory::callFunction<void>("_ZN21CAEVehicleAudioEntity7ServiceEv", &pVehicle->m_VehicleAudioEntity);

		pVehicle->nControlFlags = savedControlFlags;
		pVehicle->pDriver->m_nPedType = sa::PEDTYPE_PLAYER1;

		// set the internal player to the passed actor.
		*pbyteCurrentPlayer = byteCurPlayer;

		CPlane_ProcessControl(pVehicle);

		// restore the local player's internal ID.
		*pbyteCurrentPlayer = 0;

		// restore the camera modes.
		*pbyteCameraMode = byteSavedCameraMode;
		*wCameraMode2 = wSavedCameraMode2;

		// restore aim switching.
		GameSetLocalPlayerAim();
	}
	else {
		if (pVehicle && pVehicle->pDriver && pVehicle->pDriver->m_nPedType == 0 && GamePool_FindPlayerPed() == pVehicle->pDriver) {
			// Tyre burst fix
			if (pVehicle->pDriver) {
				if (pVehicle->m_nVehicleFlags.bTyresDontBurst) {
					pVehicle->m_nVehicleFlags.bTyresDontBurst = 0;
				}
				if (!pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = true;
			}
			else {
				if (!pVehicle->m_nVehicleFlags.bTyresDontBurst) {
					pVehicle->m_nVehicleFlags.bTyresDontBurst = 1;
				}
				if (pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = false;
			}
		}

		// CAEVehicleAudioEntity::Service
		// Memory::callFunction<void>("_ZN21CAEVehicleAudioEntity7ServiceEv", &pVehicle->m_VehicleAudioEntity);

		CPlane_ProcessControl(pVehicle);
	}
}

DECL_HOOK(void, CBmx_ProcessControl, sa::CVehicle* _this)
{
	sa::CVehicle* pVehicle = _this;
	byteInternalPlayer = *pbyteCurrentPlayer;

	if (pVehicle && pVehicle->pDriver && pVehicle->pDriver->m_nPedType == 0 && pVehicle->pDriver != GamePool_FindPlayerPed() && byteInternalPlayer == 0) {
		byteCurPlayer = FindPlayerNumFromPedPtr(pVehicle->pDriver);

		// save the internal cammode, apply the context.
		byteSavedCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);

		// save the second internal cammode, apply the context.
		wSavedCameraMode2 = *wCameraMode2;
		*wCameraMode2 = GameGetPlayerCameraMode(byteCurPlayer);
		if (*wCameraMode2 == 4) *wCameraMode2 = 0;

		// aim switching.
		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(byteCurPlayer);

		// Tyre burst fix
		if (pVehicle->pDriver) {
			if (pVehicle->m_nVehicleFlags.bTyresDontBurst) {
				pVehicle->m_nVehicleFlags.bTyresDontBurst = 0;
			}
			if (!pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = true;
		}
		else {
			if (!pVehicle->m_nVehicleFlags.bTyresDontBurst) {
				pVehicle->m_nVehicleFlags.bTyresDontBurst = 1;
			}
			if (pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = false;
		}

		*pbyteCurrentPlayer = 0;

		pVehicle->pDriver->m_nPedType = sa::PEDTYPE_CIVMALE; // So CPed::IsPlayer returns FALSE
		savedControlFlags = pVehicle->nControlFlags;
		pVehicle->nControlFlags = 26;

		// CAEVehicleAudioEntity::Service
		// Memory::callFunction<void>("_ZN21CAEVehicleAudioEntity7ServiceEv", &pVehicle->m_VehicleAudioEntity);

		pVehicle->nControlFlags = savedControlFlags;
		pVehicle->pDriver->m_nPedType = sa::PEDTYPE_PLAYER1;

		// set the internal player to the passed actor.
		*pbyteCurrentPlayer = byteCurPlayer;

		CBmx_ProcessControl(pVehicle);

		// restore the local player's internal ID.
		*pbyteCurrentPlayer = 0;

		// restore the camera modes.
		*pbyteCameraMode = byteSavedCameraMode;
		*wCameraMode2 = wSavedCameraMode2;

		// restore aim switching.
		GameSetLocalPlayerAim();
	}
	else {
		if (pVehicle && pVehicle->pDriver && pVehicle->pDriver->m_nPedType == 0 && GamePool_FindPlayerPed() == pVehicle->pDriver) {
			// Tyre burst fix
			if (pVehicle->pDriver) {
				if (pVehicle->m_nVehicleFlags.bTyresDontBurst) {
					pVehicle->m_nVehicleFlags.bTyresDontBurst = 0;
				}
				if (!pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = true;
			}
			else {
				if (!pVehicle->m_nVehicleFlags.bTyresDontBurst) {
					pVehicle->m_nVehicleFlags.bTyresDontBurst = 1;
				}
				if (pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = false;
			}
		}

		// CAEVehicleAudioEntity::Service
		// Memory::callFunction<void>("_ZN21CAEVehicleAudioEntity7ServiceEv", &pVehicle->m_VehicleAudioEntity);

		CBmx_ProcessControl(pVehicle);
	}
}

DECL_HOOK(void, CMonsterTruck_ProcessControl, sa::CVehicle* _this)
{
	sa::CVehicle* pVehicle = _this;
	byteInternalPlayer = *pbyteCurrentPlayer;

	if (pVehicle && pVehicle->pDriver && pVehicle->pDriver->m_nPedType == 0 && pVehicle->pDriver != GamePool_FindPlayerPed() && byteInternalPlayer == 0) {
		byteCurPlayer = FindPlayerNumFromPedPtr(pVehicle->pDriver);

		// save the internal cammode, apply the context.
		byteSavedCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);

		// save the second internal cammode, apply the context.
		wSavedCameraMode2 = *wCameraMode2;
		*wCameraMode2 = GameGetPlayerCameraMode(byteCurPlayer);
		if (*wCameraMode2 == 4) *wCameraMode2 = 0;

		// aim switching.
		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(byteCurPlayer);

		// Tyre burst fix
		if (pVehicle->pDriver) {
			if (pVehicle->m_nVehicleFlags.bTyresDontBurst) {
				pVehicle->m_nVehicleFlags.bTyresDontBurst = 0;
			}
			if (!pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = true;
		}
		else {
			if (!pVehicle->m_nVehicleFlags.bTyresDontBurst) {
				pVehicle->m_nVehicleFlags.bTyresDontBurst = 1;
			}
			if (pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = false;
		}

		*pbyteCurrentPlayer = 0;

		pVehicle->pDriver->m_nPedType = sa::PEDTYPE_CIVMALE; // So CPed::IsPlayer returns FALSE
		savedControlFlags = pVehicle->nControlFlags;
		pVehicle->nControlFlags = 26;

		// CAEVehicleAudioEntity::Service
		// Memory::callFunction<void>("_ZN21CAEVehicleAudioEntity7ServiceEv", &pVehicle->m_VehicleAudioEntity);

		pVehicle->nControlFlags = savedControlFlags;
		pVehicle->pDriver->m_nPedType = sa::PEDTYPE_PLAYER1;

		// set the internal player to the passed actor.
		*pbyteCurrentPlayer = byteCurPlayer;

		CMonsterTruck_ProcessControl(pVehicle);

		// restore the local player's internal ID.
		*pbyteCurrentPlayer = 0;

		// restore the camera modes.
		*pbyteCameraMode = byteSavedCameraMode;
		*wCameraMode2 = wSavedCameraMode2;

		// restore aim switching.
		GameSetLocalPlayerAim();
	}
	else {
		if (pVehicle && pVehicle->pDriver && pVehicle->pDriver->m_nPedType == 0 && GamePool_FindPlayerPed() == pVehicle->pDriver) {
			// Tyre burst fix
			if (pVehicle->pDriver) {
				if (pVehicle->m_nVehicleFlags.bTyresDontBurst) {
					pVehicle->m_nVehicleFlags.bTyresDontBurst = 0;
				}
				if (!pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = true;
			}
			else {
				if (!pVehicle->m_nVehicleFlags.bTyresDontBurst) {
					pVehicle->m_nVehicleFlags.bTyresDontBurst = 1;
				}
				if (pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = false;
			}
		}

		// CAEVehicleAudioEntity::Service
		// Memory::callFunction<void>("_ZN21CAEVehicleAudioEntity7ServiceEv", &pVehicle->m_VehicleAudioEntity);

		CMonsterTruck_ProcessControl(pVehicle);
	}
}

DECL_HOOK(void, CQuadBike_ProcessControl, sa::CVehicle* _this)
{
	sa::CVehicle* pVehicle = _this;
	byteInternalPlayer = *pbyteCurrentPlayer;

	if (pVehicle && pVehicle->pDriver && pVehicle->pDriver->m_nPedType == 0 && pVehicle->pDriver != GamePool_FindPlayerPed() && byteInternalPlayer == 0) {
		byteCurPlayer = FindPlayerNumFromPedPtr(pVehicle->pDriver);

		// save the internal cammode, apply the context.
		byteSavedCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);

		// save the second internal cammode, apply the context.
		wSavedCameraMode2 = *wCameraMode2;
		*wCameraMode2 = GameGetPlayerCameraMode(byteCurPlayer);
		if (*wCameraMode2 == 4) *wCameraMode2 = 0;

		// aim switching.
		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(byteCurPlayer);

		// Tyre burst fix
		if (pVehicle->pDriver) {
			if (pVehicle->m_nVehicleFlags.bTyresDontBurst) {
				pVehicle->m_nVehicleFlags.bTyresDontBurst = 0;
			}
			if (!pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = true;
		}
		else {
			if (!pVehicle->m_nVehicleFlags.bTyresDontBurst) {
				pVehicle->m_nVehicleFlags.bTyresDontBurst = 1;
			}
			if (pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = false;
		}

		*pbyteCurrentPlayer = 0;

		pVehicle->pDriver->m_nPedType = sa::PEDTYPE_CIVMALE; // So CPed::IsPlayer returns FALSE
		savedControlFlags = pVehicle->nControlFlags;
		pVehicle->nControlFlags = 26;

		// CAEVehicleAudioEntity::Service
		// Memory::callFunction<void>("_ZN21CAEVehicleAudioEntity7ServiceEv", &pVehicle->m_VehicleAudioEntity);

		pVehicle->nControlFlags = savedControlFlags;
		pVehicle->pDriver->m_nPedType = sa::PEDTYPE_PLAYER1;

		// set the internal player to the passed actor.
		*pbyteCurrentPlayer = byteCurPlayer;

		CQuadBike_ProcessControl(pVehicle);

		// restore the local player's internal ID.
		*pbyteCurrentPlayer = 0;

		// restore the camera modes.
		*pbyteCameraMode = byteSavedCameraMode;
		*wCameraMode2 = wSavedCameraMode2;

		// restore aim switching.
		GameSetLocalPlayerAim();
	}
	else {
		if (pVehicle && pVehicle->pDriver && pVehicle->pDriver->m_nPedType == 0 && GamePool_FindPlayerPed() == pVehicle->pDriver) {
			// Tyre burst fix
			if (pVehicle->pDriver) {
				if (pVehicle->m_nVehicleFlags.bTyresDontBurst) {
					pVehicle->m_nVehicleFlags.bTyresDontBurst = 0;
				}
				if (!pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = true;
			}
			else {
				if (!pVehicle->m_nVehicleFlags.bTyresDontBurst) {
					pVehicle->m_nVehicleFlags.bTyresDontBurst = 1;
				}
				if (pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = false;
			}
		}

		// CAEVehicleAudioEntity::Service
		// Memory::callFunction<void>("_ZN21CAEVehicleAudioEntity7ServiceEv", &pVehicle->m_VehicleAudioEntity);

		CQuadBike_ProcessControl(pVehicle);
	}
}

DECL_HOOK(void, CTrain_ProcessControl, sa::CVehicle* _this)
{
	sa::CVehicle* pVehicle = _this;
	byteInternalPlayer = *pbyteCurrentPlayer;

	if (pVehicle && pVehicle->pDriver && pVehicle->pDriver->m_nPedType == 0 && pVehicle->pDriver != GamePool_FindPlayerPed() && byteInternalPlayer == 0) {
		byteCurPlayer = FindPlayerNumFromPedPtr(pVehicle->pDriver);

		// save the internal cammode, apply the context.
		byteSavedCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);

		// save the second internal cammode, apply the context.
		wSavedCameraMode2 = *wCameraMode2;
		*wCameraMode2 = GameGetPlayerCameraMode(byteCurPlayer);
		if (*wCameraMode2 == 4) *wCameraMode2 = 0;

		// aim switching.
		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(byteCurPlayer);

		// Tyre burst fix
		if (pVehicle->pDriver) {
			if (pVehicle->m_nVehicleFlags.bTyresDontBurst) {
				pVehicle->m_nVehicleFlags.bTyresDontBurst = 0;
			}
			if (!pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = true;
		}
		else {
			if (!pVehicle->m_nVehicleFlags.bTyresDontBurst) {
				pVehicle->m_nVehicleFlags.bTyresDontBurst = 1;
			}
			if (pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = false;
		}

		*pbyteCurrentPlayer = 0;

		pVehicle->pDriver->m_nPedType = sa::PEDTYPE_CIVMALE; // So CPed::IsPlayer returns FALSE
		savedControlFlags = pVehicle->nControlFlags;
		pVehicle->nControlFlags = 26;

		// CAEVehicleAudioEntity::Service
		Memory::callFunction<void>("_ZN21CAEVehicleAudioEntity7ServiceEv", &pVehicle->m_VehicleAudioEntity);

		pVehicle->nControlFlags = savedControlFlags;
		pVehicle->pDriver->m_nPedType = sa::PEDTYPE_PLAYER1;

		// set the internal player to the passed actor.
		*pbyteCurrentPlayer = byteCurPlayer;

		CTrain_ProcessControl(pVehicle);

		// restore the local player's internal ID.
		*pbyteCurrentPlayer = 0;

		// restore the camera modes.
		*pbyteCameraMode = byteSavedCameraMode;
		*wCameraMode2 = wSavedCameraMode2;

		// restore aim switching.
		GameSetLocalPlayerAim();
	}
	else {
		if (pVehicle && pVehicle->pDriver && pVehicle->pDriver->m_nPedType == 0 && GamePool_FindPlayerPed() == pVehicle->pDriver) {
			// Tyre burst fix
			if (pVehicle->pDriver) {
				if (pVehicle->m_nVehicleFlags.bTyresDontBurst) {
					pVehicle->m_nVehicleFlags.bTyresDontBurst = 0;
				}
				if (!pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = true;
			}
			else {
				if (!pVehicle->m_nVehicleFlags.bTyresDontBurst) {
					pVehicle->m_nVehicleFlags.bTyresDontBurst = 1;
				}
				if (pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = false;
			}
		}

		// CAEVehicleAudioEntity::Service
		Memory::callFunction<void>("_ZN21CAEVehicleAudioEntity7ServiceEv", &pVehicle->m_VehicleAudioEntity);

		CTrain_ProcessControl(pVehicle);
	}
}

DECL_HOOK(void, CTrailer_ProcessControl, sa::CVehicle* _this)
{
	sa::CVehicle* pVehicle = _this;
	byteInternalPlayer = *pbyteCurrentPlayer;

	if (pVehicle && pVehicle->pDriver && pVehicle->pDriver->m_nPedType == 0 && pVehicle->pDriver != GamePool_FindPlayerPed() && byteInternalPlayer == 0) {
		byteCurPlayer = FindPlayerNumFromPedPtr(pVehicle->pDriver);

		// save the internal cammode, apply the context.
		byteSavedCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);

		// save the second internal cammode, apply the context.
		wSavedCameraMode2 = *wCameraMode2;
		*wCameraMode2 = GameGetPlayerCameraMode(byteCurPlayer);
		if (*wCameraMode2 == 4) *wCameraMode2 = 0;

		// aim switching.
		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(byteCurPlayer);

		// Tyre burst fix
		if (pVehicle->pDriver) {
			if (pVehicle->m_nVehicleFlags.bTyresDontBurst) {
				pVehicle->m_nVehicleFlags.bTyresDontBurst = 0;
			}
			if (!pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = true;
		}
		else {
			if (!pVehicle->m_nVehicleFlags.bTyresDontBurst) {
				pVehicle->m_nVehicleFlags.bTyresDontBurst = 1;
			}
			if (pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = false;
		}

		*pbyteCurrentPlayer = 0;

		pVehicle->pDriver->m_nPedType = sa::PEDTYPE_CIVMALE; // So CPed::IsPlayer returns FALSE
		savedControlFlags = pVehicle->nControlFlags;
		pVehicle->nControlFlags = 26;

		// CAEVehicleAudioEntity::Service
		// Memory::callFunction<void>("_ZN21CAEVehicleAudioEntity7ServiceEv", &pVehicle->m_VehicleAudioEntity);

		pVehicle->nControlFlags = savedControlFlags;
		pVehicle->pDriver->m_nPedType = sa::PEDTYPE_PLAYER1;

		// set the internal player to the passed actor.
		*pbyteCurrentPlayer = byteCurPlayer;

		CTrailer_ProcessControl(pVehicle);

		// restore the local player's internal ID.
		*pbyteCurrentPlayer = 0;

		// restore the camera modes.
		*pbyteCameraMode = byteSavedCameraMode;
		*wCameraMode2 = wSavedCameraMode2;

		// restore aim switching.
		GameSetLocalPlayerAim();
	}
	else {
		if (pVehicle && pVehicle->pDriver && pVehicle->pDriver->m_nPedType == 0 && GamePool_FindPlayerPed() == pVehicle->pDriver) {
			// Tyre burst fix
			if (pVehicle->pDriver) {
				if (pVehicle->m_nVehicleFlags.bTyresDontBurst) {
					pVehicle->m_nVehicleFlags.bTyresDontBurst = 0;
				}
				if (!pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = true;
			}
			else {
				if (!pVehicle->m_nVehicleFlags.bTyresDontBurst) {
					pVehicle->m_nVehicleFlags.bTyresDontBurst = 1;
				}
				if (pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = false;
			}
		}

		// CAEVehicleAudioEntity::Service
		// Memory::callFunction<void>("_ZN21CAEVehicleAudioEntity7ServiceEv", &pVehicle->m_VehicleAudioEntity);

		CTrailer_ProcessControl(pVehicle);
	}
}

bool NotifyEnterVehicle(sa::CVehicle* _pVehicle)
{
	spdlog::info("NotifyEnterVehicle: 0x{:X}", reinterpret_cast<uintptr_t>(_pVehicle));

	if (!pNetGame) {
		return false;
	}

	VEHICLEID vehicleId;

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (pVehiclePool) {
		vehicleId = pVehiclePool->FindIDFromGtaPtr(_pVehicle);
		if (vehicleId == INVALID_VEHICLE_ID) {
			return false;
		}

		if (!pVehiclePool->GetSlotState(vehicleId)) {
			return false;
		}

		CVehicle* pVehicle = pVehiclePool->GetAt(vehicleId);
		if (pVehicle) {
			if (pVehicle->GetDoorState()) {
				return false;
			}

			if (pVehicle->m_pVehicle->m_nModelIndex == TRAIN_PASSENGER) {
				return false;
			}

			if (pVehicle->m_pVehicle->pDriver && pVehicle->m_pVehicle->pDriver->m_nPedType != 0) {
				return false;
			}
		}
	}

	CLocalPlayer* pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
	if (pLocalPlayer) {
		if (pLocalPlayer->GetPlayerPed() && pLocalPlayer->GetPlayerPed()->GetCurrentWeapon() == WEAPON_PARACHUTE) {
			pLocalPlayer->GetPlayerPed()->SetArmedWeapon(0);
		}

		pLocalPlayer->SendEnterVehicleNotification(vehicleId, false);
		return true;
	}

	return false;
}

DECL_HOOK(void, CTaskComplexEnterCarAsDriver, uintptr_t** _this, sa::CVehicle* pVehicle)
{
	NotifyEnterVehicle(pVehicle);
	CTaskComplexEnterCarAsDriver(_this, pVehicle);
}

DECL_HOOK(void, CTaskComplexLeaveCar, uintptr_t** _this, sa::CVehicle* pVehicle, int iTargetDoor, int iDelayTime, bool bSensibleLeaveCar, bool bForceGetOut)
{
	if (pNetGame) {
		if (GamePool_FindPlayerPed()->pVehicle == pVehicle) {
			CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
			VEHICLEID VehicleID = pVehiclePool->FindIDFromGtaPtr(GamePool_FindPlayerPed()->pVehicle);
			if (VehicleID != INVALID_VEHICLE_ID) {
				CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
				CLocalPlayer* pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
				if (pVehicle && pLocalPlayer) {
					if (pVehicle->IsATrainPart()) {
						RwMatrix mat = pVehicle->m_pVehicle->GetMatrix().ToRwMatrix();
						pLocalPlayer->GetPlayerPed()->RemoveFromVehicleAndPutAt(mat.pos.x + 2.5f, mat.pos.y + 2.5f, mat.pos.z);
					}
					else {
						pLocalPlayer->SendExitVehicleNotification(VehicleID);
					}
				}
			}
		}
	}

	CTaskComplexLeaveCar(_this, pVehicle, iTargetDoor, iDelayTime, bSensibleLeaveCar, bForceGetOut);
}

DECL_HOOK(uint32_t, CTaskSimpleUseGun_SetPedPosition, uintptr_t _this, sa::CPed* ped)
{
	dwCurPlayerActor = ped;
	byteInternalPlayer = *pbyteCurrentPlayer;
	byteCurPlayer = FindPlayerNumFromPedPtr(ped);

	if (dwCurPlayerActor && (byteCurPlayer != 0) && byteInternalPlayer == 0) // remote player
	{
		// save the internal mode, apply the context.
		byteSavedCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);

		// save the second internal cammode, apply the context.
		wSavedCameraMode2 = *wCameraMode2;
		*wCameraMode2 = GameGetPlayerCameraMode(byteCurPlayer);
		if (*wCameraMode2 == 4) *wCameraMode2 = 0;

		// save the camera zoom factor, apply the context.
		GameStoreLocalPlayerCameraExtZoomAndAspect();
		GameSetRemotePlayerCameraExtZoomAndAspect(byteCurPlayer);

		// aim switching.
		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(byteCurPlayer);

		// weapon skills.
		//GameStoreLocalPlayerSkills();
		//GameSetRemotePlayerSkills(byteCurPlayer);

		// set the internal player to the passed actor.
		*pbyteCurrentPlayer = byteCurPlayer;

		CTaskSimpleUseGun_SetPedPosition(_this, ped);

		// restore weapon skills.
		//GameSetLocalPlayerSkills();

		// restore the camera modes.
		*pbyteCameraMode = byteSavedCameraMode;
		*wCameraMode2 = wSavedCameraMode2;

		// remote the local player's camera zoom factor
		GameSetLocalPlayerCameraExtZoomAndAspect();

		// restore the local player's internal ID.
		*pbyteCurrentPlayer = 0;

		// restore aim switching.
		GameSetLocalPlayerAim();
	}
	else // localplayer
	{
		CTaskSimpleUseGun_SetPedPosition(_this, ped);
	}

	return 0;
}

DECL_HOOK(uint32_t, CPad_GetWeapon, uintptr_t _this, sa::CPed* pPed, bool bShoulderPossible)
{
	if (*pbyteCurrentPlayer) {
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE] ? 0xFF : 0x00;
	}
	else {
		uint32_t dwResult = CPad_GetWeapon(_this, pPed, bShoulderPossible);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = dwResult;
		return dwResult;
	}
}

DECL_HOOK(bool, CPad_CycleWeaponLeftJustDown, uintptr_t _this)
{
	if (*pbyteCurrentPlayer) {
		return false;
	}
	else {
		bool isPressed = CPad_CycleWeaponLeftJustDown(_this);
		if (isPressed) {
			spdlog::info("CycleWeaponLeftJustDown isPressed");
		}
		return isPressed;
	}
}

DECL_HOOK(uint32_t, CPad_CycleWeaponRightJustDown, uintptr_t _this)
{
	if (*pbyteCurrentPlayer) {
		return 0;
	}
	else {
		return CPad_CycleWeaponRightJustDown(_this);
	}
}

DECL_HOOK(uint32_t, CPad_GetEnterTargeting, uintptr_t _this)
{
	if (*pbyteCurrentPlayer) {
		return 0;
	}
	else {
		return CPad_GetEnterTargeting(_this);
	}
}

DECL_HOOK(uint32_t, CCamera_IsTargetingActive, uintptr_t _this, sa::CPed* pPed)
{
	//if (*pbyteCurrentPlayer)
	if (pPed != GamePool_FindPlayerPed()) {
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE] ? 1 : 0;
	}
	else {
		/* CCamera::IsTargetingActive */
		sa::CCamera* TheCamera = &sa::getTheCamera();
		bool bIsTargeting = Memory::callFunction<bool>("_ZN7CCamera17IsTargetingActiveEv", TheCamera);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = bIsTargeting;
		return bIsTargeting;
	}
}

RwMatrix* RwMatrixOrthoNormalize(RwMatrix* matrixOut, const RwMatrix* matrixIn)
{
	return Memory::callFunction<RwMatrix*>("_Z22RwMatrixOrthoNormalizeP11RwMatrixTagPKS_", matrixOut, matrixIn);
}

void SendBulletSync(sa::CVector* vecOrigin, sa::CVector* vecPos, sa::CEntity** ppEntity)
{
	static BULLET_DATA bulletData;
	memset(&bulletData, 0, sizeof(BULLET_DATA));

	bulletData.vecOrigin.x = vecOrigin->x;
	bulletData.vecOrigin.y = vecOrigin->y;
	bulletData.vecOrigin.z = vecOrigin->z;
	bulletData.vecPos.x = vecPos->x;
	bulletData.vecPos.y = vecPos->y;
	bulletData.vecPos.z = vecPos->z;

	if (ppEntity) {
		static sa::CEntity* pEntity;
		pEntity = *ppEntity;
		if (pEntity) {
			if (pEntity->m_matrix) {
				if (g_iLagCompensationMode) {
					bulletData.vecOffset.x = vecPos->x - pEntity->m_matrix->m_pos.x;
					bulletData.vecOffset.y = vecPos->y - pEntity->m_matrix->m_pos.y;
					bulletData.vecOffset.z = vecPos->z - pEntity->m_matrix->m_pos.z;
				}
				else {
					static RwMatrix mat1;
					memset(&mat1, 0, sizeof(mat1));

					static RwMatrix mat2;
					memset(&mat2, 0, sizeof(mat2));

					RwMatrix entityMat = pEntity->m_matrix->ToRwMatrix();
					RwMatrixOrthoNormalize(&mat2, &entityMat);
					RwMatrixInvert(&mat1, &mat2);
					ProjectMatrix(&bulletData.vecOffset, reinterpret_cast<sa::CMatrix*>(&mat1), vecPos);
				}
			}

			bulletData.pEntity = pEntity;
		}
	}

	pGame->FindPlayerPed()->ProcessBulletData(&bulletData);
}

DECL_HOOK(uint32_t, CWeapon_FireInstantHit, sa::CWeapon* _this, sa::CPed* pFiringEntity, sa::CVector* vecOrigin, sa::CVector* muzzlePosn, sa::CEntity* targetEntity, sa::CVector* target, sa::CVector* originForDriveBy, bool arg6, bool muzzle)
{
	GlossHookEnable(ghook_CWorld_ProcessLineOfSight);

	if (pFiringEntity != GamePool_FindPlayerPed()) {
		return muzzle;
	}

	if (pNetGame) {
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		if (pPlayerPool) {
			pPlayerPool->ApplyCollisionChecking();
		}
	}

	if (pGame) {
		CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
		if (pPlayerPed) {
			pPlayerPed->FireInstant();
		}
	}

	if (pNetGame) {
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		if (pPlayerPool) {
			pPlayerPool->ResetCollisionChecking();
		}
	}

	GlossHookDisable(ghook_CWorld_ProcessLineOfSight);
	return muzzle;
}

DECL_HOOK(uint32_t, CWeapon_FireInstantHit_full, sa::CWeapon* _this, sa::CPed* pFiringEntity, sa::CVector* vecOrigin, sa::CVector* muzzlePosn, sa::CEntity* targetEntity, sa::CVector* target, sa::CVector* originForDriveBy, bool arg6, bool muzzle)
{
	GlossHookEnable(ghook_CWorld_ProcessLineOfSight);
	return CWeapon_FireInstantHit_full(_this, pFiringEntity, vecOrigin, muzzlePosn, targetEntity, target, originForDriveBy, arg6, muzzle);
	GlossHookDisable(ghook_CWorld_ProcessLineOfSight);
}

DECL_HOOK(uint32_t, CWeapon_FireSniper, sa::CWeapon* _this, sa::CPed* pFiringEntity, sa::CEntity* victim, sa::CVector* target)
{
	if (pFiringEntity == GamePool_FindPlayerPed()) {
		if (pGame) {
			CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
			if (pPlayerPed) {
				pPlayerPed->FireInstant();
			}
		}
	}

	return true;
}

DECL_HOOK(uint32_t, CWorld_ProcessLineOfSight, sa::CVector* vecOrigin, sa::CVector* vecEnd, sa::CColPoint* colPoint, sa::CEntity** ppEntity, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7, bool b8)
{
	static sa::CVector vecPosPlusOffset;

	if (g_iLagCompensationMode != 2) {
		if (g_pCurrentFiredPed != pGame->FindPlayerPed()) {
			if (g_pCurrentBulletData && g_pCurrentBulletData->pEntity) {
				if (!IsEntityPlaceable(g_pCurrentBulletData->pEntity)) {
					if (g_iLagCompensationMode) {
						vecPosPlusOffset.x = g_pCurrentBulletData->pEntity->m_matrix->m_pos.x + g_pCurrentBulletData->vecOffset.x;
						vecPosPlusOffset.y = g_pCurrentBulletData->pEntity->m_matrix->m_pos.y + g_pCurrentBulletData->vecOffset.y;
						vecPosPlusOffset.z = g_pCurrentBulletData->pEntity->m_matrix->m_pos.z + g_pCurrentBulletData->vecOffset.z;
					}
					else {
						if (g_pCurrentBulletData->pEntity->m_matrix && g_pCurrentBulletData->vecOffset != nullptr) {
							if (g_pCurrentBulletData->pEntity->m_matrix->m_up != nullptr &&
									g_pCurrentBulletData->pEntity->m_matrix->m_forward != nullptr &&
									g_pCurrentBulletData->pEntity->m_matrix->m_right != nullptr &&
									g_pCurrentBulletData->pEntity->m_matrix->m_pos != nullptr) {
								ProjectMatrix(&vecPosPlusOffset, g_pCurrentBulletData->pEntity->m_matrix, &g_pCurrentBulletData->vecOffset);
							}
						}
					}

					vecEnd->x = vecPosPlusOffset.x - vecOrigin->x + vecPosPlusOffset.x;
					vecEnd->y = vecPosPlusOffset.y - vecOrigin->y + vecPosPlusOffset.y;
					vecEnd->z = vecPosPlusOffset.z - vecOrigin->z + vecPosPlusOffset.z;
				}
			}
		}
	}

	uint32_t result = CWorld_ProcessLineOfSight(vecOrigin, vecEnd, colPoint, ppEntity, b1, b2, b3, b4, b5, b6, b7, b8);

	if (g_iLagCompensationMode == 2) {
		if (g_pCurrentFiredPed == pGame->FindPlayerPed()) {
			SendBulletSync(vecOrigin, &colPoint->m_vecPoint, ppEntity);
		}
		return result;
	}

	if (g_pCurrentFiredPed) {
		if (g_pCurrentFiredPed != pGame->FindPlayerPed()) {
			if (g_pCurrentBulletData) {
				if (g_pCurrentBulletData->pEntity == nullptr) {
					sa::CPed* pLocalPed = GamePool_FindPlayerPed();
					if (*ppEntity == GamePool_FindPlayerPed() || pLocalPed->IsInVehicle() && *ppEntity == pLocalPed->pVehicle) {
						result = 0;
						*ppEntity = nullptr;
						colPoint->m_vecPoint.x = 0.0f;
						colPoint->m_vecPoint.y = 0.0f;
						colPoint->m_vecPoint.z = 0.0f;
						return result;
					}
				}
			}
		}
		else {
			SendBulletSync(vecOrigin, &colPoint->m_vecPoint, ppEntity);
		}
	}

	return result;
}

DECL_HOOK(sa::CEntity *, CBulletInfo_Update, uintptr_t _this)
{
	GlossHookEnable(ghook_CWorld_ProcessLineOfSight);
	sa::CEntity* entity = CBulletInfo_Update(_this);
	GlossHookDisable(ghook_CWorld_ProcessLineOfSight);
	return entity;
}

#pragma pack(push, 1)
struct CPedDamageResponseCalculator {
	sa::CPed* m_pInflictor;
	float m_fRawDamage;
	int32_t m_eHitZone;
	int32_t m_eWeaponUsed;
	bool m_bJumpedOutOfMovingCar;
	uint8_t pad[3];
};
#pragma pack(pop)
VALIDATE_SIZE(CPedDamageResponseCalculator, (VER_x32 ? 0x14 : 0x18));

PLAYERID FindPlayerIDFromGtaPtr(sa::CEntity* pEntity)
{
	if (pEntity == nullptr) return INVALID_PLAYER_ID;

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();

	PLAYERID PlayerID = pPlayerPool->FindRemotePlayerIDFromGtaPtr((sa::CPed*) pEntity);
	if (PlayerID != INVALID_PLAYER_ID) return PlayerID;

	VEHICLEID VehicleID = pVehiclePool->FindIDFromGtaPtr((sa::CVehicle*) pEntity);
	if (VehicleID != INVALID_VEHICLE_ID) {
		for (PLAYERID i = 0; i < MAX_PLAYERS; i++) {
			CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(i);
			if (pRemotePlayer && pRemotePlayer->CurrentVehicleID() == VehicleID) {
				return i;
			}
		}
	}

	return INVALID_PLAYER_ID;
}

PLAYERID FindActorIDFromGtaPtr(sa::CPed* pPed)
{
	if (pPed) {
		return pNetGame->GetActorPool()->FindIDFromGtaPtr(pPed);
	}

	return INVALID_PLAYER_ID;
}

bool ComputeDamageResponse(CPedDamageResponseCalculator* calculator, sa::CPed* pPed)
{
	sa::CPed* pGamePed = GamePool_FindPlayerPed();
	bool isLocalPed = false;

	if (!pNetGame) {
		return false;
	}

	sa::CPed* pDamager = calculator->m_pInflictor;
	if (pDamager != pGamePed && (pPed && *(uintptr_t*) pPed == g_saSym->Abs<uintptr_t>(VER_x32 ? 0x668A94 : 0x831BA8))) { /* CCivilianPed */
		return true;
	}

	if (pPed == pGamePed) {
		isLocalPed = true;
	}

	else if (pDamager != pGamePed) {
		return false;
	}

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	CLocalPlayer* pLocalPlayer = pPlayerPool->GetLocalPlayer();
	PLAYERID PlayerID;

	if (isLocalPed) {
		auto pSurfingVehicle = reinterpret_cast<CVehicle*>(pPlayerPool->GetLocalPlayer()->m_surfSync.pSurfInst);
		if (pSurfingVehicle && pSurfingVehicle->m_entity == (sa::CEntity*) pDamager && calculator->m_eWeaponUsed == WEAPON_COLLISION) {
			return true;
		}

		auto pSurfingObject = reinterpret_cast<CObject*>(pPlayerPool->GetLocalPlayer()->m_surfSync.pSurfInst);
		if (pSurfingObject && pSurfingObject->m_entity == (sa::CEntity*) pDamager && calculator->m_eWeaponUsed == WEAPON_COLLISION) {
			return true;
		}

		PlayerID = FindPlayerIDFromGtaPtr(pDamager);
		pLocalPlayer->SendTakeDamageEvent(PlayerID, calculator->m_fRawDamage, calculator->m_eWeaponUsed, calculator->m_eHitZone);
	}
	else {
		PlayerID = FindPlayerIDFromGtaPtr(pPed);
		if (PlayerID != INVALID_PLAYER_ID) {
			pLocalPlayer->SendGiveDamageEvent(PlayerID, calculator->m_fRawDamage, calculator->m_eWeaponUsed, calculator->m_eHitZone);
			if (pPlayerPool->GetAt(PlayerID)->IsNPC()) {
				return true;
			}
		}
		else {
			PLAYERID ActorID = FindActorIDFromGtaPtr(pPed);
			if (ActorID != INVALID_PLAYER_ID) {
				pLocalPlayer->SendGiveDamageActorEvent(ActorID, calculator->m_fRawDamage, calculator->m_eWeaponUsed, calculator->m_eHitZone);
				return true;
			}
		}
	}

	// :check_friendly_fire
	if (!pNetGame->m_netSet->friendlyFire) {
		return false;
	}

	uint8_t byteTeam = pPlayerPool->GetLocalPlayer()->m_byteTeam;
	if (byteTeam == NO_TEAM || PlayerID == INVALID_PLAYER_ID || pPlayerPool->GetAt(PlayerID)->m_byteTeam != byteTeam) {
		return false;
	}

	return true;
}

DECL_HOOK(void, CPedDamageResponseCalculator_ComputeDamageResponse, CPedDamageResponseCalculator* _this, sa::CPed* pVictim, uintptr_t damageResponse, const bool bTriggerPainAudio)
{
	if (_this != nullptr && pVictim != nullptr) {
		ComputeDamageResponse(_this, pVictim);
	}

	return CPedDamageResponseCalculator_ComputeDamageResponse(_this, pVictim, damageResponse, bTriggerPainAudio);
}

DECL_HOOK(uint32_t, CBulletInfo_AddBullet, sa::CEntity* creator, int weaponType, sa::CVector pos, sa::CVector velocity)
{
	velocity.x *= 50.0f;
	velocity.y *= 50.0f;
	velocity.z *= 50.0f;

	CBulletInfo_AddBullet(creator, weaponType, pos, velocity);

	// CBulletInfo::Update
	Memory::callFunction("_ZN11CBulletInfo6UpdateEv");
	return true;
}

void Hooks::installPadHooks()
{
	spdlog::info("Installing pad hooks...");

	memset(&LocalPlayerKeys, 0, sizeof(PAD_KEYS));

	// playerped
	HOOK("_ZN10CPlayerPed14ProcessControlEv", CPed_ProcessControl);

	// vehicles
	HOOK("_ZN11CAutomobile14ProcessControlEv", CAutomobile_ProcessControl);
	HOOK("_ZN5CBoat14ProcessControlEv", CBoat_ProcessControl);
	HOOK("_ZN5CBike14ProcessControlEv", CBike_ProcessControl);
	HOOK("_ZN6CPlane14ProcessControlEv", CPlane_ProcessControl);
	HOOK("_ZN5CHeli14ProcessControlEv", CHeli_ProcessControl);
	HOOK("_ZN4CBmx14ProcessControlEv", CBmx_ProcessControl);
	HOOK("_ZN13CMonsterTruck14ProcessControlEv", CMonsterTruck_ProcessControl);
	HOOK("_ZN9CQuadBike14ProcessControlEv", CQuadBike_ProcessControl);
	HOOK("_ZN6CTrain14ProcessControlEv", CTrain_ProcessControl);
	HOOK("_ZN8CTrailer14ProcessControlEv", CTrailer_ProcessControl);

	HOOK("_ZN4CPad19GetPedWalkLeftRightEv", CPad_GetPedWalkLeftRight);
	HOOK("_ZN4CPad16GetPedWalkUpDownEv", CPad_GetPedWalkUpDown);

	HOOK("_ZN4CPad9GetSprintEi", CPad_GetSprint);

	HOOK("_ZN4CPad7GetJumpEv", CPad_GetJump);
	HOOK("_ZN4CPad12JumpJustDownEv", CPad_JumpJustDown);

	HOOK("_ZN4CPad7GetDuckEv", CPad_GetDuck);
	HOOK("_ZN4CPad12DuckJustDownEP4CPed", CPad_DuckJustDown);

	HOOK("_ZN4CPad19MeleeAttackJustDownEv", CPad_MeleeAttackJustDown);
	HOOK("_ZN4CPad8GetBlockEv", CPad_GetBlock);

#if VER_x32
	HOOK_ADDR(g_saSym->Abs(0x3FBBB8), CPad_GetAbortClimb, true);
#else
	HOOK("_ZN4CPad13GetAbortClimbEv", CPad_GetAbortClimb);
#endif
	HOOK("_ZN4CPad12GetAutoClimbEv", CPad_GetAutoClimb);

	HOOK("_ZN4CPad16SwimJumpJustDownEv", CPad_SwimJumpJustDown);
	HOOK("_ZN4CPad12DiveJustDownEv", CPad_DiveJustDown);

	HOOK("_ZN4CPad17GetSteeringUpDownEv", CPad_GetSteeringUpDown);
	HOOK("_ZN4CPad20GetSteeringLeftRightEv", CPad_GetSteeringLeftRight);

	HOOK("_ZN4CPad12GetHandBrakeEv", CPad_GetHandBrake);
	HOOK("_ZN4CPad8GetBrakeEv", CPad_GetBrake);
	HOOK("_ZN4CPad13GetAccelerateEv", CPad_GetAccelerate);
	HOOK("_ZN4CPad7GetHornEb", CPad_GetHorn);

	HOOK("_ZN4CPad19ExitVehicleJustDownEbP8CVehiclebRK7CVector", CPad_ExitVehicleJustDown);

	HOOK("_ZN4CPad14GetTurretRightEv", CPad_GetTurretRight);
	HOOK("_ZN4CPad13GetTurretLeftEv", CPad_GetTurretLeft);

	HOOK("_ZN4CPad16GetHydraulicJumpEv", CPad_GetHydraulicJump);
	HOOK("_ZN4CPad13GetNitroFiredEv", CPad_GetNitroFired);

	Memory::hookBL(g_saSym->Abs(addr::CTaskComplexEnterCarAsDriver_Branch1), (void*) &CTaskComplexEnterCarAsDriver_hook, (void**) &CTaskComplexEnterCarAsDriver);
	Memory::hookBL(g_saSym->Abs(addr::CTaskComplexLeaveCar_Branch1), (void*) &CTaskComplexLeaveCar_hook, (void**) &CTaskComplexLeaveCar);
	Memory::hookBL(g_saSym->Abs(addr::CTaskComplexLeaveCar_Branch2), (void*) &CTaskComplexLeaveCar_hook, (void**) &CTaskComplexLeaveCar);

	HOOK("_ZN17CTaskSimpleUseGun14SetPedPositionEP4CPed", CTaskSimpleUseGun_SetPedPosition);

	HOOK("_ZN4CPad24CycleWeaponRightJustDownEv", CPad_CycleWeaponRightJustDown);
	HOOK("_ZN4CPad23CycleWeaponLeftJustDownEv", CPad_CycleWeaponLeftJustDown);
	HOOK("_ZN4CPad9GetWeaponEP4CPedb", CPad_GetWeapon);
	HOOK("_ZN7CCamera17IsTargetingActiveEP10CPlayerPed", CCamera_IsTargetingActive);
	HOOK("_ZN4CPad17GetEnterTargetingEv", CPad_GetEnterTargeting);

	Memory::hookBL(g_saSym->Abs(addr::CWeapon_FireInstantHit_branch1), (void*) &CWeapon_FireInstantHit_hook, (void**) &CWeapon_FireInstantHit);
	Memory::hookBL(g_saSym->Abs(addr::CWeapon_FireInstantHit_branch2), (void*) &CWeapon_FireInstantHit_hook, (void**) &CWeapon_FireInstantHit);

	HOOK("_ZN7CWeapon14FireInstantHitEP7CEntityP7CVectorS3_S1_S3_S3_bb", CWeapon_FireInstantHit_full);
	HOOK("_ZN7CWeapon10FireSniperEP4CPedP7CEntityP7CVector", CWeapon_FireSniper);

	ghook_CWorld_ProcessLineOfSight = HOOK("_ZN6CWorld18ProcessLineOfSightERK7CVectorS2_R9CColPointRP7CEntitybbbbbbbb", CWorld_ProcessLineOfSight);
	GlossHookDisable(ghook_CWorld_ProcessLineOfSight); // disable by default

	HOOK("_ZN11CBulletInfo6UpdateEv", CBulletInfo_Update);
	HOOK("_ZN28CPedDamageResponseCalculator21ComputeDamageResponseEP4CPedR18CPedDamageResponseb", CPedDamageResponseCalculator_ComputeDamageResponse);
	HOOK("_ZN11CBulletInfo9AddBulletEP7CEntity11eWeaponType7CVectorS3_", CBulletInfo_AddBullet);

}
