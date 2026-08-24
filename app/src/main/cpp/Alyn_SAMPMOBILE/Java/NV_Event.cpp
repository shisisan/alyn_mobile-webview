#include <cstring>

#include "NV_Event.h"

/* NV_Event */

std::mutex NV_Event::g_EventMutex;
std::list<NVEvent*> NV_Event::g_pEvents;

void NV_Event::InsertNewest(NVEvent* ev)
{
	std::lock_guard<std::mutex> lock(g_EventMutex);

	NVEvent* pEvent = new NVEvent;
	memcpy(pEvent, ev, sizeof(NVEvent));

	g_pEvents.push_back(pEvent);
}

void NV_Event::GetNextEvent(NVEvent* ev)
{
	std::lock_guard<std::mutex> lock(g_EventMutex);

	if (g_pEvents.size() < 1) {
		ev->m_type = (NVEventType) 228;
		return;
	}

	NVEvent* pPopped = g_pEvents.front();

	if (!pPopped) {
		ev->m_type = (NVEventType) 228;
		return;
	}

	memcpy(ev, pPopped, sizeof(NVEvent));

	delete pPopped;

	g_pEvents.pop_front();
}
