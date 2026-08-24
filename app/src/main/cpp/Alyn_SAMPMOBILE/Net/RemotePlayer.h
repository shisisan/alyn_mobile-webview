#pragma once

#define PLAYER_STATE_NONE            0
#define PLAYER_STATE_ONFOOT            17
#define PLAYER_STATE_PASSENGER        18
#define PLAYER_STATE_DRIVER            19
#define PLAYER_STATE_EXIT_VEHICLE                4
#define PLAYER_STATE_ENTER_VEHICLE_DRIVER        5
#define PLAYER_STATE_ENTER_VEHICLE_PASSENGER    6
#define PLAYER_STATE_WASTED            32
#define PLAYER_STATE_SPAWNED        33

#define UPDATE_TYPE_NONE            0
#define UPDATE_TYPE_ONFOOT            16
#define UPDATE_TYPE_INCAR            17
#define UPDATE_TYPE_PASSENGER        18

class CRemotePlayer {
public:
	CRemotePlayer();
	~CRemotePlayer();

	void SetID(PLAYERID playerId)
	{
		m_PlayerID = playerId;
	}

	PLAYERID GetID()
	{
		return m_PlayerID;
	}

	void SetNPC()
	{
		m_bIsNPC = true;
	}

	bool IsNPC()
	{
		return m_bIsNPC;
	}

	uint8_t GetState() { return m_byteState; }
	void SetState(uint8_t byteState)
	{
		if (byteState != m_byteState) {
			StateChange(byteState, m_byteState);
			m_byteState = byteState;
		}
	}
	void StateChange(uint8_t byteNewState, uint8_t byteOldState);

	VEHICLEID CurrentVehicleID() { return m_VehicleID; }

	void SetPlayerColor(uint32_t dwColor);
	uint32_t GetPlayerColor();
	uint32_t GetPlayerColorAsARGB();

	void Process();
	bool Spawn(uint8_t byteTeam, int iSkin, sa::CVector* vecPos, float fRotation, uint32_t dwColor, uint8_t byteFightingStyle);
	void Remove();
	void HandleDeath();
	void WeaponSyncCheck(uint8_t netCurrentWeapon);
	void Say(char* szMessage);

	void SetTeam(uint8_t byteTeam) { m_byteTeam = byteTeam; }

	CPlayerPed* GetPlayerPed() { return m_pPlayerPed; }

	void StoreOnFootFullSyncData(ONFOOT_SYNC_DATA* ofSync, uint32_t dwTime);
	void StoreInCarFullSyncData(INCAR_SYNC_DATA* picSync, uint32_t dwTime);
	void StoreAimFullSyncData(AIM_SYNC_DATA* aimSync);
	void StorePassengerFullSyncData(PASSENGER_SYNC_DATA* psSync);
	void StoreTrailerFullSyncData(TRAILER_SYNC_DATA* trSync);
	void StoreUnoccupiedFullSyncData(UNOCCUPIED_SYNC_DATA* ucSync);
	void StoreBulletFullSyncData(BULLET_SYNC_DATA* btSync);

	void ResetAllSyncAttributes();
	bool IsActive()
	{
		if (m_pPlayerPed && m_byteState != PLAYER_STATE_NONE) {
			return true;
		}
		else {
			return false;
		}
	}

	void UpdateOnFootTargetPosition();
	void UpdateOnFootPositionAndSpeed(sa::CVector* vecPos, sa::CVector* vecMove);

	void UpdateTrainDriverMatrixAndSpeed(RwMatrix* matWorld, sa::CVector* vecMoveSpeed, float fTrainSpeed);
	void UpdateInCarTargetPosition();
	void UpdateInCarMatrixAndSpeed(RwMatrix* pMat, sa::CVector* pVecPos, sa::CVector* pVecMoveSpeed);
	void UpdateVehicleRotation();

	void InterpolateAndRotate();

	void RemoveFromVehicle();
	void PutInCurrentVehicle();
	void EnterVehicle(VEHICLEID VehicleID, bool bPassenger);
	void ExitVehicle();

	float GetDistanceFromLocalPlayer();

	void ShowGlobalMarker(short sPosX, short sPosY, short sPosZ);
	void HideGlobalMarker();

	void ProcessSpecialAction(unsigned char byteSpecialAction);
	void ProcessAnimation();

	bool IsSurfing();
	bool SurfingOnVehicle();
	bool SurfingOnObject();
	void ProcessSurfing();

public:
	bool m_bShowNameTag;
	CVehicle* m_pCurrentVehicle;
	float m_fReportedHealth;
	float m_fReportedArmour;
	bool m_bIsAFK;
	uint8_t m_byteTeam;

private:
	PLAYERID m_PlayerID;
	VEHICLEID m_VehicleID;
	bool m_bIsNPC;
	uint8_t m_byteState;
	uint8_t m_byteUpdateFromNetwork;
	uint8_t m_byteSpecialAction;

	bool m_bWasAnimSettedFlag;
	uint16_t m_usPlayingAnimIndex;

	uint8_t m_byteSeatID;
	bool m_bPassengerDriveByMode;

	sa::CVector m_vecOnFootTargetPos;
	sa::CVector m_vecOnFootTargetSpeed;
	sa::CVector m_vecPositionInaccuracy;
	sa::CQuaternion m_quat;
	sa::CVector m_vecInCarTargetPos;
	sa::CVector m_vecInCarTargetSpeed;

	ONFOOT_SYNC_DATA m_onFootSync;
	INCAR_SYNC_DATA m_inCarSync;
	PASSENGER_SYNC_DATA m_passengerSync;

	uint32_t m_dwLastRecvTick;
	uint32_t m_dwLastStoredSyncDataTime;
	uint32_t m_dwLastHeadUpdate;

	CPlayerPed* m_pPlayerPed;
	uint32_t m_dwMarker;

	bool m_bGlobalMarkerShown;
};