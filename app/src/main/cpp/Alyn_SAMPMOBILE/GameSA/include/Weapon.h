#pragma once

#include "Base.h"
#include "Enums/WeaponType.h"

namespace sa {
class FxSystem_c;

enum eWeaponState : uint32 {
	WEAPONSTATE_READY = 0,
	WEAPONSTATE_FIRING,
	WEAPONSTATE_RELOADING,
	WEAPONSTATE_OUT_OF_AMMO,
	WEAPONSTATE_MELEE_MADECONTACT
};

enum class eWeaponSlot : uint32 {
	WEAPONSLOT_TYPE_UNARMED,
	WEAPONSLOT_TYPE_MELEE,
	WEAPONSLOT_TYPE_HANDGUN,
	WEAPONSLOT_TYPE_SHOTGUN,
	WEAPONSLOT_TYPE_SMG,        // Used for drive-by's
	WEAPONSLOT_TYPE_RIFLE,
	WEAPONSLOT_TYPE_SNIPER,
	WEAPONSLOT_TYPE_HEAVY,
	WEAPONSLOT_TYPE_THROWN,
	WEAPONSLOT_TYPE_SPECIAL,
	WEAPONSLOT_TYPE_GIFT,
	WEAPONSLOT_TYPE_PARACHUTE,
	WEAPONSLOT_TYPE_DETONATOR,
};
constexpr auto NUM_WEAPON_SLOTS = static_cast<size_t>(eWeaponSlot::WEAPONSLOT_TYPE_DETONATOR) + 1u;

struct CWeapon {
	eWeaponType dwType;
	eWeaponState dwState;
	uint32_t dwAmmoInClip;
	uint32_t dwAmmo;
	uint32_t m_nTimer;
	bool m_bFirstPersonWeaponModeSelected;
	bool m_bDontPlaceInHand;
	uint8_t pad[2];
	uintptr_t* m_pWeaponFxSys;
};  // MUST BE EXACTLY ALIGNED TO 28 bytes
VALIDATE_SIZE(CWeapon, (VER_x32 ? 0x1C : 0x20));
}
