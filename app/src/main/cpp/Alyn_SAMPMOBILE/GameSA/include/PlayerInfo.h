#pragma once

#include "Entity/Ped/PlayerPedData.h"
#include "Entity/Ped/PlayerPed.h"
#include "RenderWare/rwcore.h"

namespace sa {
struct CPed;
struct CVehicle;

enum ePlayerState : uint8_t {
	PLAYERSTATE_PLAYING,
	PLAYERSTATE_HAS_DIED,
	PLAYERSTATE_HAS_BEEN_ARRESTED,
	PLAYERSTATE_FAILED_MISSION,
	PLAYERSTATE_LEFT_GAME
};

struct CPlayerInfo {
	CPlayerPed* m_pPed;
	CPlayerPedData PlayerPedData;
	CVehicle* pRemoteVehicle;
	CVehicle* pSpecCar;
	int32 Score;
	int32 DisplayScore;
	int32 CollectablesPickedUp;
	int32 TotalNumCollectables;
	uint32 nLastBumpPlayerCarTimer;
	uint32 TaxiTimer;
	uint32 vehicle_time_counter;
	bool bTaxiTimerScore;
	bool m_bTryingToExitCar;
	uint8 pad0[2];
	CVehicle* pLastTargetVehicle;
	ePlayerState PlayerState;

	bool bAfterRemoteVehicleExplosion;
	bool bCreateRemoteVehicleExplosion;
	bool bFadeAfterRemoteVehicleExplosion;
	uint32 TimeOfRemoteVehicleExplosion;
	uint32 LastTimeEnergyLost;
	uint32 LastTimeArmourLost;
	uint32 LastTimeBigGunFired;
	uint32 TimesUpsideDownInARow;
	uint32 TimesStuckInARow;
	uint32 nCarTwoWheelCounter;
	float fCarTwoWheelDist;
	uint32 nCarLess3WheelCounter;
	uint32 nBikeRearWheelCounter;
	float fBikeRearWheelDist;
	uint32 nBikeFrontWheelCounter;
	float fBikeFrontWheelDist;
	uint32 nTempBufferCounter;
	uint32 nBestCarTwoWheelsTimeMs;
	float fBestCarTwoWheelsDistM;
	uint32 nBestBikeWheelieTimeMs;
	float fBestBikeWheelieDistM;
	uint32 nBestBikeStoppieTimeMs;
	float fBestBikeStoppieDistM;
	uint32 CarDensityForCurrentZone;
	float RoadDensityAroundPlayer;
	uint32 TimeOfLastCarExplosionCaused;
	int32 ExplosionMultiplier;
	int32 HavocCaused;
	int32 TimeLastEaten;
	float CurrentChaseValue;
	bool DoesNotGetTired;
	bool FastReload;
	bool FireProof;
	uint8 MaxHealth;
	uint8 MaxArmour;
	bool bGetOutOfJailFree;
	bool bFreeHealthCare;
	bool bCanDoDriveBy;
	uint8 m_nBustedAudioStatus;
	int16 m_nLastBustMessageNumber;

	uint8 CrossHair[0xC];

	bool bGetOject;
	char m_skinName[32];
	RwTexture* m_pSkinTexture;
	bool m_bParachuteReferenced;
	uint32 m_nRequireParachuteTimer;
};
VALIDATE_SIZE(CPlayerInfo, (VER_x32 ? 0x194 : 0x1D8));
}
