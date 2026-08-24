#include <sys/sysinfo.h>
#include <EGL/egl.h>

#include "Game.h"
#include "../UI/UI.h"
#include "../Net/NetGame.h"
#include "../Client.h"
#include "../Settings.h"
#include "ThreadOptimizer.h"
#include "../Java/Java.h"
#include "Pickups.h"

extern UI *pUI;
extern Game *pGame;
extern NetGame *pNetGame;

DECL_HOOK(void, AND_TouchEvent, int type, int num, int posX, int posY)
{
	if (Game::IsGamePaused())
	{
		return AND_TouchEvent(type, num, posX, posY);
	}

	if (pUI != nullptr)
	{
		switch (type)
		{
		case 2: // push
			pUI->touchEvent(ImVec2(posX, posY), TouchType::push);
			break;

		case 3: // move
			pUI->touchEvent(ImVec2(posX, posY), TouchType::move);
			break;

		case 1: // pop
			pUI->touchEvent(ImVec2(posX, posY), TouchType::pop);
			break;
		}

		if (pUI->keyboard()->visible() || pUI->dialog()->visible())
		{
			AND_TouchEvent(1, 0, 0, 0);
			return;
		}
		else
		{
			if (pNetGame && pNetGame->GetTextDrawPool())
			{
				if (!pNetGame->GetTextDrawPool()->onTouchEvent(type, posX, posY))
				{
					return AND_TouchEvent(1, 0, 0, 0);
				}
			}
		}
	}

	if (pGame->IsGameInputEnabled())
	{
		AND_TouchEvent(type, num, posX, posY);
	}
	else
	{
		AND_TouchEvent(1, 0, 0, 0);
	}
}

DECL_HOOK(void, DisplayScreen)
{
	/*
	 * using the loadingscreen from java so we don't need this anymore
	 */

	/*if (RwCameraBeginUpdate(sa::Scene->m_pRwCamera)) {
		// DefinedState2d
		Memory::callFunction<void>("_Z14DefinedState2dv");

		// CSprite2d::InitPerFrame
		Memory::callFunction<void>("_ZN9CSprite2d12InitPerFrameEv");

		RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*) rwTEXTUREADDRESSCLAMP);

		// emu_GammaSet
		Memory::callFunction("_Z12emu_GammaSeth", false);

		// CLoadingScreen::m_PercentLoaded
		float fPercentLoaded = *g_saSym->GetSymbol<float*>("_ZN14CLoadingScreen15m_PercentLoadedE");

		pUI->splashscreen()->setProgressBarValue(fPercentLoaded / 100.0f);
		pUI->render();

		RwCameraEndUpdate(sa::Scene->m_pRwCamera);
		RwCameraShowRaster(sa::Scene->m_pRwCamera, nullptr, 0);
	}*/
}

DECL_HOOK(int, OS_FileOpen, int a1, uintptr_t handle, char *name, int a2)
{
	char path[0xff] = {0};

	if (!strncmp(name, "data\\script\\mainV1.scm", 22))
	{
		spdlog::info("Loading mainV1.scm..");
		sprintf(path, "SAMP\\main.scm");
		name = path;
		goto ret;
	}

	if (!strncmp(name, "DATA\\SCRIPT\\SCRIPTV1.IMG", 24))
	{
		spdlog::info("Loading scriptV1.img..");
		sprintf(path, "SAMP\\script.img");
		name = path;
		goto ret;
	}

	if (!strncmp(name, "DATA\\GTA.DAT", 12))
	{
		spdlog::info("Loading gta.dat..");
		sprintf(path, "SAMP\\gta.dat");
		name = path;
		goto ret;
	}

	if (!strncmp(name, "DATA\\PEDS.IDE", 13))
	{
		spdlog::info("Loading peds.ide..");
		sprintf(path, "SAMP\\peds.ide");
		name = path;
		goto ret;
	}

	if (!strncmp(name, "DATA\\TIMECYC.DAT", 16))
	{
		spdlog::info("Loading timecyc.dat..");
		sprintf(path, "SAMP\\timecyc.dat");
		name = path;
		goto ret;
	}

	if (!strncmp(name, "data\\paths\\tracks2.dat", 22))
	{
		spdlog::info("Loading tracks2.dat...");
		sprintf(path, "tracks2.dat");
		name = path;
		goto ret;
	}

	if (!strncmp(name, "data\\paths\\tracks4.dat", 22))
	{
		spdlog::info("Loading tracks4.dat...");
		sprintf(path, "tracks4.dat");
		name = path;
		goto ret;
	}

ret:
	return OS_FileOpen(a1, handle, name, a2);
}

DECL_HOOK(sa::CPedModelInfo *, CModelInfo_AddPedModel, int id)
{
	// spdlog::info("Adding ped model: {}", id);
	auto model = new sa::CPedModelInfo{};
	model->vtable = g_saSym->GetVmtFunction<uintptr_t>("_ZTV13CPedModelInfo", 1, true); // CPedModelInfo::~CPedModelInfo

	typedef sa::CStore<sa::CPedModelInfo, 278> PedModelStore;
	auto ms_pedModelStore = g_saSym->Abs<PedModelStore *>(addr::ms_pedModelStore);
	ms_pedModelStore->AddItem();

	// CClumpModelInfo::Init
	model->Init();

	auto ms_modelInfoPtrs = g_saSym->Abs<sa::CBaseModelInfo **>(addr::CModelInfo_ms_modelInfoPtrs);
	ms_modelInfoPtrs[id] = model;
	return model;
}

DECL_HOOK(sa::CAtomicModelInfo *, CModelInfo_AddAtomicModel, int id)
{
	// spdlog::info("Adding atomic model: {}", id);
	auto model = new sa::CAtomicModelInfo{};
	model->vtable = g_saSym->GetVmtFunction<uintptr_t>("_ZTV16CAtomicModelInfo", 1, true); // CAtomicModelInfo::~CAtomicModelInfo

	typedef sa::CStore<sa::CAtomicModelInfo, 14000> AtomicModelStore;
	auto ms_atomicModelStore = g_saSym->Abs<AtomicModelStore *>(addr::ms_atomicModelStore);
	ms_atomicModelStore->AddItem();

	// CAtomicModelInfo::Init
	model->Init();

	// CModelInfo::ms_modelInfoPtrs
	auto ms_modelInfoPtrs = g_saSym->Abs<sa::CBaseModelInfo **>(addr::CModelInfo_ms_modelInfoPtrs);
	ms_modelInfoPtrs[id] = model;
	return model;
}

DECL_HOOK(RwTexture *, CTxdStore_TxdStoreFindCB, const char *texture_name)
{
	// spdlog::info("Finding texture: {}", texture_name);

	std::vector<std::string> texture_databases = {"gta_int", "gta3", "samp", "menu", "mobile"};
	for (const auto &texture_database : texture_databases)
	{
		auto database_handle = sa::TextureDatabaseRuntime::GetDatabase(texture_database.c_str());
		auto registered = sa::TextureDatabaseRuntime::registered();

		if (!registered.dataPtr)
		{
			break;
		}

		for (int index = 0; index <= registered.numEntries; ++index)
		{
			if (registered.dataPtr[index] == database_handle)
			{
				break;
			}

			if (index != registered.numEntries)
			{
				continue;
			}

			sa::TextureDatabaseRuntime::Register(database_handle);
			RwTexture *texture = sa::TextureDatabaseRuntime::GetTexture(texture_name);
			sa::TextureDatabaseRuntime::Unregister(database_handle);

			if (texture)
			{
				return texture;
			}
		}
	}

	RwTexDictionary *current = Memory::callFunction<RwTexDictionary *>("_Z25RwTexDictionaryGetCurrentv");
	while (current)
	{
		RwTexture *texture = Memory::callFunction<RwTexture *>("_Z31RwTexDictionaryFindNamedTextureP15RwTexDictionaryPKc", current, texture_name);
		if (texture)
		{
			return texture;
		}

		current = sa::CTxdStore::GetTxdParent(current);
	}

	return nullptr;
}

DECL_HOOK(int, CGame_InitialiseRenderWare)
{
	spdlog::info("Initializing samp texture database...");

	int result = CGame_InitialiseRenderWare();
	sa::TextureDatabaseRuntime::Load("samp", false, sa::DF_ETC);
	Client::initializeUI();
	return result;
}

struct CStreamingFile
{
	unsigned char m_name[40];
	bool m_bRegister;
	uint8_t m_padding[3];
	int32_t m_lsn;
};
VALIDATE_SIZE(CStreamingFile, 0x30);

CStreamingFile *CStreaming_ms_files = nullptr;
DECL_HOOK(void, CStreaming_InitImageList)
{
	spdlog::info("Initializing image list...");

	CStreaming_ms_files = g_saSym->GetSymbol<CStreamingFile *>("_ZN10CStreaming8ms_filesE");

	if (CStreaming_ms_files)
	{
		for (int i = 0; i < 8; i++)
		{
			CStreaming_ms_files[i].m_name[0] = '\0';
			CStreaming_ms_files[i].m_lsn = 0;
		}

		Memory::callFunction<void>("_ZN10CStreaming14AddImageToListEPKcb", "TEXDB\\SAMPCOL.IMG", true);
		Memory::callFunction<void>("_ZN10CStreaming14AddImageToListEPKcb", "TEXDB\\GTA3.IMG", true);
		Memory::callFunction<void>("_ZN10CStreaming14AddImageToListEPKcb", "TEXDB\\GTA_INT.IMG", true);
		Memory::callFunction<void>("_ZN10CStreaming14AddImageToListEPKcb", "TEXDB\\SAMP.IMG", true);
	}
	else
	{
		spdlog::error("Failed to get symbol for CStreaming_ms_files!");
	}
}

DECL_HOOK(void, CStreaming_Init2)
{
	CStreaming_Init2();

	// CStreaming::ms_memoryAvailable
	*g_saSym->GetSymbol<uint32_t *>("_ZN10CStreaming18ms_memoryAvailableE") *= 3;
}

DECL_HOOK(void, CTimer_StartUserPause)
{
	if (pUI)
	{
		pUI->setVisible(false);
	}

	if (g_java && pNetGame)
	{
		g_java->setPauseState(true);
	}

	*g_saSym->GetSymbol<bool *>("_ZN6CTimer11m_UserPauseE") = true;
}

DECL_HOOK(void, CTimer_EndUserPause)
{
	if (pUI)
	{
		pUI->setVisible(true);
	}

	if (g_java && pNetGame)
	{
		g_java->setPauseState(false);
	}

	*g_saSym->GetSymbol<bool *>("_ZN6CTimer11m_UserPauseE") = false;
}

unsigned int *msCollected;
unsigned int curMS;

DECL_HOOK(void, Render2dStuff)
{
	Render2dStuff();

	Client::process();

	if (Settings::displayfps())
	{
		// fps
		msCollected[curMS++ % 0x28] = Memory::callFunction<unsigned int>("_Z9OS_TimeMSv");

		RGBA bgColor = {0, 0, 0, 0};
		RGBA color = {255, 255, 255, 255};

		CFont::SetBackgroundColor(&bgColor);
		CFont::SetScale(1.12f * 0.54, 1.12f);
		CFont::SetOrientation(1);
		CFont::SetJustify(0);
		CFont::SetWrapX(640.0);
		CFont::SetProportional(0);
		CFont::SetFontStyle(1);
		CFont::SetEdge(0);
		CFont::SetColor(&color);

		char szStr[64];
		sprintf(szStr, "FPS: %.2f", (float)(39000.0 / (float)(int)(msCollected[(curMS - 1) % 0x28] - msCollected[curMS % 0x28])));
		CFont::PrintString((float)((float)sa::RsGlobal->screenWidth / 640.0) * 4.0, (float)((float)sa::RsGlobal->screenHeight / 448.0) * 200.0, szStr);

		// mem
		CFont::SetBackgroundColor(&bgColor);
		CFont::SetScale(1.12f * 0.54, 1.12f);
		CFont::SetOrientation(1);
		CFont::SetJustify(0);
		CFont::SetWrapX(640.0);
		CFont::SetProportional(0);
		CFont::SetFontStyle(1);
		CFont::SetEdge(0);
		CFont::SetColor(&color);

		struct mallinfo2 memInfo = mallinfo2();

		long long totalAllocatedMB = memInfo.uordblks / (1024 * 1024);

#if VER_x32
		long long totalMemoryMB = 0;
		FILE *meminfo = fopen("/proc/meminfo", "r");
		if (meminfo)
		{
			char line[256];
			while (fgets(line, sizeof(line), meminfo))
			{
				if (strncmp(line, "MemTotal:", 9) == 0)
				{
					totalMemoryMB = atoll(&line[9]) / 1024;
					break;
				}
			}
			fclose(meminfo);
		}
#else
		struct sysinfo info{};
		long long totalMemoryMB = 0;
		if (sysinfo(&info) == 0)
		{
			totalMemoryMB = info.totalram / (1024 * 1024); // Get total system RAM
		}
#endif

		char szStrMem[64];
		snprintf(szStrMem, sizeof(szStrMem), "MEM: %lld / %lld MB", totalAllocatedMB, totalMemoryMB);
		CFont::PrintString((float)((float)sa::RsGlobal->screenWidth / 640.0) * 4.0, (float)((float)sa::RsGlobal->screenHeight / 448.0) * 210.0, szStrMem);

		// arch
		CFont::SetBackgroundColor(&bgColor);
		CFont::SetScale(1.12f * 0.54, 1.12f);
		CFont::SetOrientation(1);
		CFont::SetJustify(0);
		CFont::SetWrapX(640.0);
		CFont::SetProportional(0);
		CFont::SetFontStyle(1);
		CFont::SetEdge(0);
		CFont::SetColor(&color);

		char szStrArch[64];
		snprintf(szStrArch, sizeof(szStrArch), "ARCH: %s", VER_x32 ? "x32" : "x64");
		CFont::PrintString((float)((float)sa::RsGlobal->screenWidth / 640.0) * 4.0, (float)((float)sa::RsGlobal->screenHeight / 448.0) * 220.0, szStrArch);
	}
}

DECL_HOOK(uint32_t, Idle, void *param, bool firstFrame)
{
	uint32_t result = Idle(param, firstFrame);

	if (pUI)
	{
		pUI->render();
	}

	RwCameraEndUpdate(sa::Scene->m_pRwCamera);

	return result;
}

DECL_HOOK(void, CRadar_ClearBlip, int32_t nIndex)
{
	uintptr_t dwRetAddr = 0;
	GET_LR(dwRetAddr);

	if ((uint16_t)nIndex > 249)
	{
		spdlog::warn("Invalid blip ID ({}). Called from 0x{:X}", nIndex, dwRetAddr);
		return;
	}

	return CRadar_ClearBlip(nIndex);
}

DECL_HOOK(void, CPools_Initialise)
{
	spdlog::info("GTA pools initializing..");
	Patches::initBetterPoolsSize();
}

ThreadOptimizer g_threadOpt;
DECL_HOOK(void, AND_RunThread, void *a1)
{
	g_threadOpt.pushThread(gettid());
	AND_RunThread(a1);
}

extern sa::CPlayerInfo *WORLD_PLAYERS;

DECL_HOOK(sa::CPlayerInfo *, GetPlayerInfoForThisPlayerPed, sa::CPed *ped)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (WORLD_PLAYERS[i].m_pPed == ped)
		{
			return &WORLD_PLAYERS[i];
		}
	}

	return nullptr;
}

DECL_HOOK(uint64_t *, RQCommand_rqSetAlphaTest, uint64_t *result)
{
	*result += 8;
	return result;
}

DECL_HOOK(int, GetInputType)
{
	return 0;
}

DECL_HOOK(void, CWidget, uintptr_t pWidget, const char *pszSprite, uintptr_t WidgetPos, unsigned int nFlags, uint32_t Mapping)
{
	spdlog::info("Widget: {}", pszSprite);
	CWidget(pWidget, pszSprite, WidgetPos, nFlags, Mapping);
}

DECL_HOOK(void, CWidget_Update, uintptr_t pWidget)
{
	if (pNetGame)
	{
		switch (ProcessFixedWidget(pWidget))
		{
		case STATE_NONE:
			break;
		case STATE_DISABLED:
			return;
		}
	}
	CWidget_Update(pWidget);
}

DECL_HOOK(void, CWidget_SetEnabled, uintptr_t pWidget, bool bEnabled)
{
	if (pNetGame)
	{
		switch (ProcessFixedWidget(pWidget))
		{
		case STATE_NONE:
			break;
		case STATE_DISABLED:
			bEnabled = false;
			break;
		}
	}
	CWidget_SetEnabled(pWidget, bEnabled);
}

DECL_HOOK(int, CWorld_FindPlayerSlotWithPedPointer, sa::CPed *pPlayersPed)
{
	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		if (WORLD_PLAYERS[i].m_pPed == pPlayersPed)
		{
			return i;
		}
	}
	return -1;
}

int g_lastObjectModel = -1;

DECL_HOOK(int32_t, CObject_Render, sa::CObject *_this)
{
	if (_this)
	{
		g_lastObjectModel = _this->m_nModelIndex;
	}

	// object material
	if (_this && !IsEntityPlaceable(_this))
	{
		if (pNetGame)
		{
			CObject *pObject = pNetGame->GetObjectPool()->FindObjectFromGtaPtr(_this);
			if (pObject)
			{
				pObject->TryChangeToCustomObjectMaterial();
			}
		}
	}

	int32_t result = CObject_Render(_this);

	if (_this && !IsEntityPlaceable(_this))
	{
		if (pNetGame)
		{
			CObject *pObject = pNetGame->GetObjectPool()->FindObjectFromGtaPtr(_this);
			if (pObject)
			{
				pObject->ChangeToOriginalObjectMaterial();
			}
		}
	}

	return result;
}

DECL_HOOK(void, CRenderer_RenderEverythingBarRoads)
{
	CRenderer_RenderEverythingBarRoads();

	if (pNetGame)
	{
		CObjectPool *pObjectPool = pNetGame->GetObjectPool();
		if (pObjectPool)
		{
			for (OBJECTID i = 0; i < MAX_OBJECTS; i++)
			{
				CObject *pObject = pObjectPool->GetAt(i);
				if (pObject && pObject->m_bForceRender)
				{
					pObject->Render();
				}
			}
		}
	}
}

DECL_HOOK(void, CWorld_ProcessPedsAfterPreRender)
{
	CWorld_ProcessPedsAfterPreRender();

	if (pNetGame)
	{
		CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
		if (pPlayerPool)
		{
			pPlayerPool->ProcessAttachedObjects();
		}
	}
}

DECL_HOOK(int, CustomPipeRenderCB, RwResEntry *resEntry, uintptr_t object, uint8_t type, uint32_t flags)
{
	if (!resEntry || !object)
	{
		spdlog::warn("CustomPipeRenderCB: Prevent crash - resEntry or object is null");
		return 0;
	}

	return CustomPipeRenderCB(resEntry, object, type, flags);
}

DECL_HOOK(int, rxOpenGLDefaultAllInOneRenderCB, RwResEntry *resEntry, uintptr_t object, uint8_t type, uint32_t flags)
{
	if (!resEntry || !object)
	{
		spdlog::warn("rxOpenGLDefaultAllInOneRenderCB: Prevent crash - resEntry or object is null");
		return 0;
	}

	return rxOpenGLDefaultAllInOneRenderCB(resEntry, object, type, flags);
}

DECL_HOOK(RwBool, _RwTextureDestroy, uintptr_t texture)
{
	if (!texture)
	{
		spdlog::warn("RwTextureDestroy: Prevent crash");
		return 0;
	}
	return _RwTextureDestroy(texture);
}

DECL_HOOK(int, RwFrameAddChild, int a1, int a2)
{
	if (!a1 || !a2)
	{
		spdlog::warn("RwFrameAddChild: Prevent crash");
		return 0;
	}
	return RwFrameAddChild(a1, a2);
}

DECL_HOOK(int, RLEDecompress, int a1, unsigned int a2, const char *a3, unsigned int a4, unsigned int a5)
{
	if (!a3)
	{
		spdlog::warn("RLEDecompress: Prevent crash");
		return 0;
	}
	return RLEDecompress(a1, a2, a3, a4, a5);
}

DECL_HOOK(int, RwResourcesFreeResEntry, int a1)
{
	if (!a1)
	{
		spdlog::warn("RwResourcesFreeResEntry: Prevent crash");
		return 0;
	}
	return RwResourcesFreeResEntry(a1);
}

DECL_HOOK(int, TextureDatabaseRuntime_GetEntry, uintptr_t _this, const char *a2, bool *a3)
{
	if (!_this)
	{
		spdlog::warn("TextureDatabaseRuntime_GetEntry: Prevent crash");
		return -1;
	}
	return TextureDatabaseRuntime_GetEntry(_this, a2, a3);
}

DECL_HOOK(void, CGame_Process)
{
	CGame_Process();

	if (pNetGame)
	{
		CTextDrawPool *pTextDrawPool = pNetGame->GetTextDrawPool();
		if (pTextDrawPool)
		{
			pTextDrawPool->SnapshotProcess();
		}

		CObjectPool *pObjectPool = pNetGame->GetObjectPool();
		if (pObjectPool)
		{
			pObjectPool->Process();
			pObjectPool->ProcessMaterialText();
		}
	}
}

DECL_HOOK(int, CRadar_SetCoordBlip, uint32_t BlpType, sa::CVector vecPosition, uint32_t nColour, uint32_t DispFlag, char *pScriptName)
{
	// CWorld::FindGroundZForCoord
	vecPosition.z = Memory::callFunction<float>("_ZN6CWorld19FindGroundZForCoordEff", vecPosition.x, vecPosition.y);

	if (Client::offlineMode())
	{
		RwMatrix mat = pGame->FindPlayerPed()->m_ped->GetMatrix().ToRwMatrix();
		mat.pos = sa::CVector(vecPosition.x, vecPosition.y, vecPosition.z + 5.0f);
		if (pGame->FindPlayerPed()->IsInVehicle())
		{
			pGame->FindPlayerPed()->m_ped->pVehicle->SetMatrix((sa::CMatrix &)mat);
			pGame->FindPlayerPed()->m_ped->SetMatrix((sa::CMatrix &)mat);
		}
		else
		{
			pGame->FindPlayerPed()->m_ped->SetMatrix((sa::CMatrix &)mat);
		}

		pUI->chat()->addInfoMessage("-> Teleported to: %.2f %.2f %.2f", vecPosition.x, vecPosition.y, vecPosition.z);
	}

	if (pNetGame && !strncmp(pScriptName, "CODEWAY", 7))
	{
		RakNet::BitStream bsSend;
		bsSend.Write(vecPosition.x);
		bsSend.Write(vecPosition.y);
		bsSend.Write(vecPosition.z);
		pNetGame->GetRakClient()->RPC(&RPC_MapMarker, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
	}

	return CRadar_SetCoordBlip(BlpType, vecPosition, nColour, DispFlag, pScriptName);
}

DECL_HOOK(void, CRadar_DrawRadarGangOverlay, bool bFullMap)
{
	if (pNetGame)
	{
		CGangZonePool *pGangZonePool = pNetGame->GetGangZonePool();
		if (pGangZonePool)
		{
			pGangZonePool->Draw(bFullMap);
		}
	}
}

#if VER_x32
DECL_HOOK(uint32_t, CHudColours_GetIntColour, uint32_t index)
{
	return TranslateColorCodeToRGBA(index);
}
#else
DECL_HOOK(uint32_t, CHudColours_GetIntColour, uintptr_t _this, uint32_t index)
{
	return TranslateColorCodeToRGBA(index);
}
#endif

DECL_HOOK(uint32_t, CAutoMobile_ProcessEntityCollision, sa::CVehicle *_this, sa::CVehicle *pCollidedVehicle, uintptr_t a3)
{
	if (!pNetGame || !pNetGame->m_netSet->disableVehicleCollisions ||
		!_this || !pCollidedVehicle ||
		pCollidedVehicle->m_nModelIndex < 400 || pCollidedVehicle->m_nModelIndex > 611 ||
		!_this->pDriver || !pCollidedVehicle->pDriver)
	{
		return CAutoMobile_ProcessEntityCollision(_this, pCollidedVehicle, a3);
	}

	return 0;
}

DECL_HOOK(uint32_t, CBike_ProcessEntityCollision, sa::CVehicle *_this, sa::CVehicle *pCollidedVehicle, uintptr_t a3)
{
	if (!pNetGame || !pNetGame->m_netSet->disableVehicleCollisions ||
		!_this || !pCollidedVehicle ||
		pCollidedVehicle->m_nModelIndex < 400 || pCollidedVehicle->m_nModelIndex > 611 ||
		!_this->pDriver || !pCollidedVehicle->pDriver)
	{
		return CBike_ProcessEntityCollision(_this, pCollidedVehicle, a3);
	}

	return 0;
}

DECL_HOOK(uint32_t, CMonsterTruck_ProcessEntityCollision, sa::CVehicle *_this, sa::CVehicle *pCollidedVehicle, uintptr_t a3)
{
	if (!pNetGame || !pNetGame->m_netSet->disableVehicleCollisions ||
		!_this || !pCollidedVehicle ||
		pCollidedVehicle->m_nModelIndex < 400 || pCollidedVehicle->m_nModelIndex > 611 ||
		!_this->pDriver || !pCollidedVehicle->pDriver)
	{
		return CMonsterTruck_ProcessEntityCollision(_this, pCollidedVehicle, a3);
	}

	return 0;
}

DECL_HOOK(uint32_t, CTrailer_ProcessEntityCollision, sa::CVehicle *_this, sa::CVehicle *pCollidedVehicle, uintptr_t a3)
{
	if (!pNetGame || !pNetGame->m_netSet->disableVehicleCollisions ||
		!_this || !pCollidedVehicle ||
		pCollidedVehicle->m_nModelIndex < 400 || pCollidedVehicle->m_nModelIndex > 611 ||
		!_this->pDriver || !pCollidedVehicle->pDriver)
	{
		if (!pCollidedVehicle->pDriver)
		{
			if (pNetGame)
			{
				CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
				if (pVehiclePool)
				{
					VEHICLEID vehicleId = pVehiclePool->FindIDFromGtaPtr(pCollidedVehicle);
					if (vehicleId != INVALID_VEHICLE_ID)
					{
						CVehicle *pVehicle = pVehiclePool->GetAt(vehicleId);
						if (pVehicle)
						{
							// trailer has attached to any tractor
							if (pVehicle->GetTractor() != NULL)
							{
								return 0;
							}
						}
					}
				}
			}
		}

		return CTrailer_ProcessEntityCollision(_this, pCollidedVehicle, a3);
	}

	return 0;
}

// fix crosshair
float *m_f3rdPersonCHairMultX, *m_f3rdPersonCHairMultY;

DECL_HOOK(void, DrawCrosshair)
{
	float save1 = *m_f3rdPersonCHairMultX;
	*m_f3rdPersonCHairMultX = 0.52f;

	float save2 = *m_f3rdPersonCHairMultY;
	*m_f3rdPersonCHairMultY = 0.435f;

	DrawCrosshair();

	*m_f3rdPersonCHairMultX = save1;
	*m_f3rdPersonCHairMultY = save2;
}

void Hooks::installWidgetFixHooks()
{
	spdlog::info("Installing widget fix hooks...");

	g_pWidgets = *g_saSym->Abs<uintptr_t **>(VER_x32 ? 0x679474 : 0x850910);

	// HOOK("_ZN7CWidgetC2EPKcRK14WidgetPositionj10HIDMapping", CWidget);
	HOOK("_ZN7CWidget6UpdateEv", CWidget_Update);
	HOOK("_ZN7CWidget10SetEnabledEb", CWidget_SetEnabled);
}

void Hooks::installCrosshairFixHooks()
{
	spdlog::info("Installing crosshair fix hooks...");

	m_f3rdPersonCHairMultX = g_saSym->GetSymbol<float *>("_ZN7CCamera22m_f3rdPersonCHairMultXE");
	m_f3rdPersonCHairMultY = g_saSym->GetSymbol<float *>("_ZN7CCamera22m_f3rdPersonCHairMultYE");

	HOOK("_ZN4CHud14DrawCrossHairsEv", DrawCrosshair);
}

void Hooks::install()
{
	spdlog::info("Installing hooks...");

	msCollected = g_saSym->GetSymbol<unsigned int *>("msCollected");
	curMS = *g_saSym->GetSymbol<unsigned int *>("curMS");

	HOOK("_Z11OS_FileOpen14OSFileDataAreaPPvPKc16OSFileAccessType", OS_FileOpen);
	HOOK("_ZN10CStreaming13InitImageListEv", CStreaming_InitImageList);
	HOOK("_ZN10CStreaming5Init2Ev", CStreaming_Init2);
	HOOK("_ZN6CTimer14StartUserPauseEv", CTimer_StartUserPause);
	HOOK("_ZN6CTimer12EndUserPauseEv", CTimer_EndUserPause);
	HOOK("_ZN5CGame20InitialiseRenderWareEv", CGame_InitialiseRenderWare);
	HOOK("_ZN10CModelInfo11AddPedModelEi", CModelInfo_AddPedModel);
	HOOK("_ZN10CModelInfo14AddAtomicModelEi", CModelInfo_AddAtomicModel);
	HOOK("_ZN6CPools10InitialiseEv", CPools_Initialise);
	HOOK("_ZN9CTxdStore14TxdStoreFindCBEPKc", CTxdStore_TxdStoreFindCB);

	// SAMP hooks //

	// UI
	HOOK("_Z12ANDRunThreadPv", AND_RunThread);
	HOOK("_Z13Render2dStuffv", Render2dStuff);
	HOOK("_Z4IdlePvb", Idle);
	HOOK("_Z14AND_TouchEventiiii", AND_TouchEvent);
	HOOK("_ZN14CLoadingScreen15DisplayPCScreenEv", DisplayScreen);

	// Game process
	HOOK("_ZN5CGame7ProcessEv", CGame_Process);

	// click map rpc
	HOOK("_ZN6CRadar12SetCoordBlipE9eBlipType7CVectorj12eBlipDisplayPc", CRadar_SetCoordBlip);

	// gangzone
	HOOK("_ZN6CRadar20DrawRadarGangOverlayEb", CRadar_DrawRadarGangOverlay);

	// radar color
	HOOK("_ZN11CHudColours12GetIntColourEh", CHudColours_GetIntColour);

	// vehicle collisions
	HOOK("_ZN11CAutomobile22ProcessEntityCollisionEP7CEntityP9CColPoint", CAutoMobile_ProcessEntityCollision);
	HOOK("_ZN5CBike22ProcessEntityCollisionEP7CEntityP9CColPoint", CBike_ProcessEntityCollision);
	HOOK("_ZN13CMonsterTruck22ProcessEntityCollisionEP7CEntityP9CColPoint", CMonsterTruck_ProcessEntityCollision);
	HOOK("_ZN8CTrailer22ProcessEntityCollisionEP7CEntityP9CColPoint", CTrailer_ProcessEntityCollision);

	// obj material
	HOOK("_ZN7CObject6RenderEv", CObject_Render);

	// render objects 3000+- pos
	HOOK("_ZN9CRenderer24RenderEverythingBarRoadsEv", CRenderer_RenderEverythingBarRoads);

	// attched objects
	HOOK("_ZN6CWorld25ProcessPedsAfterPreRenderEv", CWorld_ProcessPedsAfterPreRender);

	// crash fixes
	HOOK("_ZN6CRadar9ClearBlipEi", CRadar_ClearBlip);
	HOOK("_ZN6CWorld28FindPlayerSlotWithPedPointerEPv", CWorld_FindPlayerSlotWithPedPointer);
	HOOK("_ZN25CCustomBuildingDNPipeline18CustomPipeRenderCBEP10RwResEntryPvhj", CustomPipeRenderCB);
	HOOK("_Z32_rxOpenGLDefaultAllInOneRenderCBP10RwResEntryPvhj", rxOpenGLDefaultAllInOneRenderCB);
	HOOK("_Z16RwTextureDestroyP9RwTexture", _RwTextureDestroy);
	HOOK("_Z15RwFrameAddChildP7RwFrameS0_", RwFrameAddChild);
	HOOK("_Z13RLEDecompressPhjPKhjj", RLEDecompress);
	HOOK("_Z23RwResourcesFreeResEntryP10RwResEntry", RwResourcesFreeResEntry);
	HOOK("_ZN22TextureDatabaseRuntime8GetEntryEPKcRb", TextureDatabaseRuntime_GetEntry);
	HOOK("_ZN10CPlayerPed29GetPlayerInfoForThisPlayerPedEv", GetPlayerInfoForThisPlayerPed);

	if (!eglGetProcAddress("glAlphaFuncQCOM"))
	{
		// If "glAlphaFuncQCOM" is not available, try "glAlphaFunc"
		if (eglGetProcAddress("glAlphaFunc"))
		{
			// If "glAlphaFunc" is found, store the address in the global library
			*((void **)(g_saSym->Abs(VER_x32 ? 0x6BCBF8 : 0x89A1B0))) = (void *)eglGetProcAddress("glAlphaFunc");
		}
		else
		{
			// If neither function is available, hook the fallback symbol
			HOOK("_Z25RQ_Command_rqSetAlphaTestRPc", RQCommand_rqSetAlphaTest);
		}
	}

	// fix spin on keyboard/controller
	HOOK("_ZN4CHID12GetInputTypeEv", GetInputType);

	Pickups::installHooks();
	RemoveBuilding::installHooks();

	installPadHooks();
	installWidgetFixHooks();
	installCrosshairFixHooks();
}
