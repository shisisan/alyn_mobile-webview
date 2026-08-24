#pragma once

#include <list>
#include <map>

class Chat : public ListBox {
public:
	Chat();

	void addChatMessage(const std::string& message, const std::string& nick, const ImColor& nick_color);
	void addInfoMessage(const std::string& format, ...);
	void addDebugMessage(const std::string& format, ...);
	void addClientMessage(const std::string& message, const ImColor& color);

	void addClientCommand(const std::string& cmd_name, const std::function<void(const std::string&)>& callback);

	static void setTimestamp(bool timestamp) { m_timestamp = timestamp; }
	static bool timestamp() { return m_timestamp; }

	virtual void draw(ImGuiRenderer* renderer) override;

	virtual void activateEvent(bool active) override;
	virtual void touchPopEvent() override;
	virtual void keyboardEvent(const std::string& input) override;

private:
	void addMessage(const std::string& messsage, const ImColor& color = ImColor(1.0f, 1.0f, 1.0f));

	class MessageItem : public ListBoxItem {
	public:
		MessageItem(const std::string& message, const ImColor& color)
		{
			m_text = new Label(message, color, true, UISettings::fontSize() / 2);
			this->addChild(m_text);
		}

		void performLayout() override
		{
			if (m_timestamp) {
				time_t rawtime;
				tm* timeinfo;
				char buffer[80];
				time(&rawtime);
				timeinfo = localtime(&rawtime);
				strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);

				// check if the message already has a timestamp
				if (m_text->text().size() < 10 || !(isdigit(m_text->text()[1]) && isdigit(m_text->text()[2]) && m_text->text()[3] == ':' &&
						isdigit(m_text->text()[4]) && isdigit(m_text->text()[5]) && m_text->text()[6] == ':' &&
						isdigit(m_text->text()[7]) && isdigit(m_text->text()[8]) && m_text->text()[9] == ']')) {
					m_text->setText("[" + std::string(buffer) + "] " + m_text->text());
				}
			}
			else {
				// check and remove only a valid timestamp (format: [hh:mm:ss])
				if (m_text->text().size() >= 10 && m_text->text()[0] == '[' &&
						isdigit(m_text->text()[1]) && isdigit(m_text->text()[2]) && m_text->text()[3] == ':' &&
						isdigit(m_text->text()[4]) && isdigit(m_text->text()[5]) && m_text->text()[6] == ':' &&
						isdigit(m_text->text()[7]) && isdigit(m_text->text()[8]) && m_text->text()[9] == ']') {
					m_text->setText(m_text->text().substr(11)); // remove "[hh:mm:ss] " (10 characters + 1 space)
				}
			}

			m_text->performLayout();
			m_text->setPosition(ImVec2(0.0f, (height() - m_text->height()) / 2));
		}

	private:
		Label* m_text;
	};

private:
	std::map<std::string, std::function<void(const std::string&)>> m_commands;
	static bool m_timestamp;
};
