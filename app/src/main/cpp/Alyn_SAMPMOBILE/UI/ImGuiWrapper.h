#pragma once

#include "ImGuiRenderer.h"
#include <RenderWare/rw.h>

class ImGuiWrapper {
public:
	ImGuiWrapper(const ImVec2& display_size, const std::string& fonts_path);

	virtual bool initialize();
	virtual void render();
	virtual void shutdown();

	ImFont* font() const { return m_font; }
	ImFont* weapFont() const { return m_weapFont; }

	ImGuiRenderer* renderer() const { return m_renderer; }

	void renderDrawData(ImDrawData* draw_data);

protected:
	virtual ~ImGuiWrapper();

	virtual void drawList() = 0;

	const ImVec2& displaySize() { return m_displaySize; }

private:
	ImFont* loadFont(const std::string& font_name, float font_size);

	void setupRenderState(ImDrawData* draw_data);
	void checkVertexBuffer(ImDrawData* draw_data);

	bool createFontTexture();
	void destroyFontTexture();

private:
	ImVec2 m_displaySize;
	std::string m_fontsPath;

	ImFont* m_font;
	ImFont* m_weapFont;

	ImGuiRenderer* m_renderer;
	RwRaster* m_fontRaster;

	RwIm2DVertex* m_vertexBuffer;
	int m_vertexBufferSize;
};
