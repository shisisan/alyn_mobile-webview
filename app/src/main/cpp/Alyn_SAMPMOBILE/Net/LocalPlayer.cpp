#include "../Client.h"
#include "../Game/Game.h"
#include "../Net/NetGame.h"
#include "../UI/UI.h"

extern UI* pUI;
extern Game* pGame;
extern NetGame* pNetGame;

extern int g_iLagCompensationMode;
extern int iNetModeNormalOnFootSendRate;
extern int iNetModeNormalInCarSendRate;
extern int iNetModeFiringSendRate;
extern int iNetModeSendMultiplier;

uint32_t dwEnterVehTimeElasped = -1;
bool bFirstSpawn = true;
// bool g_bLockEnterVehicleWidget = false;

CLocalPlayer::CLocalPlayer()
{
	m_bInRCMode = false;

	m_pPlayerPed = pGame->FindPlayerPed();
	m_bIsActive = false;
	m_bIsWasted = false;

	m_iDisplayZoneTick = 0;
	m_dwLastSendTick = GetTickCount();
	m_dwLastSendSpecTick = GetTickCount();
	m_dwLastSendSyncTick = GetTickCount();
	m_dwLastSendAimSyncTick = GetTickCount();
	m_dwLastStatsUpdateTick = GetTickCount();
	m_bIsSpectating = false;
	m_byteSpectateType = SPECTATE_TYPE_NONE;
	m_SpectateID = 0xFFFFFFFF;
	m_bSpawnDialogShowed = false;
	ResetAllSyncAttributes();

	m_statsSync.dwLastMoney = 0;
	m_statsSync.dwLastDrunkLevel = 0;

	m_surfSync.bIsActive = false;
	m_surfSync.pSurfInst = 0;
	m_surfSync.bIsVehicle = false;
	m_surfSync.vecOffsetPos = sa::CVector{0.0f, 0.0f, 0.0f};

	for (int i = 0; i < 13; i++) {
		m_byteLastWeapon[i] = 0;
		m_dwLastAmmo[i] = 0;
	}

	m_byteTeam = NO_TEAM;
}

CLocalPlayer::~CLocalPlayer()
{
	// ~
}

bool CLocalPlayer::Process()
{
	uint32_t dwThisTick;

	if (m_bIsActive && m_pPlayerPed != nullptr) {
		// local player is dead
		if (!m_bIsWasted && m_pPlayerPed->GetActionTrigger() == ACTION_DEATH || m_pPlayerPed->IsDead()) {
			ToggleSpectating(false);

			if (m_pPlayerPed->IsDancing()) {
				m_pPlayerPed->StopDancing();
			}

			if (m_pPlayerPed->IsCellphoneEnabled()) {
				m_pPlayerPed->ToggleCellphone(false);
			}

			if (m_pPlayerPed->IsPissing()) {
				m_pPlayerPed->TogglePissing(false);
			}

			if (m_pPlayerPed->IsCuffed()) {
				m_pPlayerPed->ToggleCuffed(false);
			}

			if (m_pPlayerPed->IsCarry()) {
				m_pPlayerPed->ToggleCarry(false);
			}

			if (m_pPlayerPed->IsInJetpack()) {
				m_pPlayerPed->StopJetpack();
			}

			m_pPlayerPed->ExtinguishFire();

			// reset tasks/anims
			m_pPlayerPed->TogglePlayerControllable(true);

			if (m_bInRCMode) {
				m_bInRCMode = false;
				m_pPlayerPed->Add();
			}

			if (m_pPlayerPed->IsInVehicle() && !m_pPlayerPed->IsAPassenger()) {
				SendInCarFullSyncData(); // for explosion
				m_LastVehicle = pNetGame->GetVehiclePool()->FindIDFromGtaPtr(m_pPlayerPed->GetGtaVehicle());
			}

			m_pPlayerPed->ExtinguishFire();
			SendWastedNotification();
			m_bIsActive = false;
			m_bIsWasted = true;
			pGame->EnableZoneNames(false);
			return true;
		}

		uint16_t wKeys, lrAnalog, udAnalog;
		uint8_t extKeys;
		wKeys = m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog, &extKeys, false);

		// HANDLE DANCING LOCAL PED
		if (m_pPlayerPed->IsDancing()) {
			m_pPlayerPed->ProcessDancing();
			if (LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] || m_pPlayerPed->IsInVehicle()) {
				m_pPlayerPed->StopDancing();
			}
		}

		// HANDLE I GOT MY HANDS UP BUT DON'T WANT TO ANYMORE
		if (m_pPlayerPed->HasHandsUp()) {
			if (LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] || m_pPlayerPed->IsInVehicle()) {
				m_pPlayerPed->TogglePlayerControllable(true);
			}
		}

		// HANDLE CELLPHONE LOCAL PED
		if (m_pPlayerPed->IsCellphoneEnabled()) {
			if (LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] || m_pPlayerPed->IsInVehicle()) {
				m_pPlayerPed->ToggleCellphone(false);
			}
		}

		// HANDLE PISSING LOCAL PED
		if (m_pPlayerPed->IsPissing()) {
			if (LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] || m_pPlayerPed->IsInVehicle()) {
				m_pPlayerPed->TogglePissing(false);
			}
		}

		// HANDLE CUFF LOCAL PED
		if (m_pPlayerPed->IsCuffed()) {
			if (LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] || m_pPlayerPed->IsInVehicle()) {
				m_pPlayerPed->ToggleCuffed(false);
			}
		}

		// HANDLE CARRY LOCAL PED
		if (m_pPlayerPed->IsCarry()) {
			if (LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] || m_pPlayerPed->IsInVehicle()) {
				m_pPlayerPed->ToggleCarry(false);
			}
		}

		// HANDLE JETPACK LOCAL PED
		if (m_pPlayerPed->IsInJetpack()) {
			if (LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] || m_pPlayerPed->IsInVehicle()) {
				m_pPlayerPed->StopJetpack();
			}
		}

		m_pPlayerPed->DrunkProcess();

		m_pPlayerPed->SatisfyHunger();

		dwThisTick = GetTickCount();

		if (dwEnterVehTimeElasped != -1 && (dwThisTick - dwEnterVehTimeElasped) > 5000 && !m_pPlayerPed->IsInVehicle()) {
			pGame->GetCamera()->SetBehindPlayer();
			dwEnterVehTimeElasped = -1;
		}

		if (dwThisTick >= m_iDisplayZoneTick) {
			pGame->EnableZoneNames(pNetGame->m_netSet->zoneNames);
		}

		pGame->UpdateCheckpoints();

		if ((dwThisTick - m_dwLastStatsUpdateTick) > 1000) {
			SendStatsUpdate();
			m_dwLastStatsUpdateTick = dwThisTick;
		}

		CheckWeapons();

		UpdateSurfing();

		uint8_t byteInterior = pGame->GetActiveInterior();
		if (byteInterior != m_byteCurInterior) {
			UpdateRemoteInterior(byteInterior);
		}

		UpdateCameraTarget();

		// PLAYER DATA UPDATES
		if (m_bIsSpectating) {
			ProcessSpectating();
			m_bPassengerDriveByMode = false;
		}
			// DRIVER CONDITIONS
		else if (m_pPlayerPed->GetActionTrigger() == ACTION_INCAR && !m_pPlayerPed->IsAPassenger()) {
			// g_bLockEnterVehicleWidget = false;

			m_pPlayerPed->RemoveWeaponWhenEnteringVehicle();

			ProcessInCarWorldBounds();

			if ((dwThisTick - m_dwLastSendAimSyncTick) > 1000) {
				m_dwLastSendAimSyncTick = dwThisTick;
				SendAimSyncData();
			}

			CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
			CVehicle* pVehicle = nullptr;

			if (pVehiclePool) {
				m_CurrentVehicle = pVehiclePool->FindIDFromGtaPtr(m_pPlayerPed->GetGtaVehicle());
			}

			UpdateVehicleDamage(m_CurrentVehicle);

			pVehicle = pVehiclePool->GetAt(m_CurrentVehicle);
			if (pVehicle && !m_bInRCMode && pVehicle->IsRCVehicle()) {
				m_pPlayerPed->Remove();
				m_bInRCMode = true;
			}

			if (m_bInRCMode && !pVehicle) {
				m_pPlayerPed->SetHealth(0.0f);
				m_pPlayerPed->SetDead();
			}

			if (m_bInRCMode && pVehicle && pVehicle->GetHealth() == 0.0f) {
				m_pPlayerPed->SetHealth(0.0f);
				m_pPlayerPed->SetDead();
			}

			if ((dwThisTick - m_dwLastSendTick) > GetOptimumInCarSendRate()) {
				m_dwLastSendTick = GetTickCount();
				SendInCarFullSyncData();
			}

			m_bPassengerDriveByMode = false;
		}
			// ONFOOT CONDITIONS
		else if (m_pPlayerPed->GetActionTrigger() == ACTION_NORMAL || m_pPlayerPed->GetActionTrigger() == ACTION_SCOPE) {
			// g_bLockEnterVehicleWidget = true;

			ProcessSurfing();

			if (m_bInRCMode) {
				m_bInRCMode = false;
				m_pPlayerPed->Add();
			}

			ProcessOnFootWorldBounds();

			if (m_CurrentVehicle != 0xFFFF) {
				m_LastVehicle = m_CurrentVehicle;
				m_CurrentVehicle = 0xFFFF;
			}

			if (CompareOnFootSyncKeys(wKeys, udAnalog, lrAnalog) || (dwThisTick - m_dwLastSendTick) > GetOptimumOnFootSendRate()) {
				m_dwLastSendTick = GetTickCount();
				SendOnFootFullSyncData();
			}

			bool bWannaSync = false;
			if (wKeys & 0x84) {
				if (g_iLagCompensationMode == 2) {
					bWannaSync = (dwThisTick - m_dwLastSendAimSyncTick) > iNetModeFiringSendRate;
				}
				else {
					bWannaSync = (dwThisTick - m_dwLastSendAimSyncTick) > 100;
				}
			}
			else {
				bWannaSync = (dwThisTick - m_dwLastSendAimSyncTick) > 1000;
			}

			if (bWannaSync) {
				m_dwLastSendAimSyncTick = dwThisTick;
				SendAimSyncData();
			}

			m_bPassengerDriveByMode = false;
		}
			// PASSENGER CONDITIONS
		else if (m_pPlayerPed->GetActionTrigger() == ACTION_INCAR && m_pPlayerPed->IsAPassenger()) {
			// g_bLockEnterVehicleWidget = false;

			if (m_bInRCMode) {
				m_bInRCMode = false;
				m_pPlayerPed->Add();
			}

			// FOR ENTERING PASSENGER DRIVEBY MODE
			if (!m_bPassengerDriveByMode && LocalPlayerKeys.bKeys[ePadKeys::KEY_CTRL_BACK]) {
				if (m_pPlayerPed->StartPassengerDriveByMode()) {
					m_bPassengerDriveByMode = true;
				}
			}
			else if (m_bPassengerDriveByMode && LocalPlayerKeys.bKeys[ePadKeys::KEY_CTRL_BACK]) {
				m_pPlayerPed->StopPassengerDriveByMode();
				m_bPassengerDriveByMode = false;
			}

			if (dwThisTick - m_dwLastSendTick > GetOptimumInCarSendRate()) {
				m_dwLastSendTick = GetTickCount();
				SendPassengerFullSyncData();
			}
		}
	}

	// HANDLE !IsActive spectating
	if (m_bIsSpectating && !m_bIsActive) {
		if (m_bSpawnDialogShowed) {
			m_bSpawnDialogShowed = false;
			if (pUI) {
				pUI->spawn()->setVisible(false);
			}
		}

		ProcessSpectating();
		return true;
	}

	// HANDLE NEEDS TO RESPAWN AFTER DEATH
	if (m_bIsWasted && m_pPlayerPed->GetActionTrigger() != ACTION_WASTED && m_pPlayerPed->GetActionTrigger() != ACTION_DEATH) {
		if (m_pPlayerPed->HasAttachedObject()) {
			m_pPlayerPed->RemoveAllAttachedObjects();
		}

		if (IsClearedToSpawn() && pNetGame->GetGameState() == GAMESTATE_CONNECTED) {
			if (m_pPlayerPed->GetHealth() > 0.0f) {
				Spawn();
			}
		}
		else {
			m_bIsWasted = false;
			HandleClassSelection();
		}

		return true;
	}

	if (m_pPlayerPed->GetActionTrigger() != ACTION_WASTED && m_pPlayerPed->GetActionTrigger() != ACTION_DEATH && pNetGame->GetGameState() == GAMESTATE_CONNECTED && !m_bIsActive && !m_bIsSpectating) {
		ProcessClassSelection();
	}

	return true;
}

void CLocalPlayer::ProcessClassSelection()
{
	if (!m_bSpawnDialogShowed) {
		if (pUI) {
			pUI->spawn()->setVisible(true);
		}
		RequestClass(m_iSelectedClass);
		m_bSpawnDialogShowed = true;
	}
}

void CLocalPlayer::ResetAllSyncAttributes()
{
	m_bHasSpawnInfo = false;
	m_bWaitingForSpawnRequestReply = false;
	m_iSelectedClass = 0;
	m_byteCurInterior = 0;
	m_LastVehicle = 0xFFFF;
	m_bInRCMode = false;

	memset(&m_ofSync, 0, sizeof(ONFOOT_SYNC_DATA));
	memset(&m_icSync, 0, sizeof(INCAR_SYNC_DATA));
	memset(&m_psSync, 0, sizeof(PASSENGER_SYNC_DATA));
	memset(&m_trSync, 0, sizeof(TRAILER_SYNC_DATA));
	memset(&m_ucSync, 0, sizeof(UNOCCUPIED_SYNC_DATA));

	m_dwLastWeaponsUpdateTick = GetTickCount();
	m_byteCurrentWeapon = 0;
}

void CLocalPlayer::ToggleSpectating(bool bToggle)
{
	if (m_bIsSpectating && !bToggle) {
		Spawn();
	}

	m_bIsSpectating = bToggle;
	m_byteSpectateType = SPECTATE_TYPE_NONE;
	m_bSpectateProcessed = false;
	m_SpectateID = 0xFFFFFFFF;
}

void CLocalPlayer::ProcessSpectating()
{
	RakNet::BitStream bsSpectatorSync;
	SPECTATOR_SYNC_DATA spSync;
	RwMatrix matPos;

	uint16_t lrAnalog, udAnalog;
	uint8_t exKeys;
	uint16_t wKeys = m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog, &exKeys);
	pGame->GetCamera()->GetMatrix(&matPos);

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();

	if (!pPlayerPool || !pVehiclePool) {
		return;
	}

	spSync.vecPos.x = matPos.pos.x;
	spSync.vecPos.y = matPos.pos.y;
	spSync.vecPos.z = matPos.pos.z;
	spSync.lrAnalog = lrAnalog;
	spSync.udAnalog = udAnalog;
	spSync.wKeys = wKeys;

	if ((GetTickCount() - m_dwLastSendSpecTick) > GetOptimumOnFootSendRate()) {
		m_dwLastSendSpecTick = GetTickCount();
		bsSpectatorSync.Write((uint8_t) ID_SPECTATOR_SYNC);
		bsSpectatorSync.Write((char*) &spSync, sizeof(SPECTATOR_SYNC_DATA));
		pNetGame->GetRakClient()->Send(&bsSpectatorSync, HIGH_PRIORITY, UNRELIABLE, 0);

		if ((GetTickCount() - m_dwLastSendAimSyncTick) > (GetOptimumOnFootSendRate() * 2)) {
			m_dwLastSendAimSyncTick = GetTickCount();
			SendAimSyncData();
		}
	}

	pGame->DisplayHUD(false);

	m_pPlayerPed->SetHealth(100.0f);
	GetPlayerPed()->TeleportTo(spSync.vecPos.x, spSync.vecPos.y, spSync.vecPos.z + 20.0f);

	// handle spectate player left the server
	if (m_byteSpectateType == SPECTATE_TYPE_PLAYER && !pPlayerPool->GetSlotState(m_SpectateID)) {
		m_byteSpectateType = SPECTATE_TYPE_NONE;
		m_bSpectateProcessed = false;
	}

	// handle spectate player is no longer active (ie Died)
	if (m_byteSpectateType == SPECTATE_TYPE_PLAYER && pPlayerPool->GetSlotState(m_SpectateID) && (!pPlayerPool->GetAt(m_SpectateID)->IsActive() || pPlayerPool->GetAt(m_SpectateID)->GetState() == PLAYER_STATE_WASTED)) {
		m_byteSpectateType = SPECTATE_TYPE_NONE;
		m_bSpectateProcessed = false;
	}

	if (m_bSpectateProcessed) {
		return;
	}

	if (m_byteSpectateType == SPECTATE_TYPE_NONE) {
		GetPlayerPed()->RemoveFromVehicleAndPutAt(0.0f, 0.0f, 10.0f);
		pGame->GetCamera()->SetPosition(50.0f, 50.0f, 50.0f, 0.0f, 0.0f, 0.0f);
		pGame->GetCamera()->LookAtPoint(60.0f, 60.0f, 50.0f, 2);
		m_bSpectateProcessed = true;
	}
	else if (m_byteSpectateType == SPECTATE_TYPE_PLAYER) {
		uint32_t dwGTAId = 0;
		CPlayerPed* pPlayerPed = 0;

		if (pPlayerPool->GetSlotState(m_SpectateID)) {
			pPlayerPed = pPlayerPool->GetAt(m_SpectateID)->GetPlayerPed();
			if (pPlayerPed) {
				dwGTAId = pPlayerPed->m_gtaId;
				ScriptCommand(&camera_on_actor, dwGTAId, m_byteSpectateMode, 2);
				m_bSpectateProcessed = true;
			}
		}
	}
	else if (m_byteSpectateType == SPECTATE_TYPE_VEHICLE) {
		CVehicle* pVehicle = nullptr;
		uint32_t dwGTAId = 0;

		pVehicle = pVehiclePool->GetAt(m_SpectateID);
		if (pVehicle) {
			dwGTAId = pVehicle->m_gtaId;
			ScriptCommand(&camera_on_vehicle, dwGTAId, m_byteSpectateMode, 2);
			m_bSpectateProcessed = true;
		}
	}
}

bool CLocalPlayer::Spawn()
{
	if (!m_bHasSpawnInfo) {
		return false;
	}

	if (m_bSpawnDialogShowed) {
		m_bSpawnDialogShowed = false;
		if (pUI) {
			pUI->spawn()->setVisible(false);
		}
	}

	spdlog::info("Spawn localplayer");

	if (!g_saSym->GetSymbol("_ZZ20Menu_SwitchOffToGamevE12bInitWidgets")) {
		spdlog::info("Widgets not inited");
		Memory::callFunction("_Z20Menu_SwitchOffToGamev");
	}

	pGame->GetCamera()->Restore();
	pGame->GetCamera()->SetBehindPlayer();
	pGame->DisplayHUD(true);
	m_pPlayerPed->TogglePlayerControllable(true);

	if (!bFirstSpawn) {
		m_pPlayerPed->SetInitialState();
	}
	else {
		bFirstSpawn = false;
	}

	pGame->RefreshStreamingAt(m_SpawnInfo.vecPos.x, m_SpawnInfo.vecPos.y);

	if (m_pPlayerPed->IsCuffed()) {
		m_pPlayerPed->ToggleCuffed(false);
	}

	m_pPlayerPed->RestartIfWastedAt(&m_SpawnInfo.vecPos, m_SpawnInfo.fRotation);
	m_pPlayerPed->SetModelIndex(m_SpawnInfo.iSkin);
	m_pPlayerPed->ClearWeapons();
	m_pPlayerPed->ResetDamageEntity();

	ApplySpecialAction(0);

	if (m_SpawnInfo.iSpawnWeapons[2] != -1) {
		m_pPlayerPed->GiveWeapon(m_SpawnInfo.iSpawnWeapons[2], m_SpawnInfo.iSpawnWeaponsAmmo[2]);
	}

	if (m_SpawnInfo.iSpawnWeapons[1] != -1) {
		m_pPlayerPed->GiveWeapon(m_SpawnInfo.iSpawnWeapons[1], m_SpawnInfo.iSpawnWeaponsAmmo[1]);
	}

	if (m_SpawnInfo.iSpawnWeapons[0] != -1) {
		m_pPlayerPed->GiveWeapon(m_SpawnInfo.iSpawnWeapons[0], m_SpawnInfo.iSpawnWeaponsAmmo[0]);
	}

	pGame->DisableTrainTraffic();

	m_pPlayerPed->TeleportTo(m_SpawnInfo.vecPos.x, m_SpawnInfo.vecPos.y, m_SpawnInfo.vecPos.z + 0.5f);
	m_pPlayerPed->SetTargetRotation(m_SpawnInfo.fRotation);

	m_bIsWasted = false;
	m_bIsActive = true;
	m_bWaitingForSpawnRequestReply = false;

	m_iDisplayZoneTick = GetTickCount() + 1000;

	m_surfSync.bIsActive = false;

	RakNet::BitStream bsSendSpawn;
	pNetGame->GetRakClient()->RPC(&RPC_Spawn, &bsSendSpawn, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
	return true;
}

void CLocalPlayer::HandleClassSelection()
{
	m_bClearedToSpawn = false;
	if (m_pPlayerPed) {
		m_pPlayerPed->SetInitialState();
		m_pPlayerPed->SetHealth(100.0f);
		m_pPlayerPed->TogglePlayerControllable(false);
	}
}

void CLocalPlayer::SendWastedNotification()
{
	uint8_t byteDeathReason;
	PLAYERID WhoWasResponsible;
	RakNet::BitStream bsPlayerDeath;
	byteDeathReason = m_pPlayerPed->FindDeathReasonAndResponsiblePlayer(&WhoWasResponsible);
	bsPlayerDeath.Write(byteDeathReason);
	bsPlayerDeath.Write(WhoWasResponsible);
	pNetGame->GetRakClient()->RPC(&RPC_Death, &bsPlayerDeath, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}

void CLocalPlayer::SendOnFootFullSyncData()
{
	RakNet::BitStream bsPlayerSync;
	RwMatrix matPlayer;
	sa::CVector vecMoveSpeed;
	uint16_t lrAnalog, udAnalog;
	uint8_t exKeys;
	uint16_t wKeys = m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog, &exKeys);

	ONFOOT_SYNC_DATA ofSync;

	matPlayer = m_pPlayerPed->m_ped->GetMatrix().ToRwMatrix();
	vecMoveSpeed = m_pPlayerPed->m_ped->GetMoveSpeed();

	ofSync.lrAnalog = lrAnalog;
	ofSync.udAnalog = udAnalog;
	ofSync.wKeys = wKeys;

	ofSync.vecPos.x = matPlayer.pos.x;
	ofSync.vecPos.y = matPlayer.pos.y;
	ofSync.vecPos.z = matPlayer.pos.z;

	ofSync.quat.SetFromMatrix(&matPlayer);
	ofSync.quat.Normalize();

	if (FloatOffset(ofSync.quat.w, m_ofSync.quat.w) < 0.00001 &&
			FloatOffset(ofSync.quat.x, m_ofSync.quat.x) < 0.00001 &&
			FloatOffset(ofSync.quat.y, m_ofSync.quat.y) < 0.00001 &&
			FloatOffset(ofSync.quat.z, m_ofSync.quat.z) < 0.00001) {
		ofSync.quat.Set(m_ofSync.quat);
	}

	ofSync.byteHealth = (uint8_t) m_pPlayerPed->GetHealth();
	ofSync.byteArmour = (uint8_t) m_pPlayerPed->GetArmour();

	ofSync.byteCurrentWeapon = (exKeys << 6) | ofSync.byteCurrentWeapon & 0x3F;
	ofSync.byteCurrentWeapon ^= (ofSync.byteCurrentWeapon ^ m_pPlayerPed->GetCurrentWeapon()) & 0x3F;
	ofSync.byteSpecialAction = GetSpecialAction();

	ofSync.vecMoveSpeed.x = vecMoveSpeed.x;
	ofSync.vecMoveSpeed.y = vecMoveSpeed.y;
	ofSync.vecMoveSpeed.z = vecMoveSpeed.z;

	ofSync.vecSurfOffsets.x = 0.0f;
	ofSync.vecSurfOffsets.y = 0.0f;
	ofSync.vecSurfOffsets.z = 0.0f;
	ofSync.wSurfInfo = 0;
	if (m_surfSync.bIsActive) {
		if (m_surfSync.bIsVehicle && m_surfSync.dwSurfVehID != INVALID_VEHICLE_ID) {
			ofSync.vecSurfOffsets.x = m_surfSync.vecOffsetPos.x;
			ofSync.vecSurfOffsets.y = m_surfSync.vecOffsetPos.y;
			ofSync.vecSurfOffsets.z = m_surfSync.vecOffsetPos.z;
			ofSync.wSurfInfo = m_surfSync.dwSurfVehID;
		}
	}

	ofSync.dwAnimation = m_pPlayerPed->GetCurrentAnimationIndexFlag();

	if ((GetTickCount() - m_dwLastSendSyncTick) > 500 || memcmp(&m_ofSync, &ofSync, sizeof(ONFOOT_SYNC_DATA))) {
		m_dwLastSendSyncTick = GetTickCount();

		bsPlayerSync.Write((uint8_t) ID_PLAYER_SYNC);
		bsPlayerSync.Write((char*) &ofSync, sizeof(ONFOOT_SYNC_DATA));
		pNetGame->GetRakClient()->Send(&bsPlayerSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 1);
		memcpy(&m_ofSync, &ofSync, sizeof(ONFOOT_SYNC_DATA));
	}
}

void CLocalPlayer::SendInCarFullSyncData()
{
	RakNet::BitStream bsVehicleSync;
	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	CPlayerPed* pPlayerPed = m_pPlayerPed;

	if (!pPlayerPed || !pVehiclePool) {
		return;
	}

	uint16_t lrAnalog, udAnalog;
	uint8_t exKeys;
	uint16_t wKeys = m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog, &exKeys);

	INCAR_SYNC_DATA icSync;
	memset(&icSync, 0, sizeof(INCAR_SYNC_DATA));

	sa::CVehicle* pGtaVehicle = pPlayerPed->GetGtaVehicle();
	icSync.VehicleID = pVehiclePool->FindIDFromGtaPtr(pGtaVehicle);

	if (icSync.VehicleID == INVALID_VEHICLE_ID ||
			icSync.VehicleID >= MAX_VEHICLES) {
		return;
	}

	CVehicle* pVehicle = pVehiclePool->GetAt(icSync.VehicleID);
	if (!pVehicle) {
		return;
	}

	icSync.lrAnalog = lrAnalog;
	icSync.udAnalog = udAnalog;
	icSync.wKeys = wKeys;

	RwMatrix mat = pVehicle->m_pVehicle->GetMatrix().ToRwMatrix();
	sa::CVector vecMoveSpeed = pVehicle->m_pVehicle->GetMoveSpeed();

	icSync.quat.SetFromMatrix(&mat);
	icSync.quat.Normalize();

	if (FloatOffset(icSync.quat.w, m_icSync.quat.w) < 0.00001f
			&& FloatOffset(icSync.quat.x, m_icSync.quat.x) < 0.00001f
			&& FloatOffset(icSync.quat.y, m_icSync.quat.y) < 0.00001f
			&& FloatOffset(icSync.quat.z, m_icSync.quat.z) < 0.00001f) {
		icSync.quat.Set(m_icSync.quat);
	}

	icSync.vecPos.x = mat.pos.x;
	icSync.vecPos.y = mat.pos.y;
	icSync.vecPos.z = mat.pos.z;

	icSync.vecMoveSpeed.x = vecMoveSpeed.x;
	icSync.vecMoveSpeed.y = vecMoveSpeed.y;
	icSync.vecMoveSpeed.z = vecMoveSpeed.z;

	icSync.fCarHealth = pVehicle->GetHealth();
	icSync.bytePlayerHealth = m_pPlayerPed->GetHealth();
	icSync.bytePlayerArmour = m_pPlayerPed->GetArmour();

	icSync.byteCurrentWeapon = (exKeys << 6) | icSync.byteCurrentWeapon & 0x3F;
	icSync.byteCurrentWeapon ^=
			(icSync.byteCurrentWeapon ^ m_pPlayerPed->GetCurrentWeapon()) & 0x3F;
	icSync.TrailerID = 0;

	CVehicle* pTrailer = pVehicle->GetTrailer();
	if (pTrailer && pTrailer->m_pVehicle && pTrailer->GetTractor() == pVehicle) {
		pVehicle->SetTrailer(pTrailer);
		icSync.TrailerID = pVehiclePool->FindIDFromGtaPtr(pTrailer->m_pVehicle);
	}
	else {
		pVehicle->SetTrailer(nullptr);
	}

	if (pVehicle->GetModelIndex() == TRAIN_PASSENGER_LOCO || pVehicle->GetModelIndex() == TRAIN_FREIGHT_LOCO || pVehicle->GetModelIndex() == TRAIN_TRAM) {
		icSync.fTrainSpeed = pVehicle->GetTrainSpeed();
	}
	else if (pVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_BIKE || pVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_PUSHBIKE) {
		icSync.fTrainSpeed = pVehicle->GetBikeLean();
	}
	else if (pVehicle->GetModelIndex() == HYDRA) {
		icSync.fTrainSpeed = pVehicle->GetHydraThrusters();
	}
	else {
		icSync.fTrainSpeed = 0.0f;
	}

	icSync.byteSirenOn = pVehicle->IsSirenOn();
//	 icSync.byteLandingGearState = pVehicle->GetLandingGearState();

	if (IsNeedSyncDataSend(&m_icSync, &icSync, sizeof(INCAR_SYNC_DATA))) {
		m_dwLastSendSyncTick = GetTickCount();

		bsVehicleSync.Write((uint8_t) ID_VEHICLE_SYNC);
		bsVehicleSync.Write((char*) &icSync, sizeof(INCAR_SYNC_DATA));
		pNetGame->GetRakClient()->Send(&bsVehicleSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, true);

		memcpy(&m_icSync, &icSync, sizeof(INCAR_SYNC_DATA));
	}

	if (icSync.TrailerID && icSync.TrailerID < MAX_VEHICLES) {
		SendTrailerFullSyncData(icSync.TrailerID);
	}

	// if (pVehicle->HasTurret() || GetTickCount() - m_dwLastSendAimSyncTick > 1000)
	//{
	//	m_dwLastSendAimSyncTick = GetTickCount();
	//	SendAimSyncData();
	// }
}

void CLocalPlayer::SendPassengerFullSyncData()
{
	RakNet::BitStream bsData;
	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	uint16_t lrAnalog, udAnalog;
	uint8_t exKeys;
	uint16_t wkeys = m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog, &exKeys);

	PASSENGER_SYNC_DATA psSync;
	memset(&psSync, 0, sizeof(PASSENGER_SYNC_DATA));

	sa::CVehicle* pGtaVehicle = m_pPlayerPed->GetGtaVehicle();
	psSync.VehicleID = pVehiclePool->FindIDFromGtaPtr(pGtaVehicle);
	if (psSync.VehicleID == INVALID_VEHICLE_ID) {
		return;
	}

	psSync.lrAnalog = lrAnalog;
	psSync.udAnalog = udAnalog;
	psSync.wKeys = wkeys;

	psSync.bytePlayerHealth = m_pPlayerPed->GetHealth();
	psSync.bytePlayerArmour = m_pPlayerPed->GetArmour();

	psSync.byteSeatFlags ^= (psSync.byteSeatFlags ^ m_pPlayerPed->GetVehicleSeatID()) & 0x3F;
	uint8_t byteUnk = psSync.byteSeatFlags & 0x7F;
	if (m_pPlayerPed->IsCuffed()) byteUnk = psSync.byteSeatFlags | 0x80;
	psSync.byteSeatFlags = (byteUnk ^ (m_bPassengerDriveByMode << 6)) & 0x40 ^ byteUnk;

	psSync.byteCurrentWeapon = (exKeys << 6) | psSync.byteCurrentWeapon & 0x3F;
	psSync.byteCurrentWeapon ^= (psSync.byteCurrentWeapon ^ m_pPlayerPed->GetCurrentWeapon()) & 0x3F;

	RwMatrix mat = m_pPlayerPed->m_ped->GetMatrix().ToRwMatrix();
	psSync.vecPos.x = mat.pos.x;
	psSync.vecPos.y = mat.pos.y;
	psSync.vecPos.z = mat.pos.z;

	if (IsNeedSyncDataSend(&m_psSync, &psSync, sizeof(PASSENGER_SYNC_DATA))) {
		bsData.Write((char) ID_PASSENGER_SYNC);
		bsData.Write((char*) &psSync, sizeof(PASSENGER_SYNC_DATA));
		pNetGame->GetRakClient()->Send(&bsData, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 1);
		memcpy(&m_psSync, &psSync, sizeof(PASSENGER_SYNC_DATA));
	}

	if (m_bPassengerDriveByMode) SendAimSyncData();
}

void CLocalPlayer::SendTrailerFullSyncData(VEHICLEID vehicleId)
{
	RwMatrix matTrailer;

	TRAILER_SYNC_DATA trSync;
	memset(&trSync, 0, sizeof(TRAILER_SYNC_DATA));

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) {
		return;
	}

	CVehicle* pTrailer = pVehiclePool->GetAt(vehicleId);
	if (pTrailer) {
		matTrailer = pTrailer->m_pVehicle->GetMatrix().ToRwMatrix();

		trSync.quat.SetFromMatrix(&matTrailer);
		trSync.quat.Normalize();

		trSync.trailerId = vehicleId;

		trSync.vecPos.x = matTrailer.pos.x;
		trSync.vecPos.y = matTrailer.pos.y;
		trSync.vecPos.z = matTrailer.pos.z;

		trSync.vecMoveSpeed = pTrailer->m_pVehicle->GetMoveSpeed();
		trSync.vecTurnSpeed = pTrailer->m_pVehicle->GetTurnSpeed();

		if (IsNeedSyncDataSend(&m_trSync, &trSync, sizeof(TRAILER_SYNC_DATA))) {
			RakNet::BitStream bsTrailerSync;
			bsTrailerSync.Write((uint8_t) ID_TRAILER_SYNC);
			bsTrailerSync.Write((char*) &trSync, sizeof(TRAILER_SYNC_DATA));
			pNetGame->GetRakClient()->Send(&bsTrailerSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
			memcpy(&m_trSync, &trSync, sizeof(TRAILER_SYNC_DATA));
		}
	}
}

void CLocalPlayer::SendAimSyncData()
{
	AIM_SYNC_DATA aimSync;

	CAMERA_AIM* caAim = m_pPlayerPed->GetCurrentAim();

	aimSync.byteCamMode = m_pPlayerPed->GetCameraMode();
	aimSync.vecAimf1.x = caAim->f1x;
	aimSync.vecAimf1.y = caAim->f1y;
	aimSync.vecAimf1.z = caAim->f1z;
	aimSync.vecAimPos.x = caAim->pos1x;
	aimSync.vecAimPos.y = caAim->pos1y;
	aimSync.vecAimPos.z = caAim->pos1z;

	aimSync.fAimZ = m_pPlayerPed->GetAimZ();

	aimSync.byteCamExtZoom = (uint8_t) (m_pPlayerPed->GetCameraExtendedZoom() * 63.0f) & 0x3F;
	aimSync.byteAspectRatio = (uint8_t) GameGetAspectRatio() * 255.0f;

	sa::CWeapon* pwstWeapon = m_pPlayerPed->GetCurrentWeaponSlot();
	if (pwstWeapon->dwState == 2) {
		aimSync.byteWeaponState = WS_RELOADING;
	}
	else {
		aimSync.byteWeaponState = (pwstWeapon->dwAmmoInClip > 1) ? WS_MORE_BULLETS : pwstWeapon->dwAmmoInClip;
	}

	if ((GetTickCount() - m_dwLastSendSyncTick) > 500 || memcmp(&m_aimSync, &aimSync, sizeof(AIM_SYNC_DATA))) {
		m_dwLastSendSyncTick = GetTickCount();
		RakNet::BitStream bsAimSync;
		bsAimSync.Write((char) ID_AIM_SYNC);
		bsAimSync.Write((char*) &aimSync, sizeof(AIM_SYNC_DATA));
		pNetGame->GetRakClient()->Send(&bsAimSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 1);
		memcpy(&m_aimSync, &aimSync, sizeof(AIM_SYNC_DATA));
	}
}

void CLocalPlayer::SendStatsUpdate()
{
	if (m_statsSync.dwLastMoney != pGame->GetLocalMoney() || m_statsSync.dwLastDrunkLevel != m_pPlayerPed->GetDrunkLevel()) {
		m_statsSync.dwLastMoney = pGame->GetLocalMoney();
		m_statsSync.dwLastDrunkLevel = m_pPlayerPed->GetDrunkLevel();

		RakNet::BitStream bsStats;
		bsStats.Write((uint8_t) ID_STATS_UPDATE);
		bsStats.Write(m_statsSync.dwLastMoney);
		bsStats.Write(m_statsSync.dwLastDrunkLevel);
		pNetGame->GetRakClient()->Send(&bsStats, HIGH_PRIORITY, UNRELIABLE, 0);
	}
}

void CLocalPlayer::CheckWeapons()
{
	uint8_t byteCurWeapon = m_pPlayerPed->GetCurrentWeapon();
	PLAYERID TargetPlayerID = INVALID_PLAYER_ID;
	PLAYERID TargetActorID = INVALID_PLAYER_ID;
	bool bMSend = false;

	if (byteCurWeapon == m_byteCurrentWeapon) {
		if (GetTickCount() - m_dwLastWeaponsUpdateTick < 100) {
			return;
		}
	}
	else {
		m_byteCurrentWeapon = byteCurWeapon;
	}

	RakNet::BitStream bsSend;
	bsSend.Write((char) ID_WEAPONS_UPDATE);
	bsSend.Write(TargetPlayerID);
	bsSend.Write(TargetActorID);

	for (int i = 0; i < 13; i++) {
		if (m_byteLastWeapon[i] != m_pPlayerPed->m_ped->m_aWeapons[i].dwType || m_dwLastAmmo[i] != m_pPlayerPed->m_ped->m_aWeapons[i].dwAmmo) {
			m_byteLastWeapon[i] = m_pPlayerPed->m_ped->m_aWeapons[i].dwType;
			m_dwLastAmmo[i] = m_pPlayerPed->m_ped->m_aWeapons[i].dwAmmo;
			bsSend.Write((char) i);
			bsSend.Write(m_byteLastWeapon[i]);
			bsSend.Write((uint16_t) m_dwLastAmmo[i]);
			bMSend = true;
		}
	}

	if (bMSend) {
		m_dwLastWeaponsUpdateTick = GetTickCount();
		pNetGame->GetRakClient()->Send(&bsSend, PacketPriority::HIGH_PRIORITY, UNRELIABLE, 0);
	}
}

void CLocalPlayer::UpdateRemoteInterior(uint8_t byteInterior)
{
	m_byteCurInterior = byteInterior;
	RakNet::BitStream bsUpdateInterior;
	bsUpdateInterior.Write(byteInterior);
	pNetGame->GetRakClient()->RPC(&RPC_SetInteriorId, &bsUpdateInterior, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}

void CLocalPlayer::UpdateCameraTarget()
{
	// todo
}

bool CLocalPlayer::EnterVehicleAsPassenger()
{
	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (pVehiclePool) {
		VEHICLEID ClosetVehicleID = pVehiclePool->FindNearestToLocalPlayerPed();
		if (ClosetVehicleID < MAX_VEHICLES && pVehiclePool->GetSlotState(ClosetVehicleID)) {
			CVehicle* pVehicle = pVehiclePool->GetAt(ClosetVehicleID);
			if (pVehicle && pVehicle->GetDistanceFromLocalPlayerPed() < 4.0f) {
				if (m_pPlayerPed->GetCurrentWeapon() == WEAPON_PARACHUTE) {
					m_pPlayerPed->SetArmedWeapon(0);
				}

				m_pPlayerPed->EnterVehicle(pVehicle->m_gtaId, true);
				SendEnterVehicleNotification(ClosetVehicleID, true);
				return true;
			}
		}
	}

	return false;
}

bool CLocalPlayer::HandlePassengerEntry()
{
	return false;
}

void CLocalPlayer::ProcessOnFootWorldBounds()
{
	if (pGame->GetActiveInterior() != 0) {
		return;
	} // can't enforce inside interior

	if (m_pPlayerPed->EnforceWorldBoundries(pNetGame->m_netSet->worldBounds[0], pNetGame->m_netSet->worldBounds[1], pNetGame->m_netSet->worldBounds[2], pNetGame->m_netSet->worldBounds[3])) {
		m_pPlayerPed->SetArmedWeapon(0);
		pGame->DisplayGameText("Stay within the ~r~world boundries", 1000, 5);
	}
}

void CLocalPlayer::ProcessInCarWorldBounds()
{
	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (pVehiclePool) {
		if (pGame->GetActiveInterior() != 0) {
			return;
		} // can't enforce inside interior

		VEHICLEID vehicleId = pVehiclePool->FindIDFromGtaPtr(m_pPlayerPed->GetGtaVehicle());
		if (vehicleId != INVALID_VEHICLE_ID) {
			CVehicle* pVehicle = pVehiclePool->GetAt(vehicleId);
			if (pVehicle) {
				if (pVehicle->EnforceWorldBoundries(
						pNetGame->m_netSet->worldBounds[0], pNetGame->m_netSet->worldBounds[1],
						pNetGame->m_netSet->worldBounds[2],
						pNetGame->m_netSet->worldBounds[3])) {
					pGame->DisplayGameText("Stay within the ~r~world boundries", 1000, 5);
				}
			}
		}
	}
}

bool CLocalPlayer::CompareOnFootSyncKeys(uint16_t wKeys, uint16_t lrAnalog, uint16_t udAnalog)
{
	return wKeys != m_ofSync.wKeys || udAnalog != m_ofSync.udAnalog || lrAnalog != m_ofSync.lrAnalog;
}

int CLocalPlayer::GetOptimumOnFootSendRate()
{
	if (!m_pPlayerPed) {
		return 1000;
	}
	if (pNetGame->m_lanMode) {
		return 15;
	}

	return iNetModeNormalOnFootSendRate + pGame->GetPedSlotsUsed();
}

int CLocalPlayer::GetOptimumInCarSendRate()
{
	if (!m_pPlayerPed) {
		return 1000;
	}
	if (pNetGame->m_lanMode) {
		return 15;
	}

	return iNetModeNormalInCarSendRate + pGame->GetPedSlotsUsed();
}

void CLocalPlayer::UpdateVehicleDamage(VEHICLEID vehicleID)
{
	// todo
}

void CLocalPlayer::SendNextClass()
{
	RwMatrix matPlayer;

	if (!m_bSpawnDialogShowed) {
		return;
	}

	m_bClearedToSpawn = false;
	matPlayer = m_pPlayerPed->m_ped->GetMatrix().ToRwMatrix();

	if (m_iSelectedClass == (pNetGame->m_netSet->spawnsAvailable - 1)) {
		m_iSelectedClass = 0;
	}
	else {
		m_iSelectedClass++;
	}

	pGame->PlaySound(1052, matPlayer.pos.x, matPlayer.pos.y, matPlayer.pos.z);
	RequestClass(m_iSelectedClass);
}

void CLocalPlayer::SendPrevClass()
{
	RwMatrix matPlayer;

	if (!m_bSpawnDialogShowed) {
		return;
	}

	m_bClearedToSpawn = false;
	matPlayer = m_pPlayerPed->m_ped->GetMatrix().ToRwMatrix();

	if (m_iSelectedClass == 0) {
		m_iSelectedClass = (pNetGame->m_netSet->spawnsAvailable - 1);
	}
	else {
		m_iSelectedClass--;
	}

	pGame->PlaySound(1053, matPlayer.pos.x, matPlayer.pos.y, matPlayer.pos.z);
	RequestClass(m_iSelectedClass);
}

void CLocalPlayer::SendSpawn()
{
	if (!m_bSpawnDialogShowed) {
		return;
	}

	RequestSpawn();
	m_bWaitingForSpawnRequestReply = true;
}

void CLocalPlayer::RequestClass(int iClass)
{
	RakNet::BitStream bsClassRequest;
	bsClassRequest.Write(iClass);
	pNetGame->GetRakClient()->RPC(&RPC_RequestClass, &bsClassRequest, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}

void CLocalPlayer::RequestSpawn()
{
	RakNet::BitStream bsSpawnRequest;
	pNetGame->GetRakClient()->RPC(&RPC_RequestSpawn, &bsSpawnRequest, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}

void CLocalPlayer::ApplySpecialAction(uint8_t byteSpecialAction)
{
	if (!m_pPlayerPed) {
		return;
	}

	m_pPlayerPed->SetSpecialAction(byteSpecialAction);

	if (m_pPlayerPed->IsAdded()) {
		if ((GetTickCount() - m_dwLastHeadUpdate) > 500) {
			sa::CVector LookAt;
			CAMERA_AIM* Aim = GameGetInternalAim();
			LookAt.x = Aim->pos1x + (Aim->f1x * 20.0f);
			LookAt.y = Aim->pos1y + (Aim->f1y * 20.0f);
			LookAt.z = Aim->pos1z + (Aim->f1z * 20.0f);
			m_pPlayerPed->ApplyCommandTask("FollowPedSA", 0, 2000, -1, &LookAt, 0, 0.1f, 500, 3, 0);
			m_dwLastHeadUpdate = GetTickCount();
		}
	}
}

void CLocalPlayer::SetSpawnInfo(PLAYER_SPAWN_INFO* pSpawnInfo)
{
	memcpy(&m_SpawnInfo, pSpawnInfo, sizeof(PLAYER_SPAWN_INFO));
	m_bHasSpawnInfo = true;
}

void CLocalPlayer::HandleClassSelectionOutcome(bool bOutcome)
{
	if (bOutcome) {
		if (m_pPlayerPed) {
			m_pPlayerPed->ClearWeapons();
			m_pPlayerPed->SetModelIndex(m_SpawnInfo.iSkin);
		}

		m_bClearedToSpawn = true;
	}
}

uint8_t CLocalPlayer::GetSpecialAction()
{
	if (m_pPlayerPed) {
		return m_pPlayerPed->GetSpecialAction();
	}

	return SPECIAL_ACTION_NONE;
}

uint32_t CLocalPlayer::GetPlayerColorAsARGB()
{
	return (TranslateColorCodeToRGBA(pNetGame->GetPlayerPool()->GetLocalPlayerID()) >> 8) | 0xFF000000;
}

uint32_t CLocalPlayer::GetPlayerColorAsRGBA()
{
	return TranslateColorCodeToRGBA(pNetGame->GetPlayerPool()->GetLocalPlayerID());
}

bool CLocalPlayer::IsNeedSyncDataSend(const void* data1, const void* data2, size_t size)
{
	if (GetTickCount() - m_dwLastSendSyncTick <= 500 && !memcmp(data1, data2, size)) {
		return false;
	}

	m_dwLastSendSyncTick = GetTickCount();
	return true;
}

void CLocalPlayer::SendEnterVehicleNotification(VEHICLEID VehicleID, bool bPassenger)
{
	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (pVehiclePool) {
		CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
		if (pVehicle) {
			if (pVehicle->IsATrainPart()) {
				ScriptCommand(&camera_on_vehicle, pVehicle->m_gtaId, 3, 2);
				dwEnterVehTimeElasped = GetTickCount();
			}
		}
	}

	RakNet::BitStream bsSend;
	bsSend.Write(VehicleID);
	bsSend.Write((uint8_t) bPassenger);
	pNetGame->GetRakClient()->RPC(&RPC_EnterVehicle, &bsSend, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}

void CLocalPlayer::SpectatePlayer(PLAYERID PlayerID)
{
	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();

	if (pPlayerPool && pPlayerPool->GetSlotState(PlayerID)) {
		CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(PlayerID);
		if (pRemotePlayer->GetState() != PLAYER_STATE_NONE && pRemotePlayer->GetState() != PLAYER_STATE_WASTED) {
			m_byteSpectateType = SPECTATE_TYPE_PLAYER;
			m_SpectateID = PlayerID;
			m_bSpectateProcessed = false;
		}
	}
}

void CLocalPlayer::SpectateVehicle(VEHICLEID VehicleID)
{
	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();

	if (pVehiclePool && pVehiclePool->GetSlotState(VehicleID)) {
		m_byteSpectateType = SPECTATE_TYPE_VEHICLE;
		m_SpectateID = VehicleID;
		m_bSpectateProcessed = false;
	}
}

void CLocalPlayer::SetPlayerColor(uint32_t dwColor)
{
	SetRadarColor(pNetGame->GetPlayerPool()->GetLocalPlayerID(), dwColor);
}

void CLocalPlayer::SendExitVehicleNotification(VEHICLEID VehicleID)
{
	RakNet::BitStream bsSend;

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
	if (pVehicle) {
		if (!m_pPlayerPed->IsAPassenger()) {
			m_LastVehicle = VehicleID;
		}

		if (pVehicle->IsATrainPart()) {
			pGame->GetCamera()->SetBehindPlayer();
		}

		if (!pVehicle->IsRCVehicle()) {
			bsSend.Write(VehicleID);
			pNetGame->GetRakClient()->RPC(&RPC_ExitVehicle, &bsSend, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
		}
	}
}

void CLocalPlayer::SendTakeDamageEvent(PLAYERID PlayerID, float fDamageFactor, int weaponType, int pedPieceType)
{
	spdlog::info("SendTakeDamageEvent: PlayerID: {}, fDamageFactor: {}, weaponType: {}, pedPieceType: {}", PlayerID, fDamageFactor, weaponType, pedPieceType);

	fDamageFactor *= 0.33f;

	RakNet::BitStream bsSend;
	bsSend.Write(true);
	bsSend.Write(PlayerID);
	bsSend.Write(fDamageFactor);
	bsSend.Write(weaponType);
	bsSend.Write(pedPieceType);

	pNetGame->GetRakClient()->RPC(&RPC_PlayerGiveTakeDamage, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}

void CLocalPlayer::SendGiveDamageEvent(PLAYERID PlayerID, float fDamageFactor, int weaponType, int pedPieceType)
{
	spdlog::info("SendGiveDamageEvent: PlayerID: {}, fDamageFactor: {}, weaponType: {}, pedPieceType: {}", PlayerID, fDamageFactor, weaponType, pedPieceType);

	fDamageFactor *= 0.33f;

	RakNet::BitStream bsSend;
	bsSend.Write(false);
	bsSend.Write(PlayerID);
	bsSend.Write(fDamageFactor);
	bsSend.Write(weaponType);
	bsSend.Write(pedPieceType);

	pNetGame->GetRakClient()->RPC(&RPC_PlayerGiveTakeDamage, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}

void CLocalPlayer::SendGiveDamageActorEvent(PLAYERID ActorID, float fDamageFactor, int weaponType, int pedPieceType)
{
	RakNet::BitStream bsSend;
	bsSend.Write(false);
	bsSend.Write(ActorID);
	bsSend.Write(fDamageFactor);
	bsSend.Write(weaponType);
	bsSend.Write(pedPieceType);

	pNetGame->GetRakClient()->RPC(&RPC_GiveDamageActor, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}

void CLocalPlayer::ProcessSurfing()
{
	if (m_pPlayerPed && !m_pPlayerPed->IsDead() && !LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] || m_pPlayerPed->IsJumpTask()) {
		auto contactVeh = (sa::CVehicle*) m_pPlayerPed->GetEntityUnderPlayer();
		if (contactVeh) {
			VEHICLEID vehicleId = pNetGame->GetVehiclePool()->FindIDFromGtaPtr(contactVeh);
			if (vehicleId && vehicleId != INVALID_VEHICLE_ID) {
				CVehicle* pVeh = pNetGame->GetVehiclePool()->GetAt(vehicleId);
				if (pVeh && (pVeh->HasADriver() || pVeh->m_pVehicle->GetModelId() == 569 || pVeh->m_pVehicle->GetModelId() == 570) && pVeh->GetDistanceFromLocalPlayerPed() < 30.0) {
					if (m_surfSync.bIsActive) {
						return;
					}
					memset(&m_surfSync, 0, sizeof(m_surfSync));
					m_surfSync.vecOffsetPos = sa::CVector{0.0f, 0.0f, 0.0f};
					m_surfSync.dwSurfVehID = vehicleId;
					m_surfSync.pSurfInst = (uintptr_t) pVeh;
					m_surfSync.bIsVehicle = true;

					static RwMatrix matVeh;
					matVeh = pVeh->m_pVehicle->GetMatrix().ToRwMatrix();
					static RwMatrix matPed;
					matPed = m_pPlayerPed->m_ped->GetMatrix().ToRwMatrix();
					static RwMatrix matOut;
					RwMatrixInvert(&matOut, &matVeh);
					ProjectMatrix(&m_surfSync.vecOffsetPos, (sa::CMatrix*) (&matOut), (sa::CVector*) &matPed.pos);
					m_surfSync.bIsActive = true;
					return;
				}
			}
			else {
				auto contactEntity = (sa::CObject*) m_pPlayerPed->GetEntityUnderPlayer();
				if (contactEntity) {
					uint32_t objectId = pNetGame->GetObjectPool()->FindIDFromGtaPtr((contactEntity));
					if (objectId && objectId != INVALID_OBJECT_ID) {
						CObject* pObject = pNetGame->GetObjectPool()->GetAt(objectId);
						if (pObject) {
							if (m_surfSync.bIsActive) {
								return;
							}
							memset(&m_surfSync, 0, sizeof(m_surfSync));
							m_surfSync.bIsVehicle = false;
							m_surfSync.pSurfInst = (uintptr_t) pObject;
							m_surfSync.bIsActive = true;
							return;
						}
					}
				}
			}
		}
	}

	m_surfSync.bIsActive = false;
	m_surfSync.dwSurfVehID = INVALID_VEHICLE_ID;
	m_surfSync.pSurfInst = 0;
	m_surfSync.vecOffsetPos = sa::CVector{0.0f, 0.0f, 0.0f};
}

void CLocalPlayer::UpdateSurfing()
{
	static RwMatrix surfInstMatrix;
	static RwMatrix surfPedMatrix;
	static sa::CVector surfInstMoveSpeed;
	static sa::CVector surfInstTurnSpeed;

	if (m_pPlayerPed) {
		if (LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] || m_pPlayerPed->IsJumpTask()) {
			return;
		}
		if (m_surfSync.bIsActive) {
			if (m_surfSync.bIsVehicle && m_surfSync.pSurfInst) {
				auto pVeh = (CVehicle*) m_surfSync.pSurfInst;
				surfInstMatrix = pVeh->m_pVehicle->GetMatrix().ToRwMatrix();
				surfPedMatrix = m_pPlayerPed->m_ped->GetMatrix().ToRwMatrix();
				surfInstMoveSpeed = pVeh->m_pVehicle->GetMoveSpeed();
				surfInstTurnSpeed = pVeh->m_pVehicle->GetTurnSpeed();

				uint16_t lrAnalog;
				uint16_t udAnalog;
				m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog);

				if (lrAnalog || udAnalog) {
					static RwMatrix matOut;
					RwMatrixInvert(&matOut, &surfInstMatrix);
					ProjectMatrix(&m_surfSync.vecOffsetPos, (sa::CMatrix*) &matOut, (sa::CVector*) &surfPedMatrix.pos);
				}
				else {
					ProjectMatrix((sa::CVector*) &surfPedMatrix.pos, (sa::CMatrix*) &surfInstMatrix, &m_surfSync.vecOffsetPos);

					m_pPlayerPed->m_ped->SetMatrix((sa::CMatrix&) surfPedMatrix);
					sa::CVector vecMoveSpeed;
					vecMoveSpeed = m_pPlayerPed->m_ped->GetMoveSpeed();
					m_pPlayerPed->m_ped->SetVelocity(sa::CVector{surfInstMoveSpeed.x, surfInstMoveSpeed.y, vecMoveSpeed.z});

					sa::CVector vecTurnSpeed = m_pPlayerPed->m_ped->GetTurnSpeed();
					m_pPlayerPed->m_ped->SetTurnSpeed(sa::CVector{vecTurnSpeed.x, vecTurnSpeed.y, surfInstTurnSpeed.z});
				}
			}
			else {
				auto pObject = (CObject*) m_surfSync.pSurfInst;
				if (pObject && pObject->m_byteMoving & 1) {
					surfInstMatrix = pObject->m_entity->GetMatrix().ToRwMatrix();
					surfPedMatrix = m_pPlayerPed->m_ped->GetMatrix().ToRwMatrix();
					surfInstMoveSpeed = pObject->m_entity->GetMoveSpeed();
					surfInstTurnSpeed = pObject->m_entity->GetTurnSpeed();

					uint16_t lrAnalog;
					uint16_t udAnalog;
					m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog);

					if (lrAnalog || udAnalog) {
						static RwMatrix matOut;
						RwMatrixInvert(&matOut, &surfInstMatrix);
						ProjectMatrix(&m_surfSync.vecOffsetPos, (sa::CMatrix*) &matOut, (sa::CVector*) &surfPedMatrix.pos);
					}
					else {
						ProjectMatrix((sa::CVector*) &surfPedMatrix.pos, (sa::CMatrix*) &surfInstMatrix, &m_surfSync.vecOffsetPos);

						m_pPlayerPed->m_ped->SetMatrix((sa::CMatrix&) surfPedMatrix);
						sa::CVector vecMoveSpeed = m_pPlayerPed->m_ped->GetMoveSpeed();
						m_pPlayerPed->m_ped->SetVelocity(sa::CVector{surfInstMoveSpeed.x, surfInstMoveSpeed.y, vecMoveSpeed.z});

						sa::CVector vecTurnSpeed = m_pPlayerPed->m_ped->GetTurnSpeed();
						m_pPlayerPed->m_ped->SetTurnSpeed(sa::CVector{vecTurnSpeed.x, vecTurnSpeed.y, surfInstTurnSpeed.z});
					}
				}
			}
		}
	}
}
