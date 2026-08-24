#pragma once

#include "Ped.h"

namespace sa {
class CPlayerPed : public CPed {
public:
	CPed* m_p3rdPersonMouseTarget;
	int32_t field_7A0;

	// did we display "JCK_HLP" message
	static bool& bHasDisplayedPlayerQuitEnterCarHelpText;

	// Android
	static bool bDebugPlayerInvincible;
	static bool bDebugTargeting;
	static bool bDebugTapToTarget;
};
VALIDATE_SIZE(CPlayerPed, (VER_x32 ? 0x7AC : 0x998));
}
