#pragma once

#include "../TaskTimer.h"
#include "../Tasks/Task.h"

namespace sa {
class CEventHandlerHistory {
public:
	CTask* m_task = nullptr;
	CEvent* m_nonTempEvent = nullptr;
	CEvent* m_tempEvent = nullptr;
	CEvent* m_storedActiveEvent = nullptr;
	CTaskTimer m_storedActiveEventTimer;
};

VALIDATE_SIZE(CEventHandlerHistory, (VER_x32 ? 0x1C : 0x30));
}
