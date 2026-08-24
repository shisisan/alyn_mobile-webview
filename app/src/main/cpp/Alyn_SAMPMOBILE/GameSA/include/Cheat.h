#pragma once
#include "../../Memory/include/Memory.h"

namespace sa {
class CCheat {
public:
	static void WeaponSkillsCheat()
	{
		Memory::callFunction("_ZN6CCheat17WeaponSkillsCheatEv");
	}

	static void VehicleSkillsCheat()
	{
		Memory::callFunction("_ZN6CCheat18VehicleSkillsCheatEv");
	}

	static void StaminaCheat()
	{
		Memory::callFunction("_ZN6CCheat12StaminaCheatEv");
	}
};
}
