//
// Created by ALYN on 2024/8/17.
//
#pragma once

#include "AutoMobile.h"

namespace sa {
class CPlane : public CAutomobile {
public:
	float m_fYawControl;
	float m_fPitchControl;
	float m_fRollControl;
	float m_fThrottleControl;
	float m_fScriptThrottleControl;
	float m_fPreviousRoll;
	uint32 m_nStallCounter;
	float m_TakeOffDirection;
	float m_LowestFlightHeight;
	float m_DesiredHeight;
	float m_MinHeightAboveTerrain;
	float m_FlightDirection;
	float m_FlightDirectionAvoidingTerrain;
	float m_OldTilt;
	UInt32 m_OnGroundTimer;
	float m_fEngineSpeed;
	float m_fPropellerAngle;
	float m_fLGearAngle;
	uint32 m_nDamageControlWaveCounter;
#if !VER_x32
	uint8 padd[4];
#endif
	void** m_GunflashFxPtrs; // FxSystem_c**
	uint8 m_FiringRateMultiplier;
	uint8 padd1[3];
	uint32 m_FireMissilePressedTime;
	CEntity* m_pLastMissileTarget;
	uint32_t m_LastHSMissileLOSTime : 31;
	uint32_t m_bLastHSMissileLOS : 1;
#if !VER_x32
	uint8 padd2[4];
#endif
	void* m_fxSysNozzle[4]; // FxSystem_c*
	void* m_fxSysFire; // FxSystem_c*
	int32 m_fireTime;
	bool8 m_fxActive;
	uint8 padd3[3];
};
VALIDATE_SIZE(CPlane, (VER_x32 ? 0xA18 : 0xC68));
}
