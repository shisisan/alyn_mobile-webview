#pragma once

class DebugLabels : public Widget {
public:
	DebugLabels() = default;
	virtual void draw(ImGuiRenderer* renderer) override;
};
