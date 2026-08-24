#pragma once

#include "../../Memory/include/Memory.h"
#include "RenderWare/rwcore.h"
#include "../../Game/Common.h"
#include "RenderWare/rw.h"

namespace sa {
class CSprite2d {
public:
	CSprite2d()
	{
		spdlog::info("CSprite2d::CSprite2d()");
		m_pTexture = nullptr;
	}

	~CSprite2d()
	{
		spdlog::info("CSprite2d::~CSprite2d()");
		if (m_pTexture) {
			RwTextureDestroy(m_pTexture);
			m_pTexture = nullptr;
		}
	}

	void SetTexture(const char* pFilename)
	{
		spdlog::info("CSprite2d::SetTexture({})", pFilename);
		Memory::callFunction("_ZN9CSprite2d10SetTextureEPc", this, pFilename);
	}

	void Draw(sa::CRect* rect, uint32_t dwColor, float* uv)
	{
		Memory::callFunction("_ZN9CSprite2d4DrawERK5CRectRK5CRGBAffffffff", this, rect, &dwColor, uv[0], uv[1], uv[2], uv[3], uv[4], uv[5], uv[6], uv[7]);
	}

public:
	RwTexture* m_pTexture;
};
VALIDATE_SIZE(CSprite2d, (VER_x32 ? 0x4 : 0x8));
}
