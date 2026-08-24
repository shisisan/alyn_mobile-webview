#pragma once

constexpr auto DEFAULT_PLATE_FONT = "Arial";
constexpr auto DEFAULT_PLATE_TEXT = "XYZSR998";

enum {
	DEFAULT_PLATE_ALIGN = 1,
	DEFAULT_PLATE_TEXT_COLOR = 0xEE444470,
	DEFAULT_PLATE_BG_COLOR = 0xFFBEB6A8,
};

#include <imgui.h>

class MaterialTextGenerator {
public:
	MaterialTextGenerator();

	RwTexture* Create(int iSizeX, int iSizeY, const char* szFontName, uint8_t byteFontSize, uint8_t byteFontBold, uint32_t dwFontColor, uint32_t dwBackgroundColor, uint8_t byteAlign, const char* szText);
	RwTexture* CreateCustomCarPlate(const char* szPlate)
	{
		if (!szPlate || !strlen(szPlate)) {
			szPlate = DEFAULT_PLATE_TEXT;
		}

		return Create(64, 32, NULL, 30, 0, DEFAULT_PLATE_TEXT_COLOR, DEFAULT_PLATE_BG_COLOR, DEFAULT_PLATE_ALIGN, (const char*) szPlate);
	}

private:
	void SetUpScene();
	void Render(int iSizeX, int iSizeY, const char* szFontName, uint8_t byteFontSize, uint8_t byteFontBold, uint32_t dwFontColor, uint8_t byteAlign, const char* szText);

	RwCamera* m_camera;
	RwFrame* m_frame;
};