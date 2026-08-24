//
// Created by ALYN on 16-Jan-25.
//

#pragma once

struct BuildingToRemove {
	uint32_t model;
	sa::CVector pos;
	float radius;
};

class RemoveBuilding {
public:
	static void add(uint32_t model, sa::CVector pos, float radius);
	static bool needsRemove(uint32_t model, sa::CVector pos);
	static void installHooks();

private:
	static void removeObjectInRange(uint32_t model, sa::CVector pos, float radius);
	static void removeOccludersInRadius(sa::CVector pos, float radius);

private:
	static inline int m_buildingToRemoveCount;
	static inline BuildingToRemove m_buildingToRemove[1000];
};
