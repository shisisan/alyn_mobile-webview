#include "Game.h"
#include "../UI/UI.h"
#include "../Client.h"

extern UI* pUI;

MaterialTextGenerator::MaterialTextGenerator()
{
	m_camera = nullptr;
	m_frame = nullptr;

	SetUpScene();
}

void MaterialTextGenerator::SetUpScene()
{
	m_camera = RwCameraCreate();
	m_frame = RwFrameCreate();
	if (!m_camera || !m_frame) return;

	RwObjectHasFrameSetFrame(m_camera, m_frame);

	RwCameraSetFarClipPlane(m_camera, 300.0f);
	RwCameraSetNearClipPlane(m_camera, 0.01f);

	RwV2d view = {0.5f, 0.5f};
	RwCameraSetViewWindow(m_camera, &view);
	RwCameraSetProjection(m_camera, rwPERSPECTIVE);

	rpWorldAddCamera(m_camera);
}

RwTexture* MaterialTextGenerator::Create(int iSizeX, int iSizeY, const char* szFontName, uint8_t byteFontSize, uint8_t byteFontBold, uint32_t dwFontColor, uint32_t dwBackgroundColor, uint8_t byteAlign, const char* szText)
{
	RwRaster* raster = RwRasterCreate(iSizeX, iSizeY, 32, rwRASTERFORMAT8888 | rwRASTERTYPECAMERATEXTURE);
	if (!raster) return 0;

	RwTexture* bufferTexture = RwTextureCreate(raster);
	if (!bufferTexture) return 0;

	// background color
	uint8_t b = (dwBackgroundColor) & 0xFF;
	uint8_t g = (dwBackgroundColor >> 8) & 0xFF;
	uint8_t r = (dwBackgroundColor >> 16) & 0xFF;
	uint8_t a = (dwBackgroundColor >> 24) & 0xFF;
	uint32_t dwBackground = (r | (g << 8) | (b << 16) | (a << 24));

	m_camera->frameBuffer = raster;
	SetRenderWareCamera(m_camera);

	RwCameraClear(m_camera, (RwRGBA*) &dwBackground, 3);
	RwCameraBeginUpdate(m_camera);

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*) true);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*) true);
	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*) rwSHADEMODENASHADEMODE);
	RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*) 0);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*) rwCULLMODENACULLMODE);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*) false);
	DefinedState();

	Render(iSizeX, iSizeY, szFontName, byteFontSize, byteFontBold, dwFontColor, byteAlign, szText);

	RwCameraEndUpdate((RwCamera*) m_camera);

	if (bufferTexture) ++bufferTexture->refCount;
	return bufferTexture;
}

void MaterialTextGenerator::Render(int iSizeX, int iSizeY, const char* szFontName, uint8_t byteFontSize, uint8_t byteFontBold, uint32_t dwFontColor, uint8_t byteAlign, const char* szText)
{
	if (pUI) {
		ImGui::NewFrame();

		ImFont* pFont = pUI->font();

		// text color
		uint8_t b = (dwFontColor) & 0xFF;
		uint8_t g = (dwFontColor >> 8) & 0xFF;
		uint8_t r = (dwFontColor >> 16) & 0xFF;
		uint8_t a = (dwFontColor >> 24) & 0xFF;

		// text align
		ImVec2 _veecPos;
		switch (byteAlign) {
			case MATERIAL_ALIGN_LEFT: {
				// render text with left align
				ImVec2 veecPos = ImVec2(pUI->ScaleX(0), pUI->ScaleY(0));
				pUI->renderer()->drawText(veecPos, ImColor(r, g, b, a), szText, false, byteFontSize, pFont);
				break;
			}
			case MATERIAL_ALIGN_CENTER: {
				// render text with center align
				int newLineCount = 0;
				std::string strText = szText;
				std::stringstream ssLine(strText);
				std::string tmpLine;
				while (std::getline(ssLine, tmpLine, '\n')) {
					if (newLineCount == 0) { _veecPos.y = (iSizeY - pUI->renderer()->calculateTextSize(tmpLine.c_str(), byteFontSize).y) / 2; }
					else { _veecPos.y -= pUI->renderer()->calculateTextSize(tmpLine.c_str(), byteFontSize).y / 2; }
					newLineCount++;
				}

				// if pos y is minus change the pos to 0 (so make it like samp pc?)
				if (_veecPos.y < 0.0) {
					_veecPos.y = 0.0;
				}

				std::string strTexts = szText;
				std::stringstream ssLines(strTexts);
				std::string tmpLines;
				while (std::getline(ssLines, tmpLines, '\n')) {
					if (tmpLines[0] != 0) {
						_veecPos.x = (iSizeX - pUI->renderer()->calculateTextSize(tmpLines.c_str(), byteFontSize).x) / 2;
						pUI->renderer()->drawText(_veecPos, ImColor(r, g, b, a), tmpLines.c_str(), false, byteFontSize, pFont);
						_veecPos.y += byteFontSize;
					}
				}
				break;
			}
			case MATERIAL_ALIGN_RIGHT: {
				// render text with right align
				std::string strText = szText;
				std::stringstream ssLine(strText);
				std::string tmpLine;
				while (std::getline(ssLine, tmpLine, '\n')) {
					if (tmpLine[0] != 0) {
						_veecPos.x = (iSizeX - pUI->renderer()->calculateTextSize(tmpLine.c_str(), byteFontSize).x);
						pUI->renderer()->drawText(_veecPos, ImColor(r, g, b, a), tmpLine.c_str(), false, byteFontSize, pFont);
						_veecPos.y += byteFontSize;
					}
				}
				break;
			}
		}

		ImGui::EndFrame();
		ImGui::Render();
		pUI->renderDrawData(ImGui::GetDrawData());
	}
}
