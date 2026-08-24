#include "../../Client.h"
#include "MultiTouch.h"
#include "NV_Event.h"

/* MultiTouch */

extern void AND_TouchEvent_hook(int type, int num, int posX, int posY);

void touch_event(int type, int num, int x, int y)
{
	// LOG::I("touch_event: type %d | num %d | x %d | y %d", type, num, x, y);

	AND_TouchEvent_hook(type, num, x, y);
}

DECL_HOOK(int32_t, NVEventGetNextEvent, NVEvent* ev, int waitMSecs)
{
	if (!ev) {
		spdlog::warn("NVEvent: %d", waitMSecs);
	}
	int32_t ret = NVEventGetNextEvent(ev, waitMSecs);

	if (ret) {
		if (ev->m_type == NV_EVENT_MULTITOUCH) {
			ev->m_type = (NVEventType) 228;
		}
	}

	NVEvent event;
	NV_Event::GetNextEvent(&event);

	if (event.m_type == NV_EVENT_MULTITOUCH) {
		int type = event.m_data.m_multi.m_action & NV_MULTITOUCH_ACTION_MASK;
		int num = (event.m_data.m_multi.m_action & NV_MULTITOUCH_POINTER_MASK) >> NV_MULTITOUCH_POINTER_SHIFT;

		int x1 = event.m_data.m_multi.m_x1;
		int y1 = event.m_data.m_multi.m_y1;

		int x2 = event.m_data.m_multi.m_x2;
		int y2 = event.m_data.m_multi.m_y2;

		int x3 = event.m_data.m_multi.m_x3;
		int y3 = event.m_data.m_multi.m_y3;

		int x4 = event.m_data.m_multi.m_x4;
		int y4 = event.m_data.m_multi.m_y4;

		if (type == NV_MULTITOUCH_CANCEL) {
			type = NV_MULTITOUCH_UP;
		}

		if ((x1 || y1) || num == 0) {
			if (num == 0 && type != NV_MULTITOUCH_MOVE) {
				touch_event(type, 0, x1, y1);
			}
			else {
				touch_event(NV_MULTITOUCH_MOVE, 0, x1, y1);
			}
		}

		if ((x2 || y2) || num == 1) {
			if (num == 1 && type != NV_MULTITOUCH_MOVE) {
				touch_event(type, 1, x2, y2);
			}
			else {
				touch_event(NV_MULTITOUCH_MOVE, 1, x2, y2);
			}
		}

		if ((x3 || y3) || num == 2) {
			if (num == 2 && type != NV_MULTITOUCH_MOVE) {
				touch_event(type, 2, x3, y3);
			}
			else {
				touch_event(NV_MULTITOUCH_MOVE, 2, x3, y3);
			}
		}

		if ((x4 || y4) || num == 3) {
			if (num == 3 && type != NV_MULTITOUCH_MOVE) {
				touch_event(type, 3, x4, y4);
			}
			else {
				touch_event(NV_MULTITOUCH_MOVE, 3, x4, y4);
			}
		}
	}

	return ret;
}

int g_points[1000];
int g_pointers[1000];

void MultiTouch::initialize()
{
	spdlog::info("Initializing multi touch..");

	// Points
	memset(g_points, 0, 999 * sizeof(int));
	Memory::protectAddr(g_saSym->Abs(VER_x32 ? 0x679E90 : 0x851D38));
	*g_saSym->Abs<int**>(VER_x32 ? 0x679E90 : 0x851D38) = &g_points[0];

	// pointers
	memset(g_pointers, 0, 999 * sizeof(int));
	Memory::protectAddr(g_saSym->Abs(VER_x32 ? 0x6D7178 : 0x8B5028));
	*g_saSym->Abs<int**>(VER_x32 ? 0x6D7178 : 0x8B5028) = &g_pointers[0];

#if VER_x32
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x26B03C), (uintptr_t) "\x04\x20", 2); // OS_PointerGetNumber
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x26B046), (uintptr_t) "\x04\x28", 2); // OS_PointerGetType
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x2700AC), (uintptr_t) "\x03\x28", 2); // LIB_PointerGetCoordinates
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x270118), (uintptr_t) "\x03\x28", 2); // LIB_PointerGetWheel
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x270164), (uintptr_t) "\x03\x28", 2); // LIB_PointerDoubleClicked
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x2700F2), (uintptr_t) "\x03\x2A", 2); // LIB_PointerGetButton
#else
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x320844), (uintptr_t) "\x80\x00\x80\x52", 4); // OS_PointerGetNumber
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x320850), (uintptr_t) "\x1F\x10\x00\x71", 4); // OS_PointerGetType
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x326FEC), (uintptr_t) "\x1F\x0C\x00\x71", 4); // LIB_PointerGetCoordinates
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x32706C), (uintptr_t) "\x1F\x0C\x00\x71", 4); // LIB_PointerGetWheel
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x3270AC), (uintptr_t) "\x1F\x0C\x00\x71", 4); // LIB_PointerDoubleClicked
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x327040), (uintptr_t) "\x1F\x0D\x00\x71", 4); // LIB_PointerGetButton
#endif

	// NVEventGetNextEvent
	HOOK("_Z19NVEventGetNextEventP7NVEventi", NVEventGetNextEvent);
}

extern "C"
{
JNIEXPORT jboolean JNICALL Java_ro_alynsampmobile_game_SAMP_multiTouchEvent4Ex(JNIEnv* env, jobject obj, jint action,
		jint pointer, jint x1, jint y1, jint x2, jint y2, jint x3, jint y3, jint x4, jint y4)
{
	static jclass motionEvent = env->FindClass("android/view/MotionEvent");

	static jfieldID ACTION_DOWN_id = env->GetStaticFieldID(motionEvent, "ACTION_DOWN", "I");
	static jfieldID ACTION_UP_id = env->GetStaticFieldID(motionEvent, "ACTION_UP", "I");

	static jfieldID ACTION_POINTER_DOWN_id = env->GetStaticFieldID(motionEvent, "ACTION_POINTER_DOWN", "I");
	static jfieldID ACTION_POINTER_UP_id = env->GetStaticFieldID(motionEvent, "ACTION_POINTER_UP", "I");

	static jfieldID ACTION_CANCEL_id = env->GetStaticFieldID(motionEvent, "ACTION_CANCEL", "I");
	static jfieldID ACTION_POINTER_INDEX_SHIFT_id = env->GetStaticFieldID(motionEvent, "ACTION_POINTER_ID_SHIFT", "I");
	static jfieldID ACTION_POINTER_INDEX_MASK_id = env->GetStaticFieldID(motionEvent, "ACTION_POINTER_ID_MASK", "I");
	static jfieldID ACTION_MASK_id = env->GetStaticFieldID(motionEvent, "ACTION_MASK", "I");

	static int ACTION_DOWN = env->GetStaticIntField(motionEvent, ACTION_DOWN_id);
	static int ACTION_UP = env->GetStaticIntField(motionEvent, ACTION_UP_id);

	static int ACTION_POINTER_DOWN = env->GetStaticIntField(motionEvent, ACTION_POINTER_DOWN_id);
	static int ACTION_POINTER_UP = env->GetStaticIntField(motionEvent, ACTION_POINTER_UP_id);

	static int ACTION_CANCEL = env->GetStaticIntField(motionEvent, ACTION_CANCEL_id);
	static int ACTION_POINTER_INDEX_MASK = env->GetStaticIntField(motionEvent, ACTION_POINTER_INDEX_MASK_id);
	static int ACTION_POINTER_INDEX_SHIFT = env->GetStaticIntField(motionEvent, ACTION_POINTER_INDEX_SHIFT_id);
	static int ACTION_MASK = env->GetStaticIntField(motionEvent, ACTION_MASK_id);

	NVEvent ev;

	ev.m_type = NV_EVENT_MULTITOUCH;

	if (action == ACTION_UP) {
		ev.m_data.m_multi.m_action = NV_MULTITOUCH_UP;
	}
	else if (action == ACTION_DOWN) {
		ev.m_data.m_multi.m_action = NV_MULTITOUCH_DOWN;
	}
	else if (action == ACTION_POINTER_DOWN) {
		ev.m_data.m_multi.m_action = NV_MULTITOUCH_DOWN;
	}
	else if (action == ACTION_POINTER_UP) {
		ev.m_data.m_multi.m_action = NV_MULTITOUCH_UP;
	}
	else if (action == ACTION_CANCEL) {
		ev.m_data.m_multi.m_action = NV_MULTITOUCH_CANCEL;
	}
	else {
		ev.m_data.m_multi.m_action = NV_MULTITOUCH_MOVE;
	}

	ev.m_data.m_multi.m_action = (NVMultiTouchEventType) (ev.m_data.m_multi.m_action | (pointer << NV_MULTITOUCH_POINTER_SHIFT));

	ev.m_data.m_multi.m_x1 = x1;
	ev.m_data.m_multi.m_y1 = y1;

	ev.m_data.m_multi.m_x2 = x2;
	ev.m_data.m_multi.m_y2 = y2;

	ev.m_data.m_multi.m_x3 = x3;
	ev.m_data.m_multi.m_y3 = y3;

	ev.m_data.m_multi.m_x4 = x4;
	ev.m_data.m_multi.m_y4 = y4;

	NV_Event::InsertNewest(&ev);

	return JNI_TRUE;
}
}
