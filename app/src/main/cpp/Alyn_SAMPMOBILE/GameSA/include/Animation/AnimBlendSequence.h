//
// Created by ALYN on 2024/8/18.
//

#pragma once

#include "../SA.h"

namespace sa {
enum ePedBones : int16 {
	BONE_UNKNOWN = -1,

	BONE_NORMAL = 0, // Normal or Root, both are same
	BONE_PELVIS = 1,
	BONE_SPINE = 2,
	BONE_SPINE1 = 3,
	BONE_NECK = 4,
	BONE_HEAD = 5,
	BONE_L_BROW = 6,
	BONE_R_BROW = 7,
	BONE_JAW = 8,

	BONE_R_CLAVICLE = 21,
	BONE_R_UPPER_ARM = 22,
	BONE_R_FORE_ARM = 23,
	BONE_R_HAND = 24,
	BONE_R_FINGER = 25,
	BONE_R_FINGER_01 = 26,

	BONE_L_CLAVICLE = 31,
	BONE_L_UPPER_ARM = 32,
	BONE_L_FORE_ARM = 33,
	BONE_L_HAND = 34,
	BONE_L_FINGER = 35,
	BONE_L_FINGER_01 = 36,

	BONE_L_THIGH = 41,
	BONE_L_CALF = 42,
	BONE_L_FOOT = 43,
	BONE_L_TOE_0 = 44,

	BONE_R_THIGH = 51,
	BONE_R_CALF = 52,
	BONE_R_FOOT = 53,
	BONE_R_TOE_0 = 54,

	BONE_BELLY = 201,

	BONE_L_BREAST = 302,
	BONE_R_BREAST = 301,

	BONE_NUM = 303
};
// The sequence of key frames of one animated node
class CAnimBlendSequence {
public:
	// thanks to jte for some info
	union {
		ePedBones m_boneId; // IF m_hasBoneIdSet == TRUE
		uint32 m_hash;   // IF m_hasBoneIdSet == FALSE
	};
	union {
		struct {
			uint16 m_numFramesSet : 1;
			uint16 m_isRoot : 1;              // Root key frames have translation values (quaternion).
			uint16 m_isCompressed : 1;        // Compressed key frames.
			uint16 m_usingExternalMemory : 1; // When this flag is NOT set, you have to loop through all key frames in m_pFrames and free them separately.
			uint16 m_hasBoneIdSet : 1;
		};
		uint16 m_nFlags;
	};
	uint16 m_nFrameCount;
	void* m_pFrames;
};

VALIDATE_SIZE(CAnimBlendSequence, (VER_x32 ? 0xC : 0x10));
}
