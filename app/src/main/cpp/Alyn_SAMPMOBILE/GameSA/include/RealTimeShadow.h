/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Base.h"
#include "ShadowCamera.h"
#include "RenderWare/rwcore.h"
#include "RenderWare/rpworld.h"

namespace sa {
class CPhysical;

class CRealTimeShadow { // AKA `CCutsceneShadow`
public:

public: // Eventually make this private
	CPhysical* m_pOwner{};       /// If `nullptr` the shadow isn't in use
	bool m_bKeepAlive{};   /// If false it starts to fade out and eventually gets destroyed.
	uint8 m_nIntensity{};
	CShadowCamera* m_camera{};
	bool m_bBlurred{};
//    CShadowCamera*m_blurCamera{};
	uint32 m_nBlurPasses{};
	bool m_bDrawMoreBlur;
	uint32 m_nRwObjectType{(uint32) -1};
	RpLight* m_pLight{};
	CSphere m_boundingSphere{};
	CSphere m_baseSphere{};
};
VALIDATE_SIZE(CRealTimeShadow, (VER_x32 ? 0x40 : 0x50));
}
