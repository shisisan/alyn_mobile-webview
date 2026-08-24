#pragma once

#include "../Base.h"
#include "../../../Memory/include/Memory.h"

namespace sa {
enum eScriptParameterType : int8 {
	SCRIPT_PARAM_END_OF_ARGUMENTS, //< Special type used for vararg stuff

	SCRIPT_PARAM_STATIC_INT_32BITS,
	SCRIPT_PARAM_GLOBAL_NUMBER_VARIABLE, //< Global int32 variable
	SCRIPT_PARAM_LOCAL_NUMBER_VARIABLE, //< Local int32 variable
	SCRIPT_PARAM_STATIC_INT_8BITS,
	SCRIPT_PARAM_STATIC_INT_16BITS,
	SCRIPT_PARAM_STATIC_FLOAT,

	// Types below are only available in GTA SA

	// Number arrays
	SCRIPT_PARAM_GLOBAL_NUMBER_ARRAY, //< Global array of numbers (always int32)
	SCRIPT_PARAM_LOCAL_NUMBER_ARRAY, //< Local array of numbers (always int32)

	SCRIPT_PARAM_STATIC_SHORT_STRING, //< Static 8 byte string

	SCRIPT_PARAM_GLOBAL_SHORT_STRING_VARIABLE, //< Local 8 byte string
	SCRIPT_PARAM_LOCAL_SHORT_STRING_VARIABLE, //< Local 8 byte string

	SCRIPT_PARAM_GLOBAL_SHORT_STRING_ARRAY, //< Global 8 byte string array
	SCRIPT_PARAM_LOCAL_SHORT_STRING_ARRAY,  //< Local 8 byte string array

	SCRIPT_PARAM_STATIC_PASCAL_STRING, //< Pascal string is a sequence of characters with optional size specification. (So says Google)
	SCRIPT_PARAM_STATIC_LONG_STRING,    //< 16 byte string

	SCRIPT_PARAM_GLOBAL_LONG_STRING_VARIABLE, //< Global 16 byte string
	SCRIPT_PARAM_LOCAL_LONG_STRING_VARIABLE, //< Local 16 byte string

	SCRIPT_PARAM_GLOBAL_LONG_STRING_ARRAY, //< Global array of 16 byte strings
	SCRIPT_PARAM_LOCAL_LONG_STRING_ARRAY, //< Local array of 16 byte strings
};

class CRunningScript {
public:
	void Init()
	{
		Memory::callFunction("_ZN14CRunningScript4InitEv", this);
	}

	void ProcessOneCommand()
	{
		Memory::callFunction("_ZN14CRunningScript17ProcessOneCommandEv", this);
	}

	void setName(const char* name) { strcpy(m_szScriptName, name); }

public:
	CRunningScript* m_pNext;
	CRunningScript* m_pPrevious;
	char m_szScriptName[8];
	uint8* m_pBaseAddressOfThisScript;
	uint8* m_pPCPointer;
	uint8* m_pPCStack[8];
	uint16 m_nStackDepth;
	uint8 pad[2];
	uint32 m_nLocals[42];
	bool8 m_bActive;
	bool8 m_bCmpFlag;
	bool8 m_bIsThisAMissionScript;
	bool8 m_bIsThisAStreamedScript;
	bool8 m_bIsThisAMiniGameScript;
	int8 m_nScriptBrainType;
	uint8 pad2[2];
	int32 m_nActivateTime;
	uint16 m_nAndOrState;
	bool8 m_bNotForLatestExpression;
	bool8 m_bDeathArrestEnabled;
	bool8 m_bDeathArrestExecuted;
	uint8 pad3[3];
	uint32 m_nEndOfScriptedCutscenePC;
	bool8 m_bThisMustBeTheOnlyMissionRunning;
	uint8 pad4[3];
};
VALIDATE_SIZE(CRunningScript, (VER_x32 ? 0x100 : 0x130));
}
