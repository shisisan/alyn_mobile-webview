#pragma once

#include "Base.h"
#include "Collision/ColPoint.h"
#include "Core/Vector.h"

namespace sa {
struct CStoredCollPoly {
	CVector verts[3]{}; // triangle vertices
	bool valid{};
	char _pad[3];
	tColLighting ligthing{};
	char _pad2[3];
};
VALIDATE_SIZE(CStoredCollPoly, 0x2c);
}
