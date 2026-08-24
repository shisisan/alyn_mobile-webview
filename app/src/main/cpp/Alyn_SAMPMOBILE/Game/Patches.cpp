#include "Game.h"
#include "../Client.h"
#include "VehicleColoursTable.h"

sa::CPlayerInfo* WORLD_PLAYERS = nullptr;

void Patches::preApply()
{
	spdlog::info("Applying FPS patches...");

	// fps limit
#if VER_x32
	uint8_t fps = 0x78; // 120
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x5E49E0), (uintptr_t) &fps, 1);
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x5E492E), (uintptr_t) &fps, 1);
#else
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x70A38C), (uintptr_t) "\x09\x0F\x80\x52", 4);
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x70A43C), (uintptr_t) "\x08\x0F\x80\x52", 4);
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x70A458), (uintptr_t) "\x09\x0F\x80\x52", 4);
#endif

	// show fps
	//*(bool*) (g_saSym->GetSymbol<uintptr_t>("FrontEndMenuManager") + (VER_x32 ? 0xB5 : 0xC1)) = true;

	// Set max stats
	sa::CCheat::WeaponSkillsCheat();
	sa::CCheat::VehicleSkillsCheat();
	sa::CCheat::StaminaCheat();
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN6CStats12SetStatValueEtf")); // CStats::SetStatValue
}

void Patches::allocPlayers()
{
	spdlog::info("Allocating world players...");

	// relocate CWorld::Players[]
	WORLD_PLAYERS = (sa::CPlayerInfo*) malloc(404 * PLAYER_PED_SLOTS);
	memset(WORLD_PLAYERS, 0, 404 * PLAYER_PED_SLOTS);

	Memory::protectAddr(g_saSym->Abs(addr::_ZN6CWorld7PlayersE_ptr));
	*g_saSym->Abs<sa::CPlayerInfo**>(addr::_ZN6CWorld7PlayersE_ptr) = WORLD_PLAYERS;

	spdlog::info("CWorld::Players new address: 0x{:X}, old address: 0x{:X}, size: {}", (uintptr_t) WORLD_PLAYERS, (uintptr_t) g_saSym->GetSymbol("_ZN6CWorld7PlayersE"), 404 * PLAYER_PED_SLOTS);

}
void Patches::initBetterPoolsSize()
{
	spdlog::info("Initializing better pools size...");

	typedef sa::CPool<void> CPool;

#if VER_x32
	// 600000 / 75000 = 8
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools25ms_pPtrNodeSingleLinkPoolE") = new CPool(100000, 8);
	// 72000 / 6000 = 12
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools25ms_pPtrNodeDoubleLinkPoolE") = new CPool(20000, 12);
	// 10000 / 500 = 20
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools21ms_pEntryInfoNodePoolE") = new CPool(20000, 20);
	// 279440 / 140 = 1996
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools11ms_pPedPoolE") = new CPool(240, 1996);
	// 286440 / 110 = 2604
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools15ms_pVehiclePoolE") = new CPool(2000, 2604);
	// 840000 / 14000 = 60
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools16ms_pBuildingPoolE") = new CPool(20000, 60);
	// 147000 / 350 = 420
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools14ms_pObjectPoolE") = new CPool(3000, 420);
	// 210000 / 3500 = 60
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools13ms_pDummyPoolE") = new CPool(40000, 60);
	// 487200 / 10150 = 48
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools16ms_pColModelPoolE") = new CPool(50000, 48);
	// 64000 / 500 = 128
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools12ms_pTaskPoolE") = new CPool(5000, 128);
	// 13600 / 200 = 68
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools13ms_pEventPoolE") = new CPool(1000, 68);
	// 6400 / 64 = 100
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools18ms_pPointRoutePoolE") = new CPool(200, 100);
	// 13440 / 32 = 420
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools19ms_pPatrolRoutePoolE") = new CPool(200, 420);
	// 2304 / 64 = 36
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools17ms_pNodeRoutePoolE") = new CPool(200, 36);
	// 512 / 16 = 32
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools21ms_pTaskAllocatorPoolE") = new CPool(3000, 32);
	// 92960 / 140 = 664
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools23ms_pPedIntelligencePoolE") = new CPool(240, 664);
	// 15104 / 64 = 236
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools20ms_pPedAttractorPoolE") = new CPool(200, 236);
#else
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools25ms_pPtrNodeSingleLinkPoolE") = new CPool(100000, 18); // 1200000/7500 ==18
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools25ms_pPtrNodeDoubleLinkPoolE") = new CPool(20000, 24);  // 144000/6000 ==24
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools21ms_pEntryInfoNodePoolE") = new CPool(20000, 40); // 20000/500 == 40
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools11ms_pPedPoolE") = new CPool(240, 2504); // 350560/140==2504
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools15ms_pVehiclePoolE") = new CPool(2000, 3176); // 349360/110==3176
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools16ms_pBuildingPoolE") = new CPool(20000, 960); // 1344000/14000==960
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools14ms_pObjectPoolE") = new CPool(3000, 544); // 190400/350==544
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools13ms_pDummyPoolE") = new CPool(40000, 96); // 336000/3500==96
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools16ms_pColModelPoolE") = new CPool(50000, 56); // 568400/10150==56
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools12ms_pTaskPoolE") = new CPool(5000, 168); // 84000/500==168
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools13ms_pEventPoolE") = new CPool(1000, 88); // 17600/200==88
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools18ms_pPointRoutePoolE") = new CPool(200, 100); // 6400/64==100
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools19ms_pPatrolRoutePoolE") = new CPool(200, 420); // 13440/32==420
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools17ms_pNodeRoutePoolE") = new CPool(200, 36); // 2304/64==36
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools21ms_pTaskAllocatorPoolE") = new CPool(3000, 48); // 768/16==48
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools23ms_pPedIntelligencePoolE") = new CPool(240, 1088); // 152320/140==1088
	*g_saSym->GetSymbol<CPool**>("_ZN6CPools20ms_pPedAttractorPoolE") = new CPool(200, 236); // 15104/64==236
#endif
}

void Patches::apply()
{
	spdlog::info("Applying patches...");

	allocPlayers();

	// Menu_SwitchOffToGame call in StartGameScreen::OnNewGameCheck
	Memory::nop(g_saSym->Abs<uintptr_t>(addr::Menu_SwitchOffToGame_StartGameScreen_OnNewGameCheck), 2);

	// Reveal all zones
	Memory::writeMemory(g_saSym->GetSymbol<uintptr_t>("_ZN9CTheZones12ZonesVisitedE"), (uintptr_t) "\x01", 100); // CTheZones::ZonesVisited[10][10]
	*g_saSym->GetSymbol<int*>("_ZN9CTheZones13ZonesRevealedE") = 100; // CTheZones::ZonesRevealed

	// RwCameraEndUpdate (for Idle hook)
	Memory::nop(g_saSym->Abs<uintptr_t>(addr::RwCameraEndUpdate_patch), 2);

	if (Client::offlineMode()) {
		return;
	}

	// DefaultPCSaveFileName
	Memory::writeMemory(g_saSym->GetSymbol<uintptr_t>("DefaultPCSaveFileName"), (uintptr_t) "GTASAMP", 8);

	// Disable update time
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN6CClock6UpdateEv")); // CClock::Update

	// Disable update weather
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN8CWeather6UpdateEv")); // CWeather::Update

	// No message print
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN4CHud14SetHelpMessageEPKcPtbbbj")); // CHud::SetHelpMessage
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN4CHud24SetHelpMessageStatUpdateEhtff")); // CHud::SetHelpMessageStatUpdate
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN4CHud23DrawBustedWastedMessageEv")); // CHud::DrawBustedWastedMessage

	// Disable cheat
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN6CCheat16ProcessCheatMenuEv")); // CCheat::ProcessCheatMenu
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN6CCheat13ProcessCheatsEv")); // CCheat::ProcessCheats
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN6CCheat16AddToCheatStringEc")); // CCheat::AddToCheatString
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN6CCheat8DoCheatsEv")); // CCheat::DoCheats

	// fix alpha bug in _rwOpenGLRasterCreate
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(addr::_rwOpenGLRasterCreate_patch), !VER_x32 ? (uintptr_t) "\x22\x00\x80\x52" : (uintptr_t) "\x01\x22", !VER_x32 ? 4 : 2);

	// fix texture animation
	*g_saSym->GetSymbol<bool*>("RunUVAnim") = true;

	// fix black markers
#if VER_x32
	// CRadar::Draw3dMarkers (translate color)
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x442120), (uintptr_t) "\x2C\xE0", 2); // B 0x44217C
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x44217C), (uintptr_t) "\x30\x46", 2); // mov r0, r6

	// CRadar::DrawEntityBlip (translate color)
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x4404C0), (uintptr_t) "\x3A\xE0", 2); // B 0x440538
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x440538), (uintptr_t) "\x30\x46", 2); // mov r0, r6

	// CRadar::DrawCoordBlip (translate color)
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x43FB5E), (uintptr_t) "\x12\xE0", 2); // B 0x43FB86
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x43FB86), (uintptr_t) "\x48\x46", 2); // mov r0, r9
#else
	// CRadar::Draw3dMarkers (translate color)
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x52737C), (uintptr_t) "\x1E\x00\x00\x14", 4); // B 0x5273F4
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x5273F4), (uintptr_t) "\xE1\x03\x14\x2A", 4); // mov w1, w20

	// CRadar::DrawEntityBlip (translate color)
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x5258D8), (uintptr_t) "\x22\x00\x00\x14", 4); // B 0x525960
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x525960), (uintptr_t) "\xE1\x03\x16\x2A", 4); // mov w1, W22

	// CRadar::DrawCoordBlip (translate color)
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x524F58), (uintptr_t) "\xCC\xFF\xFF\x17", 4); // B 0x524E88
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(0x524E88), (uintptr_t) "\xE1\x03\x16\x2A", 4); // mov w1, W22
#endif

	// crash fix
	Memory::nop(g_saSym->Abs<uintptr_t>(addr::CPlayerPed_ProcessAnimGroups_patch), 2); // CPlayerPed::ProcessAnimGroups

	// CRealTimeShadowManager::ReturnRealTimeShadow crash fix
	Memory::nop(g_saSym->Abs<uintptr_t>(addr::CRealTimeShadowManager_ReturnRealTimeShadow_patch1), 2);
	Memory::nop(g_saSym->Abs<uintptr_t>(addr::CRealTimeShadowManager_ReturnRealTimeShadow_patch2), 2);

	// CRealTimeShadowManager::Update crash fix
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN22CRealTimeShadowManager6UpdateEv"));

	// CPlayerPed::CPlayerPed remove player count restriction (orig: 7)
#if VER_x32
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(addr::CPlayerPed_CPlayerPed_patch), (uintptr_t) "\xE0", 1);
#else
	Memory::writeMemory(g_saSym->Abs<uintptr_t>(addr::CPlayerPed_CPlayerPed_patch), (uintptr_t) "\x34\x00\x80\x52", 4);
#endif

	// Avoid direct teleportation of vehicles
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN13CCarEnterExit17SetPedInCarDirectEP4CPedP8CVehicleib")); // CCarEnterExit::SetPedInCarDirect

	// add pedestrian
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN11CPopulation6AddPedE8ePedTypejRK7CVectorb")); // CPopulation::AddPed

	// no vehicle audio processing
	Memory::nop(g_saSym->Abs<uintptr_t>(addr::CAEVehicleAudioEntity_Service_patch1), 2); // CAutomobile
	Memory::nop(g_saSym->Abs<uintptr_t>(addr::CAEVehicleAudioEntity_Service_patch2), 2); // CBike
	Memory::nop(g_saSym->Abs<uintptr_t>(addr::CAEVehicleAudioEntity_Service_patch3), 2); // CBoat
	Memory::nop(g_saSym->Abs<uintptr_t>(addr::CAEVehicleAudioEntity_Service_patch4), 2); // CTrain

	// CAudioEngine::StartLoadingTune
	Memory::nop(g_saSym->Abs<uintptr_t>(addr::CAudioEngine_StartLoadingTune_patch), 2);

	// CBike::ProcessAI
#if !VER_x32
	Memory::nop(g_saSym->Abs<uintptr_t>(addr::CBike_ProcessAI_patch), !VER_x32 ? 2 : 1); // crashes on x32
#endif

	// CPlayerInfo::KillPlayer -> CMessages::AddBigMessage
	Memory::nop(g_saSym->Abs<uintptr_t>(addr::CMessages_AddBigMessage_patch), 2);

	// CAEGlobalWeaponAudioEntity::ServiceAmbientGunFire
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN26CAEGlobalWeaponAudioEntity21ServiceAmbientGunFireEv"));

	// CPlane::DoPlaneGenerationAndRemoval
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN6CPlane27DoPlaneGenerationAndRemovalEv"));

	// CFileLoader::LoadPickup
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN11CFileLoader10LoadPickupEPKc"));

	// disable Generate scene npc
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN10CEntryExit19GenerateAmbientPedsERK7CVector")); // CEntryExit::GenerateAmbientPeds

	// fix hospital car
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN11CPlayerInfo9AddHealthEi")); // CPlayerInfo::AddHealth

	// draw map icon
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN6CRadar10DrawLegendEiii")); // CRadar::DrawLegend

	// CRadar::AddBlipToLegendList
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN6CRadar19AddBlipToLegendListEhi"));

	// NOP calling CSprite2d::Draw from CRadar::DrawCoordBlip
	Memory::nop(g_saSym->Abs<uintptr_t>(addr::CRadar_DrawCoordBlip_patch), 2);

	// NOP calling CSprite2d::Draw from CRadar::DrawEntityBlip
	Memory::nop(g_saSym->Abs<uintptr_t>(addr::CRadar_DrawEntityBlip_patch), 2);

	// CVehicleModelInfo::ms_vehicleColourTable
	*g_saSym->GetSymbol<uintptr_t*>("_ZN17CVehicleModelInfo21ms_vehicleColourTableE") = reinterpret_cast<uintptr_t>(VehicleColoursTableRGBA);

	// stuff
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN7CWanted14ReportCrimeNowE10eCrimeTypeRK7CVectorb"));
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_Z16SaveGameForPause10eSaveTypesPc"));
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN8C_PcSave8SaveSlotEab"));

	// Remove the FindPlayerInfoForThisPlayerPed
	Memory::nop(g_saSym->Abs<uintptr_t>(addr::FindPlayerInfoForThisPlayerPed_patch), 6);

	// CTaskSimplePlayerOnFoot::PlayIdleAnimations
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN23CTaskSimplePlayerOnFoot18PlayIdleAnimationsEP10CPlayerPed"));

	// DoFade
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_Z6DoFadev"));

	// generate random cars
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN8CCarCtrl18GenerateRandomCarsEv"));
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN17CTheCarGenerators7ProcessEv"));

	// vehicle radio
	/*Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN21CAEVehicleAudioEntity32PlayerAboutToExitVehicleAsDriverEv"));
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN21CAEVehicleAudioEntity18JustWreckedVehicleEv"));
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN21CAEVehicleAudioEntity21TurnOnRadioForVehicleEv"));*/

	// Disable in-game radio
	Memory::ret(g_saSym->GetSymbol<uintptr_t>("_ZN20CAERadioTrackManager7ServiceEi"));
}

void InstallVehicleEngineLightPatches()
{
	// ~
}
