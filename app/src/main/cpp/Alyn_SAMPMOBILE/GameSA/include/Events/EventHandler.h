#pragma once

#include "EventHandlerHistory.h"
#include "../Tasks/TaskSimple.h"

namespace sa {
class CTask;
class CEvent;
struct CPed;

class CEventHandler {
public:
	CPed* m_ped;
	CEventHandlerHistory m_history;
	CTask* m_physicalResponseTask;
	CTask* m_eventResponseTask;
	CTask* m_attackTask;
	CTask* m_sayTask;
	CTask* m_partialAnimTask;

public:
	auto& GetHistory() { return m_history; }
};
VALIDATE_SIZE(CEventHandler, (VER_x32 ? 0x34 : 0x60));
}
