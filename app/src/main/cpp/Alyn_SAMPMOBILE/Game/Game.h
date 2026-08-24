#pragma once

#include <SA.h>

#include "Common.h"
#include "Aimstuff.h"
#include "Camera.h"
#include "Entity.h"
#include "Vehicle.h"
#include "Object.h"
#include "PlayerPed.h"
#include "Actor.h"
#include "Animation.h"
#include "Hooks.h"
#include "Pad.h"
#include "Patches.h"
#include "Font.h"
#include "TextDraw.h"
#include "Scripting.h"
#include "Util.h"
#include "RadarColors.h"
#include "SnapShotHelper.h"
#include "MaterialTextGenerator.h"
#include "RemoveBuilding.h"
#include "RGBA.h"
#include "../Java/EditObject.h"

class Game {
public:
	Game();
	~Game();

	void Initialize();
	void StartGame();

	void ToggleThePassingOfTime(bool bOnOff);
	void EnableClock(bool bEnable);
	void EnableZoneNames(bool bEnable);
	void SetWorldTime(int iHour, int iMinute);
	void GetWorldTime(int* iHour, int* iMinute);
	void PreloadObjectsAnims();
	void SetWorldWeather(int iWeatherID);
	void DisplayHUD(bool bDisp);
	void UpdateCheckpoints();
	uint8_t GetActiveInterior();
	uint8_t GetPedSlotsUsed();
	void PlaySound(int iSound, float fX, float fY, float fZ);
	void RefreshStreamingAt(float x, float y);
	void DisableTrainTraffic();
	void UpdateGlobalTimer(uint32_t dwTime);
	void SetGravity(float fGravity);
	float FindGroundZForCoord(float fX, float fY, float fZ);

	void DrawGangZone(float fPos[], uint32_t dwColor, bool bFullMap);

	uint32_t CreatePickup(int iModel, int iType, float fX, float fY, float fZ, int* pdwindex);
	CObject* NewObject(int iModel, sa::CVector vecPos, sa::CVector vecRot, float fDrawDistance);
	CPlayerPed* NewPlayer(int iSkin, float fX, float fY, float fZ, float fRotation, bool unk, bool bIsNPC);
	bool RemovePlayer(CPlayerPed* pPlayer);

	CVehicle* NewVehicle(int iVehicleType, float fX, float fY, float fZ, float fRotation, bool bAddSiren);

	void DisableMarker(uint32_t dwMarker);

	uint32_t CreateRadarMarkerIcon(uint8_t byteType, float fPosX, float fPosY, float fPosZ, uint32_t dwColor, uint8_t byteStyle);

	bool IsModelLoaded(int iModel);
	void RequestModel(int iModel);
	void LoadRequestedModels();
	void RemoveModel(int iModel, bool bFromStreaming);

	bool IsAnimationLoaded(const char* szAnimLib);
	void RequestAnimation(const char* szAnimLib);

	CCamera* GetCamera()
	{
		if (m_gameCamera == nullptr) {
			m_gameCamera = new CCamera();
		}
		return m_gameCamera;
	}

	CPlayerPed* FindPlayerPed()
	{
		if (m_gamePlayer == nullptr) {
			m_gamePlayer = new CPlayerPed();
		}
		return m_gamePlayer;
	}

	static bool IsGamePaused();
	void DisableAutoAim();
	void SetWantedLevel(uint8_t level);
	void EnableStuntBonus(bool bEnable);
	void DisplayGameText(const char* szStr, int iTime, int iSize);
	void AddToLocalMoney(int iAmmount);
	void ResetLocalMoney();
	int GetLocalMoney();
	void DisableEnterExits();

	void SetCheckpointInformation(sa::CVector* vecPos, sa::CVector* vecSize);
	void SetRaceCheckpointInformation(uint8_t byteType, sa::CVector* vecPos, sa::CVector* vecNextPos, float fRadius);
	void MakeRaceCheckpoint();
	void DisableRaceCheckpoint();

	void EnableGameInput(bool enable) { m_gameInput = enable; };
	[[nodiscard]] bool IsGameInputEnabled() const { return m_gameInput; }

	void ToggleCJWalk(bool bUseCJWalk);

	void EnableCheckpoints(bool enable) { m_checkpointsEnabled = enable; };
	[[nodiscard]] bool AreCheckpointsEnabled() const { return m_checkpointsEnabled; }

	void EnableRaceCheckpoints(bool enable) { m_raceCheckpointsEnabled = enable; };
	[[nodiscard]] bool AreRaceCheckpointsEnabled() const { return m_raceCheckpointsEnabled; }

private:
	CCamera* m_gameCamera;
	CPlayerPed* m_gamePlayer;

	bool m_gameInput;
	bool m_clockEnabled;
	bool m_preloadedVehicleModels[212];

	bool m_checkpointsEnabled;
	sa::CVector m_checkpointPos;
	sa::CVector m_checkpointExtent;
	uint32_t m_checkpointMarker;

	bool m_raceCheckpointsEnabled;
	sa::CVector m_raceCheckpointPos;
	sa::CVector m_raceCheckpointNextPos;
	uint8_t m_raceType;
	float m_raceCheckpointRadius;
	uint32_t m_raceCheckpointMarker;
	uint32_t m_raceCheckpointHandle;
};
