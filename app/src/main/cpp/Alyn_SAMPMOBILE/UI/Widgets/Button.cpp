#include "../UI.h"
#include "Button.h"

Button::Button(const std::string& caption, float font_size)
{
	m_callback = nullptr;

	m_label = new Label(caption, ImColor(1.0f, 1.0f, 1.0f), false, font_size);
	this->addChild(m_label);

	m_color = UISettings::buttonColor();
	m_colorFocused = UISettings::buttonFocusedColor();
}

void Button::performLayout()
{
	float padding = UISettings::padding();

	m_label->performLayout();
	this->setSize(m_label->size() + ImVec2(padding * 2, padding / 2 * 2));

	m_label->setPosition((size() - m_label->size()) / 2);
}

void Button::draw(ImGuiRenderer* renderer)
{
	renderer->drawRect(absolutePosition(), absolutePosition() + size(),
			focused() ? m_colorFocused : m_color, true);

	// Outline
	renderer->drawRect(absolutePosition() + ImVec2(UISettings::outlineSize(), UISettings::outlineSize()),
			(absolutePosition() + size()) - ImVec2(UISettings::outlineSize(), UISettings::outlineSize()),
			ImColor(0x32, 0x91, 0xF5), false, UISettings::outlineSize());

	Widget::draw(renderer);
}

void Button::touchPopEvent()
{
	if (m_callback) m_callback();
}
