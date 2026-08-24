#pragma once

#include "Base.h"
#include "Core/Vector.h"
#include "Events/EventGroup.h"
#include "Events/EventHandler.h"
#include "Memory.h"
#include "Tasks/TaskManager.h"

namespace sa {
class CPed;

class CPedIntelligence {
public:
	CPed* m_pPed;
	CTaskManager m_TaskMgr;
	CEventHandler m_eventHandler;
	CEventGroup m_eventGroup;

	int m_iDecisionMakerType;
	int m_iDecisionMakerTypeInGroup;
	float m_fHearingRange;
	float m_fSeeingRange;

	int m_iMaxNumFriendsToInform;
	float m_fMaxInformFriendDistance;
	float m_fFollowNodeThresholdDistance;

	int8 m_iNextEventResponseSequence;
	uint8 m_iHighestPriorityEventType;
	uint8 m_iHighestPriorityEventPriority;
	uint8 pad0;

#if VER_x32
	uint8 pad1[0x1C8];
#else
	uint8 pad1[0x2C8];
#endif
};
VALIDATE_SIZE(CPedIntelligence, (VER_x32 ? 0x298 : 0x440));
}
