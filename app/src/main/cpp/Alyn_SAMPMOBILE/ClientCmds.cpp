#include "ClientCmds.h"
#include "Client.h"
#include "Settings.h"
#include "UI/UI.h"
#include "Java/Java.h"
#include "Game/Game.h"
#include "Net/NetGame.h"
#include "Game/AudioStream.h"

#include <thread>

extern UI* pUI;
extern NetGame* pNetGame;
extern Game* pGame;
extern AudioStream* pAudioStream;

void cmdQuit(const std::string& params)
{
	if (pUI) {
		pUI->chat()->addInfoMessage("-> Exiting game...");
	}

	if (pNetGame) {
		if (pNetGame->GetRakClient()) {
			pNetGame->GetRakClient()->Disconnect(0);
		}
	}

	if (g_java) {
		g_java->exitGame();
	}
}

void cmdRcon(const std::string& params)
{
	if (params.empty()) {
		return;
	}

	uint8_t bytePacketId = ID_RCON_COMMAND;
	RakNet::BitStream bsCommand;
	bsCommand.Write(bytePacketId);
	auto dwCmdLen = (uint32_t) strlen(params.c_str());
	bsCommand.Write(dwCmdLen);
	bsCommand.Write(params.c_str(), dwCmdLen);
	pNetGame->GetRakClient()->Send(&bsCommand, HIGH_PRIORITY, RELIABLE, 0);
}

void cmdSave(const std::string& params)
{
	CPlayerPed* pPlayer = pGame->FindPlayerPed();
	FILE* fileOut;
	uint32_t dwVehicleID;
	float fZAngle;
	char szPath[0xFF];

	sprintf(szPath, "%sSAMP/savedpositions.txt", Client::gameDir());
	fileOut = fopen(szPath, "a");

	if (!fileOut) {
		if (pUI) {
			pUI->chat()->addInfoMessage("Save failed");
		}

		return;
	}

	// incar savepos

	if (pPlayer->IsInVehicle()) {

		sa::CVehicle* pVehicle = pPlayer->GetGtaVehicle();

		dwVehicleID = GamePool_Vehicle_GetIndex(pVehicle);
		ScriptCommand(&get_car_z_angle, dwVehicleID, &fZAngle);

		fprintf(fileOut, "AddStaticVehicle(%u, %.4f, %.4f, %.4f, %.4f, %u, %u); // %s\n",
				pVehicle->m_nModelIndex,
				pVehicle->GetPosition().x,
				pVehicle->GetPosition().y,
				pVehicle->GetPosition().z,
				fZAngle, pVehicle->m_nPrimaryColor,
				pVehicle->m_nSecondaryColor,
				params.c_str());

		fclose(fileOut);

		if (pUI) {
			pUI->chat()->addInfoMessage("-> Successfully saved vehicle position");
		}

		return;
	}

	// onfoot savepos

	sa::CPed* pActor = pPlayer->m_ped;
	ScriptCommand(&get_actor_z_angle, pPlayer->m_gtaId, &fZAngle);

	fprintf(fileOut, "AddPlayerClass(%u, %.4f, %.4f, %.4f, %.4f, 0, 0, 0, 0, 0, 0); // %s\n",
			pPlayer->GetModelIndex(),
			pActor->GetPosition().x,
			pActor->GetPosition().y,
			pActor->GetPosition().z,
			fZAngle,
			params.c_str());

	if (pUI) {
		pUI->chat()->addInfoMessage("-> Successfully saved onfoot position");
	}

	fclose(fileOut);
}

void cmdDL(const std::string& params)
{
	if (pUI) {
		pUI->chat()->addInfoMessage("-> DebugLabels %s", pUI->debuglabels()->visible() ? "OFF" : "ON");
		pUI->debuglabels()->setVisible(!pUI->debuglabels()->visible());
	}
}

void cmdDW(const std::string& params)
{
	if (pUI) {
		pUI->chat()->addInfoMessage("-> DeathWindow: %s", pUI->deathwindow()->visible() ? "OFF" : "ON");
		pUI->deathwindow()->setVisible(!pUI->deathwindow()->visible());
	}
}

void cmdClearMyChat(const std::string& params)
{
	if (!pUI) {
		return;
	}

	for (int i = 0; i < 25; i++) {
		pUI->chat()->addInfoMessage(" ");
	}
}

void cmdTimestamp(const std::string& params)
{
	if (pUI) {
		pUI->chat()->addInfoMessage("-> Timestamp %s", Chat::timestamp() ? "OFF" : "ON");
		Chat::setTimestamp(!Chat::timestamp());
		pUI->chat()->performLayout();
	}
}

void cmdCmds(const std::string& params)
{
	if (pUI) {
		pUI->chat()->addInfoMessage(" ");
		pUI->chat()->addInfoMessage("Client ClientCmds:");
		pUI->chat()->addInfoMessage("/quit (/q) -> Exit Game");
		pUI->chat()->addInfoMessage("/rcon -> Login to rcon console (Usage: /rcon login [password])");
		pUI->chat()->addInfoMessage("/save -> Save onfoot/incar position (as savedpositions.txt in SAMP data folder)");
		pUI->chat()->addInfoMessage("/dl -> Toggle Debug Labels");
		pUI->chat()->addInfoMessage("/dw -> Toggle Death Window");
		pUI->chat()->addInfoMessage("/clearmychat -> Clear Your Chat");
		pUI->chat()->addInfoMessage("/timestamp -> Toggle Timestamp");
		pUI->chat()->addInfoMessage(" ");
	}
}

void cmdJetpack(const std::string& params)
{
	if (pGame->FindPlayerPed()->IsInVehicle()) {
		pUI->chat()->addInfoMessage("-> You can't use jetpack while in vehicle");
		return;
	}

	pGame->FindPlayerPed()->StartJetpack();
	pUI->chat()->addInfoMessage("-> Jetpack started");
}

void ClientCmds::initialize()
{
	spdlog::info("Initializing client commands..");

	if (Client::offlineMode()) {
		pUI->chat()->addClientCommand("j", cmdJetpack);
	}
	else {
		pUI->chat()->addClientCommand("q", cmdQuit);
		pUI->chat()->addClientCommand("quit", cmdQuit);
		pUI->chat()->addClientCommand("rcon", cmdRcon);
		pUI->chat()->addClientCommand("save", cmdSave);
		pUI->chat()->addClientCommand("dl", cmdDL);
		pUI->chat()->addClientCommand("dw", cmdDW);
		pUI->chat()->addClientCommand("clearmychat", cmdClearMyChat);
		pUI->chat()->addClientCommand("timestamp", cmdTimestamp);
		pUI->chat()->addClientCommand("clientcmds", cmdCmds);
	}
}
