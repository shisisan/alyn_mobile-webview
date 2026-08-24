//
// Created by ALYN on 2024/8/17.
//
#pragma once

#include "Vehicle.h"

namespace sa {
class CTrain : public CVehicle {
public:
	struct CTrainFlags {
		uint8_t bDoorsReady : 1;
		uint8_t bAtStation : 1;
		uint8_t bPassengersCanBoard : 1;
		uint8_t bEngine : 1;
		uint8_t bCaboose : 1;
		uint8_t bMissionTrain : 1;
		uint8_t bDirection : 1;
		uint8_t bStopForStations : 1;
		uint8_t bDerailed : 1;
		uint8_t bIsForcedToSlowDown : 1;
		uint8_t bHasPassengerCarriages : 1;
	};

	UInt16 CurrentNode;
	uint8 padd[2];
	float LinearSpeed;          // fTrainSpeed
	float PositionOnTrack;
	float OffsetFromLeader;
	float Gas;
	float Brake;
	CTrainFlags m_nTrainFlags;
	uint8 pad2[2];
	UInt32 m_StopAtStationTimer;
	UInt8 TrainType;
	uint8 pad3[3];
	uint32 m_nDoorTimer;
	Int16 DoorState;
	uint8 m_PassengersMode;
	uint8_t m_NumPassengersOnTrain : 4;
	uint8_t m_NumPassengersToBoard : 4;
#if !VER_x32
	uint8 pad4[4];
#endif
	CPed* m_pRandomPedForTrain;
	CTrain* pLinkedToForward;
	CTrain* pLinkedToBackward;
	CDoor Door[6];
	RwFrame* m_aTrainNodes[17];
};
VALIDATE_SIZE(CTrain, (VER_x32 ? 0x6C0 : 0x8B8));
}
