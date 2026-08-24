#include "../UI.h"
#include "../../Game/Game.h"
#include "../../Net/NetGame.h"

/* ButtonPanel */

extern UI* pUI;
extern Game* pGame;
extern NetGame* pNetGame;

ButtonPanel::ButtonPanel()
		: Layout(Orientation::HORIZONTAL)
{
	m_extended = false;

	m_extend = new Button(">>", UISettings::fontSize() / 2);
	m_esc = new Button("ESC", UISettings::fontSize() / 2);
	m_tab = new Button("TAB", UISettings::fontSize() / 2);
	m_alt = new Button("ALT", UISettings::fontSize() / 2);
	m_spc = new Button("SPC", UISettings::fontSize() / 2);
	m_f = new Button("F", UISettings::fontSize() / 2);
	m_h = new Button("H", UISettings::fontSize() / 2);
	m_2 = new Button("2", UISettings::fontSize() / 2);
	m_y = new Button("Y", UISettings::fontSize() / 2);
	m_n = new Button("N", UISettings::fontSize() / 2);
	m_c = new Button("C", UISettings::fontSize() / 2);
	//m_g = new Button("G", UISettings::fontSize() / 2);

	m_extend->setCallback([&]() {
		if (extended()) {
			minimize();
		}
		else {
			maximize();
		}
	});

	m_esc->setCallback([]() {
		if (pUI->scoreboard()->visible()) {
			pUI->scoreboard()->setVisible(false);
		}

		if (pNetGame) {
			if (pNetGame->GetTextDrawPool()) {
				pNetGame->GetTextDrawPool()->SetSelectState(false);
			}
		}
	});

	m_tab->setCallback([]() {
		if (!pUI->dialog()->visible() && !pUI->keyboard()->visible()) {
			pUI->scoreboard()->setVisible(!pUI->scoreboard()->visible());
		}

		if (!pGame->FindPlayerPed()->IsInVehicle()) {
			LocalPlayerKeys.bKeys[ePadKeys::KEY_ACTION] = true;
		}
	});

	m_alt->setCallback([]() {
		if (pGame->FindPlayerPed()->IsInVehicle()) {
			LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = true;
		}
		else {
			LocalPlayerKeys.bKeys[ePadKeys::KEY_WALK] = true;
		}
	});

	m_spc->setCallback([]() {
		if (pGame->FindPlayerPed()->IsInVehicle()) {
			LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = true;
		}
		else {
			LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT] = true;
		}
	});

	m_f->setCallback([]() {
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = true;
	});

	m_h->setCallback([]() {
		if (pGame->FindPlayerPed()->IsInVehicle() && !pGame->FindPlayerPed()->IsAPassenger()) {
			LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH] = true;
		}
		else {
			LocalPlayerKeys.bKeys[ePadKeys::KEY_CTRL_BACK] = true;
		}
	});

	m_2->setCallback([]() {
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SUBMISSION] = true;
	});

	m_y->setCallback([]() {
		LocalPlayerKeys.bKeys[ePadKeys::KEY_YES] = true;
	});

	m_n->setCallback([]() {
		LocalPlayerKeys.bKeys[ePadKeys::KEY_NO] = true;
	});

	m_c->setCallback([]() {
		LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH] = true;
	});

	/*m_g->setCallback([]() {
		if (pNetGame && pNetGame->GetPlayerPool() && pNetGame->GetPlayerPool()->GetLocalPlayer()) {
			pNetGame->GetPlayerPool()->GetLocalPlayer()->EnterVehicleAsPassenger();
		}
	});*/

	addChild(m_extend);
	addChild(m_esc);
	addChild(m_tab);
	addChild(m_alt);
	addChild(m_spc);
	addChild(m_f);
	addChild(m_h);
	addChild(m_2);
	addChild(m_y);
	addChild(m_n);
	addChild(m_c);
	//addChild(m_g);

	minimize();
}

void ButtonPanel::minimize()
{
	m_extended = false;

	m_extend->setVisible(true);
	m_extend->setCaption(">>");

	m_esc->setVisible(false);
	m_tab->setVisible(false);
	m_alt->setVisible(false);
	m_spc->setVisible(false);
	m_f->setVisible(false);
	m_h->setVisible(false);
	m_2->setVisible(false);
	m_y->setVisible(false);
	m_n->setVisible(false);
	m_c->setVisible(false);
	//m_g->setVisible(false);
}

void ButtonPanel::maximize()
{
	m_extended = true;

	m_extend->setVisible(true);
	m_extend->setCaption("<<");

	m_esc->setVisible(true);
	m_tab->setVisible(true);
	m_alt->setVisible(true);
	m_spc->setVisible(true);
	m_f->setVisible(true);
	m_h->setVisible(true);
	m_2->setVisible(true);
	m_y->setVisible(true);
	m_n->setVisible(true);
	m_c->setVisible(true);
	//m_g->setVisible(true);
}

/* PassengerButton */

ButtonPanel::PassengerButton::PassengerButton()
		: Image("samp", "gtexture")
{

}

void ButtonPanel::PassengerButton::draw(ImGuiRenderer* renderer)
{
	if (pNetGame) {
		CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
		CPlayerPed* pPlayerPed = pGame->FindPlayerPed();

		if (pVehiclePool && pPlayerPed && !pPlayerPed->IsInVehicle() && !pPlayerPed->IsAPassenger()) {
			VEHICLEID ClosetVehicleID = pVehiclePool->FindNearestToLocalPlayerPed();
			if (ClosetVehicleID < MAX_VEHICLES && pVehiclePool->GetSlotState(ClosetVehicleID)) {
				CVehicle* pVehicle = pVehiclePool->GetAt(ClosetVehicleID);
				if (pVehicle && pVehicle->GetDistanceFromLocalPlayerPed() < 4.0f) {
					Image::draw(renderer);
				}
			}
		}
	}
}

void ButtonPanel::PassengerButton::touchPopEvent()
{
	if (pNetGame && pNetGame->GetPlayerPool() && pNetGame->GetPlayerPool()->GetLocalPlayer()) {
		pNetGame->GetPlayerPool()->GetLocalPlayer()->EnterVehicleAsPassenger();
	}
}
