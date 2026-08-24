//
// Created by ALYN on 2024/8/17.
//
#pragma once

#include "Vehicle.h"

namespace sa {
enum tWheelState : int32_t {
	WS_ROLLING = 0x0,
	WS_SPINNING = 0x1,
	WS_SKIDDING = 0x2,
	WS_LOCKED = 0x3,
};

enum eSkidmarkType : int32_t {
	SKIDMARKTYPE_DEFAULT = 0x0,
	SKIDMARKTYPE_MUDDY = 0x1,
	SKIDMARKTYPE_SANDY = 0x2,
	SKIDMARKTYPE_BLOODY = 0x3,
};

class CDamageManager {
public:
	float fWheelDamageEffect;
	uint8 m_Engine;
	uint8 m_Wheel[4]; // byteTireStatus
	uint8 m_Door[6];  // byteDoorStatus
	uint8 __pad1;
	uint32 m_Lights;
	uint32 m_Panels;
};
VALIDATE_SIZE(CDamageManager, 0x18);

struct CDoor {
	float m_fOpenAngle;
	float m_fClosedAngle;
	uint16 m_nDirn;
	uint8 m_nAxis;
	uint8 m_nDoorState;
	float m_fAngle;
	float m_fPrevAngle;
	float m_fAngVel;
};
VALIDATE_SIZE(CDoor, 0x18);

struct CBouncingPanel {
	int16 m_nComponentIndex;
	int16 m_nBounceAxis;
	float m_fBounceApplyMult;
	CVector m_vecBounceAngle;
	CVector m_vecBounceTurnSpeed;
};
VALIDATE_SIZE(CBouncingPanel, 0x20);

class CAutomobile : public CVehicle {
public:
	struct CAutomobileFlags {
		uint8_t bTaxiLight : 1;
		uint8_t bShouldNotChangeColour : 1;
		uint8_t bWaterTight : 1;
		uint8_t bDoesNotGetDamagedUpsideDown : 1;
		uint8_t bCanBeVisiblyDamaged : 1;
		uint8_t bTankExplodesCars : 1;
		uint8_t bIsBoggedDownInSand : 1;
		uint8_t bIsMonsterTruck : 1;
	};

	CDamageManager Damage;
	CDoor Door[6];
	RwFrame* m_aCarNodes[25];
	CBouncingPanel BouncingPanels[3];
	CDoor ChassisDoor;
	CColPoint m_aWheelColPoints[4];
	float m_aWheelRatios[4];
	float m_aRatioHistory[4];
	float m_aWheelCounts[4];
	float fBrakeCount;
	float m_fEngineRevs;
	float m_fEngineForce;
	eSkidmarkType aWheelSkidmarkType[4];
	bool bWheelBloody[4];
	bool bMoreSkidMarks[4];
	float m_aWheelPitchAngles[4];
	float m_aWheelSuspensionHeights[4];
	float m_aWheelAngularVelocity[4];
	float m_aWheelLongitudinalSlip[4];
	CAutomobileFlags m_nAutomobileFlags;
	uint8 __padding;
	uint16 nBrakesOn;
	uint16 m_nSuspensionHydraulics;
	uint16 m_nOldSuspensionHydraulics;
	uint32 m_nBusDoorTimer;
	uint32 m_nBusDoorStart;
	float m_fSuspensionLength[4];
	float m_fLineLength[4];
	float m_fHeightAboveRoad;
	float m_fRearHeightAboveRoad;
	float m_fExtraTractionMult;
	float m_fTyreTemp;
	float ZRot;
	float ZRotSpeed;
	float fPrevSpeed;
	CVector m_vecOldMoveSpeed;
	CVector m_vecOldTurnSpeed;
	float DoorRotation[6];
	float m_BlowUpTimer;
#if VER_x32 == FALSE
	uint8 __padding2[4];
#endif
	CPhysical* m_aGroundPhysicalPtrs[4];
	CVector m_aGroundOffsets[4];
	CEntity* pEntityThatSetUsOnFire;
	float m_fTransformPosition;
	int16 m_nTransformState;
#if VER_x32 == FALSE
	uint8 __padding3[2];
#endif
	float m_fBasePositions[4];
	float LeftDoorOpenForDriveBys;
	float RightDoorOpenForDriveBys;
	float GunOrientation;
	float GunElevation;
	float HeliRequestedOrientation;
	float PropRotate;
	float CumulativeDamage;
	uint8 nNoOfContactWheels;
	uint8 m_nDriveWheelsOnGround;
	uint8 m_nDriveWheelsOnGroundLastFrame;
	uint8 __padding4;
	float m_fGasPedalAudioRevs;
	tWheelState m_aWheelState[4];
#if VER_x32 == FALSE
	uint8 __padding5[4];
#endif
	void* m_pFxSysNitro1; // FxSystem*
	void* m_pFxSysNitro2; // FxSystem*
	uint8 m_HarvesterTimer;
	uint8 m_statusWhenEngineBlown;
	uint8 __padding6[2];
	float m_heliDustRatio;
};
VALIDATE_SIZE(CAutomobile, (VER_x32 ? 0x99C : 0xBC8));
}
