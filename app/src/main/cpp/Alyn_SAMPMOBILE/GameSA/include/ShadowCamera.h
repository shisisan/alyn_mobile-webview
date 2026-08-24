/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare/rwcore.h"

namespace sa {
class CShadowCamera { // AKA CShadowImage
public:
	RwCamera* m_pShadowCamera;
	RwTexture* m_pShadowTexture;
};
VALIDATE_SIZE(CShadowCamera, (VER_x32 ? 0x8 : 0x10));
}
