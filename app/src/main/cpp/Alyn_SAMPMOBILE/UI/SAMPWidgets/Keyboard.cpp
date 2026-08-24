#include "../../Client.h"
#include "../UI.h"
#include "../../Game/Game.h"
#include "../../Java/Java.h"
#include "../../Settings.h"

extern UI* pUI;
extern Game* pGame;

/* Keyboard */

Keyboard::Keyboard()
{
	m_input = new KeyboardInput();
	this->addChild(m_input);

	m_layoutLowerENG = new KeyboardLayoutLowerENG();
	this->addChild(m_layoutLowerENG);
	m_layoutLowerENG->setPadding(0.0f);
	m_layoutLowerENG->setSpacing(0.0f);
	m_layoutLowerENG->setVisible(false);

	m_layoutUpperENG = new KeyboardLayoutUpperENG();
	this->addChild(m_layoutUpperENG);
	m_layoutUpperENG->setPadding(0.0f);
	m_layoutUpperENG->setSpacing(0.0f);
	m_layoutUpperENG->setVisible(false);

	m_layoutNUM = new KeyboardLayoutNUM();
	this->addChild(m_layoutNUM);
	m_layoutNUM->setPadding(0.0f);
	m_layoutNUM->setSpacing(0.0f);
	m_layoutNUM->setVisible(false);

	m_activeLayout = m_layoutLowerENG;
	m_activeLayoutType = KeyboardLayout::Type::ENG_LOWER;
	m_activeLayout->setVisible(true);

	m_history = new KeyboardHistory();
}

void Keyboard::performLayout()
{
	m_input->setFixedSize(ImVec2(width(), UISettings::keyboardRowHeight()));
	m_input->setPosition(ImVec2(0.0f, 0.0f));

	/* ENG */
	m_layoutLowerENG->setFixedSize(ImVec2(width(), height() - m_input->height()));
	m_layoutLowerENG->setPosition(ImVec2(0.0f, UISettings::keyboardRowHeight()));
	m_layoutUpperENG->setFixedSize(ImVec2(width(), height() - m_input->height()));
	m_layoutUpperENG->setPosition(ImVec2(0.0f, UISettings::keyboardRowHeight()));

	/* NUM */
	m_layoutNUM->setFixedSize(ImVec2(width(), height() - m_input->height()));
	m_layoutNUM->setPosition(ImVec2(0.0f, UISettings::keyboardRowHeight()));

	Widget::performLayout();
}

void Keyboard::draw(ImGuiRenderer* renderer)
{
	renderer->drawRect(absolutePosition(), absolutePosition() + size(),
			UISettings::keyboardBackgroundColor(), true);
	Widget::draw(renderer);
}

void Keyboard::setActiveLayout(KeyboardLayout::Type type)
{
	m_activeLayout->setVisible(false);

	switch (type) {
		case KeyboardLayout::Type::ENG_LOWER: m_layoutLowerENG->setVisible(true);
			m_activeLayout = m_layoutLowerENG;
			break;

		case KeyboardLayout::Type::ENG_UPPER: m_layoutUpperENG->setVisible(true);
			m_activeLayout = m_layoutUpperENG;
			break;

		case KeyboardLayout::Type::NUM: m_layoutNUM->setVisible(true);
			m_activeLayout = m_layoutNUM;
			break;
	}

	m_activeLayoutType = type;
}

void Keyboard::changeCase(KeyboardLayout::KeyCase key_case)
{
	switch (key_case) {
		case KeyboardLayout::KeyCase::LOWER:
			if (m_activeLayoutType == KeyboardLayout::Type::ENG_UPPER) {
				setActiveLayout(KeyboardLayout::Type::ENG_LOWER);
			}
			break;

		case KeyboardLayout::KeyCase::UPPER:
			if (m_activeLayoutType == KeyboardLayout::Type::ENG_LOWER) {
				setActiveLayout(KeyboardLayout::Type::ENG_UPPER);
			}
			break;
	}

}

void Keyboard::show(Widget* caller, bool hidden)
{
	if (Settings::systemkeyboard()) {
		if (g_java) {
			g_java->showKeyboard(true);
		}
	}
	else {
		m_history->reset();
		m_input->clear();
		this->setVisible(true);
		pGame->EnableGameInput(false);
	}

	m_caller = caller;
	m_hiddenInput = hidden;
}

void Keyboard::hide()
{
	if (Settings::systemkeyboard()) {
		if (g_java) {
			g_java->showKeyboard(false);
		}
	}
	else {
		this->setVisible(false);
		pGame->EnableGameInput(true);
	}

	m_caller = nullptr;
}

void Keyboard::send()
{
	const std::string input = this->input()->inputString();

	m_history->add(input);
	if (m_caller) m_caller->keyboardEvent(input);
	this->hide();
}

void Keyboard::newSend(JNIEnv* env, jbyteArray str)
{
	std::string::size_type sz;
	sz = env->GetArrayLength(str);

	char* buffer = (char*) malloc(sz + 1);
	jbyte* elements = env->GetByteArrayElements(str, nullptr);

	memcpy(buffer, elements, sz);
	buffer[sz] = 0;

	if (m_caller) m_caller->keyboardEvent(buffer);
	hide();

	env->ReleaseByteArrayElements(str, elements, JNI_ABORT);
}

void Keyboard::activateEvent(bool active)
{
	if (!active) this->hide();
}

/* KeyboardInput */
KeyboardInput::KeyboardInput()
{
	m_buttonSlash = new Button("/", UISettings::fontSize());
	this->addChild(m_buttonSlash);

	m_buttonPrev = new Button("<<", UISettings::fontSize());
	this->addChild(m_buttonPrev);

	m_buttonNext = new Button(">>", UISettings::fontSize());
	this->addChild(m_buttonNext);

	m_buttonPrev->setCallback([]() {
		std::string text = pUI->keyboard()->history()->prev();
		pUI->keyboard()->input()->setInputString(text);
	});

	m_buttonNext->setCallback([]() {
		std::string text = pUI->keyboard()->history()->next();
		pUI->keyboard()->input()->setInputString(text);
	});

	m_buttonSlash->setCallback([]() {
		pUI->keyboard()->input()->setInputString("/");
	});

	m_buttonPrev->setVisible(true);
	m_buttonNext->setVisible(true);
	m_buttonSlash->setVisible(true);
}

void KeyboardInput::performLayout()
{
	Widget::performLayout();

	m_buttonSlash->setPosition(ImVec2(width() - (m_buttonSlash->width() + UISettings::padding() + m_buttonPrev->width() * 2.3 + UISettings::padding()), (height() - m_buttonSlash->height()) / 2));
	m_buttonPrev->setPosition(ImVec2(width() - (m_buttonPrev->width() + UISettings::padding() + m_buttonNext->width() + UISettings::padding()), (height() - m_buttonPrev->height()) / 2));
	m_buttonNext->setPosition(ImVec2(width() - (m_buttonNext->width() + UISettings::padding()), (height() - m_buttonNext->height()) / 2));
}

void KeyboardInput::addCharToInput(char value)
{
	m_input += value;
	m_caption = Encoding::cp2utf(m_input);
}

void KeyboardInput::popCharFromInput()
{
	if (m_input.empty()) return;

	m_input.pop_back();
	m_caption = Encoding::cp2utf(m_input);
}

void KeyboardInput::draw(ImGuiRenderer* renderer)
{
	float divLinePosY = (absolutePosition().y + size().y) - UISettings::outlineSize();
	float font_sz = UISettings::fontSize();

	if (!pUI->keyboard()->hiddenInput()) {
		renderer->drawText(absolutePosition() + ImVec2(UISettings::padding(), (height() - font_sz) / 2), ImColor(1.0f, 1.0f, 1.0f), m_caption, false, font_sz);
	}
	else {
		char hiddenCaption[512];
		strcpy(hiddenCaption, m_caption.c_str());

		for (int i = 0; i < strlen(hiddenCaption); i++) {
			if (hiddenCaption[i] == '\0') {
				break;
			}
			hiddenCaption[i] = '*';
		}

		renderer->drawText(absolutePosition() + ImVec2(UISettings::padding(), (height() - font_sz) / 2), ImColor(1.0f, 1.0f, 1.0f), hiddenCaption, false, font_sz);
	}

	renderer->drawLine(ImVec2(absolutePosition().x, divLinePosY), ImVec2(absolutePosition().x + size().x, divLinePosY), UISettings::buttonFocusedColor(), UISettings::outlineSize());

	Widget::draw(renderer);
}

/* KeyboardLayout */

KeyboardLayout::KeyboardLayout()
		: Layout(Orientation::VERTICAL)
{
	for (auto& m_row : m_rows) {
		m_row = new Row();
		this->addChild(m_row);
	}
}

void KeyboardLayout::performLayout()
{
	Layout::performLayout();
}

/* KeyboardLayout::Row */

KeyboardLayout::Row::Row()
		: Layout(Orientation::HORIZONTAL)
{

}

KeyboardLayout::Row::Key* KeyboardLayout::Row::addKey(const std::string& caption, char value)
{
	Key* key = new Key(caption, value);
	this->addChild(key);
	return key;
}

KeyboardLayout::Row::Key* KeyboardLayout::Row::addKey(ExtendedKey extended_key)
{
	Key* key = new Key(extended_key);
	this->addChild(key);
	return key;
}

/* KeyboardLayout::Row::Key */

KeyboardLayout::Row::Key::Key(const std::string& caption, char value)
		: Button(caption)
{
	m_extendedKey = ExtendedKey::NONE;
	m_value = value;
}

KeyboardLayout::Row::Key::Key(ExtendedKey extended_key)
		: Button("")
{
	switch (extended_key) {
		case ExtendedKey::NUM: this->setCaption("123");
			break;
		case ExtendedKey::LANG_ENG: this->setCaption("ENG");
			break;
	}

	m_extendedKey = extended_key;
}

void KeyboardLayout::Row::Key::draw(ImGuiRenderer* renderer)
{
	if (focused() && !pUI->keyboard()->hiddenInput()) {
		renderer->drawRect(absolutePosition(), absolutePosition() + size(), UISettings::buttonFocusedColor(), true);
	}

	switch (m_extendedKey) {
		case ExtendedKey::SHIFT_ON:
			renderer->drawTriangle(
					ImVec2(absolutePosition().x + width() * 0.37f, absolutePosition().y + height() * 0.50f),
					ImVec2(absolutePosition().x + width() * 0.50f, absolutePosition().y + height() * 0.25f),
					ImVec2(absolutePosition().x + width() * 0.63f, absolutePosition().y + height() * 0.50f),
					ImColor(1.0f, 1.0f, 1.0f), true
			);
			renderer->drawRect(
					ImVec2(absolutePosition().x + width() * 0.44f, absolutePosition().y + height() * 0.50f - 1),
					ImVec2(absolutePosition().x + width() * 0.56f, absolutePosition().y + height() * 0.68f),
					ImColor(1.0f, 1.0f, 1.0f), true
			);
			break;

		case ExtendedKey::SHIFT_OFF:
			renderer->drawTriangle(
					ImVec2(absolutePosition().x + width() * 0.37f, absolutePosition().y + height() * 0.50f),
					ImVec2(absolutePosition().x + width() * 0.50f, absolutePosition().y + height() * 0.25f),
					ImVec2(absolutePosition().x + width() * 0.63f, absolutePosition().y + height() * 0.50f),
					UISettings::buttonFocusedColor(), true
			);
			renderer->drawRect(
					ImVec2(absolutePosition().x + width() * 0.44f, absolutePosition().y + height() * 0.50f - 1),
					ImVec2(absolutePosition().x + width() * 0.56f, absolutePosition().y + height() * 0.68f),
					UISettings::buttonFocusedColor(), true
			);
			break;

		case ExtendedKey::BACKSPACE: static ImVec2 points[5];
			points[0] = ImVec2(absolutePosition().x + width() * 0.35f, absolutePosition().y + height() * 0.50f);
			points[1] = ImVec2(absolutePosition().x + width() * 0.45f, absolutePosition().y + height() * 0.25f);
			points[2] = ImVec2(absolutePosition().x + width() * 0.65f, absolutePosition().y + height() * 0.25f);
			points[3] = ImVec2(absolutePosition().x + width() * 0.65f, absolutePosition().y + height() * 0.65f);
			points[4] = ImVec2(absolutePosition().x + width() * 0.45f, absolutePosition().y + height() * 0.65f);
			renderer->drawConvexPolyFilled(points, 5, ImColor(1.0f, 1.0f, 1.0f));
			break;

		case ExtendedKey::SPACE:
			renderer->drawRect(
					ImVec2(absolutePosition().x + width() * 0.07f, absolutePosition().y + height() * 0.35f),
					ImVec2(absolutePosition().x + width() * 0.93f, absolutePosition().y + height() * 0.65f),
					ImColor(1.0f, 1.0f, 1.0f), true
			);
			break;

		case ExtendedKey::SEND:
			renderer->drawTriangle(
					ImVec2(absolutePosition().x + width() * 0.3f, absolutePosition().y + height() * 0.25f),
					ImVec2(absolutePosition().x + width() * 0.3f, absolutePosition().y + height() * 0.75f),
					ImVec2(absolutePosition().x + width() * 0.7f, absolutePosition().y + height() * 0.50f),
					ImColor(1.0f, 1.0f, 1.0f), true
			);
			break;
	}

	Widget::draw(renderer);
}

void KeyboardLayout::Row::Key::touchPopEvent()
{
	switch (m_extendedKey) {
		case ExtendedKey::BACKSPACE: pUI->keyboard()->input()->popCharFromInput();
			break;

		case ExtendedKey::SPACE: pUI->keyboard()->input()->addCharToInput(' ');
			break;

		case ExtendedKey::SEND: pUI->keyboard()->send();
			break;

		case ExtendedKey::SHIFT_ON: pUI->keyboard()->changeCase(KeyCase::UPPER);
			break;

		case ExtendedKey::SHIFT_OFF: pUI->keyboard()->changeCase(KeyCase::LOWER);
			break;

		case ExtendedKey::LANG_ENG: pUI->keyboard()->setActiveLayout(KeyboardLayout::Type::ENG_LOWER);
			break;

		case ExtendedKey::NUM: pUI->keyboard()->setActiveLayout(KeyboardLayout::Type::NUM);
			break;

		case ExtendedKey::NONE: pUI->keyboard()->input()->addCharToInput(m_value);
			break;
	}
}

/* KeyboardLayoutLowerENG */

KeyboardLayoutLowerENG::KeyboardLayoutLowerENG()
		: KeyboardLayout()
{

}

void KeyboardLayoutLowerENG::performLayout()
{
	m_rows[0]->setPadding(0.0f);
	m_rows[0]->setSpacing(0.0f);
	m_rows[0]->addKey("q", 'q');
	m_rows[0]->addKey("w", 'w');
	m_rows[0]->addKey("e", 'e');
	m_rows[0]->addKey("r", 'r');
	m_rows[0]->addKey("t", 't');
	m_rows[0]->addKey("y", 'y');
	m_rows[0]->addKey("u", 'u');
	m_rows[0]->addKey("i", 'i');
	m_rows[0]->addKey("o", 'o');
	m_rows[0]->addKey("p", 'p');

	ImVec2 keySize = ImVec2(width() / 10, UISettings::keyboardRowHeight());
	float padding = keySize.x / 2;
	m_rows[1]->setPadding(0.0f);
	m_rows[1]->setSpacing(0.0f);
	m_rows[1]->setPaddingLeft(padding);
	m_rows[1]->setPaddingRight(padding);
	m_rows[1]->addKey("a", 'a');
	m_rows[1]->addKey("s", 's');
	m_rows[1]->addKey("d", 'd');
	m_rows[1]->addKey("f", 'f');
	m_rows[1]->addKey("g", 'g');
	m_rows[1]->addKey("h", 'h');
	m_rows[1]->addKey("j", 'j');
	m_rows[1]->addKey("k", 'k');
	m_rows[1]->addKey("l", 'l');

	m_rows[2]->setPadding(0.0f);
	m_rows[2]->setSpacing(0.0f);
	m_rows[2]->addKey(ExtendedKey::SHIFT_ON)->setFixedSize(ImVec2(keySize.x * 1.5f, 0.0f));
	m_rows[2]->addKey("z", 'z');
	m_rows[2]->addKey("x", 'x');
	m_rows[2]->addKey("c", 'c');
	m_rows[2]->addKey("v", 'v');
	m_rows[2]->addKey("b", 'b');
	m_rows[2]->addKey("n", 'n');
	m_rows[2]->addKey("m", 'm');
	m_rows[2]->addKey(ExtendedKey::BACKSPACE)->setFixedSize(ImVec2(keySize.x * 1.5f, 0.0f));

	m_rows[3]->setPadding(0.0f);
	m_rows[3]->setSpacing(0.0f);
	m_rows[3]->addKey("/", '/');
	m_rows[3]->addKey(",", ',');
	m_rows[3]->addKey(ExtendedKey::NUM);
	m_rows[3]->addKey(ExtendedKey::SPACE)->setFixedSize(ImVec2(keySize.x * 4, 0.0f));
	m_rows[3]->addKey("?", '?');
	m_rows[3]->addKey("!", '!');
	m_rows[3]->addKey(ExtendedKey::SEND);

	KeyboardLayout::performLayout();
}

/* KeyboardLayoutUpperENG */

KeyboardLayoutUpperENG::KeyboardLayoutUpperENG()
		: KeyboardLayout()
{

}

void KeyboardLayoutUpperENG::performLayout()
{
	m_rows[0]->setPadding(0.0f);
	m_rows[0]->setSpacing(0.0f);
	m_rows[0]->addKey("Q", 'Q');
	m_rows[0]->addKey("W", 'W');
	m_rows[0]->addKey("E", 'E');
	m_rows[0]->addKey("R", 'R');
	m_rows[0]->addKey("T", 'T');
	m_rows[0]->addKey("Y", 'Y');
	m_rows[0]->addKey("U", 'U');
	m_rows[0]->addKey("I", 'I');
	m_rows[0]->addKey("O", 'O');
	m_rows[0]->addKey("P", 'P');

	ImVec2 keySize = ImVec2(width() / 10, UISettings::keyboardRowHeight());
	float padding = keySize.x / 2;
	m_rows[1]->setPadding(0.0f);
	m_rows[1]->setSpacing(0.0f);
	m_rows[1]->setPaddingLeft(padding);
	m_rows[1]->setPaddingRight(padding);
	m_rows[1]->addKey("A", 'A');
	m_rows[1]->addKey("S", 'S');
	m_rows[1]->addKey("D", 'D');
	m_rows[1]->addKey("F", 'F');
	m_rows[1]->addKey("G", 'G');
	m_rows[1]->addKey("H", 'H');
	m_rows[1]->addKey("J", 'J');
	m_rows[1]->addKey("K", 'K');
	m_rows[1]->addKey("L", 'L');

	m_rows[2]->setPadding(0.0f);
	m_rows[2]->setSpacing(0.0f);
	m_rows[2]->addKey(ExtendedKey::SHIFT_OFF)->setFixedSize(ImVec2(keySize.x * 1.5f, 0.0f));
	m_rows[2]->addKey("Z", 'Z');
	m_rows[2]->addKey("X", 'X');
	m_rows[2]->addKey("C", 'C');
	m_rows[2]->addKey("V", 'V');
	m_rows[2]->addKey("B", 'B');
	m_rows[2]->addKey("N", 'N');
	m_rows[2]->addKey("M", 'M');
	m_rows[2]->addKey(ExtendedKey::BACKSPACE)->setFixedSize(ImVec2(keySize.x * 1.5f, 0.0f));

	m_rows[3]->setPadding(0.0f);
	m_rows[3]->setSpacing(0.0f);
	m_rows[3]->addKey("/", '/');
	m_rows[3]->addKey(",", ',');
	m_rows[3]->addKey(ExtendedKey::NUM);
	m_rows[3]->addKey(ExtendedKey::SPACE)->setFixedSize(ImVec2(keySize.x * 4, 0.0f));
	m_rows[3]->addKey("?", '?');
	m_rows[3]->addKey("!", '!');
	m_rows[3]->addKey(ExtendedKey::SEND);

	KeyboardLayout::performLayout();
}

/* KeyboardLayoutNUM */

KeyboardLayoutNUM::KeyboardLayoutNUM()
		: KeyboardLayout()
{

}

void KeyboardLayoutNUM::performLayout()
{
	m_rows[0]->setPadding(0.0f);
	m_rows[0]->setSpacing(0.0f);
	m_rows[0]->addKey("1", '1');
	m_rows[0]->addKey("2", '2');
	m_rows[0]->addKey("3", '3');
	m_rows[0]->addKey("4", '4');
	m_rows[0]->addKey("5", '5');
	m_rows[0]->addKey("6", '6');
	m_rows[0]->addKey("7", '7');
	m_rows[0]->addKey("8", '8');
	m_rows[0]->addKey("9", '9');
	m_rows[0]->addKey("0", '0');

	m_rows[1]->setPadding(0.0f);
	m_rows[1]->setSpacing(0.0f);
	m_rows[1]->addKey("@", '@');
	m_rows[1]->addKey("#", '#');
	m_rows[1]->addKey("$", '$');
	m_rows[1]->addKey("%", '%');
	m_rows[1]->addKey("\"", '\"');
	m_rows[1]->addKey("*", '*');
	m_rows[1]->addKey("(", '(');
	m_rows[1]->addKey(")", ')');
	m_rows[1]->addKey("-", '-');
	m_rows[1]->addKey("_", '_');

	m_rows[2]->setPadding(0.0f);
	m_rows[2]->setSpacing(0.0f);
	m_rows[2]->addKey(".", '.');
	m_rows[2]->addKey(":", ':');
	m_rows[2]->addKey(";", ';');
	m_rows[2]->addKey("+", '+');
	m_rows[2]->addKey("=", '=');
	m_rows[2]->addKey("<", '<');
	m_rows[2]->addKey(">", '>');
	m_rows[2]->addKey("[", '[');
	m_rows[2]->addKey("]", ']');
	m_rows[2]->addKey(ExtendedKey::BACKSPACE);

	ImVec2 keySize = ImVec2(width() / 10, UISettings::keyboardRowHeight());

	m_rows[3]->setPadding(0.0f);
	m_rows[3]->setSpacing(0.0f);
	m_rows[3]->addKey("/", '/');
	m_rows[3]->addKey(",", ',');
	m_rows[3]->addKey(ExtendedKey::LANG_ENG);
	m_rows[3]->addKey(ExtendedKey::SPACE)->setFixedSize(ImVec2(keySize.x * 4, 0.0f));
	m_rows[3]->addKey("?", '?');
	m_rows[3]->addKey("!", '!');
	m_rows[3]->addKey(ExtendedKey::SEND);

	KeyboardLayout::performLayout();
}

KeyboardHistory::KeyboardHistory()
{
	m_index = 0;
	m_count = 0;
	m_vMessages.resize(KEYBOARD_MAX_HISTORY_MESSAGES);
}

void KeyboardHistory::add(const std::string& input)
{
	for (int i = KEYBOARD_MAX_HISTORY_MESSAGES - 2; i > 0; i--) {
		std::string str = m_vMessages[i];
		m_vMessages[i] = m_vMessages[i - 1];
		m_vMessages[i - 1] = str;
	}

	m_vMessages[0] = input;
	if (m_count < KEYBOARD_MAX_HISTORY_MESSAGES) m_count++;
}

void KeyboardHistory::reset()
{
	m_index = 0;
}

std::string KeyboardHistory::next()
{
	if (m_count == 0) return "";

	std::string str = m_vMessages[m_index];

	if (m_index < m_count - 1) m_index++;
	return str;
}

std::string KeyboardHistory::prev()
{
	if (m_count == 0) return "";

	std::string str = m_vMessages[m_index];
	if (m_index > 0) m_index--;
	return str;
}
