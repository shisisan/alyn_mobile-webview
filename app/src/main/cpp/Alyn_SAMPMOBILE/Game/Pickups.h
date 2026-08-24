//
// Created by ALYN on 18-Feb-25.
//

#ifndef ALYN_SA_MP_MOBILE_PICKUPS_H
#define ALYN_SA_MP_MOBILE_PICKUPS_H

class Pickups {
public:
	static void installHooks();
};

namespace sa {
struct CompressedVector {
	int16_t x;
	int16_t y;
	int16_t z;
};

inline CVector UncompressLargeVector(const CompressedVector& compressedVec)
{
	return {
			static_cast<float>(compressedVec.x) / 8.0f,
			static_cast<float>(compressedVec.y) / 8.0f,
			static_cast<float>(compressedVec.z) / 8.0f
	};
}

constexpr uint32_t MAX_COLLECTED_PICKUPS = 20;
constexpr uint32_t MAX_PICKUP_MESSAGES = 16;
constexpr uint32_t MAX_NUM_PICKUPS = 620;

#pragma pack(push, 4)
class CPickup {
public:
	bool IsVisible()
	{
		CCamera& TheCamera = getTheCamera();
		return DistanceBetweenPoints2D(GetPosn2D(), TheCamera.GetPosition()) < 100.0f;
	}

	[[nodiscard]] CVector GetPosn() const { return UncompressLargeVector(m_vecPos); }
	[[nodiscard]] CVector2D GetPosn2D() const { return GetPosn(); };

	void GiveUsAPickUpObject(CObject** obj, int32 slotIndex)
	{
		Memory::callFunction("_ZN7CPickup19GiveUsAPickUpObjectEPP7CObjecti", this, obj, slotIndex);
	}

	bool Update();

	void Remove()
	{
		Memory::callFunction("_ZN7CPickup6RemoveEv", this);
	}

	void GetRidOfObjects()
	{
		Memory::callFunction("_ZN7CPickup15GetRidOfObjectsEv", this);
	}

public:
	float m_fRevenueValue;
	sa::CObject* m_pObject;
	uint32_t m_nAmmo;
	uint32_t m_nRegenerationTime;
	CompressedVector m_vecPos;
	uint16_t m_bIsPicked;
	int16_t m_nModelIndex;
	int16_t m_nReferenceIndex;
	enum ePickupType : uint8_t {
		PICKUP_NONE = 0,
		PICKUP_IN_SHOP = 1,
		PICKUP_ON_STREET = 2,
		PICKUP_ONCE = 3,
		PICKUP_ONCE_TIMEOUT = 4,
		PICKUP_ONCE_TIMEOUT_SLOW = 5,
		PICKUP_COLLECTABLE1 = 6,
		PICKUP_IN_SHOP_OUT_OF_STOCK = 7,
		PICKUP_MONEY = 8,
		PICKUP_MINE_INACTIVE = 9,
		PICKUP_MINE_ARMED = 10,
		PICKUP_NAUTICAL_MINE_INACTIVE = 11,
		PICKUP_NAUTICAL_MINE_ARMED = 12,
		PICKUP_FLOATINGPACKAGE = 13,
		PICKUP_FLOATINGPACKAGE_FLOATING = 14,
		PICKUP_ON_STREET_SLOW = 15,
		PICKUP_ASSET_REVENUE = 16,
		PICKUP_PROPERTY_LOCKED = 17,
		PICKUP_PROPERTY_FORSALE = 18,
		PICKUP_MONEY_DOESNTDISAPPEAR = 19,
		PICKUP_SNAPSHOT = 20,
		PICKUP_2P = 21,
		PICKUP_ONCE_FOR_MISSION = 22
	} m_nPickupType;
	struct {
		uint8_t bDisabled : 1; // waiting for regeneration
		uint8_t bEmpty : 1;    // no ammo
		uint8_t bHelpMessageDisplayed : 1;
		uint8_t bVisible : 1;
		uint8_t nPropertyTextIndex : 3; // see enum ePickupPropertyText
	} m_nFlags;
};
#pragma pack(pop)
VALIDATE_SIZE(CPickup, (VER_x32 ? 0x20 : 0x24));
}

#endif //ALYN_SA_MP_MOBILE_PICKUPS_H
