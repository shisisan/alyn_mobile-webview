#include "../Client.h"
#include "../game/Game.h"
#include "../net/NetGame.h"
#include "UI.h"
#include "../net/PlayerBubblePool.h"
#include "../UI/SAMPWidgets/VoiceButton.h"
#include "../Voice/SpeakerList.h"
#include "../Voice/include/util/Render.h"

extern Game* pGame;
extern NetGame* pNetGame;
extern PlayerTags* pPlayerTags;

UI::UI(const ImVec2& display_size, const std::string& fonts_path)
		: Widget(), ImGuiWrapper(display_size, fonts_path)
{
	UISettings::Initialize(display_size);
	this->setFixedSize(display_size);
}

bool UI::initialize()
{
	if (!ImGuiWrapper::initialize()) return false;

	m_splashScreen = new SplashScreen();
	this->addChild(m_splashScreen);
	m_splashScreen->setFixedSize(size());
	m_splashScreen->setPosition(ImVec2(0.0f, 0.0f));
	m_splashScreen->setVisible(true);

	m_deathWindow = new DeathWindow();
	this->addChild(m_deathWindow);
	m_deathWindow->setSize(ImVec2(0, 0));
	m_deathWindow->setPosition(ImVec2(0, 0));
	m_deathWindow->setVisible(true);

	m_debugLabels = new DebugLabels();
	this->addChild(m_debugLabels);
	m_debugLabels->setSize(ImVec2(0, 0));
	m_debugLabels->setPosition(ImVec2(0, 0));
	m_debugLabels->setVisible(false);

	m_chat = new Chat();
	this->addChild(m_chat);
	m_chat->setFixedSize(UISettings::chatSize());
	m_chat->setPosition(UISettings::chatPos());
	m_chat->setItemSize(UISettings::chatItemSize());
	m_chat->setVisible(false);

	m_buttonPanel = new ButtonPanel();
	this->addChild(m_buttonPanel);
	m_buttonPanel->setFixedSize(UISettings::buttonPanelSize());
	m_buttonPanel->setPosition(UISettings::buttonPanelPos());
	m_buttonPanel->setVisible(false);

	auto passengerButton = new ButtonPanel::PassengerButton();
	this->addChild(passengerButton);
	passengerButton->setFixedSize(UISettings::buttonVoiceSize());
	passengerButton->setPosition(UISettings::buttonVoicePos() + ImVec2(ScaleX(150.0f), 0.0f));
	passengerButton->setVisible(true);

	m_voiceButton = new VoiceButton();
	this->addChild(m_voiceButton);
	m_voiceButton->setFixedSize(UISettings::buttonVoiceSize());
	m_voiceButton->setPosition(UISettings::buttonVoicePos());
	m_voiceButton->setVisible(false);

	m_spawn = new Spawn();
	this->addChild(m_spawn);
	m_spawn->setFixedSize(UISettings::spawnSize());
	m_spawn->setPosition(UISettings::spawnPos());
	m_spawn->setVisible(false);

	m_dialog = new Dialog();
	this->addChild(m_dialog);
	m_dialog->setVisible(false);
	m_dialog->setMinSize(UISettings::dialogMinSize());
	m_dialog->setMaxSize(UISettings::dialogMaxSize());

	m_keyboard = new Keyboard();
	this->addChild(m_keyboard);
	m_keyboard->setFixedSize(UISettings::keyboardSize());
	m_keyboard->setPosition(UISettings::keyboardPos());
	m_keyboard->setVisible(false);

	m_scoreboard = new Scoreboard();
	this->addChild(m_scoreboard);
	m_scoreboard->setSize(UISettings::scoreboardSize());
	m_scoreboard->setPosition(ImVec2(((displaySize().x - UISettings::scoreboardSize().x) / 2), ((displaySize().y - UISettings::scoreboardSize().y) / 2)));
	m_scoreboard->setVisible(false);

	/*auto copyright = new Label(OBFUSCATE("Alyn_SAMPMOBILE"), ImColor(1.0f, 1.0f, 1.0f), true, UISettings::fontSize() / 3);
	this->addChild(copyright);
	copyright->setPosition(ImVec2(RsGlobal->maximumWidth * 0.83, ScaleY(5)));*/

	return true;
}

void UI::render()
{
	ImGuiWrapper::render();

	if (m_needClearMousePos) {
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(-1, -1);
		m_needClearMousePos = false;
	}
}

void UI::shutdown()
{
	ImGuiWrapper::shutdown();
}

void UI::drawList()
{
	if (!visible()) return;

	if (pPlayerTags) pPlayerTags->Render(renderer());
	if (pNetGame && pNetGame->GetTextLabelPool()) pNetGame->GetTextLabelPool()->Render(renderer());
	if (pNetGame && pNetGame->GetPlayerBubblePool()) pNetGame->GetPlayerBubblePool()->Render(renderer());

	if (Settings::voice()) {
		if (pNetGame) {
			if (dialog()->visible() || scoreboard()->visible() || keyboard()->visible()) {
				SpeakerList::Hide();
			}
			else {
				SpeakerList::Show();
			}

			for (const auto& renderCallback : Render::renderCallbacks) {
				if (renderCallback != nullptr) {
					renderCallback();
				}
			}
		}
	}

	draw(renderer());
}

void UI::touchEvent(const ImVec2& pos, TouchType type)
{
	if (m_keyboard->visible() && m_keyboard->contains(pos)) {
		m_keyboard->touchEvent(pos, type);
		return;
	}

	if (m_dialog->visible() && m_dialog->contains(pos)) {
		m_dialog->touchEvent(pos, type);
		return;
	}

	ImGuiIO& io = ImGui::GetIO();
	switch (type) {
		case TouchType::push: io.MousePos = ImVec2(pos.x, pos.y);
			io.MouseDown[0] = true;
			break;
		case TouchType::move: io.MousePos = ImVec2(pos.x, pos.y);
			break;
		case TouchType::pop: io.MouseDown[0] = false;
			m_needClearMousePos = true;
			break;
	}

	Widget::touchEvent(pos, type);
}
