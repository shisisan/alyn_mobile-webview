#pragma once

#include "Base.h"

namespace sa {
class CTaskTimer {
public:
	uint32_t m_nStartTime;
	int32_t m_nInterval;
	bool m_bStarted;
	bool m_bStopped;
	uint8_t pad0[2];

public:
	// 0x420E10
	inline CTaskTimer(uint32_t start, uint32_t interval)
	{
		m_nStartTime = start;
		m_nInterval = interval;
		m_bStarted = true;
	}

	inline CTaskTimer()
	{
		m_nStartTime = 0;
		m_nInterval = 0;
		m_bStarted = false;
		m_bStopped = false;
	}
};
VALIDATE_SIZE(CTaskTimer, 0xC);
}
