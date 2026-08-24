//
// Created by ALYN on 2024/8/16.
//
#pragma once

namespace sa {
class CEntryExit {
public:
	uint8_t cTitle[8];
	CRect rect;
	float pz;
	float prot;
	float spawnx;
	float spawny;
	float spawnz;
	float spawnrot;
	uint16 flags;
	uint8 areacode;
	uint8 extracol;
	uint8 openTime;
	uint8 shutTime;
	int8 NumRandomPedsToCreate;
	uint8 pad[1];
	CEntryExit* pLink;
};
VALIDATE_SIZE(CEntryExit, (VER_x32 ? 0x3C : 0x40));
}
