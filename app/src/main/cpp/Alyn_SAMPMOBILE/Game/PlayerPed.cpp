#include "Game.h"
#include "../UI/UI.h"
#include "../Net/NetGame.h"
#include "../Client.h"
#include "RenderWare/rphanim.h"
#include "PlayerPed.h"

extern UI* pUI;
extern Game* pGame;
extern NetGame* pNetGame;

CPlayerPed::CPlayerPed()
{
	m_gtaId = 1;
	m_ped = GamePool_FindPlayerPed();
	m_entity = GamePool_FindPlayerPed();
	m_bytePlayerNumber = 0;
	SetPlayerPedPtrRecord(m_bytePlayerNumber, m_ped);
	ScriptCommand(&set_actor_weapon_droppable, m_gtaId, 1);
	ScriptCommand(&set_actor_can_be_decapitated, m_gtaId, 0);

	m_bCuffedState = false;
	m_bCarryState = false;

	m_iDanceState = 0;
	m_bCellPhoneEnabled = false;
	m_bPissingState = 0;
	m_dwPissParticlesHandle = 0;

	m_dwArrow = 0;

	m_iParachuteState = 0;
	m_iParachuteAnim = 0;
	m_dwParachuteObject = 0;

	m_dwDrunkLevel = 0;
	m_dwDrunkLastUpdateTick = 0;

	for (auto& m_attachedObject : m_attachedObjects) {
		m_attachedObject.slotUsed = false;
		memset(&m_attachedObject.info, 0, sizeof(NEW_ATTACHED_OBJECT));
		m_attachedObject.object = nullptr;
	}
}

CPlayerPed::CPlayerPed(int iNum, int iSkin, float fX, float fY, float fZ, float fRotation)
{
	spdlog::info("CPlayerPed::CPlayerPed({}, {}, {}, {}, {}, {})", iNum, iSkin, fX, fY, fZ, fRotation);

	uint32_t dwPlayerActorID;
	ScriptCommand(&create_player, &iNum, fX, fY, fZ, &dwPlayerActorID);
	ScriptCommand(&create_actor_from_player, &iNum, &dwPlayerActorID);

	m_gtaId = dwPlayerActorID;
	m_ped = GamePool_Ped_GetAt(m_gtaId);
	m_entity = m_ped;
	m_bytePlayerNumber = iNum;

	SetPlayerPedPtrRecord(iNum, m_ped);

	ScriptCommand(&set_actor_weapon_droppable, m_gtaId, 1);
	ScriptCommand(&set_actor_immunities, m_gtaId, 0, 0, 1, 0, 0);
	ScriptCommand(&set_actor_can_be_decapitated, m_gtaId, 0);

	if (pNetGame) {
		ScriptCommand(&set_actor_money, m_gtaId, 0);
		ScriptCommand(&set_actor_money, m_gtaId, pNetGame->m_netSet->deathDropMoney);
	}

	m_bCuffedState = false;
	m_bCarryState = false;

	m_dwArrow = 0;

	m_iParachuteState = 0;
	m_iParachuteAnim = 0;
	m_dwParachuteObject = 0;

	m_dwDrunkLevel = 0;

	SetModelIndex(iSkin);

	// GameResetPlayerKeys

	SetTargetRotation(fRotation);
	RwMatrix mat = m_ped->GetMatrix().ToRwMatrix();
	mat.pos.x = fX;
	mat.pos.y = fY;
	mat.pos.z = fZ + 0.15f;
	m_ped->SetMatrix((sa::CMatrix&) mat);

	for (auto& m_attachedObject : m_attachedObjects) {
		m_attachedObject.slotUsed = false;
		memset(&m_attachedObject.info, 0, sizeof(NEW_ATTACHED_OBJECT));
		m_attachedObject.object = nullptr;
	}

	m_bHaveBulletData = false;
	memset(&m_bulletData, 0, sizeof(BULLET_DATA));
}

CPlayerPed::~CPlayerPed()
{
	spdlog::info("Destroying PlayerPed({})", m_bytePlayerNumber);

	// GameResetPlayerKeys

	SetPlayerPedPtrRecord(m_bytePlayerNumber, nullptr);

	if (m_ped && (GamePool_Ped_GetAt(m_gtaId) != nullptr) && !IsEntityPlaceable(m_ped)) {
		if (m_dwParachuteObject) {
			ScriptCommand(&disassociate_object, m_dwParachuteObject, 0.0, 0.0, 0.0, 0);
			ScriptCommand(&destroy_object_with_fade, m_dwParachuteObject);
			m_dwParachuteObject = 0;
		}

		if (HasAttachedObject()) {
			RemoveAllAttachedObjects();
		}

		if (m_ped->IsInVehicle()) {
			RemoveFromVehicleAndPutAt(100.0f, 100.0f, 20.0f);
		}

		m_ped->m_pPlayerData->m_nPlayerGroup = 0;

		// CPlayerPed::Destructor
		m_ped->Destructor();

		m_ped = nullptr;
		m_entity = nullptr;
	}
	else {
		m_ped = nullptr;
		m_entity = nullptr;
		m_gtaId = 0;
	}
}

void CPlayerPed::ShowMarker(int nIndex)
{
	if (!m_ped || !GamePool_Ped_GetAt(m_gtaId)) {
		if (m_dwArrow) {
			spdlog::info("diable Arrow marker: {}", m_dwArrow);
			ScriptCommand(&disable_marker, m_dwArrow);
			m_dwArrow = 0;
		}
	}
	ScriptCommand(&create_arrow_above_actor, m_gtaId, &m_dwArrow);
	ScriptCommand(&set_marker_color, m_dwArrow, nIndex);
	ScriptCommand(&show_on_radar2, m_dwArrow, 2); //2==player marker
}

bool CPlayerPed::IsInVehicle()
{
	if (!m_ped) {
		return false;
	}

	if (m_ped->IsInVehicle()) {
		return true;
	}

	return false;
}

bool CPlayerPed::IsAPassenger()
{
	if (!m_ped) {
		return false;
	}

	if (m_ped->IsAPassenger()) {
		return true;
	}

	return false;
}

void CPlayerPed::RemoveFromVehicleAndPutAt(float fX, float fY, float fZ)
{
	if (!GamePool_Ped_GetAt(m_gtaId)) {
		return;
	}
	if (m_ped && m_ped->IsInVehicle()) {
		ScriptCommand(&remove_actor_from_car_and_put_at, m_gtaId, fX, fY, fZ);
	}
}

void CPlayerPed::SetInitialState()
{
	m_ped->SetInitialState(false);
}

void CPlayerPed::TogglePlayerControllable(bool bControllable)
{
	RwMatrix mat;

	if (GamePool_Ped_GetAt(m_gtaId)) {
		if (bControllable) {
			ScriptCommand(&toggle_player_controllable, m_bytePlayerNumber, 1);
			ScriptCommand(&lock_actor, m_gtaId, 0);

			if (!IsInVehicle()) {
				mat = m_ped->GetMatrix().ToRwMatrix();
				TeleportTo(mat.pos.x, mat.pos.y, mat.pos.z);
			}
		}
		else {
			ScriptCommand(&toggle_player_controllable, m_bytePlayerNumber, 0);
			ScriptCommand(&lock_actor, m_gtaId, 1);
		}
	}
}

void CPlayerPed::RemoveWeaponWhenEnteringVehicle()
{
	if (m_ped) {
		// CPed::RemoveWeaponWhenEnteringVehicle
		Memory::callFunction("_ZN4CPed31RemoveWeaponWhenEnteringVehicleEi", m_ped, 0);
	}
}

uint8_t CPlayerPed::GetActionTrigger()
{
	return m_ped->m_nPedState;
}

bool CPlayerPed::IsDead()
{
	if (!m_ped) {
		return true;
	}
	if (m_ped->m_fHealth > 0.0f) {
		return false;
	}
	return true;
}

void CPlayerPed::SetSpecialAction(uint8_t byteSpecialAction)
{
	// duck:start
	if (byteSpecialAction == SPECIAL_ACTION_DUCK && !IsCrouching()) {
		ApplyCrouch();
	}

	// duck:stop
	if (byteSpecialAction != SPECIAL_ACTION_DUCK && IsCrouching()) {
		ResetCrouch();
	}

	// cellphone:start
	if (byteSpecialAction == SPECIAL_ACTION_USECELLPHONE && !IsCellphoneEnabled()) {
		ToggleCellphone(true);
	}

	// cellphone:stop
	if (byteSpecialAction != SPECIAL_ACTION_USECELLPHONE && IsCellphoneEnabled()) {
		ToggleCellphone(false);
	}

	// jetpack:start
	if (byteSpecialAction == SPECIAL_ACTION_USEJETPACK && !IsInJetpack()) {
		StartJetpack();
	}

	// jetpack:stop
	if (byteSpecialAction != SPECIAL_ACTION_USEJETPACK && IsInJetpack()) {
		StopJetpack();
	}

	// carry:start
	if (byteSpecialAction == SPECIAL_ACTION_CARRY && !IsCarry()) {
		ToggleCarry(true);
	}

	// carry:stop
	if (byteSpecialAction != SPECIAL_ACTION_CARRY && IsCarry()) {
		ToggleCarry(false);
	}

	// cuffed:start
	if (byteSpecialAction == SPECIAL_ACTION_CUFFED && !IsCuffed()) {
		ToggleCuffed(true);
	}

	// cuffed:stop
	if (byteSpecialAction != SPECIAL_ACTION_CUFFED && IsCuffed()) {
		ToggleCuffed(false);
	}

	// handsup:start
	if (byteSpecialAction == SPECIAL_ACTION_HANDSUP && !HasHandsUp()) {
		HandsUp();
	}

	// handsup:stop
	if (byteSpecialAction != SPECIAL_ACTION_HANDSUP && HasHandsUp()) {
		TogglePlayerControllable(true);
	}

	// urinating:start
	if (byteSpecialAction == SPECIAL_ACTION_PISSING && !IsPissing()) {
		TogglePissing(true);
	}

	// urinating:stop
	if (byteSpecialAction != SPECIAL_ACTION_PISSING && IsPissing()) {
		TogglePissing(false);
	}

	// dancing:start
	/*if (GetDanceStyle() == -1) {
		if (byteSpecialAction == SPECIAL_ACTION_DANCE1) {
			StartDancing(0);
		}
		if (byteSpecialAction == SPECIAL_ACTION_DANCE2) {
			StartDancing(1);
		}
		if (byteSpecialAction == SPECIAL_ACTION_DANCE3) {
			StartDancing(2);
		}
		if (byteSpecialAction == SPECIAL_ACTION_DANCE4) {
			StartDancing(3);
		}
	}

	// dancing:stop
	if (GetDanceStyle() != -1) {
		if (byteSpecialAction != SPECIAL_ACTION_DANCE1 &&
				byteSpecialAction != SPECIAL_ACTION_DANCE2 &&
				byteSpecialAction != SPECIAL_ACTION_DANCE3 &&
				byteSpecialAction != SPECIAL_ACTION_DANCE4) {
			StopDancing();
		}
	}*/

	// stuff:start
	/*if (GetStuff() == eStuffType::STUFF_TYPE_NONE) {
		if (byteSpecialAction == SPECIAL_ACTION_DRINK_BEER) {
			GiveStuff(eStuffType::STUFF_TYPE_BEER);
		}

		if (byteSpecialAction == SPECIAL_ACTION_SMOKE_CIGGY) {
			GiveStuff(eStuffType::STUFF_TYPE_CIGGI);
		}

		if (byteSpecialAction == SPECIAL_ACTION_DRINK_WINE) {
			GiveStuff(eStuffType::STUFF_TYPE_DYN_BEER);
		}

		if (byteSpecialAction == SPECIAL_ACTION_DRINK_SPRUNK) {
			GiveStuff(eStuffType::STUFF_TYPE_PINT_GLASS);
		}
	}

	// stuff:stop
	if (GetStuff() != eStuffType::STUFF_TYPE_NONE) {
		if (byteSpecialAction != SPECIAL_ACTION_DRINK_BEER ||
				byteSpecialAction != SPECIAL_ACTION_SMOKE_CIGGY ||
				byteSpecialAction != SPECIAL_ACTION_DRINK_WINE ||
				byteSpecialAction != SPECIAL_ACTION_DRINK_SPRUNK) {
			DropStuff();
		}
	}*/

	ProcessDancing();
	ProcessParachutes();
}

uint8_t CPlayerPed::GetSpecialAction()
{
	if (IsCrouching()) {
		return SPECIAL_ACTION_DUCK;
	}

	if (IsEnteringVehicle() != 0) {
		return SPECIAL_ACTION_ENTER_VEHICLE;
	}

	if (IsExitingVehicle()) {
		return SPECIAL_ACTION_EXIT_VEHICLE;
	}

	if (IsSitTask()) {
		return SPECIAL_ACTION_SITTING;
	}

	if (IsInJetpack()) {
		return SPECIAL_ACTION_USEJETPACK;
	}

	if (IsCuffed()) {
		return SPECIAL_ACTION_CUFFED;
	}

	if (IsCarry()) {
		return SPECIAL_ACTION_CARRY;
	}

	/*if (GetDanceStyle() != -1) {
		switch (GetDanceStyle()) {
			case 0: return SPECIAL_ACTION_DANCE1;
				break;
			case 1: return SPECIAL_ACTION_DANCE2;
				break;
			case 2: return SPECIAL_ACTION_DANCE3;
				break;
			case 3: return SPECIAL_ACTION_DANCE4;
				break;
		}
	}*/

	if (HasHandsUp()) {
		return SPECIAL_ACTION_HANDSUP;
	}

	if (IsCellphoneEnabled()) {
		return SPECIAL_ACTION_USECELLPHONE;
	}

	if (IsPissing()) {
		return SPECIAL_ACTION_PISSING;
	}

	/*if (GetStuff() == eStuffType::STUFF_TYPE_BEER) {
		return SPECIAL_ACTION_DRINK_BEER;
	}

	if (GetStuff() == eStuffType::STUFF_TYPE_DYN_BEER) {
		return SPECIAL_ACTION_DRINK_WINE;
	}

	if (GetStuff() == eStuffType::STUFF_TYPE_PINT_GLASS) {
		return SPECIAL_ACTION_DRINK_SPRUNK;
	}

	if (GetStuff() == eStuffType::STUFF_TYPE_CIGGI) {
		return SPECIAL_ACTION_SMOKE_CIGGY;
	}*/

	return SPECIAL_ACTION_NONE;
}

char DanceStyleLibs[4][16] = {"WOP", "GFUNK", "RUNNINGMAN", "STRIP"};
char DanceIdleLoops[4][16] = {"DANCE_LOOP", "DANCE_LOOP", "DANCE_LOOP", "STR_Loop_B"};

char szDanceAnimNamesFemale[16][16] = {
		"DANCE_G1", "DANCE_G2", "DANCE_G3", "DANCE_G4",
		"DANCE_G5", "DANCE_G6", "DANCE_G7", "DANCE_G8",
		"DANCE_G9", "DANCE_G10", "DANCE_G11", "DANCE_G12",
		"DANCE_G13", "DANCE_G14", "DANCE_G15", "DANCE_G16"};

char szDanceAnimNamesMale[16][16] = {
		"DANCE_B1", "DANCE_B2", "DANCE_B3", "DANCE_B4",
		"DANCE_B5", "DANCE_B6", "DANCE_B7", "DANCE_B8",
		"DANCE_B9", "DANCE_B10", "DANCE_B11", "DANCE_B12",
		"DANCE_B13", "DANCE_B14", "DANCE_B15", "DANCE_B16"};

char szStripAnims[16][16] = {
		"strip_A", "strip_B", "strip_C", "strip_D",
		"strip_E", "strip_F", "strip_G", "STR_A2B",
		"STR_B2A", "STR_B2C", "STR_C1", "STR_C2",
		"STR_C2B", "STR_A2B", "STR_B2C", "STR_C2"};

bool CPlayerPed::IsDancing()
{
	if (m_iDanceState) {
		return true;
	}
	return false;
}

void CPlayerPed::StopDancing()
{
	if (!m_ped || !IsAdded()) {
		return;
	}

	m_iDanceState = 0;
	RwMatrix mat = m_ped->GetMatrix().ToRwMatrix();
	TeleportTo(mat.pos.x, mat.pos.y, mat.pos.z);
}

void CPlayerPed::ProcessDancing()
{
	if (!m_ped || !IsAdded() || IsInVehicle()) {
		return;
	}

	if (!m_iDanceState) return;

	/*uint16_t wKeysUpDown, wKeysLeftRight, iExtra;
	if (!m_bytePlayerNumber) {
		wKeysUpDown = LocalPlayerKeys.wKeyUD;
		wKeysLeftRight = LocalPlayerKeys.wKeyLR;
		iExtra = LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT];
	}
	else {
		wKeysUpDown = RemotePlayerKeys[m_bytePlayerNumber].wKeyUD;
		wKeysLeftRight = RemotePlayerKeys[m_bytePlayerNumber].wKeyLR;
		iExtra = RemotePlayerKeys[m_bytePlayerNumber].bKeys[ePadKeys::KEY_SPRINT];
	}

	if (!m_ped->m_pIntelligence->m_TaskMgr.m_aSecondaryTasks[sa::TASK_PRIMARY_PRIMARY]) {
		ApplyAnimation(DanceIdleLoops[m_iDanceStyle], DanceStyleLibs[m_iDanceStyle], 4.0, 1, 0, 0, 0, -1);
	}

	int iNewMove = 0;
	if (wKeysUpDown > 128 && !wKeysLeftRight && !iExtra) {
		iNewMove = 0; // UP
	}
	else if (wKeysUpDown < 128 && !wKeysLeftRight && !iExtra) {
		iNewMove = 1; // DOWN
	}
	else if (!wKeysUpDown && wKeysLeftRight < 128 && !iExtra) {
		iNewMove = 2; // LEFT
	}
	else if (!wKeysUpDown && wKeysLeftRight > 128 && !iExtra) {
		iNewMove = 3; // RIGHT
	}
	else if (wKeysUpDown > 128 && wKeysLeftRight < 128 && !iExtra) {
		iNewMove = 4; // UP-LEFT
	}
	else if (wKeysUpDown > 128 && wKeysLeftRight > 128 && !iExtra) {
		iNewMove = 5; // UP-RIGHT
	}
	else if (wKeysUpDown < 128 && wKeysLeftRight < 128 && !iExtra) {
		iNewMove = 6; // DOWN-LEFT
	}
	else if (wKeysUpDown < 128 && wKeysLeftRight > 128 && !iExtra) {
		iNewMove = 7; // DOWN-RIGHT
	}
	else if (wKeysUpDown > 128 && !wKeysLeftRight && iExtra) {
		iNewMove = 8; // UP/EX
	}
	else if (wKeysUpDown < 128 && !wKeysLeftRight && iExtra) {
		iNewMove = 9; // DOWN/EX
	}
	else if (!wKeysUpDown && wKeysLeftRight < 128 && iExtra) {
		iNewMove = 10; // LEFT/EX
	}
	else if (!wKeysUpDown && wKeysLeftRight > 128 && iExtra) {
		iNewMove = 11; // RIGHT/EX
	}
	else if (wKeysUpDown > 128 && wKeysLeftRight < 128 && iExtra) {
		iNewMove = 12; // UP-LEFT/EX
	}
	else if (wKeysUpDown > 128 && wKeysLeftRight > 128 && iExtra) {
		iNewMove = 13; // UP-RIGHT/EX
	}
	else if (wKeysUpDown < 128 && wKeysLeftRight < 128 && iExtra) {
		iNewMove = 14; // DOWN-LEFT/EX
	}
	else if (wKeysUpDown < 128 && wKeysLeftRight > 128 && iExtra) {
		iNewMove = 15; // DOWN-RIGHT/EX
	}
	else { return; }

	if (iNewMove == m_iLastDanceMove) return; // don't allow the same move twice

	m_iLastDanceMove = iNewMove;

	char* szAnimName = GetDanceAnimForMove(iNewMove);
	if (!szAnimName && !strlen(szAnimName)) return;

	ApplyAnimation(szAnimName, DanceStyleLibs[m_iDanceStyle], 4.0, 0, 0, 0, 0, -1);*/
}

char* CPlayerPed::GetDanceAnimForMove(int iMove)
{
	if (!m_iDanceStyle == -1) return ""; // shouldn't ever happen but..

	// style 0-2 have the same anim names, but M/F versions
	if (m_iDanceStyle >= 0 && m_iDanceStyle <= 2) {
		if (GetPedStat() == 5 || GetPedStat() == 22) { // female or pro (only female types)
			return szDanceAnimNamesFemale[iMove];
		}

		return szDanceAnimNamesMale[iMove];
	}
	else if (m_iDanceStyle == 3) {
		return szStripAnims[iMove];
	}

	return "";
}

void CPlayerPed::ToggleCellphone(bool on)
{
	if (!m_ped || !GamePool_Ped_GetAt(m_gtaId) || !IsAdded()) {
		return;
	}

	m_bCellPhoneEnabled = on;
	ScriptCommand(&toggle_actor_cellphone, m_gtaId, on);
}

void CPlayerPed::ApplyCrouch()
{
	if (!m_ped || !GamePool_Ped_GetAt(m_gtaId)) {
		return;
	}

	spdlog::info("Apply crouch");

	if (!IsCrouching()) {
		if (m_ped->m_pIntelligence) {
			// CPedIntelligence::SetTaskDuckSecondary
			Memory::callFunction("_ZN16CPedIntelligence20SetTaskDuckSecondaryEt", m_ped->m_pIntelligence, 0);
		}
	}
}

void CPlayerPed::ResetCrouch()
{
	if (!m_ped || !GamePool_Ped_GetAt(m_gtaId)) {
		return;
	}

	spdlog::info("Reset crouch");

	m_ped->m_nPedFlags.bIsDucking = false;

	if (m_ped->m_pIntelligence) {
		// CPedIntelligence::ClearTaskDuckSecondary
		Memory::callFunction("_ZN16CPedIntelligence22ClearTaskDuckSecondaryEv", m_ped->m_pIntelligence);
	}
}

bool CPlayerPed::IsCrouching()
{
	if (!m_ped || !GamePool_Ped_GetAt(m_gtaId)) {
		return false;
	}

	return m_ped->bIsDucking;
}

void CPlayerPed::HandsUp()
{
	if (!m_ped || !GamePool_Ped_GetAt(m_gtaId) || IsInVehicle() || !IsAdded()) {
		return;
	}

	ScriptCommand(&actor_task_handsup, m_gtaId, -1);
}

int CPlayerPed::HasHandsUp()
{
	if (!m_ped || !IsAdded()) {
		return false;
	}

	if (m_ped->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[sa::TASK_PRIMARY_PRIMARY] == nullptr) {
		return false;
	}

	uintptr_t vtable = *(uintptr_t*) m_ped->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[sa::TASK_PRIMARY_PRIMARY];
	if (vtable == (uintptr_t) g_saSym->Abs(addr::vptr$CTaskSimpleHandsUp)) {
		return true;
	}

	return false;
}

void CPlayerPed::StartJetpack()
{
	if (!m_ped || m_ped->IsInVehicle()) {
		return;
	}

	if (!GamePool_Ped_GetAt(m_gtaId)) {
		return;
	}

	ScriptCommand(&actor_task_jetpack, m_gtaId);
}

void CPlayerPed::StopJetpack()
{
	if (!m_ped || m_ped->IsInVehicle()) {
		return;
	}

	if (!GamePool_Ped_GetAt(m_gtaId)) {
		return;
	}

	if (m_ped->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[sa::TASK_PRIMARY_PRIMARY] == nullptr) {
		return;
	}

	if (IsInJetpack()) {
		sa::CTask* dwJetPackTask = m_ped->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[sa::TASK_PRIMARY_PRIMARY];
		// CTaskSimpleJetPack::~CTaskSimpleJetPack(CTaskSimpleJetPack *this)
		Memory::callFunction("_ZN18CTaskSimpleJetPackD0Ev", dwJetPackTask);
		m_ped->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[sa::TASK_PRIMARY_PRIMARY] = nullptr;
	}
}

bool CPlayerPed::IsInJetpack()
{
	if (!m_ped || m_ped->IsInVehicle()) {
		return false;
	}

	if (m_ped->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[sa::TASK_PRIMARY_PRIMARY] == nullptr) {
		return false;
	}

	uintptr_t vtable = *(uintptr_t*) m_ped->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[sa::TASK_PRIMARY_PRIMARY];
	if (vtable == (uintptr_t) g_saSym->Abs(addr::vptr$CTaskSimpleJetPack)) {
		return true;
	}

	return false;
}

bool CPlayerPed::IsTakeDamageFallTask()
{
	if (m_ped && !IsInVehicle() && &m_ped->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks && m_ped->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[sa::TASK_PRIMARY_PRIMARY]) {
		return GetTaskTypeFromTask(&m_ped->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[sa::TASK_PRIMARY_PRIMARY]) == sa::TASK_COMPLEX_FALL_AND_GET_UP;
	}

	return false;
}

uint8_t CPlayerPed::IsEnteringVehicle()
{
	if (m_ped && &m_ped->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks && m_ped->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[sa::TASK_PRIMARY_PRIMARY]) {
		int iType = GetTaskTypeFromTask(&m_ped->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[sa::TASK_PRIMARY_PRIMARY]);
		if (iType == sa::TASK_COMPLEX_ENTER_CAR_AS_PASSENGER || iType == sa::TASK_COMPLEX_ENTER_CAR_AS_PASSENGER_TIMED) {
			return 2;
		}
		if (iType == sa::TASK_COMPLEX_ENTER_CAR_AS_DRIVER || iType == sa::TASK_COMPLEX_ENTER_CAR_AS_DRIVER_TIMED) {
			return 1;
		}
	}
	return 0;
}

bool CPlayerPed::IsExitingVehicle()
{
	if (m_ped && &m_ped->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks && m_ped->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[sa::TASK_PRIMARY_PRIMARY]) {
		return GetTaskTypeFromTask(&m_ped->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[sa::TASK_PRIMARY_PRIMARY]) == sa::TASK_COMPLEX_LEAVE_CAR;
	}

	return false;
}

bool CPlayerPed::IsJumpTask()
{
	if (m_ped && !IsInVehicle() && &m_ped->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks && m_ped->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[sa::TASK_PRIMARY_PRIMARY]) {
		return GetTaskTypeFromTask(&m_ped->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[sa::TASK_PRIMARY_PRIMARY]) == sa::TASK_COMPLEX_JUMP;
	}

	return false;
}

bool CPlayerPed::IsSitTask()
{
	if (!m_ped || !GamePool_Ped_GetAt(m_gtaId) || !IsInVehicle() || !m_ped->m_pIntelligence || !m_ped->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[sa::TASK_PRIMARY_PRIMARY]) {
		return false;
	}

	return GetTaskTypeFromTask(&m_ped->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[sa::TASK_PRIMARY_PRIMARY]) == sa::TASK_SIMPLE_SIT_DOWN ||
			GetTaskTypeFromTask(&m_ped->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[sa::TASK_PRIMARY_PRIMARY]) == sa::TASK_SIMPLE_SIT_IDLE;
}

bool CPlayerPed::IsInPassengerDriveByMode()
{
	if (!m_ped || !GamePool_Ped_GetAt(m_gtaId) || !IsInVehicle() || !m_ped->m_pIntelligence || !m_ped->m_pIntelligence->m_TaskMgr.m_aSecondaryTasks[sa::TASK_PRIMARY_PRIMARY]) {
		return false;
	}

	return GetTaskTypeFromTask(&m_ped->m_pIntelligence->m_TaskMgr.m_aSecondaryTasks[sa::TASK_PRIMARY_PRIMARY]) == sa::TASK_SIMPLE_GANG_DRIVEBY;
}

bool CPlayerPed::StartPassengerDriveByMode()
{
	if (m_ped) {
		if (!m_ped->IsInVehicle() || !m_ped->pVehicle) {
			return false;
		}

		int iWeapon = GetCurrentWeapon();

		if (iWeapon == WEAPON_PARACHUTE) {
			SetArmedWeapon(0);
			return false;
		}

		// allowed weapons
		if (iWeapon != WEAPON_COLT45 && iWeapon != WEAPON_SILENCED && iWeapon != WEAPON_DEAGLE && iWeapon != WEAPON_TEC9 &&
				iWeapon != WEAPON_UZI && iWeapon != WEAPON_MP5 && iWeapon != WEAPON_AK47 && iWeapon != WEAPON_M4 &&
				iWeapon != WEAPON_SAWEDOFF && iWeapon != WEAPON_SHOTGSPA && iWeapon != WEAPON_MINIGUN &&
				iWeapon != WEAPON_GRENADE && iWeapon != WEAPON_MOLOTOV && iWeapon != WEAPON_SATCHEL) {
			return false;
		}

		SetArmedWeapon(iWeapon);

		ScriptCommand(&enter_passenger_driveby, m_gtaId, -1, -1, 0.0f, 0.0f, 0.0f, 300.0f, 8, 1, 100);

		return true;
	}
	return false;
}

void CPlayerPed::StopPassengerDriveByMode()
{
	if (!m_ped || !GamePool_Ped_GetAt(m_gtaId) || !IsInVehicle()) {
		return;
	}

	if (IsInPassengerDriveByMode()) {
		sa::CTask* dwJetPackTask = m_ped->m_pIntelligence->m_TaskMgr.m_aSecondaryTasks[sa::TASK_PRIMARY_PRIMARY];
		Memory::callFunction("_ZN22CTaskSimpleGangDriveByD0Ev", dwJetPackTask); // CTaskSimpleGangDriveBy::~CTaskSimpleGangDriveBy
		m_ped->m_pIntelligence->m_TaskMgr.m_aSecondaryTasks[sa::TASK_PRIMARY_PRIMARY] = nullptr;
	}
}

bool IsTaskRunNamedOrSlideToCoord(sa::CTask* pTask)
{
	uintptr_t vtable = *(uintptr_t*) pTask;
	if (vtable == g_saSym->GetVmtFunction<uintptr_t>("_ZTV23CTaskSimpleSlideToCoord", 1, true) || vtable == g_saSym->GetVmtFunction<uintptr_t>("_ZTV23CTaskSimpleRunNamedAnim", 2, true)) // CTaskSimpleSlideToCoord CTaskSimpleRunNamedAnim
	{
		return true;
	}
	return false;
}

sa::CTask* GetSubTaskFromTask(sa::CTask* pTask)
{
	uintptr_t vtable = *(uintptr_t*) pTask;
	return Memory::callFunction<sa::CTask*>(*reinterpret_cast<void**>(vtable + 3 * sizeof(void*)), pTask);
}

uint32_t CPlayerPed::GetCurrentAnimationIndexFlag()
{
	uint32_t dwAnim = 0;

	float fBlendData = 4.0f;

	int iAnimIdx = GetCurrentAnimationIndex(fBlendData);

	uint32_t hardcodedBlend = 0b00000100; // 4
	hardcodedBlend <<= 16;

	uint32_t hardcodedFlags = 0;

	if (iAnimIdx) {
		hardcodedFlags = 0b00010001; // 17
	}
	else {
		hardcodedFlags = 0b10000000; // 128
		iAnimIdx = 1189;
	}

	hardcodedFlags <<= 24;

	auto usAnimidx = (uint16_t) iAnimIdx;

	dwAnim = (uint32_t) usAnimidx;
	dwAnim |= hardcodedBlend;
	dwAnim |= hardcodedFlags;

	return dwAnim;
}

int CPlayerPed::GetCurrentAnimationIndex(float& blendData)
{
	blendData = 4.0f;

	if (!m_ped || !m_gtaId) {
		return 0;
	}

	if (!GamePool_Ped_GetAt(m_gtaId)) {
		return 0;
	}

	if (!m_ped->m_pRwObject) {
		return 0;
	}
	sa::CPedIntelligence* pIntelligence = m_ped->m_pIntelligence;

	if (pIntelligence) {
		sa::CTask* pTask = pIntelligence->m_TaskMgr.m_aPrimaryTasks[sa::TASK_PRIMARY_PRIMARY];

		if (pTask) {
			while (!IsTaskRunNamedOrSlideToCoord(pTask)) {
				pTask = GetSubTaskFromTask(pTask);
				if (!pTask) {
					return 0;
				}
			}

			const char* szName = (const char*) (((sa::CTaskSimpleRunNamedAnim*) pTask)->m_animName);
			const char* szGroupName = (const char*) (((sa::CTaskSimpleRunNamedAnim*) pTask)->m_animGroupName);

			std::string szStr = std::string(szGroupName);
			szStr += ":";
			szStr += szName;

			int idx = GetAnimIdxByName(szStr.c_str());
			if (idx == -1) {
				return 0;
			}
			else {
				return idx + 1;
			}
		}
	}
	return 0;
}

void CPlayerPed::ToggleCuffed(bool cuff)
{
	if (!m_ped || !GamePool_Ped_GetAt(m_gtaId) || !IsAdded()) {
		return;
	}

	if (cuff) {
		SetArmedWeapon(0);
		m_bCuffedState = true;
	}
	else {
		m_bCuffedState = false;
	}
}

void CPlayerPed::ToggleCarry(bool carry)
{
	if (!m_ped || !GamePool_Ped_GetAt(m_gtaId) || !IsAdded()) {
		return;
	}

	if (carry) {
		m_bCarryState = true;
		ApplyAnimation("CRRY_PRTIAL", "CARRY", 4.1, 0, 0, 0, 1, 1);
	}
	else {
		m_bCarryState = false;
		ApplyAnimation("CRRY_PRTIAL", "CARRY", 4.00, 0, 0, 0, 0, 1);
	}
}

void CPlayerPed::TogglePissing(bool piss)
{
	if (!m_ped || !GamePool_Ped_GetAt(m_gtaId) || IsInVehicle() || !IsAdded()) {
		return;
	}

	if (piss) {
		if (m_bPissingState) {
			return;
		}

		ApplyAnimation("PISS_LOOP", "PAULNMAC", 4.0f, 1, 0, 0, 0, -1);

		ScriptCommand(&attach_particle_to_actor2, "PETROLCAN", m_gtaId, 0.0f, 0.58f, -0.08f, 0.0f, 0.01f, 0.0f, 1, &m_dwPissParticlesHandle);
		ScriptCommand(&make_particle_visible, m_dwPissParticlesHandle);

		m_bPissingState = true;
	}
	else {
		if (!m_bPissingState) {
			return;
		}

		if (m_dwPissParticlesHandle) {
			ScriptCommand(&destroy_particle, m_dwPissParticlesHandle);
			m_dwPissParticlesHandle = 0;
		}

		RwMatrix mat = m_ped->GetMatrix().ToRwMatrix();
		TeleportTo(mat.pos.x, mat.pos.y, mat.pos.z);

		m_bPissingState = false;
	}
}

void CPlayerPed::StartDancing(int danceId)
{
	if (!m_ped || IsInVehicle() || !IsAdded()) {
		return;
	}

	if (danceId < 0 || danceId > 3) return;

	m_iDanceState = 1;
	m_iDanceStyle = danceId;

	ApplyAnimation(DanceIdleLoops[m_iDanceStyle], DanceStyleLibs[m_iDanceStyle], 16.0, 1, 0, 0, 0, -1);
}

void CPlayerPed::ExtinguishFire()
{

}

void CPlayerPed::SetImmunities(int bullet, int flame, int explode, int collision, int melee)
{
	if (m_ped && GamePool_Ped_GetAt(m_gtaId)) {
		ScriptCommand(&set_actor_immunities, m_gtaId, bullet, flame, explode, collision, melee);
	}
}

void CPlayerPed::SetSkillLevel(int iSkillID, int iLevel)
{
	// ~
}
void CPlayerPed::UpdateLocalPlayerSkill(int iSkillID, int iLevel)
{

}

void CPlayerPed::SetFightingStyle(int iStyle)
{
	if (m_gtaId) {
		ScriptCommand(&set_fighting_style, m_gtaId, iStyle, 6);
	}
}

float CPlayerPed::GetHealth()
{
	return m_ped ? m_ped->m_fHealth : 0.0f;
}

int CPlayerPed::GetVehicleSeatID()
{
	if (!m_ped) return -1;
	if (m_ped->m_nPedState == ACTION_INCAR && m_ped->pVehicle) {
		sa::CVehicle* pVehicle = (sa::CVehicle*) m_ped->pVehicle;

		if (pVehicle->pDriver == m_ped) {
			return 0;
		}
		for (int i = 0; i < 7; i++) {
			if (pVehicle->m_apPassengers[i] == m_ped) {
				return i + 1;
			}
		}
	}

	return -1;
}

void CPlayerPed::SetStateFlags(sa::CPedFlags dwState)
{
	if (m_ped) {
		m_ped->m_nPedFlags = dwState;
	}
}

sa::CPedFlags CPlayerPed::GetStateFlags()
{
	if (m_ped) {
		return m_ped->m_nPedFlags;
	}
	return {};
}

bool CPlayerPed::IsOnGround()
{
	if (m_ped) {
		const auto& flags = m_ped->m_nPedFlags;
		return flags.bIsStanding || flags.bWasStanding;
	}
	return false;
}

void CPlayerPed::SetDead()
{
	RwMatrix mat;

	if (m_gtaId && m_ped) {
		if (!m_ped->IsInVehicle()) {
			// ~
		}

		ExtinguishFire();
		mat = m_ped->GetMatrix().ToRwMatrix();
		TeleportTo(mat.pos.x, mat.pos.y, mat.pos.z);
		m_ped->m_fHealth = 0.0f;
		*pbyteCurrentPlayer = m_bytePlayerNumber;
		ScriptCommand(&kill_actor, m_gtaId);
		*pbyteCurrentPlayer = 0;
	}
}

void CPlayerPed::SetHealth(float fHealth)
{
	if (m_ped) {
		m_ped->m_fHealth = fHealth;
	}
}

float CPlayerPed::GetArmour()
{
	if (!m_ped) {
		return 0.0f;
	}
	return m_ped->m_fArmour;
}

void CPlayerPed::SetArmour(float fArmour)
{
	if (!m_ped) {
		return;
	}
	m_ped->m_fArmour = fArmour;
}

sa::CWeapon* CPlayerPed::GetCurrentWeaponSlot()
{
	return m_ped ? &m_ped->m_aWeapons[m_ped->m_nActiveWeaponSlot] : nullptr;
}

uint8_t CPlayerPed::GetCameraMode()
{
	return m_bytePlayerNumber == 0 ? GameGetLocalPlayerCameraMode() : GameGetPlayerCameraMode(m_bytePlayerNumber);
}

float CPlayerPed::GetAimZ()
{
	if (!m_ped || !m_ped->m_pPlayerData) return 0;
	return m_ped->m_pPlayerData->m_fLookPitch;
}

CAMERA_AIM* CPlayerPed::GetCurrentAim()
{
	return GameGetInternalAim();
}

uint8_t CPlayerPed::GetCurrentWeapon()
{
	if (!m_ped || !GamePool_Ped_GetAt(m_gtaId)) {
		return 0;
	}

	// crashes
	/*uint8_t weapon = 0;
	ScriptCommand(&get_actor_armed_weapon, m_gtaId, &weapon);
	return weapon;*/

	return GetCurrentWeaponSlot()->dwType;
}

sa::CVehicle* CPlayerPed::GetGtaVehicle()
{
	return m_ped->pVehicle;
}

void CPlayerPed::SatisfyHunger()
{
	// todo
}

void CPlayerPed::DrunkProcess()
{
	if (!m_ped || GetDrunkLevel() == 0) {
		return;
	}

	int iDrunkLevel = GetDrunkLevel();
	if (!m_bytePlayerNumber) {
		if (iDrunkLevel > 0 && iDrunkLevel <= 2000) {
			SetDrunkLevel(iDrunkLevel - 1);
			ScriptCommand(&set_player_drunk_visuals, m_bytePlayerNumber, 0);
		}
		else if (iDrunkLevel > 2000 && iDrunkLevel <= 50000) {
			int iDrunkVisual = iDrunkLevel * 0.02;
			if (iDrunkVisual <= 250) {
				if (iDrunkVisual < 5) {
					iDrunkVisual = 0;
				}
			}
			else {
				iDrunkVisual = 250;
			}

			SetDrunkLevel(iDrunkLevel - 1);
			ScriptCommand(&set_player_drunk_visuals, m_bytePlayerNumber, iDrunkVisual);

			if (IsInVehicle() && !IsAPassenger()) {
				sa::CVehicle* _pVehicle = GetGtaVehicle();
				if (_pVehicle) {
					if (!m_dwDrunkLastUpdateTick || (GetTickCount() - m_dwDrunkLastUpdateTick) > 200) {
						int iRandNumber = rand() % 40;
						float fRotation = 0.0;
						if (iRandNumber >= 20) {
							fRotation = 0.012;
							if (iDrunkLevel >= 5000) {
								fRotation = 0.015;
							}

							if (iRandNumber <= 30) {
								fRotation = -0.012;
								if (iDrunkLevel >= 5000) {
									fRotation = -0.015;
								}
							}
						}

						if (FloatOffset(_pVehicle->m_vecMoveSpeed.x, 0.0) > 0.050000001f ||
								FloatOffset(_pVehicle->m_vecMoveSpeed.y, 0.0) > 0.050000001f) {
							_pVehicle->m_vecTurnSpeed.z = fRotation + _pVehicle->m_vecTurnSpeed.z;
						}

						m_dwDrunkLastUpdateTick = GetTickCount();
					}
				}
			}
		}
	}
}

void CPlayerPed::SetKeys(uint16_t lrAnalog, uint16_t udAnalog, uint16_t wKeys)
{
	PAD_KEYS* pad = &RemotePlayerKeys[m_bytePlayerNumber];
	// LEFT/RIGHT
	pad->wKeyLR = lrAnalog;
	// UP/DOWN
	pad->wKeyUD = udAnalog;

	// KEY_ACTION
	pad->bKeys[ePadKeys::KEY_ACTION] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_CROUCH
	pad->bKeys[ePadKeys::KEY_CROUCH] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_FIRE
	pad->bKeys[ePadKeys::KEY_FIRE] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_SPRINT
	pad->bKeys[ePadKeys::KEY_SPRINT] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_SECONDARY_ATTACK
	pad->bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_JUMP
	pad->bKeys[ePadKeys::KEY_JUMP] = (wKeys & 1);
	if (!pad->bKeys[ePadKeys::KEY_JUMP]) {
		pad->bIgnoreJump = false;
	}
	wKeys >>= 1;
	// KEY_LOOK_RIGHT
	pad->bKeys[ePadKeys::KEY_LOOK_RIGHT] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_HANDBRAKE
	pad->bKeys[ePadKeys::KEY_HANDBRAKE] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_LOOK_LEFT
	pad->bKeys[ePadKeys::KEY_LOOK_LEFT] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_SUBMISSION
	pad->bKeys[ePadKeys::KEY_SUBMISSION] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_WALK
	pad->bKeys[ePadKeys::KEY_WALK] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_ANALOG_UP
	pad->bKeys[ePadKeys::KEY_ANALOG_UP] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_ANALOG_DOWN
	pad->bKeys[ePadKeys::KEY_ANALOG_DOWN] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_ANALOG_LEFT
	pad->bKeys[ePadKeys::KEY_ANALOG_LEFT] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_ANALOG_RIGHT
	pad->bKeys[ePadKeys::KEY_ANALOG_RIGHT] = (wKeys & 1);
}

uint16_t CPlayerPed::GetKeys(uint16_t* lrAnalog, uint16_t* udAnalog, uint8_t* ext, bool clear)
{
	*lrAnalog = LocalPlayerKeys.wKeyLR;
	*udAnalog = LocalPlayerKeys.wKeyUD;

	// ext
	if (ext != nullptr) {
		*ext = 0;
		if (LocalPlayerKeys.bKeys[ePadKeys::KEY_YES]) {
			*ext = 1;
		}
		if (LocalPlayerKeys.bKeys[ePadKeys::KEY_NO]) {
			*ext = 2;
		}
		if (LocalPlayerKeys.bKeys[ePadKeys::KEY_CTRL_BACK]) {
			*ext = 3;
		}
	}

	uint16_t wRet = 0;

	// KEY_ANALOG_RIGHT
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_ANALOG_RIGHT]) {
		wRet |= 1;
	}
	wRet <<= 1;
	// KEY_ANALOG_LEFT
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_ANALOG_LEFT]) {
		wRet |= 1;
	}
	wRet <<= 1;
	// KEY_ANALOG_DOWN
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_ANALOG_DOWN]) {
		wRet |= 1;
	}
	wRet <<= 1;
	// KEY_ANALOG_UP
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_ANALOG_UP]) {
		wRet |= 1;
	}
	wRet <<= 1;
	// KEY_WALK
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_WALK]) {
		wRet |= 1;
	}
	wRet <<= 1;
	// KEY_SUBMISSION
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_SUBMISSION]) {
		wRet |= 1;
	}
	wRet <<= 1;
	// KEY_LOOK_LEFT
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_LEFT]) {
		wRet |= 1;
	}
	wRet <<= 1;
	// KEY_HANDBRAKE
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE]) {
		wRet |= 1;
	}
	wRet <<= 1;
	// KEY_LOOK_RIGHT
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_RIGHT]) {
		wRet |= 1;
	}
	wRet <<= 1;
	// KEY_JUMP
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP]) {
		wRet |= 1;
	}
	wRet <<= 1;
	// KEY_SECONDARY_ATTACK
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK]) {
		wRet |= 1;
	}
	wRet <<= 1;
	// KEY_SPRINT
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT]) {
		wRet |= 1;
	}
	wRet <<= 1;
	// KEY_FIRE
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE]) {
		wRet |= 1;
	}
	wRet <<= 1;
	// KEY_CROUCH
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH]) {
		wRet |= 1;
	}
	wRet <<= 1;
	// KEY_ACTION
	if (LocalPlayerKeys.bKeys[ePadKeys::KEY_ACTION]) {
		wRet |= 1;
	}

	if (clear) {
		memset(LocalPlayerKeys.bKeys, 0, ePadKeys::SIZE);
	}

	return wRet;
}

void CPlayerPed::ApplyAnimation(const char* szAnimName, const char* szAnimLib, float fT, int opt1, int opt2, int opt3, int opt4, int iTime)
{
	if (!m_ped) {
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

	ScriptCommand(&apply_animation, m_gtaId, szAnimName, szAnimLib, fT, opt1, opt2, opt3, opt4, iTime);
}

void CPlayerPed::SetInterior(uint8_t byteInteriorId, bool bRefresh)
{
	if (m_ped && m_bytePlayerNumber != 0) {
		ScriptCommand(&link_actor_to_interior, m_gtaId, byteInteriorId);
	}
	else {
		ScriptCommand(&select_interior, byteInteriorId);
		ScriptCommand(&link_actor_to_interior, m_gtaId, byteInteriorId);
		if (bRefresh) {
			RwMatrix mat = m_ped->GetMatrix().ToRwMatrix();
			spdlog::info("refresh streaming at x: {}, y: {}", mat.pos.x, mat.pos.y);
			pGame->RefreshStreamingAt(mat.pos.x, mat.pos.y);
		}
	}
}

void CPlayerPed::ClearWeapons()
{
	if (!m_ped || !pbyteCurrentPlayer) {
		return;
	}

	*pbyteCurrentPlayer = m_bytePlayerNumber;
	m_ped->ClearWeapons();
	*pbyteCurrentPlayer = 0;
}

void CPlayerPed::ResetHealth()
{
	if (!m_ped) return;
	m_ped->m_fHealth = 100;
}

void CPlayerPed::SetModelIndex(uint uiModel)
{
	spdlog::info("SetModelIndex: {}", uiModel);

	if (!m_ped) {
		spdlog::error("SetModelIndex: m_ped is null");
		return;
	}

	if (!GamePool_Ped_GetAt(m_gtaId)) {
		spdlog::error("SetModelIndex: invalid gta id");
		return;
	}

	if (!IsValidPedModel(uiModel)) {
		spdlog::error("SetModelIndex: invalid model");
		uiModel = 0;
	}

	// CClothes::RebuildPlayer
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN8CClothes13RebuildPlayerEP10CPlayerPedb"));

	DestroyFollowPedTask();

	CEntity::SetModelIndex(uiModel);

	// CAEPedSpeechAudioEntity::Initialise
	Memory::callFunction("_ZN23CAEPedSpeechAudioEntity10InitialiseEP7CEntity", &m_ped->m_PedSpeechAudioEntity, m_ped);
}

void CPlayerPed::DestroyFollowPedTask()
{
	if (!m_ped) {
		return;
	}

	sa::CTask* dwIK = m_ped->m_pIntelligence->m_TaskMgr.m_aSecondaryTasks[sa::TASK_SECONDARY_IK];
	if (dwIK) {
		// CTaskSimpleIKManager::~CTaskSimpleIKManager
		Memory::callFunction("_ZN20CTaskSimpleIKManagerD0Ev", dwIK);
		m_ped->m_pIntelligence->m_TaskMgr.m_aSecondaryTasks[sa::TASK_SECONDARY_IK] = nullptr;
	}
}

void CPlayerPed::RestartIfWastedAt(sa::CVector* vecRestart, float fRotation)
{
	ScriptCommand(&restart_if_wasted_at, vecRestart->x, vecRestart->y, vecRestart->z, fRotation, 0);
}

void CPlayerPed::SetTargetRotation(float fRotation)
{
	if (m_ped && GamePool_Ped_GetAt(m_gtaId)) {
		m_ped->m_fCurrentRotation = DegToRad(fRotation);
		m_ped->m_fAimingRotation = DegToRad(fRotation);
		ScriptCommand(&set_actor_z_angle, m_gtaId, fRotation);
	}
}

void CPlayerPed::GiveWeapon(int iWeaponId, int iAmmo)
{
	if (m_ped && GamePool_Ped_GetAt(m_gtaId)) {
		int iWeaponModelID = GameGetWeaponModelIDFromWeaponID(iWeaponId);
		if (iWeaponModelID != -1) {
			if (!pGame->IsModelLoaded(iWeaponModelID)) {
				pGame->RequestModel(iWeaponModelID);
				pGame->LoadRequestedModels();
				while (!pGame->IsModelLoaded(iWeaponModelID))
					sleep(1);
			}

			*pbyteCurrentPlayer = m_bytePlayerNumber;
			// CPed::GiveWeapon
			Memory::callFunction("_ZN4CPed10GiveWeaponE11eWeaponTypejb", m_ped, iWeaponId, iAmmo, true);
			SetArmedWeapon(iWeaponId);
			*pbyteCurrentPlayer = 0;
		}
	}
}

void CPlayerPed::SetArmedWeapon(uint8_t weapon)
{
	if (m_ped && GamePool_Ped_GetAt(m_gtaId)) {
		*pbyteCurrentPlayer = m_bytePlayerNumber;
		m_ped->SetCurrentWeapon(weapon);
		*pbyteCurrentPlayer = 0;
	}
}

void CPlayerPed::SetAmmo(uint8_t byteWeapon, uint16_t wAmmo)
{
	if (m_ped) {
		sa::CWeapon* WeaponSlot = FindWeaponSlot(byteWeapon);
		if (WeaponSlot) {
			WeaponSlot->dwAmmo = (uint32_t) wAmmo;
		}
	}
}

sa::CWeapon* CPlayerPed::FindWeaponSlot(uint8_t byteWeapon)
{
	if (!m_ped) {
		return nullptr;
	}

	for (auto& m_weaponSlot : m_ped->m_aWeapons) {
		if (m_weaponSlot.dwType == byteWeapon) {
			return &m_weaponSlot;
		}
	}

	return nullptr;
}

void CPlayerPed::ApplyCommandTask(char* szTaskName, int p1, int p2, int p3, sa::CVector* p4, int p5, int p6, int p7, int p8, int p9)
{
	if (!m_ped) {
		return;
	}

	// IKChainManager_c::LookAt
	Memory::callFunction("_ZN16IKChainManager_c6LookAtEPcP4CPedP7CEntityiiP5RwV3dhfiih", g_saSym->GetSymbol("g_ikChainMan"), szTaskName, (uintptr_t) m_ped, p1, p2, p3, p4, p5, p6, p7, p8, p9);
}

void CPlayerPed::ProcessParachuteSkydiving()
{
	uint16_t lrAnalog, udAnalog;
	uint8_t exKeys = 0;
	uint16_t wKeys = GetKeys(&lrAnalog, &udAnalog, &exKeys);
	RwMatrix mat;

	if ((udAnalog > 0) && (m_iParachuteAnim != FALL_SKYDIVE_ACCEL)) {
		ApplyAnimation("FALL_SKYDIVE_ACCEL", "PARACHUTE", 1.0f, 1, 0, 0, 1, -2);
		m_iParachuteAnim = FALL_SKYDIVE_ACCEL;
	}
	else if ((udAnalog <= 0) && (m_iParachuteAnim != FALL_SKYDIVE)) {
		ApplyAnimation("FALL_SKYDIVE", "PARACHUTE", 1.0f, 1, 0, 0, 1, -2);
		m_iParachuteAnim = FALL_SKYDIVE;
	}

	// if the parachute object isn't created, do it now.
	if (!m_dwParachuteObject) {
		spdlog::info("Creating parachute object");
		mat = m_ped->GetMatrix().ToRwMatrix();
		ScriptCommand(&create_object, OBJECT_PARACHUTE, mat.pos.x, mat.pos.y, mat.pos.z, &m_dwParachuteObject);

		if (!GamePool_Object_GetAt(m_dwParachuteObject)) {
			spdlog::info("Parachute object creation failed");
			m_dwParachuteObject = 0;
			return;
		}

		ScriptCommand(&attach_object_to_actor, m_dwParachuteObject, m_gtaId, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		ScriptCommand(&set_object_visible, m_dwParachuteObject, 0);
	}

	if (!GamePool_Object_GetAt(m_dwParachuteObject)) {
		spdlog::info("Parachute object creation failed");
		m_dwParachuteObject = 0;
		return;
	}

	// process parachute opening event
	if (RemotePlayerKeys[m_bytePlayerNumber].bKeys[ePadKeys::KEY_FIRE]) {
		ApplyAnimation("PARA_OPEN", "PARACHUTE", 8.0f, 0, 0, 0, 1, -2);
		ScriptCommand(&apply_object_animation, m_dwParachuteObject, "PARA_OPEN_O", "PARACHUTE", 1000.0f, 0, 1);
		ScriptCommand(&set_object_visible, m_dwParachuteObject, 1);
		ScriptCommand(&set_object_scale, m_dwParachuteObject, 1.0f);
		m_iParachuteState = 2;
		m_iParachuteAnim = 0;
	}
}

void CPlayerPed::ProcessParachutes()
{
	if (!GamePool_Ped_GetAt(m_gtaId)) {
		return;
	}

	if (!IsAdded()) {
		return;
	}

	if (m_iParachuteState == 0) {
		if (m_dwParachuteObject) {
			ScriptCommand(&disassociate_object, m_dwParachuteObject, 0.0f, 0.0f, 0.0f, 0);
			ScriptCommand(&destroy_object_with_fade, m_dwParachuteObject);
			m_dwParachuteObject = 0;
		}

		// See if we should enter the initial parachuting state.
		if (GetCurrentWeapon() == WEAPON_PARACHUTE) // armed with para
		{
			if (ScriptCommand(&is_actor_falling_think, m_gtaId)) // is falling
			{
				float fDistanceFromGround;
				ScriptCommand(&get_actor_distance_from_ground, m_gtaId, &fDistanceFromGround); // TODO: check if this will crash??
				if (fDistanceFromGround > 20.0f) {
					// ScriptCommand(&actor_set_collision, m_dwGTAId, 0);
					m_iParachuteState = 1;
					m_iParachuteAnim = 0;
				}
			}
		}
		return;
	}

	if ((GetCurrentWeapon() != WEAPON_PARACHUTE) || ScriptCommand(&is_actor_in_the_water, m_gtaId)) {
		// A parachuting state is active, but they no longer have the parachute
		// or they've ended up in the water.
		if (m_dwParachuteObject) {
			ScriptCommand(&disassociate_object, m_dwParachuteObject, 0.0f, 0.0f, 0.0f, 0);
			ScriptCommand(&destroy_object_with_fade, m_dwParachuteObject);
			// ScriptCommand(&actor_set_collision, m_dwGTAId, 1);
			RwMatrix mat = m_ped->GetMatrix().ToRwMatrix();
			TeleportTo(mat.pos.x, mat.pos.y, mat.pos.z);
			m_dwParachuteObject = 0;
		}

		m_iParachuteState = 0;
		m_iParachuteAnim = 0;
	}

	if (m_iParachuteState == 1) {
		ProcessParachuteSkydiving();
		return;
	}

	if (m_iParachuteState == 2) {
		ProcessParachuting();
		return;
	}
}

void CPlayerPed::ProcessParachuting()
{
	uint16_t lrAnalog, udAnalog;
	uint8_t exKeys = 0;
	uint16_t wKeys = GetKeys(&lrAnalog, &udAnalog, &exKeys);

	if ((udAnalog > 0) && (m_iParachuteAnim != PARA_DECEL)) {
		ApplyAnimation("PARA_DECEL", "PARACHUTE", 1.0f, 1, 0, 0, 1, -2);
		ScriptCommand(&apply_object_animation, m_dwParachuteObject, "PARA_DECEL_O", "PARACHUTE", 1.0f, 1, 1);
		m_iParachuteAnim = PARA_DECEL;
	}
	else if ((udAnalog <= 0) && (m_iParachuteAnim != PARA_FLOAT)) {
		ApplyAnimation("PARA_FLOAT", "PARACHUTE", 1.0f, 1, 0, 0, 1, -2);
		ScriptCommand(&apply_object_animation, m_dwParachuteObject, "PARA_FLOAT_O", "PARACHUTE", 1.0f, 1, 1);
		m_iParachuteAnim = PARA_FLOAT;
	}
}

void CPlayerPed::PlayAnimByIdx(int idx, float BlendData)
{
	if (!idx) {
		// reset character animation
		RwMatrix mat = m_ped->GetMatrix().ToRwMatrix();
		TeleportTo(mat.pos.x, mat.pos.y, mat.pos.z);
		return;
	}

	std::string szAnim;
	std::string szBlock;

	char pszAnim[40];
	char pszBlock[40];

	memset(&pszAnim[0], 0, 40);
	memset(&pszBlock[0], 0, 40);

	bool bTest = false;
	const char* pBegin = GetAnimByIdx(idx - 1);
	if (!pBegin) {
		return;
	}
	while (*pBegin) {
		if (*pBegin == ':') {
			pBegin++;
			bTest = true;
			continue;
		}
		if (!bTest) {
			szBlock += *pBegin;
		}
		if (bTest) {
			szAnim += *pBegin;
		}
		pBegin++;
	}

	strcpy(&pszAnim[0], szAnim.c_str());
	strcpy(&pszBlock[0], szBlock.c_str());

	// pUI->chat()->addDebugMessage("PlayAnimByIdx: Current %s:%s anim", &pszAnim[0], &pszBlock[0]);

	ApplyAnimation(&pszAnim[0], &pszBlock[0], BlendData, 0, 1, 1, 0, 0);
}

bool CPlayerPed::IsPlayingAnim(int idx)
{
	if (!m_ped || !m_gtaId || (idx == 0)) {
		return false;
	}
	if (!GamePool_Ped_GetAt(m_gtaId)) {
		return false;
	}
	if (!m_ped->m_pRwObject) {
		return false;
	}

	const char* pAnim = GetAnimByIdx(idx - 1);
	if (!pAnim) {
		return false;
	}
	const char* pNameAnim = strchr(pAnim, ':') + 1;

	auto blendAssoc = Memory::callFunction<uintptr_t>("_Z30RpAnimBlendClumpGetAssociationP7RpClumpPKc", m_ped->m_pRwObject, pNameAnim);

	if (blendAssoc) {
		return true;
	}
	else {
		return false;
	}
}

void CPlayerPed::SetRotation(float fRotation)
{
	if (m_ped) {
		if (GamePool_Ped_GetAt(m_gtaId)) {
			m_ped->m_fCurrentRotation = DegToRad(fRotation);
			m_ped->m_fAimingRotation = DegToRad(fRotation);
		}
	}
}

void CPlayerPed::ExitCurrentVehicle()
{
	if (!m_ped) {
		return;
	}
	if (!GamePool_Ped_GetAt(m_gtaId)) {
		return;
	}

	if (m_ped->IsInVehicle()) {
		int dwGtaVehicleID = GetCurrentVehicleID();
		if (dwGtaVehicleID) {
			sa::CVehicle* pGtaVehicle = GamePool_Vehicle_GetAt(dwGtaVehicleID);
			if (pGtaVehicle) {
				int iModelIndex = pGtaVehicle->m_nModelIndex;
				if (iModelIndex != TRAIN_PASSENGER && iModelIndex != TRAIN_PASSENGER_LOCO) {
					//trigger script command LeaveCar(ped,vehicle);
					ScriptCommand(&make_actor_leave_car, m_gtaId, dwGtaVehicleID);
				}
			}
		}
	}
}

int CPlayerPed::GetCurrentVehicleID()
{
	if (m_ped) {
		return GamePool_Vehicle_GetIndex(m_ped->pVehicle);
	}

	return 0;
}

void CPlayerPed::CheckVehicleParachute()
{
	if (m_dwParachuteObject) {
		ScriptCommand(&disassociate_object, m_dwParachuteObject, 0.0, 0.0, 0.0, 0);
		ScriptCommand(&destroy_object, m_dwParachuteObject);
		m_dwParachuteObject = 0;
	}
}

// todo
void CPlayerPed::ProcessVehicleHorn()
{

}

void CPlayerPed::SetAttachedObject(int index, NEW_ATTACHED_OBJECT* pNewAttachedObject)
{
	if (m_ped && !IsEntityPlaceable(m_ped)) {
		if (m_ped->m_pRwObject) {
			if (index >= 0 && index < 10) {
				int iBoneID = pNewAttachedObject->iBoneID;
				if (iBoneID > 0 && iBoneID <= 18) {
					if (GetObjectSlotState(index)) {
						RemoveAttachedObject(index);
					}

					memcpy(&m_attachedObjects[index].info, pNewAttachedObject, sizeof(NEW_ATTACHED_OBJECT));

					RwMatrix matPlayer = m_ped->GetMatrix().ToRwMatrix();
					auto pNewObject = new CObject(pNewAttachedObject->iModel, {matPlayer.pos.x, matPlayer.pos.y, matPlayer.pos.z}, pNewAttachedObject->vecRot, 200.0f, true);

					m_attachedObjects[index].object = pNewObject;
					m_attachedObjects[index].slotUsed = true;

					if (pNewAttachedObject->dwMaterialColor1) {
						pNewObject->SetMaterial(-1, 0, nullptr, nullptr, pNewAttachedObject->dwMaterialColor1);
					}
					if (pNewAttachedObject->dwMaterialColor2) {
						pNewObject->SetMaterial(-1, 1, nullptr, nullptr, pNewAttachedObject->dwMaterialColor2);
					}

					pNewObject->SetCollisionChecking(false);
				}
			}
		}
	}
}

void CPlayerPed::RemoveAttachedObject(int index)
{
	if (GetObjectSlotState(index)) {
		if (m_attachedObjects[index].object) {
			delete m_attachedObjects[index].object;
			m_attachedObjects[index].object = nullptr;
		}

		memset(&m_attachedObjects[index].info, 0, sizeof(NEW_ATTACHED_OBJECT));
		m_attachedObjects[index].slotUsed = false;
	}
}

bool CPlayerPed::GetObjectSlotState(int index)
{
	if (index < 0 || index >= 10) {
		return false;
	}

	return m_attachedObjects[index].slotUsed;
}

bool CPlayerPed::HasAttachedObject()
{
	for (auto& m_attachedObject : m_attachedObjects) {
		if (m_attachedObject.slotUsed) {
			return true;
		}
	}

	return false;
}

void CPlayerPed::RemoveAllAttachedObjects()
{
	for (auto& m_attachedObject : m_attachedObjects) {
		if (m_attachedObject.slotUsed) {
			if (m_attachedObject.object) {
				delete m_attachedObject.object;
				m_attachedObject.object = nullptr;
			}

			memset(&m_attachedObject.info, 0, sizeof(NEW_ATTACHED_OBJECT));
			m_attachedObject.slotUsed = false;
		}
	}
}

sa::CAnimBlendClumpData** GetClumpData(RpClump* clump)
{
	RwInt32 ClumpOffset = *g_saSym->GetSymbol<RwInt32*>("ClumpOffset");
	return RWPLUGINOFFSET(sa::CAnimBlendClumpData*, clump, ClumpOffset);
}

void UpdateElementRpHAnim(sa::CEntity* ent)
{
	if (ent) {
		// CEntity::UpdateRpHAnim
		Memory::callFunction("_ZN7CEntity13UpdateRpHAnimEv", ent);

		if (ent->m_nModelIndex == 0) { // CJ skin
			RpClump* clump = ent->m_pRpClump;
			if (clump) {
				// CPed::ShoulderBoneRotation
				Memory::callFunction("_ZN4CPed20ShoulderBoneRotationEP7RpClump", clump);
			}
			else {
				spdlog::error("UpdateElementRpHAnim: clump is null");
			}
		}
	}
	else {
		spdlog::error("UpdateElementRpHAnim: ent is null");
	}
}

void CPlayerPed::ProcessCuff()
{
	if (!m_ped || !IsCuffed()) {
		return;
	}

	int boneIds[] = {BONE_L_UPPER_ARM, BONE_L_FORE_ARM, BONE_L_HAND, BONE_R_UPPER_ARM, BONE_R_FORE_ARM, BONE_R_HAND};

	// yaw pitch roll
	float rotations[][3] = {
			{26.57374382019, 61.337575733622, 59.206573486328},
			{27.843754291534, 15.3639249801636, 46.40625},
			{-81.018516340527, 342.87482380867, 326.11833715439},
			{338.839179039, 53.49357098341, 298.45233917236},
			{307.68748283386, 22.110015869141, 313.59375},
			{96.047592163086, 357.88313293457, 56.739406585693}

	};

	for (int i = 0; i < 6; i++) {
		// updating the bone frame orientation will also update its children
		// This rotation is only applied when UpdateElementRpHAnim is called
		sa::CAnimBlendClumpData* clumpData = *GetClumpData(m_ped->m_pRpClump);
		sa::AnimBlendFrameData* frameData = clumpData->GetFrameDataByNodeId(boneIds[i]);
		if (frameData) {
			RtQuat* boneOrientation = &frameData->GetFrameOrientation();
			RwV3d angles = {rotations[i][0], rotations[i][2], rotations[i][1]};
			// BoneNode_c::EulerToQuat(RwV3d_0 *euler, RtQuat_0 *quat)
			Memory::callFunction("_ZN10BoneNode_c11EulerToQuatEP5RwV3dP6RtQuat", &angles, boneOrientation);
			m_ped->m_bDontUpdateHierarchy = false;
		}
	}

	UpdateElementRpHAnim(m_ped);
}

void CPlayerPed::ProcessAttachedObjects()
{
	bool bAnimUpdated = false;
	RwMatrix boneMatrix;
	sa::CVector vecOut;

	for (auto& m_attachedObject : m_attachedObjects) {
		if (m_attachedObject.slotUsed) {
			if (m_attachedObject.object && m_attachedObject.object->m_entity) {
				if (IsAdded()) {
					if (!bAnimUpdated) {
						if (m_ped) {
							UpdateElementRpHAnim(m_ped);
							bAnimUpdated = true;
						}
					}

					int iBoneID = m_attachedObject.info.iBoneID;
					int iBoneIndex = 0;

					if (m_ped->m_apBones[iBoneID] == nullptr) {
						return;
					}

					iBoneIndex = m_ped->m_apBones[iBoneID]->m_nNodeId;

					// CPhysical::Remove
					m_attachedObject.object->m_entity->Remove();

					GetBoneMatrix(&boneMatrix, iBoneIndex);

					ProjectMatrix(&vecOut, reinterpret_cast<sa::CMatrix*>(&boneMatrix), &m_attachedObject.info.vecOffset);
					boneMatrix.pos = vecOut;

					if (m_attachedObject.info.vecRot.x != 0.0f) {
						RwMatrixRotate(&boneMatrix, 0, m_attachedObject.info.vecRot.x);
					}
					if (m_attachedObject.info.vecRot.y != 0.0f) {
						RwMatrixRotate(&boneMatrix, 1, m_attachedObject.info.vecRot.y);
					}
					if (m_attachedObject.info.vecRot.z != 0.0f) {
						RwMatrixRotate(&boneMatrix, 2, m_attachedObject.info.vecRot.z);
					}

					RwMatrixScale(&boneMatrix, &m_attachedObject.info.vecScale);

					m_attachedObject.object->m_entity->SetMatrix((sa::CMatrix&) boneMatrix);
					m_attachedObject.object->UpdateRwMatrixAndFrame();

					// CPhysical::add
					m_attachedObject.object->m_entity->Add();
				}
				else {
					m_attachedObject.object->TeleportTo(0.0f, 0.0f, 0.0f);
				}
			}
		}
	}
}

void CPlayerPed::PutDirectlyInVehicle(uint32_t dwVehicleGTAId, uint8_t byteSeatID)
{
	spdlog::info("PutDirectlyInVehicle: {} {}", dwVehicleGTAId, dwVehicleGTAId);

	if (!m_ped) {
		return;
	}

	if (!GamePool_Vehicle_GetAt(dwVehicleGTAId)) {
		return;
	}

	if (!GamePool_Ped_GetAt(m_gtaId)) {
		return;
	}

	if (GetCurrentWeapon() == WEAPON_PARACHUTE) {
		SetArmedWeapon(0);
	}

	sa::CVehicle* pGtaVehicle = GamePool_Vehicle_GetAt(dwVehicleGTAId);

	if (pGtaVehicle->fHealth != 0.0f && !IsEntityPlaceable(pGtaVehicle)) {
		if (GetVehicleSubtype(pGtaVehicle) == VEHICLE_SUBTYPE_CAR || GetVehicleSubtype(pGtaVehicle) == VEHICLE_SUBTYPE_BIKE) {
			if (byteSeatID > pGtaVehicle->m_nMaxPassengers) {
				return;
			}
		}

		if (byteSeatID) {
			ScriptCommand(&put_actor_in_car2, m_gtaId, dwVehicleGTAId, byteSeatID - 1);
		}
		else {
			if (pGtaVehicle->pDriver && pGtaVehicle->pDriver->IsInVehicle()) {
				return;
			}

			ScriptCommand(&put_actor_in_car, m_gtaId, dwVehicleGTAId);
		}

		if (m_ped == GamePool_FindPlayerPed()) {
			if (m_ped->IsInVehicle()) {
				pGame->GetCamera()->SetBehindPlayer();
			}
		}

		if (pNetGame) {
			CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
			VEHICLEID VehicleID = pVehiclePool->FindIDFromGtaPtr(pGtaVehicle);
			if (VehicleID != INVALID_VEHICLE_ID && VehicleID <= MAX_VEHICLES) {
				CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
				if (pVehicle) {
					if (pVehicle->IsATrainPart()) {
						if (m_ped == GamePool_FindPlayerPed()) {
							ScriptCommand(&camera_on_vehicle, pVehicle->m_gtaId, 3, 2);
						}
					}
				}
			}
		}
	}
}

void CPlayerPed::GetBoneMatrix(RwMatrix* matOut, int iBoneID)
{
	if (m_ped && !IsEntityPlaceable(m_ped)) {
		if (m_ped->m_pRpClump) {
			auto pAnimHierarchy = Memory::callFunction<RpHAnimHierarchy*>("_Z29GetAnimHierarchyFromSkinClumpP7RpClump", m_ped->m_pRpClump); // GetAnimHierarchyFromSkinClump

			if (!pAnimHierarchy) {
				return;
			}

			int iAnimIndex = Memory::callFunction<int>("_Z17RpHAnimIDGetIndexP16RpHAnimHierarchyi", pAnimHierarchy, iBoneID); // RpHAnimIDGetIndex
			memcpy(matOut, &pAnimHierarchy->pMatrixArray[iAnimIndex], sizeof(RwMatrix));
		}
	}
}

void CPlayerPed::EnterVehicle(uint32_t dwVehicleGTAId, bool bPassenger)
{
	if (!m_ped) {
		return;
	}

	sa::CVehicle* pGtaVehicle = GamePool_Vehicle_GetAt(dwVehicleGTAId);
	if (!pGtaVehicle) {
		return;
	}

	if (!GamePool_Ped_GetAt(m_gtaId)) {
		return;
	}

	// hide parachute
	if (GetCurrentWeapon() == WEAPON_PARACHUTE) {
		SetArmedWeapon(0);
	}

	// Is it a passenger
	if (bPassenger) {
		//Vehicle model index is not a train passenger seat
		if (pGtaVehicle->m_nModelIndex != TRAIN_PASSENGER || m_ped != GamePool_FindPlayerPed()) {
			//enter the passenger seat
			ScriptCommand(&send_actor_to_car_passenger, m_gtaId, dwVehicleGTAId, 3000, -1);
		}
		else {
			ScriptCommand(&put_actor_in_car2, m_gtaId, dwVehicleGTAId, -1);
		}
	}
	else {
		//get into the driver's seat
		ScriptCommand(&send_actor_to_car_driverseat, m_gtaId, dwVehicleGTAId, 3000);
	}
}

void CPlayerPed::SetCameraMode(uint8_t byteCameraMode)
{
	GameSetPlayerCameraMode(byteCameraMode, m_bytePlayerNumber);
}

void CPlayerPed::SetCurrentAim(CAMERA_AIM* pAim)
{
	GameStoreRemotePlayerAim(m_bytePlayerNumber, pAim);
}

void CPlayerPed::SetAimZ(float fAimZ)
{
	if (!isnan(fAimZ) && fAimZ <= 100.0f && fAimZ >= -100.0f) {
		if (m_ped) {
			m_ped->m_pPlayerData->m_fLookPitch = fAimZ;
		}
	}
}

uint8_t CPlayerPed::FindDeathReasonAndResponsiblePlayer(unsigned short* pPlayerID)
{
	uint8_t byteDeathReason;
	PLAYERID PlayerIDWhoKilled;

	CVehiclePool* pVehiclePool;
	CPlayerPool* pPlayerPool;

	if (pNetGame) {
		pVehiclePool = pNetGame->GetVehiclePool();
		pPlayerPool = pNetGame->GetPlayerPool();
	}
	else {
		*pPlayerID = INVALID_PLAYER_ID;
		return 0xFF;
	}

	if (m_ped) {
		byteDeathReason = m_ped->m_nLastDamagedWeaponType;
		if (byteDeathReason < WEAPON_CAMERA || byteDeathReason == WEAPON_HELIBLADES || byteDeathReason == WEAPON_EXPLOSION) {
			if (m_ped->m_pLastEntityDamage) {
				PlayerIDWhoKilled = pPlayerPool->FindRemotePlayerIDFromGtaPtr(static_cast<sa::CPed*>(m_ped->m_pLastEntityDamage));

				if (PlayerIDWhoKilled != INVALID_PLAYER_ID) {
					*pPlayerID = PlayerIDWhoKilled;
					return byteDeathReason;
				}
				else {
					if (pVehiclePool->FindIDFromGtaPtr(static_cast<sa::CVehicle*>(m_ped->m_pLastEntityDamage)) != INVALID_VEHICLE_ID) {
						sa::CVehicle* pGtaVehicle = static_cast<sa::CVehicle*>(m_ped->m_pLastEntityDamage);
						PlayerIDWhoKilled = pPlayerPool->FindRemotePlayerIDFromGtaPtr(pGtaVehicle->pDriver);

						if (PlayerIDWhoKilled != INVALID_PLAYER_ID) {
							*pPlayerID = PlayerIDWhoKilled;
							return byteDeathReason;
						}
					}
				}
			}
			*pPlayerID = INVALID_PLAYER_ID;
			return 0xFF;
		}
		else if (byteDeathReason == WEAPON_DROWN) {
			*pPlayerID = INVALID_PLAYER_ID;
			return WEAPON_DROWN;
		}
		else if (byteDeathReason == WEAPON_VEHICLE) {
			if (m_ped->m_pLastEntityDamage) {
				if (pVehiclePool->FindIDFromGtaPtr(static_cast<sa::CVehicle*>(m_ped->m_pLastEntityDamage)) != INVALID_VEHICLE_ID) {
					sa::CVehicle* pGtaVehicle = static_cast<sa::CVehicle*>(m_ped->m_pLastEntityDamage);

					PlayerIDWhoKilled = pPlayerPool->FindRemotePlayerIDFromGtaPtr(pGtaVehicle->pDriver);

					if (PlayerIDWhoKilled != INVALID_PLAYER_ID) {
						*pPlayerID = PlayerIDWhoKilled;
						return WEAPON_VEHICLE;
					}
				}
			}
		}
		else if (byteDeathReason == WEAPON_COLLISION) {
			if (m_ped->m_pLastEntityDamage) {
				if (pVehiclePool->FindIDFromGtaPtr(static_cast<sa::CVehicle*>(m_ped->m_pLastEntityDamage)) != INVALID_VEHICLE_ID) {
					sa::CVehicle* pGtaVehicle = static_cast<sa::CVehicle*>(m_ped->m_pLastEntityDamage);

					PlayerIDWhoKilled = pPlayerPool->FindRemotePlayerIDFromGtaPtr(pGtaVehicle->pDriver);

					if (PlayerIDWhoKilled != INVALID_PLAYER_ID) {
						*pPlayerID = PlayerIDWhoKilled;
						return WEAPON_COLLISION;
					}
				}
				else {
					*pPlayerID = INVALID_PLAYER_ID;
					return WEAPON_COLLISION;
				}
			}
		}
	}

	*pPlayerID = INVALID_PLAYER_ID;
	return 0xFF;
}

void CPlayerPed::ProcessBulletData(BULLET_DATA* btData)
{
	if (!m_ped || !GamePool_Ped_GetAt(m_gtaId)) {
		return;
	}

	BULLET_SYNC_DATA bulletSyncData;

	if (btData) {
		m_bHaveBulletData = true;
		m_bulletData.pEntity = btData->pEntity;
		m_bulletData.vecOrigin.x = btData->vecOrigin.x;
		m_bulletData.vecOrigin.y = btData->vecOrigin.y;
		m_bulletData.vecOrigin.z = btData->vecOrigin.z;

		m_bulletData.vecPos.x = btData->vecPos.x;
		m_bulletData.vecPos.y = btData->vecPos.y;
		m_bulletData.vecPos.z = btData->vecPos.z;

		m_bulletData.vecOffset.x = btData->vecOffset.x;
		m_bulletData.vecOffset.y = btData->vecOffset.y;
		m_bulletData.vecOffset.z = btData->vecOffset.z;

		uint8_t byteHitType = 0;
		unsigned short InstanceID = 0xFFFF;

		if (m_bytePlayerNumber == 0) {
			if (pNetGame) {
				CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
				if (pPlayerPool) {
					CPlayerPed* pPlayerPed = pPlayerPool->GetLocalPlayer()->GetPlayerPed();
					if (pPlayerPed) {
						memset(&bulletSyncData, 0, sizeof(BULLET_SYNC_DATA));
						if (pPlayerPed->GetCurrentWeapon() != WEAPON_SNIPER || btData->pEntity) {
							if (btData->pEntity) {
								CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
								CObjectPool* pObjectPool = pNetGame->GetObjectPool();

								PLAYERID PlayerID;
								VEHICLEID VehicleID;
								uint16_t ObjectID;

								if (pVehiclePool && pObjectPool) {
									PlayerID = pPlayerPool->FindRemotePlayerIDFromGtaPtr((sa::CPed*) btData->pEntity);
									if (PlayerID == INVALID_PLAYER_ID) {
										VehicleID = pVehiclePool->FindIDFromGtaPtr((sa::CVehicle*) btData->pEntity);
										if (VehicleID == INVALID_VEHICLE_ID) {
											ObjectID = pObjectPool->FindIDFromGtaPtr(btData->pEntity);
											if (ObjectID == INVALID_OBJECT_ID) {
												sa::CVector vecOut;
												vecOut.x = 0.0f;
												vecOut.y = 0.0f;
												vecOut.z = 0.0f;

												if (btData->pEntity->m_matrix) {
													ProjectMatrix(&vecOut, btData->pEntity->m_matrix, &btData->vecOffset);
													btData->vecOffset.x = vecOut.x;
													btData->vecOffset.y = vecOut.y;
													btData->vecOffset.z = vecOut.z;
												}
												else {
													btData->vecOffset.x = btData->pEntity->GetPosition().x + btData->vecOffset.x;
													btData->vecOffset.y = btData->pEntity->GetPosition().y + btData->vecOffset.y;
													btData->vecOffset.z = btData->pEntity->GetPosition().z + btData->vecOffset.z;
												}
											}
											else {
												// object
												byteHitType = 3;
												InstanceID = ObjectID;
											}
										}
										else {
											// vehicle
											byteHitType = 2;
											InstanceID = VehicleID;
										}
									}
									else {
										// player
										byteHitType = 1;
										InstanceID = PlayerID;
									}
								}
							}

							bulletSyncData.vecOrigin.x = btData->vecOrigin.x;
							bulletSyncData.vecOrigin.y = btData->vecOrigin.y;
							bulletSyncData.vecOrigin.z = btData->vecOrigin.z;

							bulletSyncData.vecPos.x = btData->vecPos.x;
							bulletSyncData.vecPos.y = btData->vecPos.y;
							bulletSyncData.vecPos.z = btData->vecPos.z;

							bulletSyncData.vecOffset.x = btData->vecOffset.x;
							bulletSyncData.vecOffset.y = btData->vecOffset.y;
							bulletSyncData.vecOffset.z = btData->vecOffset.z;

							bulletSyncData.targetType = byteHitType;
							bulletSyncData.targetId = InstanceID;
							bulletSyncData.weaponId = pPlayerPed->GetCurrentWeapon();

							RakNet::BitStream bsBullet;
							bsBullet.Write((char) ID_BULLET_SYNC);
							bsBullet.Write((char*) &bulletSyncData, sizeof(BULLET_SYNC_DATA));
							pNetGame->GetRakClient()->Send(&bsBullet, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
						}
					}
				}
			}
		}
	}
	else {
		m_bHaveBulletData = false;
		memset(&m_bulletData, 0, sizeof(BULLET_DATA));
	}
}

extern uint32_t (* CWeapon_FireInstantHit)(sa::CWeapon* _this, sa::CPed* pFiringEntity, sa::CVector* vecOrigin, sa::CVector* muzzlePosn, sa::CEntity* targetEntity, sa::CVector* target, sa::CVector* originForDriveBy, bool arg6, bool muzzle);
extern uint32_t (* CWeapon_FireSniper)(sa::CWeapon* _this, sa::CPed* pFiringEntity, sa::CEntity* victim, sa::CVector* target);

CPlayerPed* g_pCurrentFiredPed = nullptr;
BULLET_DATA* g_pCurrentBulletData = nullptr;

void CPlayerPed::FireInstant()
{
	spdlog::info("CPlayerPed::FireInstant");

	uint8_t byteSavedCameraMode = 0;
	uint16_t wSavedCameraMode2 = 0;

	if (m_bytePlayerNumber != 0) {
		byteSavedCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(m_bytePlayerNumber);
		wSavedCameraMode2 = *wCameraMode2;
		*wCameraMode2 = GameGetPlayerCameraMode(m_bytePlayerNumber);
		if (*wCameraMode2 == 4) {
			*wCameraMode2 = 0;
		}

		GameStoreLocalPlayerCameraExtZoomAndAspect();
		GameSetRemotePlayerCameraExtZoomAndAspect(m_bytePlayerNumber);
		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(m_bytePlayerNumber);

		//GameStoreLocalPlayerSkills();
		//GameSetRemotePlayerSkills(m_bytePlayerNumber);
	}

	g_pCurrentFiredPed = this;

	if (m_bHaveBulletData) {
		g_pCurrentBulletData = &m_bulletData;
	}
	else {
		g_pCurrentBulletData = nullptr;
	}

	sa::CVector vecBonePos, vecOut;

	if (GetCurrentWeapon() == WEAPON_SNIPER) {
		if (m_ped) {
			CWeapon_FireSniper(GetCurrentWeaponSlot(), m_ped, nullptr, nullptr);
		}
		else {
			CWeapon_FireSniper(nullptr, nullptr, nullptr, nullptr);
		}
	}
	else {
		GetWeaponInfoForFire(false, &vecBonePos, &vecOut);

		sa::CWeapon* pSlot = GetCurrentWeaponSlot();

		if (m_ped) {
			CWeapon_FireInstantHit(pSlot, m_ped, &vecBonePos, &vecOut, nullptr, nullptr, nullptr, false, true);
		}
		else {
			CWeapon_FireInstantHit(nullptr, nullptr, &vecBonePos, &vecOut, nullptr, nullptr, nullptr, false, true);
		}
	}

	g_pCurrentFiredPed = nullptr;

	if (m_bytePlayerNumber != 0) {
		*pbyteCameraMode = byteSavedCameraMode;
		*wCameraMode2 = wSavedCameraMode2;

		GameSetLocalPlayerCameraExtZoomAndAspect();
		GameSetLocalPlayerAim();

		//GameSetLocalPlayerSkills();
	}
}

void CPlayerPed::GetWeaponInfoForFire(bool bLeftWrist, sa::CVector* vecBonePos, sa::CVector* vecOut)
{
	if (!m_ped || !GamePool_Ped_GetAt(m_gtaId)) {
		return;
	}

	sa::CVector* pFireOffset = GetCurrentWeaponFireOffset();
	if (pFireOffset && vecBonePos && vecOut) {
		vecOut->x = pFireOffset->x;
		vecOut->y = pFireOffset->y;
		vecOut->z = pFireOffset->z;

		int bone_id = 24;
		if (bLeftWrist) {
			bone_id = 34;
		}

		m_ped->GetBonePosition(vecBonePos, bone_id, false);

		vecBonePos->z += pFireOffset->z + 0.15f;
		GetTransformedBonePosition(bone_id, vecOut);
	}
}

uintptr_t GetWeaponInfo(int iWeapon, int iSkill)
{
	// CWeaponInfo::GetWeaponInfo
	return Memory::callFunction<uintptr_t>("_ZN11CWeaponInfo13GetWeaponInfoE11eWeaponTypea", iWeapon, iSkill);
}

sa::CVector* CPlayerPed::GetCurrentWeaponFireOffset()
{
	if (!m_ped || !GamePool_Ped_GetAt(m_gtaId)) {
		return nullptr;
	}

	sa::CWeapon* pSlot = GetCurrentWeaponSlot();
	if (pSlot) {
		return (sa::CVector*) (GetWeaponInfo(pSlot->dwType, 1) + 0x24);
	}
	return nullptr;
}

void CPlayerPed::GetTransformedBonePosition(int iBoneID, sa::CVector* vecOut)
{
	if (!m_ped) {
		return;
	}

	// CPed::GetTransformedBonePosition
	Memory::callFunction("_ZN4CPed26GetTransformedBonePositionER5RwV3djb", m_ped, vecOut, iBoneID, false);
}

void CPlayerPed::SetCameraZoomAndAspect(float fExtZoom, float fAspectRatio)
{
	GameSetPlayerCameraExtZoomAndAspect(m_bytePlayerNumber, fExtZoom, fAspectRatio);
}

sa::CEntity* CPlayerPed::GetEntityUnderPlayer()
{
	if (!m_ped || !GamePool_Ped_GetAt(m_gtaId)) {
		return nullptr;
	}

	sa::CEntity* entity = nullptr;
	sa::CVector vecStart;
	sa::CVector vecEnd;
	sa::CColPoint colPoint{};

	if (IsInVehicle()) {
		return nullptr;
	}

	vecStart.x = m_ped->m_matrix->m_pos.x;
	vecStart.y = m_ped->m_matrix->m_pos.y;
	vecStart.z = m_ped->m_matrix->m_pos.z - 0.25f;

	vecEnd.x = m_ped->m_matrix->m_pos.x;
	vecEnd.y = m_ped->m_matrix->m_pos.y;
	vecEnd.z = vecStart.z - 1.75f;

	ProcessLineOfSight(&vecStart, &vecEnd, &colPoint, &entity, false, true, false, true, false, false, false, false);
	return entity;
}

void CPlayerPed::ResetDamageEntity()
{
	// TODO
}

int CPlayerPed::GetPedStat()
{
	if (!m_ped) {
		return -1;
	}

	return (int) ((sa::CPedModelInfo*) (GetModelInfoByID(m_ped->m_nModelIndex)))->m_defaultPedStats;
}
