/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "../Base.h"
#include "../SA.h"
#include "Task.h"

namespace sa {
enum ePrimaryTasks // array indexes
{
	TASK_PRIMARY_INVALID = -1,

	TASK_PRIMARY_PHYSICAL_RESPONSE = 0,
	TASK_PRIMARY_EVENT_RESPONSE_TEMP,
	TASK_PRIMARY_EVENT_RESPONSE_NONTEMP,
	TASK_PRIMARY_PRIMARY,
	TASK_PRIMARY_DEFAULT,
	TASK_PRIMARY_MAX
};

enum eSecondaryTask : uint32 // array indexes
{
	TASK_SECONDARY_ATTACK = 0,              // want duck to be after attack
	TASK_SECONDARY_DUCK,                    // because attack controls ducking movement
	TASK_SECONDARY_SAY,
	TASK_SECONDARY_FACIAL_COMPLEX,
	TASK_SECONDARY_PARTIAL_ANIM,
	TASK_SECONDARY_IK,
	TASK_SECONDARY_MAX
};

class CTaskComplex;
class CTaskSimple;

class CTaskManager {
public:
	CTask* m_aPrimaryTasks[TASK_PRIMARY_MAX]{};
	CTask* m_aSecondaryTasks[TASK_SECONDARY_MAX]{};
	CPed* m_pPed{};
};
VALIDATE_SIZE(CTaskManager, (VER_x32 ? 0x30 : 0x60));
}
