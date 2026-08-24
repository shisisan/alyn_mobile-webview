#pragma once

#include "../../Game/Game.h"
#include "../../Game/Util.h"

#include "../../Settings.h"

class VoiceButton : public Button {
public:
	VoiceButton()
			: Button("VOICE", UISettings::fontSize() / 2)
	{
		m_recording = false;
		m_texture_micro_on = LoadTextureFromTxd("samp", "voiceactive");
		m_texture_micro_off = LoadTextureFromTxd("samp", "voicepassive");
	}

	virtual void draw(ImGuiRenderer* renderer) override
	{
		if (!Settings::voice()) {
			return;
		}

		if (m_texture_micro_on && m_texture_micro_off) {
			renderer->drawImage(absolutePosition(), absolutePosition() + size(), recording() ? m_texture_micro_on->raster : m_texture_micro_off->raster);
		}
		else {
			Button::draw(renderer);
		}
	}

	void touchPopEvent() override
	{
		setRecording(false);
	}

	void touchPushEvent() override
	{
		setRecording(true);
	}

	void setRecording(bool recording)
	{
		m_recording = recording;
		this->setCaptionColor(m_recording ? ImColor(1.0f, 0.0f, 0.0f) : ImColor(1.0f, 1.0f, 1.0f));
	}

	[[nodiscard]] bool recording() const { return m_recording; }

private:
	bool m_recording;
	RwTexture* m_texture_micro_on;
	RwTexture* m_texture_micro_off;
};
