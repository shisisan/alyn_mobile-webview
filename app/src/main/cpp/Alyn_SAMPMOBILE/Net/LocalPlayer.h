#pragma once

#define SPECTATE_TYPE_NONE                        0
#define SPECTATE_TYPE_PLAYER                    1
#define SPECTATE_TYPE_VEHICLE                    2

#define SPECIAL_ACTION_NONE                        0
#define SPECIAL_ACTION_DUCK                        1
#define SPECIAL_ACTION_USEJETPACK                2
#define SPECIAL_ACTION_ENTER_VEHICLE            3
#define SPECIAL_ACTION_EXIT_VEHICLE                4
#define SPECIAL_ACTION_DANCE1                    5
#define SPECIAL_ACTION_DANCE2                    6
#define SPECIAL_ACTION_DANCE3                    7
#define SPECIAL_ACTION_DANCE4                    8
#define SPECIAL_ACTION_HANDSUP                    10
#define SPECIAL_ACTION_USECELLPHONE                11
#define SPECIAL_ACTION_SITTING                    12
#define SPECIAL_ACTION_STOPUSECELLPHONE            13
#define SPECIAL_ACTION_DRINK_BEER                20
#define SPECIAL_ACTION_SMOKE_CIGGY                21
#define SPECIAL_ACTION_DRINK_WINE                22
#define SPECIAL_ACTION_DRINK_SPRUNK                23
#define SPECIAL_ACTION_CUFFED                    24
#define SPECIAL_ACTION_CARRY                    25
#define SPECIAL_ACTION_PISSING                    68

#define BULLET_HIT_TYPE_NONE                    0
#define BULLET_HIT_TYPE_PLAYER                    1
#define BULLET_HIT_TYPE_VEHICLE                    2
#define BULLET_HIT_TYPE_OBJECT                    3
#define BULLET_HIT_TYPE_PLAYER_OBJECT            4

#define INVALID_PLAYER_ID    0xFFFF
#define NO_TEAM 0xFF

enum eSurfingMode {
	SURFING_MODE_NONE,
	SURFING_MODE_UNFIXED,
	SURFING_MODE_FIXED
};

enum eWeaponState {
	WS_NO_BULLETS = 0,
	WS_LAST_BULLET = 1,
	WS_MORE_BULLETS = 2,
	WS_RELOADING = 3,
};

#pragma pack(push, 1)
typedef struct _PLAYER_SPAWN_INFO {
	uint8_t byteTeam;
	int iSkin;
	uint8_t unk;
	sa::CVector vecPos;
	float fRotation;
	int iSpawnWeapons[3];
	int iSpawnWeaponsAmmo[3];
} PLAYER_SPAWN_INFO; // size 46

typedef struct _ONFOOT_SYNC_DATA {
	uint16_t lrAnalog;                // +0
	uint16_t udAnalog;                // +2
	uint16_t wKeys;                    // +4
	sa::CVector vecPos;                    // +6
	sa::CQuaternion quat;                // +18
	uint8_t byteHealth;                // +34
	uint8_t byteArmour;                // +35
	uint8_t byteCurrentWeapon;        // +36
	uint8_t byteSpecialAction;        // +37
	sa::CVector vecMoveSpeed;            // +38
	sa::CVector vecSurfOffsets;            // +50
	uint16_t wSurfInfo;                // +62
	uint32_t dwAnimation;            // +64
} ONFOOT_SYNC_DATA;                    // size = 68

typedef struct _INCAR_SYNC_DATA {
	VEHICLEID VehicleID;            // +0
	uint16_t lrAnalog;                // +2
	uint16_t udAnalog;                // +4
	uint16_t wKeys;                    // +6
	sa::CQuaternion quat;                // +8
	sa::CVector vecPos;                    // +24
	sa::CVector vecMoveSpeed;            // +36
	float fCarHealth;                // +48
	uint8_t bytePlayerHealth;        // +52
	uint8_t bytePlayerArmour;        // +53
	uint8_t byteCurrentWeapon;        // +54
	uint8_t byteSirenOn;            // +55
	uint8_t byteLandingGearState;    // +56
	VEHICLEID TrailerID;            // +57
	union {                            // +59
		uint16_t sHydraThrustAngle[2];
		float fTrainSpeed;
	};
} INCAR_SYNC_DATA;                    // size = 63

typedef struct _PASSENGER_SYNC_DATA {
	VEHICLEID VehicleID;            // +0
	uint8_t byteSeatFlags;                // +2
//	uint8_t byteSeatFlags : 7;      // +2
//	uint8_t byteDriveBy : 1;        // +2
	uint8_t byteCurrentWeapon;        // +3
	uint8_t bytePlayerHealth;        // +4
	uint8_t bytePlayerArmour;        // +5
	uint16_t lrAnalog;                // +6
	uint16_t udAnalog;                // +8
	uint16_t wKeys;                    // +10
	sa::CVector vecPos;                    // +12
} PASSENGER_SYNC_DATA;                // size = 24

typedef struct _UNOCCUPIED_SYNC_DATA {
	VEHICLEID vehicleId;    // +0
	uint8_t byteSeatId;        // +2
	sa::CVector vecRoll;            // +3
	sa::CVector vecDirection;    // +15
	sa::CVector vecPos;            // +27
	sa::CVector vecMoveSpeed;    // +39
	sa::CVector vecTurnSpeed;    // +51
	float fCarHealth;        // +63
} UNOCCUPIED_SYNC_DATA;        // size = 67

typedef struct _TRAILER_SYNC_DATA {
	VEHICLEID trailerId;    // +0
	sa::CVector vecPos;            // +2
	sa::CQuaternion quat;        // +14
	sa::CVector vecMoveSpeed;    // +30
	sa::CVector vecTurnSpeed;    // +42
} TRAILER_SYNC_DATA;        // size = 54

typedef struct _AIM_SYNC_DATA {
	uint8_t byteCamMode;            // +0
	sa::CVector vecAimf1;                // +1
	sa::CVector vecAimPos;                // +13
	float fAimZ;                    // +25
	uint8_t byteCamExtZoom : 6;        // +29 - 0-63 normalized
	uint8_t byteWeaponState : 2;    // +30 - see eWeaponState
	uint8_t byteAspectRatio;        // +31
} AIM_SYNC_DATA;                    // size = 32

typedef struct _BULLET_SYNC_DATA {
	uint8_t targetType;    // +0
	uint16_t targetId;    // +1
	sa::CVector vecOrigin;    // +3
	sa::CVector vecPos;        // +15
	sa::CVector vecOffset;    // +27
	uint8_t weaponId;    // +28
} BULLET_SYNC_DATA;        // size = 28

typedef struct _SPECTATOR_SYNC_DATA {
	uint16_t lrAnalog;    // +0
	uint16_t udAnalog;    // +2
	uint16_t wKeys;        // +4
	sa::CVector vecPos;        // +6
} SPECTATOR_SYNC_DATA;    // size = 18
#pragma pack(pop)

class CLocalPlayer {
public:
	CLocalPlayer();
	~CLocalPlayer();

	bool Process();
	bool Spawn();
	void ResetAllSyncAttributes();

	void ToggleSpectating(bool bToggle);
	void ProcessSpectating();
	bool IsSpectating() { return m_bIsSpectating; }
	bool IsInRCMode() { return m_bInRCMode; };
	void SpectatePlayer(PLAYERID PlayerID);
	void SpectateVehicle(VEHICLEID VehicleID);

	void CheckWeapons();
	void UpdateRemoteInterior(uint8_t byteInterior);
	void UpdateCameraTarget();
	void UpdateVehicleDamage(VEHICLEID vehicleID);
	bool IsClearedToSpawn() { return m_bClearedToSpawn; }
	void HandleClassSelection();

	bool EnterVehicleAsPassenger();
	bool HandlePassengerEntry();

	void ProcessOnFootWorldBounds();
	void ProcessInCarWorldBounds();
	bool CompareOnFootSyncKeys(uint16_t wKeys, uint16_t lrAnalog, uint16_t udAnalog);
	void ApplySpecialAction(uint8_t byteSpecialAction);
	void SetSpawnInfo(PLAYER_SPAWN_INFO* pSpawnInfo);
	void HandleClassSelectionOutcome(bool bOutcome);
	uint8_t GetSpecialAction();
	uint32_t GetPlayerColorAsARGB();
	uint32_t GetPlayerColorAsRGBA();
	void SetPlayerColor(uint32_t dwColor);

	int GetOptimumOnFootSendRate();
	int GetOptimumInCarSendRate();

	void SendPrevClass();
	void SendNextClass();
	void SendSpawn();
	void RequestClass(int iClass);
	void RequestSpawn();
	void ProcessClassSelection();

	void SendStatsUpdate();
	void SendWastedNotification();
	void SendOnFootFullSyncData();
	void SendInCarFullSyncData();
	void SendPassengerFullSyncData();
	void SendTrailerFullSyncData(VEHICLEID vehicleId);
	void SendUnoccupiedFullSyncData();
	void SendAimSyncData();

	bool IsNeedSyncDataSend(const void* data1, const void* data2, size_t size);

	void SendEnterVehicleNotification(VEHICLEID VehicleID, bool bPassenger);
	void SendExitVehicleNotification(VEHICLEID VehicleID);

	void SendTakeDamageEvent(PLAYERID PlayerID, float fDamageFactor, int weaponType, int pedPieceType);
	void SendGiveDamageEvent(PLAYERID PlayerID, float fDamageFactor, int weaponType, int pedPieceType);
	void SendGiveDamageActorEvent(PLAYERID ActorID, float fDamageFactor, int weaponType, int pedPieceType);

	CPlayerPed* GetPlayerPed() { return m_pPlayerPed; }

	void ProcessSurfing();
	void UpdateSurfing();
	void DisableSurfing() { m_surfSync.bIsActive = false; }

public:
	bool m_bWaitingForSpawnRequestReply;
	uint32_t m_SpectateID;
	bool m_bSpectateProcessed;
	uint8_t m_byteSpectateMode;
	uint8_t m_byteSpectateType;
	uint8_t m_byteTeam;
	bool m_bIsActive;
	int m_iSelectedClass;
	VEHICLEID m_LastVehicle;

	struct {
		sa::CVector vecOffsetPos;
		int dwSurfVehID;

		bool bIsActive;
		bool bIsVehicle;
		uintptr_t pSurfInst;
	} m_surfSync;

private:
	CPlayerPed* m_pPlayerPed;
	bool m_bIsWasted;
	bool m_bClearedToSpawn;
	bool m_bInRCMode;
	bool m_bHasSpawnInfo;
	bool m_bSpawnDialogShowed;
	uint8_t m_byteCurrentWeapon;
	uint8_t m_byteLastWeapon[13];
	uint32_t m_dwLastAmmo[13];

	PLAYER_SPAWN_INFO m_SpawnInfo;
	ONFOOT_SYNC_DATA m_ofSync;
	INCAR_SYNC_DATA m_icSync;
	PASSENGER_SYNC_DATA m_psSync;
	TRAILER_SYNC_DATA m_trSync;
	UNOCCUPIED_SYNC_DATA m_ucSync;
	AIM_SYNC_DATA m_aimSync;

	struct {
		uint32_t dwLastMoney;
		uint32_t dwLastDrunkLevel;
	} m_statsSync;

	bool m_bIsSpectating;

	bool m_bPassengerDriveByMode;

	int m_iDisplayZoneTick;
	uint32_t m_dwLastStatsUpdateTick;
	uint32_t m_dwLastSendTick;
	uint32_t m_dwLastSendAimSyncTick;
	uint32_t m_dwLastSendSyncTick;
	uint32_t m_dwLastSendSpecTick;
	uint32_t m_dwLastWeaponsUpdateTick;
	uint32_t m_dwLastHeadUpdate;

	uint8_t m_byteCurInterior;
	VEHICLEID m_CurrentVehicle;
};