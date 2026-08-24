#include "../Game/Game.h"
#include "NetGame.h"
#include "../UI/UI.h"
#include "../Java/Java.h"

extern UI* pUI;
extern Game* pGame;
extern NetGame* pNetGame;

extern int iNetModeNormalOnFootSendRate;
extern int iNetModeNormalInCarSendRate;
extern int iNetModeFiringSendRate;
extern int iNetModeSendMultiplier;

#define REJECT_REASON_BAD_VERSION 1
#define REJECT_REASON_BAD_NICKNAME 2
#define REJECT_REASON_BAD_MOD 3
#define REJECT_REASON_BAD_PLAYERID 4

int g_iLagCompensationMode = 0;

void ProcessIncommingEvent(PLAYERID playerID, int iEventType, uint32_t dwParam1, uint32_t dwParam2, uint32_t dwParam3);

void InitGame(RPCParameters* rpcParams)
{
	spdlog::info("RPC: InitGame");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();

	PLAYERID MyPlayerID;
	int iLagCompensation = 0;
	bool bStuntBonus;
	bool bLanMode;
	RakNet::BitStream bsInitGame(Data, (iBitLength / 8) + 1, false);
	//read server settings
	bsInitGame.Read(pNetGame->m_netSet->zoneNames);
	bsInitGame.Read(pNetGame->m_netSet->useCJWalk);
	bsInitGame.Read(pNetGame->m_netSet->allowWeapons);
	bsInitGame.Read(pNetGame->m_netSet->limitGlobalChatRadius);
	bsInitGame.Read(pNetGame->m_netSet->globalChatRadius);
	bsInitGame.Read(bStuntBonus);
	bsInitGame.Read(pNetGame->m_netSet->nameTagDrawDistance);
	bsInitGame.Read(pNetGame->m_netSet->disableInteriorEnterExits);
	bsInitGame.Read(pNetGame->m_netSet->nameTagLOS);
	//pNetGame->m_netSet->nameTagLOS = true;
	bsInitGame.Read(pNetGame->m_netSet->manualVehicleEngineAndLight);
	bsInitGame.Read(pNetGame->m_netSet->spawnsAvailable);
	bsInitGame.Read(MyPlayerID);
	bsInitGame.Read(pNetGame->m_netSet->showNameTags);
	bsInitGame.Read(pNetGame->m_netSet->showPlayerMarkers);
	bsInitGame.Read(pNetGame->m_netSet->worldTime_Hour);
	bsInitGame.Read(pNetGame->m_netSet->weather);
	bsInitGame.Read(pNetGame->m_netSet->gravity);
	bsInitGame.Read(bLanMode);
	bsInitGame.Read(pNetGame->m_netSet->deathDropMoney);
	bsInitGame.Read(pNetGame->m_netSet->instagib);

	bsInitGame.Read(iNetModeNormalOnFootSendRate);
	bsInitGame.Read(iNetModeNormalInCarSendRate);
	bsInitGame.Read(iNetModeFiringSendRate);
	bsInitGame.Read(iNetModeSendMultiplier);
	bsInitGame.Read(iLagCompensation);

	switch (iLagCompensation) {
		case 1: g_iLagCompensationMode = 0;
			break;
		case 2: g_iLagCompensationMode = 1;
			break;
		default: g_iLagCompensationMode = 2;
			break;
	}

	if (pGame) {
		pGame->DisableAutoAim();
	}

	uint8_t byteStrLen;
	bsInitGame.Read(byteStrLen);
	if (byteStrLen) {
		memset(pNetGame->m_hostName, 0, sizeof(pNetGame->m_hostName));
		bsInitGame.Read(pNetGame->m_hostName, byteStrLen);
	}
	pNetGame->m_hostName[byteStrLen] = 0;

	uint8_t byteVehicleModels[212];
	bsInitGame.Read((char*) byteVehicleModels, 212);
//	pGame->PreloadVehicleModels(byteVehicleModels);

	bsInitGame.Read(pNetGame->m_netSet->vehicleFriendlyFire);
	pPlayerPool->SetLocalPlayerID(MyPlayerID);
	pGame->EnableStuntBonus(bStuntBonus);

	if (bLanMode) {
		pNetGame->m_lanMode = true;
	}

	pNetGame->InitGameLogic();
	pGame->SetGravity(pNetGame->m_netSet->gravity);

	if (pNetGame->m_netSet->disableInteriorEnterExits) {
		pGame->DisableEnterExits();
	}

	if (pNetGame->m_netSet->vehicleFriendlyFire) {
		//InstallHook_VehicleFriendlyFire();
	}

	pGame->SetWorldWeather(pNetGame->m_netSet->weather);
	pGame->ToggleCJWalk(pNetGame->m_netSet->useCJWalk);

	spdlog::info("szHostName, {}", pNetGame->m_hostName);
	spdlog::info("vehicleFriendlyFire, {}", pNetGame->m_netSet->vehicleFriendlyFire);

	if (pUI) {
		pUI->chat()->addDebugMessage("Connected to {B9C9BF}%s", Encoding::cp2utf(pNetGame->m_hostName).c_str());
		pUI->voicebutton()->setVisible(true);
	}

	pNetGame->SetGameState(GAMESTATE_CONNECTED);

	pPlayerPool->GetLocalPlayer()->HandleClassSelection();
}

//Prompt box rpc processing
void DialogBox(RPCParameters* rpcParams)
{
	spdlog::info("RPC: DialogBox");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	int16_t wDialogID;
	uint8_t byteDialogStyle;
	uint8_t byteLength;
	char szTitle[256 + 1];
	char szButton1[256 + 1];
	char szButton2[256 + 1];
	char szMessage[4096 + 1];
	memset(szTitle, 0, sizeof(szTitle));
	memset(szButton1, 0, sizeof(szButton1));
	memset(szButton2, 0, sizeof(szButton2));
	memset(szMessage, 0, sizeof(szMessage));

	// id & style
	bsData.Read(wDialogID);
	bsData.Read(byteDialogStyle);

	// title
	bsData.Read(byteLength);
	bsData.Read(szTitle, byteLength);

	// button1
	bsData.Read(byteLength);
	bsData.Read(szButton1, byteLength);

	// button2
	bsData.Read(byteLength);
	bsData.Read(szButton2, byteLength);

	// message
	stringCompressor->DecodeString(szMessage, 4096, &bsData);

	spdlog::info("wDialogID {}", wDialogID);
	spdlog::info("byteDialogStyle {}", (int) byteDialogStyle);
	spdlog::info("szTitle {}", szTitle);
	spdlog::info("szButton1 {}", szButton1);
	spdlog::info("szButton2 {}", szButton2);
	spdlog::info("szMessage {}", szMessage);

	/*if (wDialogID == 65535 || wDialogID < 0) // fix Blank Dialog
	{
		pNetGame->SendDialogResponse(wDialogID, 1, -1, " ");
		return;
	}*/

	if (g_java) {
		g_java->showDialog(wDialogID, byteDialogStyle, (char*) Encoding::cp2utf(szTitle).c_str(), (char*) Encoding::cp2utf(szMessage).c_str(), (char*) Encoding::cp2utf(szButton1).c_str(), (char*) Encoding::cp2utf(szButton2).c_str());
	}
}

//chat handling
void Chat(RPCParameters* rpcParams)
{
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	if (pNetGame->GetGameState() != GAMESTATE_CONNECTED) {
		return;
	}

	PLAYERID playerId;
	uint8_t byteLen;
	unsigned char szText[256 + 1];

	bsData.Read(playerId);
	bsData.Read(byteLen);
	bsData.Read((char*) szText, byteLen);
	szText[byteLen] = '\0';

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (playerId == pPlayerPool->GetLocalPlayerID()) {
		if (pUI) {
			pUI->chat()->addChatMessage(Encoding::cp2utf((char*) szText), Encoding::cp2utf(pPlayerPool->GetLocalPlayerName()), pPlayerPool->GetLocalPlayer()->GetPlayerColorAsRGBA());
		}
	}
	else {
		CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(playerId);
		if (pRemotePlayer) {
			pRemotePlayer->Say((char*) szText);
		}
	}
}

//client message
void ClientMessage(RPCParameters* rpcParams)
{
	spdlog::info("RPC: Client message ");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	uint32_t dwStrLen, dwColor;

	bsData.Read(dwColor);
	bsData.Read(dwStrLen);

	char* szMsg = (char*) malloc(dwStrLen + 1);

	bsData.Read(szMsg, dwStrLen);

	szMsg[dwStrLen] = '\0';

	spdlog::info("dwColor {}", dwColor);
	spdlog::info("dwStrLen {}", dwStrLen);
	spdlog::info("szMsg {}", szMsg);

	if (pUI) {
		pUI->chat()->addClientMessage(Encoding::cp2utf(szMsg), dwColor);
	}

	free(szMsg);
}

void RequestClass(RPCParameters* rpcParams)
{
	spdlog::info("RPC: Request class");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	uint8_t byteRequestOutcome = 0;
	PLAYER_SPAWN_INFO SpawnInfo;
	CLocalPlayer* pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();

	bsData.Read(byteRequestOutcome);
	bsData.Read((char*) &SpawnInfo, sizeof(PLAYER_SPAWN_INFO));

	if (byteRequestOutcome) {
		pLocalPlayer->SetSpawnInfo(&SpawnInfo);
		pLocalPlayer->HandleClassSelectionOutcome(true);
	}
	else {
		pLocalPlayer->HandleClassSelectionOutcome(false);
	}
}

//Request spawn players
void RequestSpawn(RPCParameters* rpcParams)
{
	spdlog::info("RPC: Request spawn");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	uint8_t byteRequestOutcome = false;
	CLocalPlayer* pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();

	bsData.Read(byteRequestOutcome);
	spdlog::info("byteRequestOutcome {}", byteRequestOutcome);

	if (byteRequestOutcome == 2 || (byteRequestOutcome && pLocalPlayer->m_bWaitingForSpawnRequestReply)) {
		pLocalPlayer->Spawn();
	}
	else {
		pLocalPlayer->m_bWaitingForSpawnRequestReply = false;
	}
}

void ToggleClock(RPCParameters* rpcParams)
{
	spdlog::info("RPC: Toggle clock");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	uint8_t byteClock;

	bsData.Read(byteClock);
	spdlog::info("byteClock {}", byteClock);

	if (byteClock) {
		pNetGame->m_netSet->holdTime = false;
	}
	else {
		pNetGame->m_netSet->holdTime = true;
		pGame->GetWorldTime((int*) &pNetGame->m_netSet->worldTime_Hour, (int*) &pNetGame->m_netSet->worldTime_Minute);
	}
}

void SetTimeEx(RPCParameters* rpcParams)
{
	spdlog::info("RPC: SetTimeEx");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	uint8_t byteHour;
	uint8_t byteMinute;

	bsData.Read(byteHour);
	bsData.Read(byteMinute);

	spdlog::info("byteHour {}", byteHour);
	spdlog::info("byteMinute {}", byteMinute);

	pGame->SetWorldTime(byteHour, byteMinute);
	pNetGame->m_netSet->worldTime_Hour = byteHour;
	pNetGame->m_netSet->worldTime_Minute = byteMinute;
}

void Weather(RPCParameters* rpcParams)
{
	spdlog::info("RPC: Weather");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	uint8_t byteWeather;

	bsData.Read(byteWeather);

	spdlog::info("weather {}", byteWeather);

	pNetGame->m_netSet->weather = byteWeather;
	pGame->SetWorldWeather(byteWeather);
}

void WorldTime(RPCParameters* rpcParams)
{
	spdlog::info("RPC: World time");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	uint8_t byteWorldTIme;
	bsData.Read(byteWorldTIme);

	spdlog::info("byteWorldTIme {}", byteWorldTIme);

	pNetGame->m_netSet->worldTime_Hour = byteWorldTIme;
}

void ConnectionRejected(RPCParameters* rpcParams)
{
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint8_t byteRejectReason;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(byteRejectReason);

	switch (byteRejectReason) {
		case REJECT_REASON_BAD_VERSION:
			if (pUI) {
				pUI->chat()->addInfoMessage("CONNECTION REJECTED: Incorrect Version.");
			}

			break;

		case REJECT_REASON_BAD_NICKNAME:
			if (pUI) {
				pUI->chat()->addInfoMessage("CONNECTION REJECTED: Unacceptable NickName");
				pUI->chat()->addInfoMessage("Please choose another nick between and 3-20 characters");
				pUI->chat()->addInfoMessage("Please use only a-z, A-Z, 0-9");
				pUI->chat()->addInfoMessage("Use /quit to exit or press ESC and select Quit Game");
			}
			pNetGame->SetGameState(GAMESTATE_WAIT_CONNECT);
			break;

		case REJECT_REASON_BAD_MOD:
			if (pUI) {
				pUI->chat()->addInfoMessage("CONNECTION REJECTED: Bad mod version.");
			}
			break;
		case REJECT_REASON_BAD_PLAYERID:
			if (pUI) {
				pUI->chat()->addInfoMessage("CONNECTION REJECTED: Unable to allocate a player slot.");
			}
			break;
	}

	pNetGame->GetRakClient()->Disconnect(500);
}

void GameModeRestart(RPCParameters* rpcParams)
{
	pNetGame->ShutdownForGameModeRestart();
}

void ServerJoin(RPCParameters* rpcParams)
{
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	CRemotePlayer* pRemotePlayer = nullptr;
	uint8_t byteNameLen;
	uint8_t byteIsNPC;
	PLAYERID PlayerID;
	uint32_t dwMarkerColor;
	char szPlayerName[256];
	memset(szPlayerName, 0, sizeof(szPlayerName));

	bsData.Read(PlayerID);
	bsData.Read(dwMarkerColor);
	bsData.Read(byteIsNPC);
	bsData.Read(byteNameLen);
	bsData.Read(szPlayerName, byteNameLen);
	szPlayerName[byteNameLen] = '\0';

	spdlog::info("PlayerID {}", PlayerID);
	spdlog::info("dwMarkerColor {}", dwMarkerColor);
	spdlog::info("byteIsNPC {}", byteIsNPC);
	spdlog::info("szPlayerName {}", szPlayerName);

	if (strlen(szPlayerName) > MAX_PLAYER_NAME) {
		return;
	}

	pPlayerPool->New(PlayerID, szPlayerName, byteIsNPC);

	pRemotePlayer = pPlayerPool->GetAt(PlayerID);

	if (dwMarkerColor && pRemotePlayer) {
		pRemotePlayer->SetPlayerColor(dwMarkerColor);
	}
}

void ServerQuit(RPCParameters* rpcParams)
{
	spdlog::info("RPC: Server quit");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	PLAYERID playerId;
	uint8_t byteReason;
	bsData.Read(playerId);
	bsData.Read(byteReason);

	pNetGame->GetPlayerPool()->Delete(playerId, byteReason);
}

void WorldPlayerAdd(RPCParameters* rpcParams)
{
	spdlog::info("RPC: World player add");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	CRemotePlayer* pRemotePlayer = nullptr;
	CPlayerPed* pPlayerPed = nullptr;
	PLAYERID playerId;
	uint8_t byteTeam;
	int iSkin;
	sa::CVector vecPos;
	float fRotation;
	uint32_t dwColor;
	uint8_t byteFightingStyle;
	uint16_t wUnkData[11];

	bsData.Read(playerId);
	bsData.Read(byteTeam);
	bsData.Read(iSkin);
	bsData.Read(vecPos.x);
	bsData.Read(vecPos.y);
	bsData.Read(vecPos.z);
	bsData.Read(fRotation);
	bsData.Read(dwColor);
	bsData.Read(byteFightingStyle);
	bsData.Read((char*) wUnkData, sizeof(wUnkData));

	if (pPlayerPool) {
		pRemotePlayer = pPlayerPool->GetAt(playerId);
		if (pRemotePlayer) {
			if (pRemotePlayer->Spawn(byteTeam, iSkin, &vecPos, fRotation, dwColor, byteFightingStyle)) {
				pPlayerPed = pRemotePlayer->GetPlayerPed();
				if (pPlayerPed) {
					for (int i = 0; i < 11; i++) {
						pPlayerPed->SetSkillLevel(i, wUnkData[i]);
					}
				}

				if (byteTeam != 0xFF) {
					pRemotePlayer->SetTeam(byteTeam);
				}

				if (pPlayerPool->IsPlayerNPC(playerId)) {
					if (pPlayerPed) {
						pPlayerPed->SetImmunities(0, 1, 1, 0, 0);
					}
				}
			}
			else {
				if (pUI) {
					pUI->chat()->addDebugMessage("Warning: Couldn't spawn player(%u)", playerId);
				}
			}
		}
	}
}

void WorldPlayerRemove(RPCParameters* rpcParams)
{
	spdlog::info("RPC: World player remove");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	CRemotePlayer* pRemotePlayer = nullptr;
	PLAYERID playerId;

	bsData.Read(playerId);
	spdlog::info("playerId {}", playerId);
	if (pPlayerPool) {
		pRemotePlayer = pPlayerPool->GetAt(playerId);
		if (pRemotePlayer) {
			pRemotePlayer->Remove();
		}
	}
}

void WorldPlayerDeath(RPCParameters* rpcParams)
{
	spdlog::info("RPC: Player Death");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	CRemotePlayer* pRemotePlayer = nullptr;
	PLAYERID playerId;

	bsData.Read(playerId);
	if (pPlayerPool) {
		pRemotePlayer = pPlayerPool->GetAt(playerId);
		if (pRemotePlayer) {
			pRemotePlayer->HandleDeath();
		}
	}
}

void WorldVehicleAdd(RPCParameters* rpcParams)
{
	spdlog::info("RPC: World vehicle add");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	NEW_VEHICLE VehicleInfo;
	VEHICLE_MOD VehicleModinfo;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) {
		return;
	}

	memset(&VehicleInfo, 0, sizeof(NEW_VEHICLE));
	memset(&VehicleModinfo, 0, sizeof(VEHICLE_MOD));

	bsData.Read((char*) &VehicleInfo, sizeof(NEW_VEHICLE));
	bsData.Read((char*) &VehicleModinfo, sizeof(VEHICLE_MOD));

	if (VehicleInfo.iVehicleType < 400 || VehicleInfo.iVehicleType > 611) {
		return;
	}

	pVehiclePool->New(&VehicleInfo);
	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleInfo.VehicleID);
	if (!pVehicle) {
		return;
	}

	bool bIsTrain = false;

	if (VehicleInfo.iVehicleType == 537) {
		VehicleInfo.iVehicleType = 569;
		bIsTrain = true;
	}

	if (VehicleInfo.iVehicleType == 538) {
		VehicleInfo.iVehicleType = 570;
		bIsTrain = true;
	}

	if (bIsTrain) {
		VehicleInfo.VehicleID++;
		pVehiclePool->New(&VehicleInfo);
		VehicleInfo.VehicleID++;
		pVehiclePool->New(&VehicleInfo);
		VehicleInfo.VehicleID++;
		pVehiclePool->New(&VehicleInfo);
	}

	if (pVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR) {
		for (int i = 0; i < 14; i++) {
			uint8_t byteMod = VehicleModinfo.byteModSlots[i];
			if (byteMod) {
				pVehicle->AddComponent(byteMod + 999);
			}
		}

		if (VehicleModinfo.bytePaintJob) {
			pVehicle->SetPaintJob(VehicleModinfo.bytePaintJob - 1);
		}
	}

	if (VehicleModinfo.iBodyColor1 != -1 || VehicleModinfo.iBodyColor2 != -1) {
		pVehicle->SetColor(VehicleModinfo.iBodyColor1, VehicleModinfo.iBodyColor2);
	}
}

void WorldVehicleRemove(RPCParameters* rpcParams)
{
	spdlog::info("RPC: World vehicle remove");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	VEHICLEID VehicleID;
	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) {
		return;
	}

	bsData.Read(VehicleID);

	spdlog::info("VehicleID {}", VehicleID);

	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
	if (!pVehicle) {
		return;
	}

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (!pPlayerPool) {
		return;
	}

	CLocalPlayer* pLocalPlayer = pPlayerPool->GetLocalPlayer();
	CPlayerPed* pPlayerPed = pGame->FindPlayerPed();

	if (pPlayerPed->IsInVehicle()) {
		sa::CVehicle* pGtaVehicle = pPlayerPed->GetGtaVehicle();
		PLAYERID MyVehicleID = pVehiclePool->FindIDFromGtaPtr(pGtaVehicle);

		if (MyVehicleID == VehicleID) {
			RwMatrix matPlayer = pPlayerPed->m_ped->GetMatrix().ToRwMatrix();
			pPlayerPed->RemoveFromVehicleAndPutAt(matPlayer.pos.x, matPlayer.pos.y, matPlayer.pos.z);
		}
	}

	for (int i = 0; i < MAX_PLAYERS; i++) {
		CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(i);
		if (pRemotePlayer) {

			if (pRemotePlayer->GetPlayerPed() && pRemotePlayer->GetState() != PLAYER_STATE_NONE && pRemotePlayer->m_pCurrentVehicle == pVehicle) {
				pRemotePlayer->RemoveFromVehicle();
			}
		}
	}

	for (int i = 0; i < MAX_VEHICLES; i++) {
		CVehicle* pVeh = pVehiclePool->GetAt(i);
		if (pVeh) {
			if (pVeh->m_pTrailer == pVehicle) {
				pVeh->DetachTrailer();
				pVeh->SetTrailer(nullptr);
			}
		}
	}

	int iModel = pVehicle->GetModelIndex();
	pVehiclePool->Delete(VehicleID);

	if (iModel == 537 || iModel == 538) {
		pVehiclePool->Delete(VehicleID + 1);
		pVehiclePool->Delete(VehicleID + 2);
		pVehiclePool->Delete(VehicleID + 3);
	}
}

void TimerUpdate(RPCParameters* rpcParams)
{
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	uint32_t dwTime = 0;

	bsData.Read(dwTime);
	spdlog::info("dwTime {}", dwTime);

	pGame->UpdateGlobalTimer(dwTime);
}

void ScmEvent(RPCParameters* rpcParams)
{
	spdlog::info("RPC: SCM event");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	PLAYERID playerId;
	int iEvent;
	uint32_t dwParam1, dwParam2, dwParam3;

	bsData.Read(playerId);
	bsData.Read(iEvent);
	bsData.Read(dwParam1);
	bsData.Read(dwParam2);
	bsData.Read(dwParam3);

	spdlog::info("playerId {}", playerId);
	spdlog::info("iEvent {}", iEvent);
	spdlog::info("dwParam1 {}", dwParam1);
	spdlog::info("dwParam2 {}", dwParam2);
	spdlog::info("dwParam3 {}", dwParam3);

	ProcessIncommingEvent(playerId, iEvent, dwParam1, dwParam2, dwParam3);
}

void UpdateScoresPingsIPs(RPCParameters* rpcParams)
{
	spdlog::info("RPC: Update scores and pings");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	PLAYERID playerId;
	int iPlayerScore = 0;
	int iPlayerPing = 0;

	for (int i = 0; i < (iBitLength / 8) / 10; i++) {
		bsData.Read(playerId);
		bsData.Read(iPlayerScore);
		bsData.Read(iPlayerPing);

		spdlog::info("RPC_PINGIP: ({}): score: {}, ping: {}", playerId, iPlayerScore, iPlayerPing);

		pPlayerPool->UpdatePlayerScore(playerId, iPlayerScore);
		pPlayerPool->UpdatePlayerPing(playerId, iPlayerPing);
	}
}

void Pickup(RPCParameters* rpcParams)
{
	spdlog::info("RPC: Pickup");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	int iIndex;
	PICKUP Pickup;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(iIndex);
	bsData.Read((char*) &Pickup, sizeof(PICKUP));

	CPickupPool* pPickupPool = pNetGame->GetPickupPool();
	if (pPickupPool) {
		pPickupPool->New(&Pickup, iIndex);
	}
}

//destroy pickup
void DestroyPickup(RPCParameters* rpcParams)
{
	spdlog::info("RPC: Destroy pickup");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	int iIndex;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(iIndex);
	//Get pickup pool
	CPickupPool* pPickupPool = pNetGame->GetPickupPool();
	if (pPickupPool) {
		//destroyed by index
		pPickupPool->Destroy(iIndex);
	}
}

//Create 3D text
void Create3DTextLabel(RPCParameters* rpcParams)
{
	spdlog::info("RPC: Create 3D text label");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CTextLabelPool* pTextLabelPool = pNetGame->GetTextLabelPool();
	if (pTextLabelPool == nullptr) {
		return;
	}

	uint16_t wLabelId;
	TEXT_LABEL label;
	char szBuffer[2048 + 1];
	memset(szBuffer, 0, sizeof(szBuffer));

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(wLabelId);
	bsData.Read(label.dwColor);
	bsData.Read(label.vecPos.x);
	bsData.Read(label.vecPos.y);
	bsData.Read(label.vecPos.z);
	bsData.Read(label.fDistance);
	bsData.Read(label.bTestLOS);
	bsData.Read(label.playerId);
	bsData.Read(label.vehicleId);

	stringCompressor->DecodeString(szBuffer, 2048, &bsData);
	strcpy(label.text, szBuffer);

	if (wLabelId < MAX_TEXT_LABELS) {
		pTextLabelPool->NewLabel(wLabelId, &label);
	}
}

//update 3d text
void Update3DTextLabel(RPCParameters* rpcParams)
{
	spdlog::info("RPC: Update 3D text label");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CTextLabelPool* pTextLabelPool = pNetGame->GetTextLabelPool();
	if (pTextLabelPool == nullptr) {
		return;
	}

	uint16_t wLabelId;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(wLabelId);

	if (pTextLabelPool->GetSlotState(wLabelId)) {
		pTextLabelPool->ClearLabel(wLabelId);
	}
}

void SetCheckpoint(RPCParameters* rpcParams)
{
	spdlog::info("RPC: Set checkpoint");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	sa::CVector vecPos;
	float fSize;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(vecPos.x);
	bsData.Read(vecPos.y);
	bsData.Read(vecPos.z);
	bsData.Read(fSize);

	sa::CVector vecSize;
	vecSize.x = fSize;
	vecSize.y = fSize;
	vecSize.z = fSize;

	pGame->SetCheckpointInformation(&vecPos, &vecSize);
	pGame->EnableCheckpoints(true);
}

void DisableCheckpoint(RPCParameters* rpcParams)
{
	spdlog::info("RPC: Disable checkpoint");
	pGame->EnableCheckpoints(false);
}

void SetRaceCheckpoint(RPCParameters* rpcParams)
{
	spdlog::info("RPC: Set race checkpoint");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint8_t byteType;
	sa::CVector vecPos;
	sa::CVector vecNextPos;
	float fRadius;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(byteType);
	bsData.Read(vecPos.x);
	bsData.Read(vecPos.y);
	bsData.Read(vecPos.z);
	bsData.Read(vecNextPos.x);
	bsData.Read(vecNextPos.y);
	bsData.Read(vecNextPos.z);
	bsData.Read(fRadius);

	pGame->SetRaceCheckpointInformation(byteType, &vecPos, &vecNextPos, fRadius);
	pGame->EnableRaceCheckpoints(true);
}

void DisableRaceCheckpoint(RPCParameters* rpcParams)
{
	spdlog::info("RPC: Disable race checkpoint");
	pGame->EnableRaceCheckpoints(false);
}

void DamageVehicle(RPCParameters* rpcParams)
{
	spdlog::info("RPC: Damage vehicle");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	VEHICLEID VehicleID;
	uint32_t dwPanels;
	uint32_t dwDoors;
	uint8_t byteLight;
	uint8_t byteTires;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(VehicleID);
	bsData.Read(dwPanels);
	bsData.Read(dwDoors);
	bsData.Read(byteLight);
	bsData.Read(byteTires);

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
	if (pVehicle) {
		pVehicle->SetDamageStatus(dwPanels, dwDoors, byteLight);
		pVehicle->SetTireDamageStatus(byteTires);
	}
}

void EnterVehicle(RPCParameters* rpcParams)
{
	spdlog::info("RPC: Enter vehicle");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID PlayerID;
	VEHICLEID VehicleID;
	uint8_t bytePassenger;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(PlayerID);
	bsData.Read(VehicleID);
	bsData.Read(bytePassenger);

	if (bytePassenger) {
		bytePassenger = 1;
	}

	CRemotePlayer* pRemotePlayer = pNetGame->GetPlayerPool()->GetAt(PlayerID);
	if (pRemotePlayer) {
		if (pRemotePlayer->GetDistanceFromLocalPlayer() < 200.0f) {
			pRemotePlayer->EnterVehicle(VehicleID, bytePassenger);
		}
	}
}

void ExitVehicle(RPCParameters* rpcParams)
{
	spdlog::info("RPC: Exit vehicle");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID PlayerID;
	VEHICLEID VehicleID;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(PlayerID);
	bsData.Read(VehicleID);

	CRemotePlayer* pRemotePlayer = pNetGame->GetPlayerPool()->GetAt(PlayerID);
	if (pRemotePlayer) {
		if (pRemotePlayer->GetDistanceFromLocalPlayer() < 200.0f) {
			pRemotePlayer->ExitVehicle();
		}
	}
}

//Vehicle parameters
void VehicleParamsEx(RPCParameters* rpcParams)
{
	spdlog::info("RPC: VehicleParamsEx");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) {
		return;
	}

	VEHICLEID VehicleID;
	VEHICLE_PARAMS_EX vehParamsEx;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(VehicleID);
	bsData.Read((char*) &vehParamsEx, sizeof(VEHICLE_PARAMS_EX));

	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
	if (!pVehicle) {
		return;
	}
	//engine status
	if (vehParamsEx.byteEngine) {
		if (vehParamsEx.byteEngine == 1) {
			pVehicle->SetEngineState(true);
		}
	}
	else {
		pVehicle->SetEngineState(false);
	}

	if (vehParamsEx.byteLight) {
		if (vehParamsEx.byteLight == 1) {
			pVehicle->SetLightState(true);
		}
	}
	else {
		pVehicle->SetLightState(false);
	}
	//door state
	if (vehParamsEx.byteDoors) {
		if (vehParamsEx.byteDoors == 1) {
			pVehicle->SetDoorState(true);
		}
	}
	else {
		pVehicle->SetDoorState(false);
	}
	//vehicle object
	if (vehParamsEx.byteObjective) {
		if (vehParamsEx.byteObjective == 1) {
			pVehicle->m_byteObjectiveVehicle = 1;
		}
	}
	else {
		pVehicle->m_byteObjectiveVehicle = 0;
	}

	if (vehParamsEx.byteBoot) {
		if (vehParamsEx.byteBoot == 1) {
			pVehicle->SetComponentOpenState(1, 17, 1.0f);
		}
	}
	else {
		pVehicle->SetComponentOpenState(1, 17, 0);
	}

	if (vehParamsEx.byteBonnet) {
		if (vehParamsEx.byteBonnet == 1) {
			pVehicle->SetComponentOpenState(0, 16, 1.0);
		}
	}
	else {
		pVehicle->SetComponentOpenState(0, 16, 0.0);
	}

	if (vehParamsEx.byteAlarm) {
		if (vehParamsEx.byteAlarm == 1) {
			pVehicle->SetSirenOn(true);
			pVehicle->SetAlarmState(20000);
		}
	}
	else {
		pVehicle->SetSirenOn(false);
		pVehicle->SetAlarmState(0);
	}

	if (vehParamsEx.byteSiren) {
		if (vehParamsEx.byteSiren == 1) {
			pVehicle->SetSirenOn(true);
		}
	}
	else {
		pVehicle->SetSirenOn(false);
	}

	if (vehParamsEx.byteDoor1) {
		if (vehParamsEx.byteDoor1 == 1) {
			pVehicle->SetComponentOpenState(2, 10, 1.0);
		}
	}
	else {
		pVehicle->SetComponentOpenState(2, 10, 0.0);
	}

	if (vehParamsEx.byteDoor2) {
		if (vehParamsEx.byteDoor2 == 1) {
			pVehicle->SetComponentOpenState(3, 8, 1.0);
		}
	}
	else {
		pVehicle->SetComponentOpenState(3, 8, 0.0);
	}

	if (vehParamsEx.byteDoor3) {
		if (vehParamsEx.byteDoor3 == 1) {
			pVehicle->SetComponentOpenState(4, 11, 1.0);
		}
	}
	else {
		pVehicle->SetComponentOpenState(4, 11, 0.0);
	}

	if (vehParamsEx.byteDoor4) {
		if (vehParamsEx.byteDoor4 == 1) {
			pVehicle->SetComponentOpenState(5, 9, 1.0);
		}
	}
	else {
		pVehicle->SetComponentOpenState(5, 9, 0.0);
	}

	if (vehParamsEx.byteWindow1) {
		if (vehParamsEx.byteWindow1 == 1) {
			pVehicle->CloseWindow(10);
		}
	}
	else {
		pVehicle->OpenWindow(10);
	}

	if (vehParamsEx.byteWindow2) {
		if (vehParamsEx.byteWindow2 == 1) {
			pVehicle->CloseWindow(8);
		}
	}
	else {
		pVehicle->OpenWindow(8);
	}

	if (vehParamsEx.byteWindow3) {
		if (vehParamsEx.byteWindow3 == 1) {
			pVehicle->CloseWindow(11);
		}
	}
	else {
		pVehicle->OpenWindow(11);
	}

	if (vehParamsEx.byteWindow4) {
		if (vehParamsEx.byteWindow4 == 1) {
			pVehicle->CloseWindow(9);
		}
	}
	else {
		pVehicle->OpenWindow(9);
	}
}

//show npc
void ShowActor(RPCParameters* rpcParams)
{
	spdlog::info("RPC: Show actor");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CActorPool* pActorPool = pNetGame->GetActorPool();
	if (!pActorPool) {
		return;
	}

	NEW_ACTOR newActor;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read((char*) &newActor, sizeof(NEW_ACTOR));

	pActorPool->New(&newActor);
}

//hide npc
void HideActor(RPCParameters* rpcParams)
{
	spdlog::info("RPC: Hide actor");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CActorPool* pActorPool = pNetGame->GetActorPool();
	if (!pActorPool) {
		return;
	}

	PLAYERID ActorID;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ActorID);

	pActorPool->Delete(ActorID);
}

//chat bubble
void ChatBubble(RPCParameters* rpcParams)
{
	spdlog::info("RPC: ChatBubble");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CPlayerBubblePool* pPlayerBubblePool = pNetGame->GetPlayerBubblePool();
	if (pPlayerBubblePool == nullptr) {
		return;
	}

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	PLAYERID playerId;
	uint32_t color;
	float drawDistance;
	uint32_t duration;
	uint8_t byteTextLen;

	char szText[256];
	memset(szText, 0, 256);

	bsData.Read(playerId);

	if (playerId == pNetGame->GetPlayerPool()->GetLocalPlayerID()) {
		return;
	}
	if (playerId >= MAX_PLAYERS) {
		return;
	}

	bsData.Read(color);
	bsData.Read(drawDistance);
	bsData.Read(duration);
	bsData.Read(byteTextLen);
	bsData.Read((char*) szText, byteTextLen);
	szText[byteTextLen] = '\0';

	pPlayerBubblePool->New(playerId, szText, color, drawDistance, duration);
}

//Edit attached object
void EditAttachedObject(RPCParameters* rpcParams)
{
	spdlog::info("RPC: EditAttachedObject");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	int attachedObjectIndex;
	bsData.Read(attachedObjectIndex);

	EditObject::startEditAttachedObject(attachedObjectIndex);
}

//edit object
void EditObject(RPCParameters* rpcParams)
{
	spdlog::info("RPC: EditObject");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	int bPlayerObject;
	int objectId;

	bsData.Read(bPlayerObject);
	bsData.Read(objectId);

	EditObject::m_playerObj = bPlayerObject;
	EditObject::startEditObject(objectId);
}

//Go to edit object
void EnterEditObject(RPCParameters* rpcParams)
{
	spdlog::info("RPC: EnterEditObject");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	// todo maybe?
}

//cancel editor object
void CancelEditObject(RPCParameters* rpcParams)
{
	spdlog::info("RPC: CancelEditObject");

	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	EditObject::cancelEdit();
}

void RegisterRPCs(RakClientInterface* pRakClient)
{
	spdlog::info("Registering RPC's..");

	// RPC_SetPlayerSkillLevel
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrCreate3DTextLabel, Create3DTextLabel);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrUpdate3DTextLabel, Update3DTextLabel);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_DialogBox, DialogBox);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetCheckpoint, SetCheckpoint);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_DisableCheckpoint, DisableCheckpoint);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetRaceCheckpoint, SetRaceCheckpoint);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_DisableRaceCheckpoint, DisableRaceCheckpoint);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_UpdateScoresPingsIPs, UpdateScoresPingsIPs);
	// RPC_SvrStats - useless
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_GameModeRestart, GameModeRestart);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ConnectionRejected, ConnectionRejected);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ClientMessage, ClientMessage);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldTime, WorldTime);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_Pickup, Pickup);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_DestroyPickup, DestroyPickup);
	// RPC_DestroyWeaponPickup - unused in 037
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScmEvent, ScmEvent);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_Weather, Weather);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetTimeEx, SetTimeEx);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ToggleClock, ToggleClock);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_TimerUpdate, TimerUpdate);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerAdd, WorldPlayerAdd);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerDeath, WorldPlayerDeath);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerRemove, WorldPlayerRemove);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleAdd, WorldVehicleAdd);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleRemove, WorldVehicleRemove);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_DamageVehicle, DamageVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_VehicleParamsEx, VehicleParamsEx);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_EnterVehicle, EnterVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ExitVehicle, ExitVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ServerJoin, ServerJoin);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ServerQuit, ServerQuit);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_InitGame, InitGame);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_Chat, Chat);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_RequestClass, RequestClass);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_RequestSpawn, RequestSpawn);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_EditAttachedObject, EditAttachedObject);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_EditObject, EditObject);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_EnterEditObject, EnterEditObject);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_CancelEditObject, CancelEditObject);

	// RPC_SetTargeting
	// RPC_ClientCheckResponse
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ShowActor, ShowActor);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_HideActor, HideActor);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ChatBubble, ChatBubble);
}

void UnregisterRPCs(RakClientInterface* pRakClient)
{
	spdlog::info("Unregistering RPC's..");

	// RPC_SetPlayerSkillLevel
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrCreate3DTextLabel);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrUpdate3DTextLabel);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_DialogBox);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetCheckpoint);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_DisableCheckpoint);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetRaceCheckpoint);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_DisableRaceCheckpoint);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_UpdateScoresPingsIPs);
	// RPC_SvrStats - useless
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_GameModeRestart);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ConnectionRejected);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ClientMessage);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldTime);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_Pickup);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_DestroyPickup);
	// RPC_DestroyWeaponPickup - unused in 037
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScmEvent);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_Weather);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetTimeEx);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ToggleClock);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_TimerUpdate);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldPlayerAdd);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldPlayerDeath);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldPlayerRemove);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldVehicleAdd);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldVehicleRemove);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_DamageVehicle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_VehicleParamsEx);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_EnterVehicle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ExitVehicle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ServerJoin);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ServerQuit);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_InitGame);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_Chat);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_RequestClass);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_RequestSpawn);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_EditAttachedObject);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_EditObject);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_EnterEditObject);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_CancelEditObject);

	// RPC_SetTargeting
	// RPC_ClientCheckResponse
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ShowActor);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_HideActor);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ChatBubble);
}
