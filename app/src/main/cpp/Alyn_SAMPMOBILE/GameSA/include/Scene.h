#pragma once

#include "../../Memory/include/Memory.h"
#include "RenderWare/rwcore.h"
#include "RenderWare/rpworld.h"

namespace sa {
class CScene {
public:
	RpWorld* m_pRpWorld;
	RwCamera* m_pRwCamera;
};
VALIDATE_SIZE(CScene, (VER_x32 ? 0x8 : 0x10));
}
