#include "ImGuiWrapper.h"
#include "UISettings.h"
#include "../game/Game.h"
#include "../net/NetGame.h"

extern NetGame* pNetGame;

ImGuiWrapper::ImGuiWrapper(const ImVec2& display_size, const std::string& fonts_path)
{
	m_displaySize = display_size;
	m_renderer = nullptr;
	m_fontRaster = nullptr;
	m_fontsPath = fonts_path;

	m_font = nullptr;
	m_weapFont = nullptr;

	m_vertexBuffer = nullptr;
	m_vertexBufferSize = 10000;
}

ImGuiWrapper::~ImGuiWrapper()
{
	shutdown();
}

bool ImGuiWrapper::initialize()
{
	spdlog::info("ImGuiWrapper::initialize");

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = m_displaySize;
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

	m_font = loadFont("arial_bold.ttf", UISettings::fontSize() / 2);
	if (m_font == nullptr) {
		return false;
	}

	m_weapFont = loadFont("gtaweap3.ttf", UISettings::fontSize() / 2);

	createFontTexture();

	m_renderer = new ImGuiRenderer(ImGui::GetBackgroundDrawList(), m_font);

	return true;
}

void ImGuiWrapper::render()
{
	ImGui::NewFrame();

	drawList();

	ImGui::EndFrame();
	ImGui::Render();
	renderDrawData(ImGui::GetDrawData());
}

void ImGuiWrapper::shutdown()
{
	spdlog::info("ImGuiWrapper::shutdown");
	destroyFontTexture();
}

ImFont* ImGuiWrapper::loadFont(const std::string& font_name, float font_size)
{
	spdlog::info("ImGuiWrapper::loadFont: {} from {}", font_name.c_str(), m_fontsPath.c_str());

	std::string fontPath = m_fontsPath + font_name;

	ImGuiIO& io = ImGui::GetIO();
	ImFontConfig fontCfg;
	fontCfg.GlyphExtraSpacing.x = UISettings::outlineSize();

	static const ImWchar ranges[] =
			{
					0x0020, 0x00FF, // Basic Latin + Latin Supplement
					0x0400, 0x04FF, // Cyrillic + Cyrillic Supplement
					0x0E00, 0x0E7F, // Thai
					0x2DE0, 0x2DFF, // Cyrillic Extended-A
					0xA640, 0xA69F, // Cyrillic Extended-B
					0xF020, 0xF0FF,
					0
			};

	ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), font_size, &fontCfg, ranges);

	if (font == nullptr) {
		spdlog::error("Failed to load font: {}", font_name.c_str());
		return nullptr;
	}

	return font;
}

void ImGuiWrapper::setupRenderState(ImDrawData* draw_data)
{
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*) 0);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*) 0);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*) 1);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*) rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*) rwBLENDINVSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*) 0);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*) rwCULLMODECULLNONE);
	RwRenderStateSet(rwRENDERSTATEBORDERCOLOR, (void*) 0);
	RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION, (void*) rwALPHATESTFUNCTIONGREATER);
	RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*) 2);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*) rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*) rwTEXTUREADDRESSCLAMP);
}

void ImGuiWrapper::checkVertexBuffer(ImDrawData* draw_data)
{
	if (m_vertexBuffer == nullptr || m_vertexBufferSize < draw_data->TotalVtxCount) {
		spdlog::info("ImGuiWrapper::checkVertexBuffer");

		if (m_vertexBuffer) {
			delete m_vertexBuffer;
			m_vertexBuffer = nullptr;
		}

		m_vertexBufferSize = draw_data->TotalVtxCount + 10000;
		m_vertexBuffer = new RwIm2DVertex[m_vertexBufferSize];
		if (!m_vertexBuffer) {
			spdlog::info("Buffer allocation error. Size {}", m_vertexBufferSize);
			return;
		}

		spdlog::info("Vertex buffer reallocated. Size {}", m_vertexBufferSize);
	}
}

void ImGuiWrapper::renderDrawData(ImDrawData* draw_data)
{
	const RwReal nearScreenZ = getNearScreenZ();
	const RwReal recipNearClip = getRecipNearClip();

	checkVertexBuffer(draw_data);
	setupRenderState(draw_data);

	RwIm2DVertex* vtx_dst = m_vertexBuffer;
	int vtx_offset = 0;

	for (int n = 0; n < draw_data->CmdListsCount; n++) {
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const ImDrawVert* vtx_src = cmd_list->VtxBuffer.Data;
		const ImDrawIdx* idx_src = cmd_list->IdxBuffer.Data;

		for (int i = 0; i < cmd_list->VtxBuffer.Size; i++) {
			RwIm2DVertexSetScreenX(vtx_dst, vtx_src->pos.x);
			RwIm2DVertexSetScreenY(vtx_dst, vtx_src->pos.y);
			RwIm2DVertexSetScreenZ(vtx_dst, nearScreenZ);
			RwIm2DVertexSetRecipCameraZ(vtx_dst, recipNearClip);
			vtx_dst->emissiveColor = vtx_src->col;
			RwIm2DVertexSetU(vtx_dst, vtx_src->uv.x, recipCameraZ);
			RwIm2DVertexSetV(vtx_dst, vtx_src->uv.y, recipCameraZ);

			vtx_dst++;
			vtx_src++;
		}

		const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;
		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback) {
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else {
				ImVec4 clip_rect;
				clip_rect.x = pcmd->ClipRect.x;
				clip_rect.y = pcmd->ClipRect.w;
				clip_rect.z = pcmd->ClipRect.z;
				clip_rect.w = pcmd->ClipRect.y;
				setScissorRect((void*) &clip_rect);

				RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*) pcmd->TextureId);
				RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, &m_vertexBuffer[vtx_offset], (RwInt32) cmd_list->VtxBuffer.Size, (RwImVertexIndex*) idx_buffer, pcmd->ElemCount);
				RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*) 0);

				clip_rect = {0.0f, 0.0f, 0.0f, 0.0f};
				setScissorRect((void*) &clip_rect);
			}

			idx_buffer += pcmd->ElemCount;
		}
		vtx_offset += cmd_list->VtxBuffer.Size;
	}

}

bool ImGuiWrapper::createFontTexture()
{
	spdlog::info("ImGuiWrapper::createFontTexture");

	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pxs;
	int width, height, bytes_per_pixel;
	io.Fonts->GetTexDataAsRGBA32(&pxs, &width, &height, &bytes_per_pixel);

	RwImage* font_img = RwImageCreate(width, height, bytes_per_pixel * 8);
	RwImageAllocatePixels(font_img);

	if (font_img == nullptr) {
		spdlog::error("Font image pointer 0x{:X}", reinterpret_cast<uintptr_t>(font_img));
		return false;
	}

	RwUInt8* pixels = font_img->cpPixels;
	for (int y = 0; y < font_img->height; y++) {
		memcpy((void*) pixels, pxs + font_img->stride * y, font_img->stride);
		pixels += font_img->stride;
	}

	RwInt32 w, h, d, flags;
	RwImageFindRasterFormat(font_img, rwRASTERTYPETEXTURE, &w, &h, &d, &flags);
	m_fontRaster = RwRasterCreate(w, h, d, flags);
	if (m_fontRaster == nullptr) {
		spdlog::error("font raster pointer 0x{:X}", reinterpret_cast<uintptr_t>(m_fontRaster));
		return false;
	}

	m_fontRaster = RwRasterSetFromImage(m_fontRaster, font_img);
	io.Fonts->TexID = (ImTextureID*) m_fontRaster;

	RwImageDestroy(font_img);

	spdlog::info("Atlas width {}", width);
	spdlog::info("Atlas height {}", height);
	return true;
}

void ImGuiWrapper::destroyFontTexture()
{
	if (m_fontRaster) {
		ImGuiIO& io = ImGui::GetIO();
		RwRasterDestroy(m_fontRaster);
		m_fontRaster = nullptr;
		io.Fonts->TexID = (ImTextureID) 0;
	}
}
