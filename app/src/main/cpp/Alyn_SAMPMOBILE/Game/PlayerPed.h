#pragma once

#define PLAYER_PED_SLOTS    210

#define FALL_SKYDIVE        1
#define FALL_SKYDIVE_ACCEL    2
#define PARA_DECEL            3
#define PARA_FLOAT            4

#pragma pack(push, 1)
typedef struct _NEW_ATTACHED_OBJECT {
	int iModel;
	int iBoneID;
	sa::CVector vecOffset;
	sa::CVector vecRot;
	sa::CVector vecScale;
	uint32_t dwMaterialColor1;
	uint32_t dwMaterialColor2;
} NEW_ATTACHED_OBJECT;

typedef struct _BULLET_DATA {
	sa::CVector vecOrigin;
	sa::CVector vecPos;
	sa::CVector vecOffset{0.0f, 0.0f, 0.0f};
	sa::CEntity* pEntity;
} BULLET_DATA;
#pragma pack(pop)

class CPlayerPed : public CEntity {
public:
	CPlayerPed();
	CPlayerPed(int iNum, int iSkin, float fX, float fY, float fZ, float fRotation);
	~CPlayerPed() override;

	bool IsInVehicle();
	bool IsAPassenger();
	void RemoveFromVehicleAndPutAt(float fX, float fY, float fZ);
	void RestartIfWastedAt(sa::CVector* vecRestart, float fRotation);

	void ExtinguishFire();
	void TogglePlayerControllable(bool bControllable);
	void DrunkProcess();
	void RemoveWeaponWhenEnteringVehicle();

	uint8_t GetActionTrigger();
	sa::CVehicle* GetGtaVehicle();
	void SetDead();
	bool IsDead();
	void SetHealth(float fHealth);
	float GetHealth();
	void SetArmour(float fArmour);
	float GetArmour();
	void SatisfyHunger();
	uint8_t GetCurrentWeapon();
	void SetInitialState();
	void SetModelIndex(uint uiModel);
	void ClearWeapons();
	void ResetHealth();
	void GiveWeapon(int iWeaponId, int iAmmo);
	void SetArmedWeapon(uint8_t weapon);
	void SetTargetRotation(float fRotation);
	void SetImmunities(int bullet, int flame, int explode, int collision, int melee);
	void ShowMarker(int nIndex);
	void SetFightingStyle(int iStyle);
	void SetRotation(float fRotation);
	void ApplyCommandTask(char* szTaskName, int p1, int p2, int p3, sa::CVector* p4, int p5, int p6, int p7, int p8, int p9);
	void DestroyFollowPedTask();
	void GetTransformedBonePosition(int iBoneID, sa::CVector* vecOut);
	void ApplyAnimation(const char* szAnimName, const char* szAnimLib, float fT, int opt1, int opt2, int opt3, int opt4, int iTime);
	void SetInterior(uint8_t byteInteriorId, bool bRefresh);
	void PutDirectlyInVehicle(uint32_t dwVehicleGTAId, uint8_t byteSeatID);
	void EnterVehicle(uint32_t dwVehicleGtaId, bool bPassenger);
	void ExitCurrentVehicle();
	int GetCurrentVehicleID();
	void SetSkillLevel(int iSkillID, int iLevel);
	void SetAmmo(uint8_t byteWeapon, uint16_t wAmmo);
	sa::CWeapon* FindWeaponSlot(uint8_t byteWeapon);
	int GetVehicleSeatID();

	sa::CPedFlags GetStateFlags();
	void SetStateFlags(sa::CPedFlags dwState);
	bool IsOnGround();

	bool IsCuffed() { return m_bCuffedState; };
	void ToggleCuffed(bool cuff);

	bool IsCarry() { return m_bCarryState; }
	void ToggleCarry(bool carry);

	CAMERA_AIM* GetCurrentAim();
	void SetCurrentAim(CAMERA_AIM* pAim);

	uint8_t GetCameraMode();
	void SetCameraMode(uint8_t byteCameraMode);

	float GetCameraExtendedZoom() { return GameGetLocalPlayerCameraExtZoom(); }
	void SetCameraZoomAndAspect(float fExtZoom, float fAspectRatio);

	float GetAimZ();
	void SetAimZ(float fAimZ);

	sa::CWeapon* GetCurrentWeaponSlot();

	void SetKeys(uint16_t lrAnalog, uint16_t udAnalog, uint16_t wKeys);
	uint16_t GetKeys(uint16_t* lrAnalog, uint16_t* udAnalog, uint8_t* ext = 0, bool clear = true);

	void CheckVehicleParachute();
	void ProcessVehicleHorn();

	void SetAttachedObject(int index, NEW_ATTACHED_OBJECT* pNewAttachedObject);
	void RemoveAttachedObject(int index);
	bool GetObjectSlotState(int index);
	bool HasAttachedObject();
	void RemoveAllAttachedObjects();
	bool IsValidAttach(int index)
	{
		if (index < 0 || index >= 10) return false;
		return m_attachedObjects[index].info.iModel != 0;
	}

	void ProcessCuff();
	void ProcessAttachedObjects();
	void GetBoneMatrix(RwMatrix* matOut, int iBoneID);

	void FireInstant();
	void GetWeaponInfoForFire(bool bLeftWrist, sa::CVector* vecBonePos, sa::CVector* vecOut);
	sa::CVector* GetCurrentWeaponFireOffset();
	void ProcessBulletData(BULLET_DATA* btData);

	uint8_t FindDeathReasonAndResponsiblePlayer(unsigned short* pPlayerID);

	void SetSpecialAction(uint8_t byteSpecialAction);
	uint8_t GetSpecialAction();

	void StartJetpack();
	void StopJetpack();
	bool IsInJetpack();

	void ApplyCrouch();
	void ResetCrouch();
	bool IsCrouching();

	int HasHandsUp();
	void HandsUp();

	bool IsDancing();
	int GetDanceStyle() { return m_iDanceStyle; }
	void StartDancing(int danceId);
	void StopDancing();
	void ProcessDancing();
	char* GetDanceAnimForMove(int iMove);

	bool IsPissing() { return m_bPissingState; }
	void TogglePissing(bool piss);

	void ToggleCellphone(bool on);
	int IsCellphoneEnabled() { return m_bCellPhoneEnabled; }

	void SetDrunkLevel(uint32_t dwLevel)
	{
		if (dwLevel > 50000) { dwLevel = 50000; }
		else if (dwLevel < 0) dwLevel = 0;

		m_dwDrunkLevel = dwLevel;
	}
	uint32_t GetDrunkLevel() { return m_dwDrunkLevel; }

	void ProcessParachutes();
	void ProcessParachuteSkydiving();
	void ProcessParachuting();

	bool IsInPassengerDriveByMode();
	bool StartPassengerDriveByMode();
	void StopPassengerDriveByMode();

	uint32_t GetCurrentAnimationIndexFlag();

	bool IsPlayingAnim(int idx);
	int GetCurrentAnimationIndex(float& blendData);
	void PlayAnimByIdx(int idx, float BlendData);

	sa::CEntity* GetEntityUnderPlayer();

	int GetPedStat();

	// tasks
	uint8_t IsEnteringVehicle();
	bool IsExitingVehicle();
	bool IsJumpTask();
	bool IsTakeDamageFallTask();
	bool IsSwimTask();
	bool IsSitTask();

	void UpdateLocalPlayerSkill(int iSkillID, int iLevel);
	void ResetDamageEntity();

private:
	int m_bCuffedState;
	int m_bCarryState;

	int m_iDanceState;
	int m_iDanceStyle;
	int m_iLastDanceMove;
	int m_iDancingState;
	int m_iDancingAnim;

	bool m_bCellPhoneEnabled;

	int m_bPissingState;
	uint32_t m_dwPissParticlesHandle;

	int m_iParachuteState;
	int m_iParachuteAnim;

	uint32_t m_dwDrunkLevel;
	uint32_t m_dwDrunkLastUpdateTick;

	bool m_bHaveBulletData;
	BULLET_DATA m_bulletData;

public:
	sa::CPed* m_ped;
	uint8_t m_bytePlayerNumber;
	uint32_t m_dwArrow;
	uint32_t m_dwParachuteObject;

	// attached object
	struct {
		bool slotUsed;
		NEW_ATTACHED_OBJECT info;
		CObject* object;
	} m_attachedObjects[10];
};
