#pragma once

#include <array>

#include "Event.h"

namespace sa {
struct CPed;

const int32 TOTAL_EVENTS_PER_EVENTGROUP = 16;

class CEventGroup {
public:
//	virtual ~CEventGroup() = default;

	CPed* m_pPed;
	int32 m_count;
#if !VER_x32
	uint8 _pad[4];
#endif
	CEvent* m_events[TOTAL_EVENTS_PER_EVENTGROUP];
};
//VALIDATE_SIZE(CEventGroup, (VER_x32 ? 0x4C : 0x98)); // tf?
}
