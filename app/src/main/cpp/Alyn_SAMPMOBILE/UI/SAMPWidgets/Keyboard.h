#pragma once

#include <jni.h>
#include <list>

class KeyboardInput : public Widget {
public:
	KeyboardInput();

	virtual void performLayout() override;

	void clear()
	{
		m_input.clear();
		m_caption.clear();
	}

	void addCharToInput(char value);
	void popCharFromInput();

	void setInputString(const std::string& string)
	{
		m_input = string;
		m_caption = Encoding::cp2utf(string);
	}
	const std::string& inputString() const { return m_input; }

	virtual void draw(ImGuiRenderer* rendderer) override;

private:
	Button* m_buttonNext;
	Button* m_buttonPrev;
	Button* m_buttonSlash;
	std::string m_caption;
	std::string m_input;
};

class KeyboardLayout : public Layout {
protected:
	enum class ExtendedKey {
		NONE,
		SHIFT_ON,
		SHIFT_OFF,
		BACKSPACE,
		NUM,
		LANG_ENG,
		SPACE,
		SEND
	};

public:
	enum class Type {
		ENG_LOWER,
		ENG_UPPER,
		NUM
	};

	enum class KeyCase {
		UPPER,
		LOWER
	};

	class Row : public Layout {
	public:
		Row();

		class Key : public Button {
		public:
			Key(const std::string& caption, char value);
			Key(ExtendedKey extended_key);

			virtual void draw(ImGuiRenderer* renderer) override;

			virtual void touchPopEvent() override;

			char value() const { return m_value; }

		private:
			ExtendedKey m_extendedKey;
			char m_value;
		};

		KeyboardLayout::Row::Key* addKey(const std::string& caption, char value);
		KeyboardLayout::Row::Key* addKey(ExtendedKey extended_key);
	};

	KeyboardLayout();

	virtual void performLayout() override;

protected:
	Row* m_rows[4];
};

class KeyboardLayoutLowerENG : public KeyboardLayout {
public:
	KeyboardLayoutLowerENG();
	virtual void performLayout() override;
};

class KeyboardLayoutUpperENG : public KeyboardLayout {
public:
	KeyboardLayoutUpperENG();
	virtual void performLayout() override;
};

class KeyboardLayoutNUM : public KeyboardLayout {
public:
	KeyboardLayoutNUM();
	virtual void performLayout() override;
};

#define KEYBOARD_MAX_HISTORY_MESSAGES    10
class KeyboardHistory {
public:
	KeyboardHistory();
	void add(const std::string& message);

	void reset();
	std::string next();
	std::string prev();

private:
	std::vector<std::string> m_vMessages;
	int m_index;
	int m_count;
};

class Keyboard : public Widget {
public:
	Keyboard();

	void show(Widget* caller, bool hidden = false);
	void hide();
	void send();
	void newSend(JNIEnv* env, jbyteArray str);

	bool hiddenInput() { return m_hiddenInput; }

	virtual void performLayout() override;
	virtual void draw(ImGuiRenderer* renderer) override;
	KeyboardInput* input() const { return m_input; }
	KeyboardHistory* history() const { return m_history; }
	void setActiveLayout(KeyboardLayout::Type type);
	void changeCase(KeyboardLayout::KeyCase key_case);

	void activateEvent(bool active) override;

private:
	KeyboardLayout* m_activeLayout;
	KeyboardLayout::Type m_activeLayoutType;

	KeyboardInput* m_input;
	KeyboardLayoutLowerENG* m_layoutLowerENG;
	KeyboardLayoutUpperENG* m_layoutUpperENG;
	KeyboardLayoutNUM* m_layoutNUM;
	Widget* m_caller;

	KeyboardHistory* m_history;

	bool m_hiddenInput;
};