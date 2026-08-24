#pragma once

class PlayerTags {
public:
	PlayerTags();
	~PlayerTags() = default;

	void Render(ImGuiRenderer* renderer);

private:
	void drawTag(ImGuiRenderer* renderer, RwV3d* vec, const char* szNick, uint32_t dwColor, float fDist, float fHealth, float fArmour, bool bAfk);

private:
	RwTexture* m_afkTexture;
};