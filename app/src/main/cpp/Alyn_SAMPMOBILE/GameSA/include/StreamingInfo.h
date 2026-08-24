/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Base.h"

namespace sa {
enum eStreamingFlags {
	STREAMING_DEFAULT = 0x0,
	STREAMING_UNKNOWN_1 = 0x1,
	STREAMING_GAME_REQUIRED = 0x2,
	STREAMING_MISSION_REQUIRED = 0x4,
	STREAMING_KEEP_IN_MEMORY = 0x8,
	STREAMING_PRIORITY_REQUEST = 0x10,
	STREAMING_LOADING_SCENE = 0x20,
	STREAMING_DONTREMOVE_IN_LOADSCENE = STREAMING_LOADING_SCENE | STREAMING_PRIORITY_REQUEST | STREAMING_KEEP_IN_MEMORY | STREAMING_MISSION_REQUIRED | STREAMING_GAME_REQUIRED,
};

enum eStreamingStatus : uint8 {
	// Model isn't loaded
	LOADSTATE_NOT_LOADED = 0,

	// Model is loaded
	LOADSTATE_LOADED = 1,

	// Model in request list, but not yet in loading channel (TODO: Verify this)
	LOADSTATE_REQUESTED = 2,

	// Model is being read
	LOADSTATE_READING = 3,

	// If the model is a `big` one this state is used to indicate
	// that the model's first half has been loaded and is yet to be
	// finished by loading the second half.
	// When it has been loaded the state is set to `LOADED`
	LOADSTATE_FINISHING = 4
};

constexpr auto STREAMING_SECTOR_SIZE = 2048u;

struct CStreamingInfo {
	int16 m_nNextIndex;
	int16 m_nPrevIndex;
	int16 m_nNextModelOnCd;     // ModelId after this file in the containing image file
	union {
		uint8 m_nFlags;         // see eStreamingFlags
		struct {
			uint8 bUnkn0x1 : 1;
			uint8 bGameRequired : 1;
			uint8 bMissionRequired : 1;
			uint8 bKeepInMemory : 1;
			uint8 bPriorityRequest : 1;
			uint8 bLoadingScene : 1;
		};
	};
	uint8 m_nImage;             // Index into CStreaming::ms_files
	uint32 m_nCdPosn;           // Position in directory (in sectors)
	uint32 m_nCdSize;           // Size of resource (in sectors); m_nCdSize * STREAMING_BLOCK_SIZE = actual size in bytes
	uint8_t m_nStatus;          // see eStreamingStatus
	uint8_t pad[3];
};
VALIDATE_SIZE(CStreamingInfo, 0x14);
}
