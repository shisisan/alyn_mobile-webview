#include "../UI.h"
#include "../../game/Game.h"
#include "../../net/NetGame.h"

extern NetGame* pNetGame;

Spawn::Spawn()
		: Layout(Orientation::HORIZONTAL)
{
	m_buttonPrev = new Button("<<", UISettings::fontSize() / 2);
	m_buttonSpawn = new Button("Spawn", UISettings::fontSize() / 2);
	m_buttonNext = new Button(">>", UISettings::fontSize() / 2);

	m_buttonPrev->setCallback([]() { if (pNetGame) pNetGame->SendPrevClass(); });
	m_buttonSpawn->setCallback([]() { if (pNetGame) pNetGame->SendSpawn(); });
	m_buttonNext->setCallback([]() { if (pNetGame) pNetGame->SendNextClass(); });

	this->addChild(m_buttonPrev);
	this->addChild(m_buttonSpawn);
	this->addChild(m_buttonNext);
}