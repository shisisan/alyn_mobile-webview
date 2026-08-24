//
// Created by ALYN on 2024/8/22.
//
#pragma once

#include "Enums/Animation.h"

namespace sa {
struct tBikeHandlingData {
	eVehicleType nVehicleID;
	float fLeanFwdCOMMult;
	float fLeanFwdForceMult;
	float fLeanBakCOMMult;
	float fLeanBakForceMult;
	float fMaxBankAngle;
	float fFullAnimAngle;
	float fDesLeanReturnFrac;
	float fSpeedSteerFrac;
	float fSlippySpeedSteerMult;
	float fNoRiderCOMz;
	float fWheelieBalancePoint;
	float fStoppieBalancePoint;
	float fWheelieSteerMult;
	float fRearBalanceMult;
	float fFrontBalanceMult;
};
VALIDATE_SIZE(tBikeHandlingData, 0x40);

struct CRideAnimData {
	AssocGroupId m_animGroup;
	float m_fBarSteerAngle;
	float m_fLeanAngle;
	float m_fDesiredLeanAngle;
	float m_fLeanFwd;
	float m_fAnimLeanLeft;
	float m_fAnimLeanFwd;
};
VALIDATE_SIZE(CRideAnimData, 0x1C);

class CBike : CVehicle {
public:
	struct CBikeFlags {
		uint8_t bShouldNotChangeColour : 1;
		uint8_t bPanelsAreThougher : 1;
		uint8_t bWaterTight : 1;
		uint8_t bGettingPickedUp : 1;
		uint8_t bOnSideStand : 1;
		uint8_t bPlayerBoost : 1;
		uint8_t bEngineOnFire : 1;
		uint8_t bWheelieForCamera : 1;
	};

	RwFrame* m_aBikeNodes[10];
	bool m_bLeanMatrix;
#if VER_x32
	uint8 padd[3];
#else
	uint8 padd[7];
#endif
	CMatrix m_LeanMatrix;
	CBikeFlags m_nBikeFlags;
	uint8 padd2[3];
	CVector m_vecAveGroundNormal;
	CVector m_vecGroundRight;
	CVector m_vecOldSpeedForPlayback;
	tBikeHandlingData* pBikeHandling;
	CRideAnimData RideAnimData;
	uint8 m_nWheelStatus[2];
	uint8 padd3[2];
	CColPoint m_aWheelColPoints[4];
	float m_aWheelRatios[4];
	float m_aRatioHistory[4];
	float m_aWheelCounts[4];
	float fBrakeCount;
	eSkidmarkType aWheelSkidmarkType[2];
	bool bWheelBloody[2];
	bool bMoreSkidMarks[2];
	float m_aWheelPitchAngles[2];
	float m_aWheelAngularVelocity[2];
	float m_aWheelSuspensionHeights[2];
	float m_aWheelOrigHeights[2];
	float m_fSuspensionLength[4];
	float m_fLineLength[4];
	float m_fHeightAboveRoad;
	float m_fExtraTractionMult;
	float m_fSwingArmLength;
	float m_fForkYOffset;
	float m_fForkZOffset;
	float m_fSteerAngleTan;
	UInt16 nBrakesOn;
	uint8 padd4[2];
	float m_fTyreTemp;
	float m_fBrakingSlide;
	uint8 m_nFixLeftHand;
	uint8 m_nFixRightHand;
	uint8 m_nTestPedCollision;
	uint8 padd5;
	float fPrevSpeed;
	float m_BlowUpTimer;
	CPhysical* m_aGroundPhysicalPtrs[4];
	CVector m_aGroundOffsets[4];
	CEntity* pEntityThatSetUsOnFire;
	UInt8 nNoOfContactWheels;
	UInt8 m_nDriveWheelsOnGround;
	UInt8 m_nDriveWheelsOnGroundLastFrame;
	uint8 padd6;
	float m_fGasPedalAudioRevs;
	tWheelState m_aWheelState[2];
};
VALIDATE_SIZE(CBike, (VER_x32 ? 0x828 : 0xA18));
}
