//
// Created by ALYN on 2024/8/14.
//
#pragma once

#include <span>

#include "Entity/Physical.h"
#include "Enums/ModelID.h"

namespace sa {
enum eVehicleType : int32_t {
	VEHICLE_TYPE_IGNORE = -1,
	VEHICLE_TYPE_AUTOMOBILE = 0,
	VEHICLE_TYPE_MTRUCK = 1,  // MONSTER TRUCK
	VEHICLE_TYPE_QUAD = 2,
	VEHICLE_TYPE_HELI = 3,
	VEHICLE_TYPE_PLANE = 4,
	VEHICLE_TYPE_BOAT = 5,
	VEHICLE_TYPE_TRAIN = 6,
	VEHICLE_TYPE_FHELI = 7,
	VEHICLE_TYPE_FPLANE = 8,
	VEHICLE_TYPE_BIKE = 9,
	VEHICLE_TYPE_BMX = 10,
	VEHICLE_TYPE_TRAILER = 11
};

enum eVehicleCreatedBy : uint8 {
	RANDOM_VEHICLE = 1,
	MISSION_VEHICLE = 2,
	PARKED_VEHICLE = 3,
	PERMANENT_VEHICLE = 4
};

enum eRotationAxis : int32 {
	AXIS_X = 0,
	AXIS_Y = 1,
	AXIS_Z = 2
};
enum eCarLock : uint32 {
	CARLOCK_NOT_USED,
	CARLOCK_UNLOCKED,
	CARLOCK_LOCKED,
	CARLOCK_LOCKOUT_PLAYER_ONLY,
	CARLOCK_LOCKED_PLAYER_INSIDE,
	CARLOCK_COP_CAR,
	CARLOCK_FORCE_SHUT_DOORS,
	CARLOCK_SKIP_SHUT_DOORS
};

enum eVehicleLightsSize : uint8_t {
	LIGHTS_LONG,
	LIGHTS_SMALL,
	LIGHTS_BIG,
	LIGHTS_TALL
};

enum eVehicleHandlingModelFlags : uint32_t {
	VEHICLE_HANDLING_MODEL_NONE = 0x0,
	VEHICLE_HANDLING_MODEL_IS_VAN = 0x1,
	VEHICLE_HANDLING_MODEL_IS_BUS = 0x2,
	VEHICLE_HANDLING_MODEL_IS_LOW = 0x4,
	VEHICLE_HANDLING_MODEL_IS_BIG = 0x8,
	VEHICLE_HANDLING_MODEL_REVERSE_BONNET = 0x10,
	VEHICLE_HANDLING_MODEL_HANGING_BOOT = 0x20,
	VEHICLE_HANDLING_MODEL_TAILGATE_BOOT = 0x40,
	VEHICLE_HANDLING_MODEL_NOSWING_BOOT = 0x80,
	VEHICLE_HANDLING_MODEL_NO_DOORS = 0x100,
	VEHICLE_HANDLING_MODEL_TANDEM_SEATS = 0x200,
	VEHICLE_HANDLING_MODEL_SIT_IN_BOAT = 0x400,
	VEHICLE_HANDLING_MODEL_CONVERTIBLE = 0x800,
	VEHICLE_HANDLING_MODEL_NO_EXHAUST = 0x1000,
	VEHICLE_HANDLING_MODEL_DOUBLE_EXHAUST = 0x2000,
	VEHICLE_HANDLING_MODEL_NO1FPS_LOOK_BEHIND = 0x4000,
	VEHICLE_HANDLING_MODEL_FORCE_DOOR_CHECK = 0x8000,
	VEHICLE_HANDLING_MODEL_AXLE_F_NOTILT = 0x10000,
	VEHICLE_HANDLING_MODEL_AXLE_F_SOLID = 0x20000,
	VEHICLE_HANDLING_MODEL_AXLE_F_MCPHERSON = 0x40000,
	VEHICLE_HANDLING_MODEL_AXLE_F_REVERSE = 0x80000,
	VEHICLE_HANDLING_MODEL_AXLE_R_NOTILT = 0x100000,
	VEHICLE_HANDLING_MODEL_AXLE_R_SOLID = 0x200000,
	VEHICLE_HANDLING_MODEL_AXLE_R_MCPHERSON = 0x400000,
	VEHICLE_HANDLING_MODEL_AXLE_R_REVERSE = 0x800000,
	VEHICLE_HANDLING_MODEL_IS_BIKE = 0x1000000,
	VEHICLE_HANDLING_MODEL_IS_HELI = 0x2000000,
	VEHICLE_HANDLING_MODEL_IS_PLANE = 0x4000000,
	VEHICLE_HANDLING_MODEL_IS_BOAT = 0x8000000,
	VEHICLE_HANDLING_MODEL_BOUNCE_PANELS = 0x10000000,
	VEHICLE_HANDLING_MODEL_DOUBLE_RWHEELS = 0x20000000,
	VEHICLE_HANDLING_MODEL_FORCE_GROUND_CLEARANCE = 0x40000000,
	VEHICLE_HANDLING_MODEL_IS_HATCHBACK = 0x80000000
};

enum eVehicleHandlingFlags : uint32_t {
	VEHICLE_HANDLING_1G_BOOST = 0x1,
	VEHICLE_HANDLING_2G_BOOST = 0x2,
	VEHICLE_HANDLING_NPC_ANTI_ROLL = 0x4,
	VEHICLE_HANDLING_NPC_NEUTRAL_HANDL = 0x8,
	VEHICLE_HANDLING_NO_HANDBRAKE = 0x10,
	VEHICLE_HANDLING_STEER_REARWHEELS = 0x20,
	VEHICLE_HANDLING_HB_REARWHEEL_STEER = 0x40,
	VEHICLE_HANDLING_ALT_STEER_OPT = 0x80,

	VEHICLE_HANDLING_WHEEL_F_NARROW2 = 0x100,
	VEHICLE_HANDLING_WHEEL_F_NARROW = 0x200,
	VEHICLE_HANDLING_WHEEL_F_WIDE = 0x400,
	VEHICLE_HANDLING_WHEEL_F_WIDE2 = 0x800,
	VEHICLE_HANDLING_FRONT_WHEELS = VEHICLE_HANDLING_WHEEL_F_NARROW2 | VEHICLE_HANDLING_WHEEL_F_NARROW | VEHICLE_HANDLING_WHEEL_F_WIDE | VEHICLE_HANDLING_WHEEL_F_WIDE2,

	VEHICLE_HANDLING_WHEEL_R_NARROW2 = 0x1000,
	VEHICLE_HANDLING_WHEEL_R_NARROW = 0x2000,
	VEHICLE_HANDLING_WHEEL_R_WIDE = 0x4000,
	VEHICLE_HANDLING_WHEEL_R_WIDE2 = 0x8000,
	VEHICLE_HANDLING_REAR_WHEELS = VEHICLE_HANDLING_WHEEL_R_NARROW2 | VEHICLE_HANDLING_WHEEL_R_NARROW | VEHICLE_HANDLING_WHEEL_R_WIDE | VEHICLE_HANDLING_WHEEL_R_WIDE2,

	VEHICLE_HANDLING_HYDRAULIC_GEOM = 0x10000,
	VEHICLE_HANDLING_HYDRAULIC_INST = 0x20000,
	VEHICLE_HANDLING_HYDRAULIC_NONE = 0x40000,
	VEHICLE_HANDLING_NOS_INST = 0x80000,
	VEHICLE_HANDLING_OFFROAD_ABILITY = 0x100000,
	VEHICLE_HANDLING_OFFROAD_ABILITY2 = 0x200000,
	VEHICLE_HANDLING_HALOGEN_LIGHTS = 0x400000,
	VEHICLE_HANDLING_PROC_REARWHEEL_1ST = 0x800000,
	VEHICLE_HANDLING_USE_MAXSP_LIMIT = 0x1000000,
	VEHICLE_HANDLING_LOW_RIDER = 0x2000000,
	VEHICLE_HANDLING_STREET_RACER = 0x4000000,
	VEHICLE_HANDLING_SWINGING_CHASSIS = 0x10000000
};

struct tTransmissionGear {
	float m_maxVelocity;
	float m_changeUpVelocity;   // max velocity needed to change the current gear to higher
	float m_changeDownVelocity; // min velocity needed to change the current gear to lower
};
VALIDATE_SIZE(tTransmissionGear, 0xC);

class cTransmission {
public:
	tTransmissionGear m_aGears[6];
	uint8_t m_nDriveType;     // F/R/4
	uint8_t m_nEngineType;    // P/D/E
	uint8_t m_nNumberOfGears; // 1 to 6
	uint8_t undefined;
	uint32_t m_handlingFlags;
	float m_fEngineAcceleration; // 0.1 to 10.0
	float m_fEngineInertia;      // 0.0 to 50.0
	float m_fMaxGearVelocity;    // 5.0 to 150.0
	float m_fMaxVelocity;
	float m_maxReverseGearVelocity;
	float m_fCurrentSpeed;

};
VALIDATE_SIZE(cTransmission, 0x68);

struct tHandlingData {
	int32_t m_nVehicleId;
	float m_fMass; // 1.0 to 50000.0
	float m_fMassRecpr; // = 1.f / m_fMass
	float m_fTurnMass;
	float m_fDragMult;
	CVector m_vecCentreOfMass;   // x, y, z - 1.0 to 50000.0
	uint8_t m_nPercentSubmerged; // 10 to 120 (> 100% vehicle sinks)
	uint8_t pad0[3];
	float m_fBuoyancyConstant;
	float m_fTractionMultiplier; // 0.5 to 2.0
	cTransmission m_transmissionData;
	float m_fBrakeDeceleration; // 0.1 to 10.0
	float m_fBrakeBias;         // 0.0 > x > 1.0
	bool m_bABS;               // 0/1
	char field_9D;
	char field_9E;
	char field_9F;
	float m_fSteeringLock; // 10.0 to 40.0
	float m_fTractionLoss;
	float m_fTractionBias;
	float m_fSuspensionForceLevel;     // not [L/M/H]
	float m_fSuspensionDampingLevel;   // not [L/M/H]
	float m_fSuspensionHighSpdComDamp; // often zero - 200.0 or more for bouncy vehicles
	float m_fSuspensionUpperLimit;
	float m_fSuspensionLowerLimit;
	float m_fSuspensionBiasBetweenFrontAndRear;
	float m_fSuspensionAntiDiveMultiplier;
	float m_fCollisionDamageMultiplier; // 0.2 to 5.0
	union {
		eVehicleHandlingModelFlags m_nModelFlags;
		struct {
			uint32_t m_bIsVan : 1;
			uint32_t m_bIsBus : 1;
			uint32_t m_bIsLow : 1;
			uint32_t m_bIsBig : 1;
			uint32_t m_bReverseBonnet : 1;
			uint32_t m_bHangingBoot : 1;
			uint32_t m_bTailgateBoot : 1;
			uint32_t m_bNoswingBoot : 1;

			uint32_t m_bNoDoors : 1;
			uint32_t m_bTandemSeats : 1;
			uint32_t m_bSitInBoat : 1;
			uint32_t m_bConvertible : 1;
			uint32_t m_bNoExhaust : 1;
			uint32_t m_bDoubleExhaust : 1;
			uint32_t m_bNo1fpsLookBehind : 1;
			uint32_t m_bForceDoorCheck : 1;

			uint32_t m_bAxleFNotlit : 1;
			uint32_t m_bAxleFSolid : 1;
			uint32_t m_bAxleFMcpherson : 1;
			uint32_t m_bAxleFReverse : 1;
			uint32_t m_bAxleRNotlit : 1;
			uint32_t m_bAxleRSolid : 1;
			uint32_t m_bAxleRMcpherson : 1;
			uint32_t m_bAxleRReverse : 1;

			uint32_t m_bIsBike : 1;
			uint32_t m_bIsHeli : 1;
			uint32_t m_bIsPlane : 1;
			uint32_t m_bIsBoat : 1;
			uint32_t m_bBouncePanels : 1;
			uint32_t m_bDoubleRwheels : 1;
			uint32_t m_bForceGroundClearance : 1;
			uint32_t m_bIsHatchback : 1;
		};
	};
	union {
		eVehicleHandlingFlags m_nHandlingFlags;
		struct {
			uint32_t m_b1gBoost : 1;
			uint32_t m_b2gBoost : 1;
			uint32_t m_bNpcAntiRoll : 1;
			uint32_t m_bNpcNeutralHandl : 1;
			uint32_t m_bNoHandbrake : 1;
			uint32_t m_bSteerRearwheels : 1;
			uint32_t m_bHbRearwheelSteer : 1;
			uint32_t m_bAltSteerOpt : 1;

			uint32_t m_bWheelFNarrow2 : 1;
			uint32_t m_bWheelFNarrow : 1;
			uint32_t m_bWheelFWide : 1;
			uint32_t m_bWheelFWide2 : 1;
			uint32_t m_bWheelRNarrow2 : 1;
			uint32_t m_bWheelRNarrow : 1;
			uint32_t m_bWheelRWide : 1;
			uint32_t m_bWheelRWide2 : 1;

			uint32_t m_bHydraulicGeom : 1;
			uint32_t m_bHydraulicInst : 1;
			uint32_t m_bHydraulicNone : 1;
			uint32_t m_bNosInst : 1;
			uint32_t m_bOffroadAbility : 1;
			uint32_t m_bOffroadAbility2 : 1;
			uint32_t m_bHalogenLights : 1;
			uint32_t m_bProcRearwheelFirst : 1;
			uint32_t m_bUseMaxspLimit : 1;

			uint32_t m_bLowRider : 1;
			uint32_t m_bStreetRacer : 1;
			uint32_t m_bSwingingChassis : 1;
		};
	};
	float m_fSeatOffsetDistance; // // ped seat position offset towards centre of car
	uint32_t m_nMonetaryValue;      // 1 to 100000
	eVehicleLightsSize m_nFrontLights;
	eVehicleLightsSize m_nRearLights;
	uint8_t m_nAnimGroup;
	uint8_t pad2;

	cTransmission& GetTransmission() { return m_transmissionData; }
};
VALIDATE_SIZE(tHandlingData, 0xE0);

class CPed;

struct CVehicle : CPhysical {
#if VER_x32
	uint8_t m_VehicleAudioEntity[588];
#else
	uint8_t m_VehicleAudioEntity[0x310];
#endif
	tHandlingData* m_pHandlingData;
	uintptr_t* pFlyingHandling;
	union {
		eVehicleHandlingFlags m_nHandlingFlagsIntValue;
		struct {
			uint32_t b1gBoost : 1;
			uint32_t b2gBoost : 1;
			uint32_t bNpcAntiRoll : 1;
			uint32_t bNpcNeutralHandl : 1;
			uint32_t bNoHandbrake : 1;
			uint32_t bSteerRearwheels : 1;
			uint32_t bHbRearwheelSteer : 1;
			uint32_t bAltSteerOpt : 1;
			uint32_t bWheelFNarrow2 : 1;
			uint32_t bWheelFNarrow : 1;
			uint32_t bWheelFWide : 1;
			uint32_t bWheelFWide2 : 1;
			uint32_t bWheelRNarrow2 : 1;
			uint32_t bWheelRNarrow : 1;
			uint32_t bWheelRWide : 1;
			uint32_t bWheelRWide2 : 1;
			uint32_t bHydraulicGeom : 1;
			uint32_t bHydraulicInst : 1;
			uint32_t bHydraulicNone : 1;
			uint32_t bNosInst : 1;
			uint32_t bOffroadAbility : 1;
			uint32_t bOffroadAbility2 : 1;
			uint32_t bHalogenLights : 1;
			uint32_t bProcRearwheelFirst : 1;
			uint32_t bUseMaxspLimit : 1;
			uint32_t bLowRider : 1;
			uint32_t bStreetRacer : 1;
			uint32_t bSwingingChassis : 1;
		} handlingFlags;
	};
#if VER_x32
	uint8_t AutoPilot[152];
#else
	uint32_t padflagsx64;
	uint8_t AutoPilot[0xA8];
#endif
	union {
		struct {
			uint32_t m_nVehicleUpperFlags;
			uint32_t m_nVehicleLowerFlags;
		};
		struct {
			uint32_t bIsLawEnforcer : 1; // Is this guy chasing the player at the moment
			uint32_t bIsAmbulanceOnDuty : 1; // Ambulance trying to get to an accident
			uint32_t bIsFireTruckOnDuty : 1; // Firetruck trying to get to a fire
			uint32_t bIsLocked : 1; // Is this guy locked by the script (cannot be removed)
			uint32_t bEngineOn : 1; // For sound purposes. Parked cars have their engines switched off (so do destroyed cars)
			uint32_t bIsHandbrakeOn : 1; // How's the handbrake doing ?
			uint32_t bLightsOn : 1; // Are the lights switched on ?
			uint32_t bFreebies : 1; // Any freebies left in this vehicle ?

			uint32_t bIsVan : 1; // Is this vehicle a van (doors at back of vehicle)
			uint32_t bIsBus : 1; // Is this vehicle a bus
			uint32_t bIsBig : 1; // Is this vehicle a bus
			uint32_t bLowVehicle : 1; // Need this for sporty type cars to use low getting-in/out anims
			uint32_t bComedyControls : 1; // Will make the car hard to control (hopefully in a funny way)
			uint32_t bWarnedPeds : 1; // Has scan and warn peds of danger been processed?
			uint32_t bCraneMessageDone : 1; // A crane message has been printed for this car allready
			uint32_t bTakeLessDamage : 1; // This vehicle is stronger (takes about 1/4 of damage)

			uint32_t bIsDamaged : 1; // This vehicle has been damaged and is displaying all its components
			uint32_t bHasBeenOwnedByPlayer : 1;// To work out whether stealing it is a crime
			uint32_t bFadeOut : 1; // Fade vehicle out
			uint32_t bIsBeingCarJacked : 1; // Fade vehicle out
			uint32_t bCreateRoadBlockPeds : 1;// If this vehicle gets close enough we will create peds (coppers or gang members) round it
			uint32_t bCanBeDamaged : 1; // Set to FALSE during cut scenes to avoid explosions
			uint32_t bOccupantsHaveBeenGenerated : 1; // Is true if the occupants have already been generated. (Shouldn't happen again)
			uint32_t bGunSwitchedOff : 1; // Level designers can use this to switch off guns on boats

			uint32_t bVehicleColProcessed : 1;// Has ProcessEntityCollision been processed for this car?
			uint32_t bIsCarParkVehicle : 1; // Car has been created using the special CAR_PARK script command
			uint32_t bHasAlreadyBeenRecorded : 1; // Used for replays
			uint32_t bPartOfConvoy : 1;
			uint32_t bHeliMinimumTilt : 1; // This heli should have almost no tilt really
			uint32_t bAudioChangingGear : 1; // sounds like vehicle is changing gear
			uint32_t bIsDrowning : 1; // is vehicle occupants taking damage in water (i.e. vehicle is dead in water)
			uint32_t bTyresDontBurst : 1; // If this is set the tyres are invincible

			uint32_t bCreatedAsPoliceVehicle : 1;// True if this guy was created as a police vehicle (enforcer, policecar, miamivice car etc)
			uint32_t bRestingOnPhysical : 1; // Dont go static cause car is sitting on a physical object that might get removed
			uint32_t bParking : 1;
			uint32_t bCanPark : 1;
			uint32_t bFireGun : 1; // Does the ai of this vehicle want to fire it's gun?
			uint32_t bDriverLastFrame : 1; // Was there a driver present last frame ?
			uint32_t bNeverUseSmallerRemovalRange : 1;// Some vehicles (like planes) we don't want to remove just behind the camera.
			uint32_t bIsRCVehicle : 1; // Is this a remote controlled (small) vehicle. True whether the player or AI controls it.

			uint32_t bAlwaysSkidMarks : 1; // This vehicle leaves skidmarks regardless of the wheels' states.
			uint32_t bEngineBroken : 1; // Engine doesn't work. Player can get in but the vehicle won't drive
			uint32_t bVehicleCanBeTargetted : 1;// The ped driving this vehicle can be targetted, (for Torenos plane mission)
			uint32_t bPartOfAttackWave : 1; // This car is used in an attack during a gang war
			uint32_t bWinchCanPickMeUp : 1; // This car cannot be picked up by any ropes.
			uint32_t bImpounded : 1; // Has this vehicle been in a police impounding garage
			uint32_t bVehicleCanBeTargettedByHS : 1;// Heat seeking missiles will not target this vehicle.
			uint32_t bSirenOrAlarm : 1; // Set to TRUE if siren or alarm active, else FALSE

			uint32_t bHasGangLeaningOn : 1;
			uint32_t bGangMembersForRoadBlock : 1;// Will generate gang members if NumPedsForRoadBlock > 0
			uint32_t bDoesProvideCover : 1; // If this is false this particular vehicle can not be used to take cover behind.
			uint32_t bMadDriver : 1; // This vehicle is driving like a lunatic
			uint32_t bUpgradedStereo : 1; // This vehicle has an upgraded stereo
			uint32_t bConsideredByPlayer : 1; // This vehicle is considered by the player to enter
			uint32_t bPetrolTankIsWeakPoint : 1;// If false shootong the petrol tank will NOT Blow up the car
			uint32_t bDisableParticles : 1; // Disable particles from this car. Used in garage.

			uint32_t bHasBeenResprayed : 1; // Has been resprayed in a respray garage. Reset after it has been checked.
			uint32_t bUseCarCheats : 1; // If this is true will set the car cheat stuff up in ProcessControl()
			uint32_t bDontSetColourWhenRemapping : 1;// If the texture gets remapped we don't want to change the colour with it.
			uint32_t bUsedForReplay : 1; // This car is controlled by replay and should be removed when replay is done.
		} m_nVehicleFlags;
	};

	unsigned int m_nCreationTime;
	uint8_t m_nPrimaryColor;            // 1076-1077	;byteColor1
	uint8_t m_nSecondaryColor;            // 1077-1078	;byteColor2
	uint8_t m_colour3;
	uint8_t m_colour4;
	uint8_t m_comp1;
	uint8_t m_comp2;
	short m_anUpgrades[15];
	float m_wheelScale;
	unsigned short m_nAlarmState;
	short m_nForcedRandomRouteSeed; // if this is non-zero the random wander gets deterministic
	CPed* pDriver;            // 1120-1124	;driver
	CPed* m_apPassengers[8];    // 1124-1152	;m_apPassengers
	unsigned char m_nNumPassengers;
	unsigned char m_nNumGettingIn;
	unsigned char m_nGettingInFlags;
	unsigned char m_nGettingOutFlags;
	uint8_t m_nMaxPassengers;
	uint8_t m_nWindowsOpenFlags; // initialised, but not used?
	uint8_t m_nNitroBoosts;
	int8_t m_vehicleSpecialColIndex;
	uintptr* m_pEntityWeAreOn;
	uintptr* m_pFire;
	float m_fSteerAngle;
	float m_f2ndSteerAngle; // used for steering 2nd set of wheels or elevators etc..
	float m_fGasPedal;
	float m_fBreakPedal;
	eVehicleCreatedBy m_nCreatedBy; // see eVehicleCreatedBy
	unsigned char abc1;
	unsigned char abc2;
	unsigned char abc3;
	uint32_t cachedTotalSteer;
	short m_nExtendedRemovalRange;
	unsigned char m_nBombOnBoard : 3; // 0 = None
	// 1 = Timed
	// 2 = On ignition
	// 3 = remotely set ?
	// 4 = Timed Bomb has been activated
	// 5 = On ignition has been activated
	unsigned char m_nOverrideLights : 2; // uses enum NO_CAR_LIGHT_OVERRIDE, FORCE_CAR_LIGHTS_OFF, FORCE_CAR_LIGHTS_ON
	unsigned char m_nWinchType : 2; // Does this vehicle use a winch?
	unsigned char m_nGunsCycleIndex : 2; // Cycle through alternate gun hardpoints on planes/helis
	unsigned char m_nOrdnanceCycleIndex : 2; // Cycle through alternate ordnance hardpoints on planes/helis
	uint8_t nUsedForCover;
	uint8_t AmmoInClip;
	uint8_t PacMansCollected;
	uint8_t PedsPositionForRoadBlock;
	uint8_t NumPedsForRoadBlock[4];
	float m_fDirtLevel; // Dirt level of vehicle body texture: 0.0f=fully clean, 15.0f=maximum dirt visible
	unsigned char m_nCurrentGear;
	uint8_t padd_1[3];
	float m_fGearChangeCount; // used as parameter for cTransmission::CalculateDriveAcceleration, but doesn't change
	float m_fWheelSpinForAudio;
	float fHealth;                // 1224-1228	;m_fHealth
	CVehicle* m_pTowingVehicle;
	CVehicle* m_pTrailer;
	uint8_t padd_2[3];
	bool m_bFireAutoFlare;
	CEntity* pBombOwner;
	uint32_t DontUseSmallerRemovalRange;
	uint32_t TimeOfLastShotFired;
	uint32_t m_nTimeOfDeath;
	uint16_t GetOutOfCarTimer;
	uint16_t DelayedExplosion;
	CEntity* pDelayedExplosionInflictor;
	float LastFrontHeight;
	float LastRearHeight;
	uint8_t NumOilSpillsToDo;
	uint8_t skip14[3]; // wtf?
	float OilSpillLastX;
	float OilSpillLastY;
	eCarLock m_nDoorLock;
	uint32_t m_LastTimePrimaryFired;
	uint32_t m_LastTimeSecondaryFired;
	uint32_t m_LastTimeGunFired;
	int8_t LastDamagedWeaponType;
	uint8_t skip15[3]; // wtf?
	CEntity* pLastDamageEntity;
	uint8_t m_nRadioStation;
	uint8_t m_nRainHitCount;
	uint8_t m_nSoundIndex;
	uint8_t m_SelectedWeapon;
	uint32_t m_cHorn;
	uint8_t m_nHornPattern;
	uint8_t m_NoHornCount;
	uint8_t ComedyControlsState;
	uint8_t m_hassleStatus;

	uint8_t StoredCollPolys[0x58]; // CStoredCollPoly StoredCollPolys[2];
	uint8_t m_storedCollisionLighting[4];
	uintptr_t* m_fxSysEngOverheat;
	uintptr_t* m_fxSysEngFire;
	uintptr_t* m_fxSysHeliDust;
	union {
		uint8_t m_nRenderLightsFlags;
		struct {
			uint8_t m_bRightFront : 1;
			uint8_t m_bLeftFront : 1;
			uint8_t m_bRightRear : 1;
			uint8_t m_bLeftRear : 1;
		} m_renderLights;
	};
	uint8_t skip16[3]; // wtf?
	RwTexture* m_pCustomCarPlate;
	float m_fRawSteerAngle; // AKA m_fSteeringLeftRight
	eVehicleType m_nVehicleType;    // Theory by forkerer:
	eVehicleType m_nVehicleSubType; // Hack to have stuff be 2 classes at once, like vortex which can act like a car and a boat
	const char* m_remapTxdName;
	const char* m_newRemapTxdName;
	RwTexture* m_pRemapTexture;

public:
public: // NOTSA functions
	// m_nVehicleType start
	[[nodiscard]] bool IsVehicleTypeValid() const { return m_nVehicleType != VEHICLE_TYPE_IGNORE; }
	[[nodiscard]] bool IsAutomobile() const { return m_nVehicleType == VEHICLE_TYPE_AUTOMOBILE; }
	[[nodiscard]] bool IsMonsterTruck() const { return m_nVehicleType == VEHICLE_TYPE_MTRUCK; }
	[[nodiscard]] bool IsQuad() const { return m_nVehicleType == VEHICLE_TYPE_QUAD; }
	[[nodiscard]] bool IsHeli() const { return m_nVehicleType == VEHICLE_TYPE_HELI; }
	[[nodiscard]] bool IsPlane() const { return m_nVehicleType == VEHICLE_TYPE_PLANE; }
	[[nodiscard]] bool IsBoat() const { return m_nVehicleType == VEHICLE_TYPE_BOAT; }
	[[nodiscard]] bool IsTrain() const { return m_nVehicleType == VEHICLE_TYPE_TRAIN; }
	[[nodiscard]] bool IsFakeAircraft() const { return m_nVehicleType == VEHICLE_TYPE_FHELI || m_nVehicleType == VEHICLE_TYPE_FPLANE; }
	[[nodiscard]] bool IsBike() const { return m_nVehicleType == VEHICLE_TYPE_BIKE; }
	[[nodiscard]] bool IsBMX() const { return m_nVehicleType == VEHICLE_TYPE_BMX; }
	[[nodiscard]] bool IsTrailer() const { return m_nVehicleType == VEHICLE_TYPE_TRAILER || m_nVehicleSubType == VEHICLE_TYPE_TRAILER; }
	[[nodiscard]] bool IsRCVehicleModelID()
	{
		switch (m_nModelIndex) {
			case 441:
			case 464:
			case 465:
			case 594:
			case 501:
			case 564:return true;

			default:break;
		}
		return false;
	}
	// m_nVehicleType end

	// m_nVehicleSubType start
	[[nodiscard]] bool IsSubVehicleTypeValid() const { return m_nVehicleSubType != VEHICLE_TYPE_IGNORE; }
	[[nodiscard]] bool IsSubAutomobile() const { return m_nVehicleSubType == VEHICLE_TYPE_AUTOMOBILE; }
	[[nodiscard]] bool IsSubMonsterTruck() const { return m_nVehicleSubType == VEHICLE_TYPE_MTRUCK; }
	[[nodiscard]] bool IsSubQuad() const { return m_nVehicleSubType == VEHICLE_TYPE_QUAD; }
	[[nodiscard]] bool IsSubHeli() const { return m_nVehicleSubType == VEHICLE_TYPE_HELI; }
	[[nodiscard]] bool IsSubPlane() const { return m_nVehicleSubType == VEHICLE_TYPE_PLANE; }
	[[nodiscard]] bool IsSubBoat() const { return m_nVehicleSubType == VEHICLE_TYPE_BOAT; }
	[[nodiscard]] bool IsSubTrain() const { return m_nVehicleSubType == VEHICLE_TYPE_TRAIN; }
	[[nodiscard]] bool IsSubFakeAircraft() const { return m_nVehicleSubType == VEHICLE_TYPE_FHELI || m_nVehicleSubType == VEHICLE_TYPE_FPLANE; }
	[[nodiscard]] bool IsSubBike() const { return m_nVehicleSubType == VEHICLE_TYPE_BIKE; }
	[[nodiscard]] bool IsSubBMX() const { return m_nVehicleSubType == VEHICLE_TYPE_BMX; }
	[[nodiscard]] bool IsSubTrailer() const { return m_nVehicleSubType == VEHICLE_TYPE_TRAILER; }

	[[nodiscard]] bool IsSubRoadVehicle() const { return !IsSubHeli() && !IsSubPlane() && !IsSubTrain(); }
	[[nodiscard]] bool IsSubFlyingVehicle() const { return IsSubHeli() && IsSubPlane(); }
	// m_nVehicleSubType end

	[[nodiscard]] bool IsTransportVehicle() const { return m_nModelIndex == MODEL_TAXI || m_nModelIndex == MODEL_CABBIE; }
	[[nodiscard]] bool IsAmphibiousHeli() const { return m_nModelIndex == MODEL_SEASPAR || m_nModelIndex == MODEL_LEVIATHN; }
	[[nodiscard]] bool IsConstructionVehicle() const { return m_nModelIndex == MODEL_DUMPER || m_nModelIndex == MODEL_DOZER || m_nModelIndex == MODEL_FORKLIFT; }

	[[nodiscard]] bool IsRealBike() const { return m_pHandlingData->m_bIsBike; }
	[[nodiscard]] bool IsRealHeli() const { return m_pHandlingData->m_bIsHeli; }
	[[nodiscard]] bool IsRealPlane() const { return m_pHandlingData->m_bIsPlane; }
	[[nodiscard]] bool IsRealBoat() const { return m_pHandlingData->m_bIsBoat; }

	[[nodiscard]] eVehicleCreatedBy GetCreatedBy() const { return m_nCreatedBy; }
	[[nodiscard]] bool IsCreatedBy(eVehicleCreatedBy v) const { return v == m_nCreatedBy; }
	[[nodiscard]] bool IsMissionVehicle() const { return m_nCreatedBy == MISSION_VEHICLE; }
	auto GetMaxPassengerSeats() { return std::span{m_apPassengers, m_nMaxPassengers}; }

	bool GetEngineStatus() { return m_nVehicleFlags.bEngineOn; }
	void SetEngineStatus(bool status) { m_nVehicleFlags.bEngineOn = status; }

	bool GetLightsStatus() { return m_nVehicleFlags.bLightsOn; }
	void SetLightsStatus(bool status) { m_nVehicleFlags.bLightsOn = status; }
};
VALIDATE_SIZE(CVehicle, (VER_x32 ? 0x5B4 : 0x758));
}
