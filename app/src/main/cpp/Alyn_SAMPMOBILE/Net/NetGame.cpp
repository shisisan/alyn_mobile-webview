#include "../Client.h"
#include "../Game/Game.h"
#include "NetGame.h"
#include "../UI/UI.h"
#include "RakNet/SAMP/samp_auth.h"
#include "../Game/AudioStream.h"
#include "../Voice/SpeakerList.h"
#include "../Voice/Network.h"
#include "../cef/SAMPMobileCef.h"

#define AUTH_BS OBFUSCATE("39FB2DEEDB49ACFB8D4EECE6953D2507988CCCF4410")
// 39FB2DEEDB49ACFB8D4EECE6953D2507988CCCF4410 arz
// 1031CA8429843C9B8C178B65F3C73602578440D17F8 unarak
// 15121F6F18550C00AC4B4F8A167D0379BB0ACA99043 build 69

extern UI* pUI;
extern Game* pGame;
extern AudioStream* pAudioStream;

int iNetModeNormalOnFootSendRate = 30;
int iNetModeNormalInCarSendRate = 30;
int iNetModeFiringSendRate = 30;
int iNetModeSendMultiplier = 2;

void RegisterRPCs(RakClientInterface* pRakClient);
void UnregisterRPCs(RakClientInterface* pRakClient);
void RegisterScriptRPCs(RakClientInterface* pRakClient);
void UnregisterScriptRPCs(RakClientInterface* pRakClient);

void InstallVehicleEngineLightPatches();

unsigned char GetPacketID(Packet* p)
{
	if (p == 0) return 255;

	if ((unsigned char) p->data[0] == ID_TIMESTAMP) {
		return (unsigned char) p->data[sizeof(unsigned char) + sizeof(unsigned long)];
	}
	else {
		return (unsigned char) p->data[0];
	}
}

static bool cefSendPacketImpl(const uint8_t* data, size_t len, bool highPriority, bool reliable)
{
	extern NetGame* pNetGame; // atau akses m_rakClient sesuai struktur project kamu
	if (!pNetGame) return false;

	RakNet::BitStream bs((unsigned char*) data, len, false);
	return pNetGame->GetRakClient()->Send(&bs,
	                                      highPriority ? HIGH_PRIORITY : LOW_PRIORITY,
	                                      reliable ? RELIABLE : UNRELIABLE, 0);
}

NetGame::NetGame(const char* host, int port, const char* playerName, const char* password)
{
	spdlog::info("NetGame initializing..");

	m_netSet = new NET_SETTINGS;
	memset(m_hostName, 0, 256);
	memset(m_host, 0, 256);

	strcpy(m_hostName, "SA-MP");
	strncpy(m_host, host, sizeof(m_host));
	m_port = port;

	m_rakClient = RakNetworkFactory::GetRakClientInterface();
	InitializePools();

	GetPlayerPool()->SetLocalPlayerName(playerName);

	RegisterRPCs(m_rakClient);
	RegisterScriptRPCs(m_rakClient);
	m_rakClient->SetPassword(password);

	cef::initNetwork(m_rakClient, ID_CEF_SYNC);
	cef::setSendCallback(cefSendPacketImpl);

	if (Settings::voice()) {
		spdlog::info("NetGame::NetGame: voice connect {}:{}", host, port);
		Network::OnRaknetConnect(host, port);
	}

	memset(m_mapIcon, 0, sizeof(m_mapIcon));

	pGame->EnableClock(false);
	pGame->EnableZoneNames(false);

	m_netSet->deathDropMoney = 0;
	m_netSet->spawnsAvailable = 0;
	m_netSet->nameTagLOS = 0;
	m_netSet->holdTime = true;
	m_netSet->useCJWalk = 0;
	m_netSet->disableInteriorEnterExits = 0;
	m_netSet->zoneNames = false;
	m_netSet->instagib = false;
	m_netSet->nameTagDrawDistance = 60.0f;
	m_netSet->friendlyFire = true;
	m_netSet->worldTime_Hour = 12;
	m_netSet->worldTime_Minute = 0;
	m_netSet->weather = 1;
	m_netSet->gravity = 0.008f;
	m_netSet->disableVehicleCollisions = false;

	m_lastConnectAttempt = GetTickCount();
	SetGameState(GAMESTATE_WAIT_CONNECT);
	m_lanMode = false;

	if (pUI) {
		pUI->chat()->addDebugMessage("{FFFFFF}SA-MP {B9C9BF}" + Client::sampVersion() + " {FFFFFF}Started");
		pUI->chat()->addInfoMessage("-> Use /clientcmds for client commands");
	}
}

NetGame::~NetGame()
{
	m_rakClient->Disconnect(0);
	UnregisterRPCs(m_rakClient);
	UnregisterScriptRPCs(m_rakClient);
	RakNetworkFactory::DestroyRakClientInterface(m_rakClient);

	UninitializePools();

	if (Settings::voice()) {
		Network::OnRaknetDisconnect();
	}

	if (m_netSet) {
		delete m_netSet;
		m_netSet = nullptr;
	}
}

void NetGame::InitializePools()
{
	m_pools = new NET_POOLS;
	m_pools->playerPool = new CPlayerPool();
	m_pools->vehiclePool = new CVehiclePool();
	m_pools->objectPool = new CObjectPool();
	m_pools->actorPool = new CActorPool();
	m_pools->pickupPool = new CPickupPool();
	m_pools->textLabelPool = new CTextLabelPool();
	m_pools->textDrawPool = new CTextDrawPool();
	m_pools->playerBubblePool = new CPlayerBubblePool();
	m_pools->gangZonePool = new CGangZonePool();
	m_pools->menuPool = new CMenuPool();
}

void NetGame::UninitializePools()
{
	if (m_pools->playerPool) {
		delete m_pools->playerPool;
		m_pools->playerPool = nullptr;
	}

	if (m_pools->vehiclePool) {
		delete m_pools->vehiclePool;
		m_pools->vehiclePool = nullptr;
	}

	if (m_pools->objectPool) {
		delete m_pools->objectPool;
		m_pools->objectPool = nullptr;
	}

	if (m_pools->actorPool) {
		delete m_pools->actorPool;
		m_pools->actorPool = nullptr;
	}

	if (m_pools->pickupPool) {
		delete m_pools->pickupPool;
		m_pools->pickupPool = nullptr;
	}

	if (m_pools->textLabelPool) {
		delete m_pools->textLabelPool;
		m_pools->textLabelPool = nullptr;
	}

	if (m_pools->textDrawPool) {
		delete m_pools->textDrawPool;
		m_pools->textDrawPool = nullptr;
	}

	if (m_pools->playerBubblePool) {
		delete m_pools->playerBubblePool;
		m_pools->playerBubblePool = nullptr;
	}

	if (m_pools->gangZonePool) {
		delete m_pools->gangZonePool;
		m_pools->gangZonePool = nullptr;
	}

	if (m_pools->menuPool) {
		delete m_pools->menuPool;
		m_pools->menuPool = nullptr;
	}

	if (m_pools) {
		delete m_pools;
		m_pools = nullptr;
	}

	if (m_netSet) {
		delete m_netSet;
		m_netSet = nullptr;
	}
}

void NetGame::Process()
{
	UpdateNetwork();
	if (m_netSet->holdTime) {
		pGame->SetWorldTime(m_netSet->worldTime_Hour, m_netSet->worldTime_Minute);
	}

	pGame->PreloadObjectsAnims();

	if (GetGameState() == GAMESTATE_CONNECTED) {
		ProcessPools();
	}
	else {
		ProcessLoadingScreen();
	}

	if (GetGameState() == GAMESTATE_WAIT_CONNECT) {
		ProcessConnecting();
	}
}

void NetGame::UpdateNetwork()
{
	Packet* pkt = nullptr;
	unsigned char packetIdentifier;
	while ((pkt = m_rakClient->Receive())) {
		packetIdentifier = GetPacketID(pkt);

		switch (packetIdentifier) {
			case ID_AUTH_KEY: Packet_AuthKey(pkt);
				break;

			case ID_CONNECTION_ATTEMPT_FAILED: Packet_ConnectAttemptFailed(pkt);
				break;

			case ID_NO_FREE_INCOMING_CONNECTIONS: Packet_NoFreeIncomingConnections(pkt);
				break;

			case ID_DISCONNECTION_NOTIFICATION: Packet_DisconnectionNotification(pkt);
				SetGameState(GAMESTATE_WAIT_CONNECT);
				break;

			case ID_CONNECTION_LOST: Packet_ConnectionLost(pkt);
				break;

			case ID_CONNECTION_REQUEST_ACCEPTED: Packet_ConnectionSucceeded(pkt);
				break;

			case ID_FAILED_INITIALIZE_ENCRIPTION: Packet_FailedInitializeEncription(pkt);
				break;

			case ID_CONNECTION_BANNED: Packet_ConnectionBanned(pkt);
				SetGameState(GAMESTATE_WAIT_CONNECT);
				break;

			case ID_INVALID_PASSWORD: Packet_InvalidPassword(pkt);
				break;

			case ID_VEHICLE_SYNC: Packet_VehicleSync(pkt);
				break;

			case ID_AIM_SYNC: Packet_AimSync(pkt);
				break;

			case ID_BULLET_SYNC: Packet_BulletSync(pkt);
				break;

			case ID_PLAYER_SYNC: Packet_PlayerSync(pkt);
				break;

			case ID_MARKERS_SYNC: Packet_MarkerSync(pkt);
				break;

			case ID_UNOCCUPIED_SYNC: Packet_UnoccupiedSync(pkt);
				break;

			case ID_TRAILER_SYNC: Packet_TrailerSync(pkt);
				break;

			case ID_PASSENGER_SYNC: Packet_PassengerSync(pkt);
				break;

			case ID_VOICE_SYNC:
				if (Settings::voice()) {
					Network::OnRaknetReceive(pkt);
				}
				break;

			case ID_CEF_SYNC:
				cef::handlePacket(pkt);
				break;
		}

		m_rakClient->DeallocatePacket(pkt);
	}
}

void NetGame::ShutdownForGameModeRestart()
{
	for (PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++) {
		CRemotePlayer* pRemotePlayer = GetPlayerPool()->GetAt(playerId);
		if (pRemotePlayer) {
			pRemotePlayer->SetTeam(NO_TEAM);
			pRemotePlayer->ResetAllSyncAttributes();
		}
	}

	GetPlayerPool()->GetLocalPlayer()->ResetAllSyncAttributes();
	GetPlayerPool()->GetLocalPlayer()->ToggleSpectating(false);
	GameResetStats();

	if (pAudioStream) {
		pAudioStream->Stop(true);
	}

	SetGameState(GAMESTATE_RESTARTING);
	GetPlayerPool()->DeactivateAll();
	GetPlayerPool()->Process();
	ResetVehiclePool();
	ResetActorPool();
	ResetTextDrawPool();
	ResetGangZonePool();
	ResetTextLabelPool();
	ResetMapIcons();
	ResetPickupPool();
	ResetObjectPool();
	ResetMenuPool();

	m_netSet->disableInteriorEnterExits = false;
	m_netSet->nameTagDrawDistance = 60.0f;
	m_netSet->worldTime_Hour = 12;
	m_netSet->worldTime_Minute = 0;
	m_netSet->weather = 1;
	m_netSet->holdTime = true;
	m_netSet->nameTagLOS = true;
	m_netSet->useCJWalk = false;
	m_netSet->gravity = 0.008f;
	m_netSet->deathDropMoney = 0;
	m_netSet->disableVehicleCollisions = false;

	pGame->EnableCheckpoints(false);
	pGame->EnableRaceCheckpoints(false);

	pGame->FindPlayerPed()->SetInterior(0, true);
	pGame->ResetLocalMoney();
	pGame->FindPlayerPed()->SetDead();
	pGame->FindPlayerPed()->SetArmour(0.0f);
	pGame->EnableZoneNames(false);
	m_netSet->zoneNames = false;
	GameResetRadarColors();
	pGame->SetGravity(m_netSet->gravity);
	pGame->SetWantedLevel(0);
	pGame->EnableClock(false);

	if (Settings::voice()) {
		Network::OnRaknetDisconnect();
		SpeakerList::Hide();
	}

	if (pUI) {
		pUI->chat()->addInfoMessage("The server is restarting..");
	}
}

void NetGame::ProcessPools()
{
	if (GetPlayerPool()) {
		GetPlayerPool()->Process();
	}

	static int iVehiclePoolProcessFlag = 0;
	if (GetVehiclePool() && iVehiclePoolProcessFlag > 2) {
		GetVehiclePool()->Process();
		iVehiclePoolProcessFlag = 0;
	}
	else {
		iVehiclePoolProcessFlag++;
	}

	static int iPickupPoolProcessFlag = 0;
	if (GetPickupPool() && iPickupPoolProcessFlag > 5) {
		GetPickupPool()->Process();
		iPickupPoolProcessFlag = 0;
	}
	else {
		iPickupPoolProcessFlag++;
	}
}

void NetGame::ProcessLoadingScreen()
{
	CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
	if (pPlayerPed->IsInVehicle()) {
		pPlayerPed->RemoveFromVehicleAndPutAt(1093.4, -2036.5, 82.710602);
	}
	else {
		pPlayerPed->TeleportTo(1133.0504, -2038.4034, 69.099998);
	}

	pGame->GetCamera()->SetPosition(1093.0, -2036.0, 90.0, 0.0, 0.0, 0.0);
	pGame->GetCamera()->LookAtPoint(384.0, -1557.0, 20.0, 2);
	pGame->SetWorldWeather(1);
	pGame->DisplayHUD(false);
}

void NetGame::ProcessConnecting()
{
	if ((GetTickCount() - m_lastConnectAttempt) > 1000) {
		if (pUI) {
			pUI->chat()->addDebugMessage("Connecting to %s:%d...", m_host, m_port);
		}

		m_rakClient->Connect(m_host, m_port, 0, 0, 2);

		if (Settings::voice()) {
			spdlog::info("NetGame::ProcessConnecting: voice connect {}:{}", m_host, m_port);
			Network::OnRaknetConnect(m_host, m_port);
		}

		m_lastConnectAttempt = GetTickCount();
		SetGameState(GAMESTATE_CONNECTING);
	}
}

void gen_auth_key(char buf[260], char* auth_in);
char* findAuthKey(const char* key)
{
	for (auto& i : AuthKeyTable) {
		if (strcmp(i[0], key) == 0) {
			return i[1];
		}
	}
	return nullptr; // Return null if key is not found
}

/*void NetGame::Packet_AuthKey(Packet* pkt)
{
	uint8_t byteAuthLen;
	char szAuth[260], szAuthKey[269];

	RakNet::BitStream bsAuth((unsigned char*) pkt->data, pkt->length, false);

	if (GetGameState() < GAMESTATE_WAIT_CONNECT || GetGameState() > GAMESTATE_AWAIT_JOIN) return;

	bsAuth.IgnoreBits(8);
	bsAuth.Read(byteAuthLen);
	bsAuth.Read(szAuth, byteAuthLen);
	szAuth[byteAuthLen] = '\0';

#if VER_x32
	gen_auth_key(szAuthKey, szAuth);
#else
	char* key = findAuthKey(szAuth);
	if (key) {
		strcpy(szAuthKey, key);
	}
	else {
		spdlog::info("cannot get auth key: {}", szAuth);
	}
#endif

	uint8_t byteAuthKeyLen = strlen(szAuthKey);

	RakNet::BitStream bsKey;
	bsKey.Write((uint8_t) ID_AUTH_KEY);
	bsKey.Write(byteAuthKeyLen);
	bsKey.Write(szAuthKey, byteAuthKeyLen);
	m_rakClient->Send(&bsKey, SYSTEM_PRIORITY, RELIABLE, 0);

	spdlog::info("[AUTH] {} -> {}", szAuth, szAuthKey);
}*/

void NetGame::Packet_AuthKey(Packet* pkt)
{
	uint8_t byteAuthLen;
	char szAuth[260];

	RakNet::BitStream bsAuth((unsigned char*) pkt->data, pkt->length, false);

	if (GetGameState() < GAMESTATE_WAIT_CONNECT || GetGameState() > GAMESTATE_AWAIT_JOIN) {
		return;
	}

	bsAuth.IgnoreBits(8);
	bsAuth.Read(byteAuthLen);
	bsAuth.Read(szAuth, byteAuthLen);
	szAuth[byteAuthLen] = '\0';

	char* auth_key;
	bool found_key = false;

	for (auto& x : AuthKeyTable) {
		if (!strcmp(szAuth, x[0])) {
			auth_key = x[1];
			found_key = true;
		}
	}

	if (found_key) {
		RakNet::BitStream bsKey;
		uint8_t byteAuthKeyLen;

		byteAuthKeyLen = (uint8_t) strlen(auth_key);

		bsKey.Write((uint8_t) ID_AUTH_KEY);
		bsKey.Write((uint8_t) byteAuthKeyLen);
		bsKey.Write(auth_key, byteAuthKeyLen);

		m_rakClient->Send(&bsKey, SYSTEM_PRIORITY, RELIABLE, NULL);

		spdlog::info("[AUTH] {} -> {}", szAuth, auth_key);
	}
	else {
		spdlog::error("Unknown AUTH_IN! ({})", ((char*) pkt->data + 2));
	}
}

void NetGame::Packet_ConnectAttemptFailed(Packet* pkt)
{
	if (pUI) {
		pUI->chat()->addDebugMessage("The server didn't respond. Retrying..");
	}

	if (pAudioStream) {
		pAudioStream->Stop(true);
	}

	if (Settings::voice()) {
		SpeakerList::Hide();
	}

	SetGameState(GAMESTATE_WAIT_CONNECT);
}

void NetGame::Packet_NoFreeIncomingConnections(Packet* pkt)
{
	if (pUI) {
		pUI->chat()->addDebugMessage("The server is full. Retrying..");
	}

	if (Settings::voice()) {
		SpeakerList::Hide();
	}

	SetGameState(GAMESTATE_WAIT_CONNECT);
}

void NetGame::Packet_DisconnectionNotification(Packet* pkt)
{
	if (pUI) {
		pUI->chat()->addDebugMessage("Server closed the connection.");
	}

	if (pAudioStream) {
		pAudioStream->Stop(true);
	}

	if (Settings::voice()) {
		SpeakerList::Hide();
	}

	m_rakClient->Disconnect(2000);
}

void NetGame::Packet_ConnectionSucceeded(Packet* pkt)
{
	RakNet::BitStream bsSuccAuth(pkt->data, pkt->length, true);
	PLAYERID MyPlayerID;
	unsigned int uiChallenge;
	int iVersion = 0xFD9;
	uint8_t byteMod = 1;

	bsSuccAuth.IgnoreBits(8);    // packetId
	bsSuccAuth.IgnoreBits(32);    // binaryAddress
	bsSuccAuth.IgnoreBits(16);    // port

	bsSuccAuth.Read(MyPlayerID);
	bsSuccAuth.Read(uiChallenge);
	uiChallenge ^= iVersion;

	if (pUI) {
		pUI->chat()->addDebugMessage("Connected. Joining the game...");
	}

	SetGameState(GAMESTATE_AWAIT_JOIN);

	uint8_t byteNameLen = strlen(GetPlayerPool()->GetLocalPlayerName());
	uint8_t byteAuthBSLen = strlen(AUTH_BS);
	uint8_t byteClientVerLen = strlen(Client::sampVersion().c_str());

	RakNet::BitStream bsSend;
	bsSend.Write(iVersion);
	bsSend.Write(byteMod);
	bsSend.Write(byteNameLen);
	bsSend.Write(GetPlayerPool()->GetLocalPlayerName(), byteNameLen);
	bsSend.Write(uiChallenge);
	bsSend.Write(byteAuthBSLen);
	bsSend.Write(AUTH_BS, byteAuthBSLen);
	bsSend.Write(byteClientVerLen);
	bsSend.Write(Client::sampVersion().c_str(), byteClientVerLen);

	if (Settings::voice()) {
		Network::OnRaknetRpc(RPC_ClientJoin, bsSend);
		SpeakerList::Hide();
	}

	m_rakClient->RPC(&RPC_ClientJoin, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
	cef::handleServerConnection();
}

void NetGame::Packet_FailedInitializeEncription(Packet* pkt)
{
	if (pUI) {
		pUI->chat()->addDebugMessage("Failed to initialize encryption.");
	}
}

void NetGame::Packet_ConnectionBanned(Packet* pkt)
{
	if (pUI) {
		pUI->chat()->addDebugMessage("You are banned from this server.");
	}
}

void NetGame::Packet_InvalidPassword(Packet* pkt)
{
	if (pUI) {
		pUI->chat()->addDebugMessage("Wrong server password.");
	}
	m_rakClient->Disconnect(0);
}

void NetGame::Packet_ConnectionLost(Packet* pkt)
{
	if (m_rakClient) {
		m_rakClient->Disconnect(0);
	}

	if (pUI) {
		pUI->chat()->addDebugMessage("Lost connection to the server. Reconnecting..");
	}

	ShutdownForGameModeRestart();

	CPlayerPool* pPlayerPool = GetPlayerPool();
	if (pPlayerPool) {
		for (PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++) {
			if (pPlayerPool->GetSlotState(playerId)) {
				pPlayerPool->Delete(playerId, 0);
			}
		}
	}

	if (pAudioStream) {
		pAudioStream->Stop(true);
	}

	SetGameState(GAMESTATE_WAIT_CONNECT);
}

void NetGame::Packet_PlayerSync(Packet* pkt)
{
	RakNet::BitStream bsData(pkt->data, pkt->length, false);
	ONFOOT_SYNC_DATA ofSync;
	uint32_t dwTime = 0;
	uint8_t bytePacketId;
	PLAYERID playerId;

	bool bHasLR, bHasUD;
	bool bHasVehicleSurfingInfo;

	if (GetGameState() != GAMESTATE_CONNECTED) return;

	memset(&ofSync, 0, sizeof(ONFOOT_SYNC_DATA));

	if (pkt->data[0] == ID_TIMESTAMP) {
		bsData.Read(bytePacketId);
		bsData.Read(dwTime);
	}

	bsData.Read(bytePacketId);
	bsData.Read(playerId);

	bsData.Read(bHasLR);
	if (bHasLR) {
		bsData.Read(ofSync.lrAnalog);
	}

	bsData.Read(bHasUD);
	if (bHasUD) {
		bsData.Read(ofSync.udAnalog);
	}

	bsData.Read(ofSync.wKeys);
	bsData.Read((char*) &ofSync.vecPos, sizeof(sa::CVector));
	float w, x, y, z;
	bsData.ReadNormQuat(w, x, y, z);
	ofSync.quat.Set(x, y, z, w);

	uint8_t byteHealthArmour;
	uint8_t byteArmTemp = 0, byteHlTemp = 0;

	bsData.Read(byteHealthArmour);
	byteArmTemp = (byteHealthArmour & 0x0F);
	byteHlTemp = (byteHealthArmour >> 4);
	if (byteArmTemp == 0xF) { ofSync.byteArmour = 100; }
	else if (byteArmTemp == 0) { ofSync.byteArmour = 0; }
	else { ofSync.byteArmour = byteArmTemp * 7; }

	// sync health
	if (byteHlTemp == 0xF) { ofSync.byteHealth = 100; }
	else if (byteHlTemp == 0) { ofSync.byteHealth = 0; }
	else { ofSync.byteHealth = byteHlTemp * 7; }
	uint8_t byteCurrentWeapon = 0;
	bsData.Read(byteCurrentWeapon);
	ofSync.byteCurrentWeapon ^= (byteCurrentWeapon ^ ofSync.byteCurrentWeapon) & 0x3F;

	bsData.Read(ofSync.byteSpecialAction);

	// crash
	// bsData.ReadVector(ofSync.vecMoveSpeed.X, ofSync.vecMoveSpeed.Y, ofSync.vecMoveSpeed.Z);
	bsData.ReadVector(x, y, z);
	ofSync.vecMoveSpeed.x = x;
	ofSync.vecMoveSpeed.y = y;
	ofSync.vecMoveSpeed.z = z;

	bsData.Read(bHasVehicleSurfingInfo);
	if (bHasVehicleSurfingInfo) {
		bsData.Read(ofSync.wSurfInfo);
		bsData.Read(ofSync.vecSurfOffsets.x);
		bsData.Read(ofSync.vecSurfOffsets.y);
		bsData.Read(ofSync.vecSurfOffsets.z);
	}
	else { ofSync.wSurfInfo = INVALID_VEHICLE_ID; }

	bool bHasAnimInfo;
	ofSync.dwAnimation = 0;
	bsData.Read(bHasAnimInfo);
	if (bHasAnimInfo) {
		bsData.Read(ofSync.dwAnimation);
	}
	else {
		ofSync.dwAnimation = 0b10000000000000000000000000000000;
	}
	CRemotePlayer* pRemotePlayer = GetPlayerPool()->GetAt(playerId);
	if (pRemotePlayer) {
		pRemotePlayer->StoreOnFootFullSyncData(&ofSync, dwTime);
	}
}
// vehicle sync
void NetGame::Packet_VehicleSync(Packet* pkt)
{
	RakNet::BitStream bsData(pkt->data, pkt->length, false);
	uint32_t dwTime = 0;
	uint8_t bytePacketId;
	INCAR_SYNC_DATA icSync;
	PLAYERID PlayerID;

	if (GetGameState() != GAMESTATE_CONNECTED) return;

	memset(&icSync, 0, sizeof(INCAR_SYNC_DATA));

	if (pkt->data[0] == ID_TIMESTAMP) {
		bsData.Read(bytePacketId);
		bsData.Read(dwTime);
	}
	// read package id
	bsData.Read(bytePacketId);
	// player id
	bsData.Read(PlayerID);
	// vehicle id
	bsData.Read(icSync.VehicleID);
	bsData.Read(icSync.lrAnalog);
	bsData.Read(icSync.udAnalog);
	bsData.Read(icSync.wKeys);

	float w, x, y, z;
	// read quaternion
	bsData.ReadNormQuat(w, x, y, z);
	// set sync quaternion
	icSync.quat.Set(x, y, z, w);

	bsData.Read((char*) &icSync.vecPos, sizeof(sa::CVector));
	bsData.ReadVector(icSync.vecMoveSpeed.x, icSync.vecMoveSpeed.y, icSync.vecMoveSpeed.z);

	// car health
	uint16_t wTempVehicleHealth;
	bsData.Read(wTempVehicleHealth);
	icSync.fCarHealth = (float) wTempVehicleHealth;

	// health/armour
	uint8_t byteHealthArmour;
	uint8_t byteArmTemp = 0, byteHlTemp = 0;

	bsData.Read(byteHealthArmour);
	byteArmTemp = (byteHealthArmour & 0x0F);
	byteHlTemp = (byteHealthArmour >> 4);

	if (byteArmTemp == 0xF) { icSync.bytePlayerArmour = 100; }
	else if (byteArmTemp == 0) { icSync.bytePlayerArmour = 0; }
	else { icSync.bytePlayerArmour = byteArmTemp * 7; }

	if (byteHlTemp == 0xF) { icSync.bytePlayerHealth = 100; }
	else if (byteHlTemp == 0) { icSync.bytePlayerHealth = 0; }
	else { icSync.bytePlayerHealth = byteHlTemp * 7; }

	// current weapon
	uint8_t byteTempWeapon;
	bsData.Read(byteTempWeapon);
	icSync.byteCurrentWeapon ^= (byteTempWeapon ^ icSync.byteCurrentWeapon) & 0x3F;

	bool bCheck;

	// siren
	bsData.Read(bCheck);
	if (bCheck) icSync.byteSirenOn = 1;
	// landinggear
	bsData.Read(bCheck);
	if (bCheck) icSync.byteLandingGearState = 1;
	// train speed
	bsData.Read(bCheck);
	if (bCheck) bsData.Read(icSync.fTrainSpeed);
	// triler id
	bsData.Read(bCheck);
	if (bCheck) bsData.Read(icSync.TrailerID);

	CRemotePlayer* pRemotePlayer = GetPlayerPool()->GetAt(PlayerID);
	if (pRemotePlayer) {
		pRemotePlayer->StoreInCarFullSyncData(&icSync, dwTime);
	}
}

void NetGame::Packet_AimSync(Packet* pkt)
{
	RakNet::BitStream bsData(pkt->data, pkt->length, false);

	if (GetGameState() == GAMESTATE_CONNECTED) {
		uint8_t bytePacketId;
		PLAYERID PlayerId;
		AIM_SYNC_DATA aimSync;
		bsData.Read(bytePacketId);
		bsData.Read(PlayerId);
		bsData.Read((char*) &aimSync, sizeof(AIM_SYNC_DATA));

		CRemotePlayer* pPlayer = m_pools->playerPool->GetAt(PlayerId);
		if (pPlayer) {
			pPlayer->StoreAimFullSyncData(&aimSync);
		}
	}
}

void NetGame::Packet_BulletSync(Packet* pkt)
{
	RakNet::BitStream bsData(pkt->data, pkt->length, false);

	if (GetGameState() != GAMESTATE_CONNECTED) return;

	BULLET_SYNC_DATA btSync;
	uint8_t bytePacketId = 0;
	PLAYERID PlayerID = 0;

	bsData.Read(bytePacketId);
	bsData.Read(PlayerID);
	bsData.Read((char*) &btSync, sizeof(BULLET_SYNC_DATA));

	CPlayerPool* pPlayerPool = GetPlayerPool();
	CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(PlayerID);
	if (pRemotePlayer) {
		pRemotePlayer->StoreBulletFullSyncData(&btSync);
	}
}

void NetGame::Packet_PassengerSync(Packet* pkt)
{
	RakNet::BitStream bsData(pkt->data, pkt->length, false);

	if (GetGameState() == GAMESTATE_CONNECTED) {
		uint8_t bytePacketId;
		PLAYERID PlayerID;
		PASSENGER_SYNC_DATA psSync;

		bsData.Read(bytePacketId);
		bsData.Read(PlayerID);
		bsData.Read((char*) &psSync, sizeof(PASSENGER_SYNC_DATA));

		CRemotePlayer* pRemotePlayer = GetPlayerPool()->GetAt(PlayerID);
		if (pRemotePlayer) {
			pRemotePlayer->StorePassengerFullSyncData(&psSync);
		}
	}
}

void NetGame::Packet_MarkerSync(Packet* pkt)
{
	uint8_t bytePacketId = 0;
	int iNumberOfPlayers = 0;
	PLAYERID playerId;
	bool bIsPlayerActive;
	short sPosX, sPosY, sPosZ;
	CPlayerPool* pPlayerPool = GetPlayerPool();
	RakNet::BitStream bsData(pkt->data, pkt->length, false);

	if (GetGameState() != GAMESTATE_CONNECTED) return;

	bsData.Read(bytePacketId);
	bsData.Read(iNumberOfPlayers);

	if (iNumberOfPlayers) {
		for (int i = 0; i < iNumberOfPlayers; i++) {
			bsData.Read(playerId);
			bsData.ReadCompressed(bIsPlayerActive);
			if (bIsPlayerActive) {
				bsData.Read(sPosX);
				bsData.Read(sPosY);
				bsData.Read(sPosZ);
			}
			if (playerId < MAX_PLAYERS && pPlayerPool->GetSlotState(playerId)) {
				CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(playerId);
				if (pRemotePlayer) {
					if (bIsPlayerActive) {
						pRemotePlayer->ShowGlobalMarker(sPosX, sPosY, sPosZ);
					}
					else {
						pRemotePlayer->HideGlobalMarker();
					}
				}
			}

		}
	}
}

void NetGame::Packet_TrailerSync(Packet* pkt)
{
	RakNet::BitStream bsTrailerSync((unsigned char*) pkt->data, pkt->length, false);

	if (GetGameState() != GAMESTATE_CONNECTED) return;

	uint8_t bytePacketID = 0;
	PLAYERID playerId;

	TRAILER_SYNC_DATA trSync;
	memset(&trSync, 0, sizeof(TRAILER_SYNC_DATA));

	bsTrailerSync.Read(bytePacketID);
	bsTrailerSync.Read(playerId);
	bsTrailerSync.Read((char*) &trSync, sizeof(TRAILER_SYNC_DATA));

	if (GetPlayerPool()) {
		CRemotePlayer* pPlayer = GetPlayerPool()->GetAt(playerId);
		if (pPlayer) {
			pPlayer->StoreTrailerFullSyncData(&trSync);
		}
	}
}

void NetGame::Packet_UnoccupiedSync(Packet* pkt)
{
	RakNet::BitStream bsUnocSync((unsigned char*) pkt->data, pkt->length, false);

	if (GetGameState() != GAMESTATE_CONNECTED) return;

	uint8_t bytePacketID = 0;
	PLAYERID playerId;

	UNOCCUPIED_SYNC_DATA unocSync;
	memset(&unocSync, 0, sizeof(UNOCCUPIED_SYNC_DATA));

	bsUnocSync.Read(bytePacketID);
	bsUnocSync.Read(playerId);

	bsUnocSync.Read((char*) &unocSync, sizeof(UNOCCUPIED_SYNC_DATA));

//	if (GetPlayerPool())
//	{
//		CRemotePlayer* pPlayer = GetPlayerPool()->GetAt(playerId);
//		if (pPlayer)
//			pPlayer->StoreUnoccupiedFullSyncData(&unocSync);
//	}
}

void NetGame::UpdatePlayerScoresAndPings()
{
	static uint32_t dwLastUpdateTick = 0;

	if (GetTickCount() - dwLastUpdateTick > 3000) {
		dwLastUpdateTick = GetTickCount();

		RakNet::BitStream bsSend;
		m_rakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false,
				UNASSIGNED_NETWORK_ID, nullptr);
	}
}

void NetGame::SendDialogResponse(uint16_t wDialogID, uint8_t byteButtonID, uint16_t wListBoxItem, const char* szInput)
{
	if (GetGameState() != GAMESTATE_CONNECTED) return;

	uint8_t length = strlen(szInput);

	RakNet::BitStream bsSend;
	bsSend.Write(wDialogID);
	bsSend.Write(byteButtonID);
	bsSend.Write(wListBoxItem);
	bsSend.Write(length);
	bsSend.Write(szInput, length);
	m_rakClient->RPC(&RPC_DialogResponse, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}

void NetGame::SendChatMessage(const char* szMsg)
{
	if (GetGameState() != GAMESTATE_CONNECTED) return;

	RakNet::BitStream bsSend;
	uint8_t byteTextLen = strlen(szMsg);

	bsSend.Write(byteTextLen);
	bsSend.Write(szMsg, byteTextLen);

	m_rakClient->RPC(&RPC_Chat, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID,
			NULL);
}

void NetGame::SendChatCommand(const char* szCommand)
{
	if (GetGameState() != GAMESTATE_CONNECTED) return;

	RakNet::BitStream bsParams;
	int iStrlen = strlen(szCommand);

	bsParams.Write(iStrlen);
	bsParams.Write(szCommand, iStrlen);
	m_rakClient->RPC(&RPC_ServerCommand, &bsParams, HIGH_PRIORITY, RELIABLE, 0, false,
			UNASSIGNED_NETWORK_ID, NULL);
}

void NetGame::SetMapIcon(uint8_t byteIconID, float fPosX, float fPosY, float fPosZ, uint8_t byteType, uint32_t dwColor, uint8_t byteStyle)
{
	if (byteIconID < MAX_MAP_ICONS) {
		if (m_mapIcon[byteIconID] != 0) {
			DisableMapIcon(byteIconID);
		}

		m_mapIcon[byteIconID] = pGame->CreateRadarMarkerIcon(byteType, fPosX, fPosY, fPosZ, dwColor, byteStyle);
	}
}

void NetGame::DisableMapIcon(uint8_t byteIconID)
{
	if (byteIconID < MAX_MAP_ICONS) {
		ScriptCommand(&disable_marker, m_mapIcon[byteIconID]);
		m_mapIcon[byteIconID] = 0;
	}
}

void NetGame::ResetVehiclePool()
{
	if (m_pools->vehiclePool) {
		delete m_pools->vehiclePool;
	}

	m_pools->vehiclePool = new CVehiclePool();
}

void NetGame::ResetActorPool()
{
	if (m_pools->actorPool) {
		delete m_pools->actorPool;
	}

	m_pools->actorPool = new CActorPool();
}

void NetGame::ResetTextDrawPool()
{
	if (m_pools->textDrawPool) {
		delete m_pools->textDrawPool;
	}

	m_pools->textDrawPool = new CTextDrawPool();
}

void NetGame::ResetGangZonePool()
{
	if (m_pools->gangZonePool) {
		delete m_pools->gangZonePool;
	}

	m_pools->gangZonePool = new CGangZonePool();
}

void NetGame::ResetTextLabelPool()
{
	if (m_pools->textLabelPool) {
		delete m_pools->textLabelPool;
	}

	m_pools->textLabelPool = new CTextLabelPool();
}

void NetGame::ResetMapIcons()
{
	for (unsigned int& i : m_mapIcon) {
		if (i) {
			ScriptCommand(&disable_marker, i);
			i = 0;
		}
	}
}

void NetGame::ResetPickupPool()
{
	if (m_pools->pickupPool) {
		delete m_pools->pickupPool;
	}

	m_pools->pickupPool = new CPickupPool();
}

void NetGame::ResetObjectPool()
{
	if (m_pools->objectPool) {
		delete m_pools->objectPool;
	}

	m_pools->objectPool = new CObjectPool();
}

void NetGame::ResetMenuPool()
{
	if (m_pools->menuPool) {
		delete m_pools->menuPool;
	}

	m_pools->menuPool = new CMenuPool();
}

void NetGame::InitGameLogic()
{
	if (m_netSet->manualVehicleEngineAndLight) {
		InstallVehicleEngineLightPatches();
	}

	m_netSet->worldBounds[0] = 20000.0f;
	m_netSet->worldBounds[1] = -20000.0f;
	m_netSet->worldBounds[2] = 20000.0f;
	m_netSet->worldBounds[3] = -20000.0f;
}

void NetGame::SendPrevClass()
{
	CPlayerPool* pPlayerPool = GetPlayerPool();
	if (pPlayerPool) {
		pPlayerPool->GetLocalPlayer()->SendPrevClass();
	}
}

void NetGame::SendSpawn()
{
	CPlayerPool* pPlayerPool = GetPlayerPool();
	if (pPlayerPool) {
		pPlayerPool->GetLocalPlayer()->SendSpawn();
	}
}

void NetGame::SendNextClass()
{
	CPlayerPool* pPlayerPool = GetPlayerPool();
	if (pPlayerPool) {
		pPlayerPool->GetLocalPlayer()->SendNextClass();
	}
}