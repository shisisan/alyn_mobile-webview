#include "Game.h"
#include "../Net/NetGame.h"
#include "../Client.h"
#include "Enums/WidgetIDs.h"

extern Game* pGame;
extern NetGame* pNetGame;

#define PI 3.14159265

extern sa::CPlayerInfo* WORLD_PLAYERS;

sa::CPed* GamePool_FindPlayerPed()
{
	//spdlog::info("GamePool_FindPlayerPed");
	return WORLD_PLAYERS->m_pPed;
}

sa::CPed* dwPlayerPedPtrs[PLAYER_PED_SLOTS];

void SetPlayerPedPtrRecord(uint8_t bytePlayer, sa::CPed* dwPedPtr)
{
	dwPlayerPedPtrs[bytePlayer] = dwPedPtr;
}

uint8_t FindPlayerNumFromPedPtr(sa::CPed* dwPedPtr)
{
	uint8_t x = 0;
	while (x != PLAYER_PED_SLOTS) {
		if (dwPlayerPedPtrs[x] == dwPedPtr) {
			return x;
		}
		x++;
	}

	return 0;
}

/**
 * @param txdName
 * @param textureName
 * @return
 */
RwTexture* LoadTextureFromTxd(const char* txdName, const char* textureName)
{
	spdlog::info("LoadTextureFromTxd: txdName: {}, textureName: {}", txdName, textureName);

	RwTexture* pTexture = nullptr;

	if (txdName && textureName) {
		int iTxdSlot = sa::CTxdStore::FindTxdSlot("*");
		if (iTxdSlot != -1) {
			sa::CTxdStore::PushCurrentTxd();
			sa::CTxdStore::SetCurrentTxd(iTxdSlot);

			sa::CSprite2d txdSprite;
			txdSprite.SetTexture(textureName);
			pTexture = txdSprite.m_pTexture;
			if (pTexture) {
				++pTexture->refCount;
			}

			sa::CTxdStore::PopCurrentTxd();
		}
	}

	if (!pTexture) {
		spdlog::warn("Texture \"{}\" was not found!", textureName);
	}

	return pTexture;
}

bool AddTxdToSlot(const char* szTxdName)
{
	if (sa::CTxdStore::FindTxdSlot(szTxdName) == -1) {
		int iTxdSlot = sa::CTxdStore::AddTxdSlot(szTxdName, "txd");
		sa::CTxdStore::AddRef(iTxdSlot);
	}

	if (sa::CTxdStore::FindTxdSlot(szTxdName) != -1) return true;
	return false;
}

uint32_t GetTickCount()
{
	struct timeval tv{};
	gettimeofday(&tv, nullptr);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

uint32_t GamePool_Vehicle_GetIndex(sa::CVehicle* pVehicle)
{
	//spdlog::info("GamePool_Vehicle_GetIndex: 0x{:X}", reinterpret_cast<uintptr_t>(pVehicle));
	return Memory::callFunction<uint32_t>("_Z18GettPoolVehicleRefP8CVehicle", pVehicle);
}

sa::CPed* GamePool_Ped_GetAt(int slot)
{
	//spdlog::info("GamePool_Ped_GetAt: {}", slot);
	return Memory::callFunction<sa::CPed*>("_Z10GetPoolPedi", slot);
}

sa::CObject* GamePool_Object_GetAt(int iID)
{
	//spdlog::info("GamePool_Object_GetAt: {}", iID);
	return Memory::callFunction<sa::CObject*>("_Z10GetPoolObji", iID);
}

/**
 * @param task
 * @return
 */
int GetTaskTypeFromTask(sa::CTask** task)
{
	if (!task || *task < g_saSym->Abs<sa::CTask*>(addr::start$CTaskAllocatorKillThreatsDriveby) || *task > g_saSym->Abs<sa::CTask*>(addr::start$CTaskSimpleUninterruptable)) {
		return 0;
	}

	sa::CTask* dwTaskVtbl = task[0];
	return g_saSym->CallFunction<int>(g_saSym->GetVmtFunction<uintptr_t>(*(uint32_t*) dwTaskVtbl, 3), dwTaskVtbl);
}

void GameResetStats()
{
	// ~
}

/**
 * @param f1
 * @param f2
 * @return
 */
float FloatOffset(float f1, float f2)
{
	if (f1 >= f2) {
		return f1 - f2;
	}
	else {
		return (f2 - f1);
	}
}

/**
 * @param fDegrees
 * @return
 */
float DegToRad(float fDegrees)
{
	if (fDegrees > 360.0f || fDegrees < 0.0f) {
		return 0.0f;
	}

	if (fDegrees > 180.0f) {
		return (float) (-(PI - (((fDegrees - 180.0f) * PI) / 180.0f)));
	}
	else {
		return (float) ((fDegrees * PI) / 180.0f);
	}
}

float fixAngle(float angle)
{
	if (angle > 180.0f) {
		angle -= 360.0f;
	}
	if (angle < -180.0f) {
		angle += 360.0f;
	}

	return angle;
}

float subAngle(float a1, float a2)
{
	return fixAngle(fixAngle(a2) - a1);
}

void DefinedState()
{
	Memory::callFunction("_Z12DefinedStatev");
}

/**
 * @param pGtaVehicle
 * @return
 */
int GetVehicleSubtype(sa::CVehicle* pGtaVehicle)
{
	if (pGtaVehicle) {
		uintptr_t vtable = *(uintptr_t*) pGtaVehicle;

		if (vtable == g_saSym->Abs<uintptr_t>(addr::vptr$CAutomobile)) { // CAutomobile
			return VEHICLE_SUBTYPE_CAR;
		}
		else if (vtable == g_saSym->Abs<uintptr_t>(addr::vptr$CBike)) { // CBike
			return VEHICLE_SUBTYPE_BIKE;
		}
		else if (vtable == g_saSym->Abs<uintptr_t>(addr::vptr$CHeli)) { // CHeli
			return VEHICLE_SUBTYPE_HELI;
		}
		else if (vtable == g_saSym->Abs<uintptr_t>(addr::vptr$CBoat)) { // CBoat
			return VEHICLE_SUBTYPE_BOAT;
		}
		else if (vtable == g_saSym->Abs<uintptr_t>(addr::vptr$CPlane)) { // CPlane
			return VEHICLE_SUBTYPE_PLANE;
		}
		else if (vtable == g_saSym->Abs<uintptr_t>(addr::vptr$CBmx)) { // CBmx
			return VEHICLE_SUBTYPE_PUSHBIKE;
		}
		else if (vtable == g_saSym->Abs<uintptr_t>(addr::vptr$CTrain)) { // CTrain
			return VEHICLE_SUBTYPE_TRAIN;
		}
		else if (vtable == g_saSym->Abs<uintptr_t>(addr::vptr$CMonsterTruck)) { // CMonsterTruck
			return VEHICLE_SUBTYPE_MONSTERTRUCK;
		}
		else if (vtable == g_saSym->Abs<uintptr_t>(addr::vptr$CQuadBike)) { // CQuadBike
			return VEHICLE_SUBTYPE_QUADBIKE;
		}
		else if (vtable == g_saSym->Abs<uintptr_t>(addr::vptr$CTrailer)) { // CTrailer
			return VEHICLE_SUBTYPE_TRAILER;
		}
	}

	return 0;
}

void SetRenderWareCamera(RwCamera* camera)
{
	// CVisibilityPlugins::SetRenderWareCamera
	Memory::callFunction("_ZN18CVisibilityPlugins19SetRenderWareCameraEP8RwCamera", camera);
}

void rpWorldAddCamera(RwCamera* camera)
{
	RpWorld* rpWorld = sa::Scene->m_pRpWorld;
	if (rpWorld) {
		RpWorldAddCamera(rpWorld, camera);
	}
}

void rpWorldAddLight(RpLight* rpLight)
{
	RpWorld* rpWorld = sa::Scene->m_pRpWorld;
	if (rpWorld) {
		RpWorldAddLight(rpWorld, rpLight);
	}
}

void rpWorldRemoveLight(RpLight* rpLight)
{
	RpWorld* rpWorld = sa::Scene->m_pRpWorld;
	if (rpWorld) {
		RpWorldRemoveLight(rpWorld, rpLight);
	}
}

uint32_t ModelInfoGetFlags(int iModelID)
{
	sa::CBaseModelInfo* pModel = GetModelInfoByID(iModelID);
	if (!pModel) {
		spdlog::error("ModelInfoGetFlags: {} is not a valid model.", iModelID);
		return 0;
	}

	spdlog::info("ModelInfoGetFlags: ({})", iModelID);
	return pModel->m_flags;
}

uint16_t ModelInfoGetReferenceCount(int iModelID)
{
	sa::CBaseModelInfo* pModel = GetModelInfoByID(iModelID);
	if (!pModel) {
		spdlog::error("ModelInfoGetReferenceCount: {} is not a valid model.", iModelID);
		return 0;
	}

	spdlog::info("ModelInfoGetReferenceCount: ({})", iModelID);
	return pModel->m_numRefs;
}

bool IsValidModel(int iModelID)
{
	sa::CBaseModelInfo* pModel = GetModelInfoByID(iModelID);
	if (pModel && *(uintptr_t*) pModel != 0xFFFFFFEC) {
		return true;
	}

	spdlog::error("IsValidModel: {} is not a valid model.", iModelID);
	return false;
}

bool IsValidPedModel(uint modelID)
{
	if (modelID <= 30000) {
		sa::CBaseModelInfo* modelInfo = GetModelInfoByID(modelID);
		if (!modelInfo) {
			spdlog::error("IsValidPedModel: {} is not a valid ped model.", modelID);
			return false;
		}

		spdlog::info("IsValidPedModel: ({})", modelID);

		if ((*(uintptr_t*) modelInfo) == g_saSym->Abs<uintptr_t>(addr::vptr$CPedModelInfo)) {
			return true;
		}
	}

	spdlog::error("IsValidPedModel: {} is not a valid ped model.", modelID);
	return false;
}

sa::CBaseModelInfo* GetModelInfoByID(int iModelID)
{
	spdlog::info("GetModelInfoByID: {}", iModelID);

	if (iModelID < 0 || iModelID > 20000) {
		spdlog::error("Invalid model ID: {}", iModelID);
		return nullptr;
	}

	auto modelInfoPtrs = g_saSym->GetSymbol<sa::CBaseModelInfo**>("_ZN10CModelInfo16ms_modelInfoPtrsE");

	if (!modelInfoPtrs) {
		spdlog::error("Failed to retrieve model info pointers.");
		return nullptr;
	}

	if (!modelInfoPtrs[iModelID]) {
		spdlog::error("Model info for ID {} is null.", iModelID);
		return nullptr;
	}

	return modelInfoPtrs[iModelID];
}

bool IsExistInfoForModel(int iModelID)
{
	spdlog::info("IsExistInfoForModel: {}", iModelID);
	return GetModelInfoByID(iModelID) != nullptr;
}

/**
 * @param uiModel
 * @return
 */
RwObject* GetModelRWObject(uint uiModel)
{
	sa::CBaseModelInfo* modelInfo = GetModelInfoByID(uiModel);
	if (!modelInfo) {
		spdlog::error("GetModelRWObject modelInfo is null.");
		return nullptr;
	}

	spdlog::info("GetModelRWObject: {}", uiModel);
	return modelInfo->m_pRwObject;
}

/**
 * @param iWeaponID
 * @return
 */
int GameGetWeaponModelIDFromWeaponID(int iWeaponID)
{
	switch (iWeaponID) {
		case WEAPON_BRASSKNUCKLE: return WEAPON_MODEL_BRASSKNUCKLE;
		case WEAPON_GOLFCLUB: return WEAPON_MODEL_GOLFCLUB;
		case WEAPON_NITESTICK: return WEAPON_MODEL_NITESTICK;
		case WEAPON_KNIFE: return WEAPON_MODEL_KNIFE;
		case WEAPON_BAT: return WEAPON_MODEL_BAT;
		case WEAPON_SHOVEL: return WEAPON_MODEL_SHOVEL;
		case WEAPON_POOLSTICK: return WEAPON_MODEL_POOLSTICK;
		case WEAPON_KATANA: return WEAPON_MODEL_KATANA;
		case WEAPON_CHAINSAW: return WEAPON_MODEL_CHAINSAW;
		case WEAPON_DILDO: return WEAPON_MODEL_DILDO;
		case WEAPON_DILDO2: return WEAPON_MODEL_DILDO2;
		case WEAPON_VIBRATOR: return WEAPON_MODEL_VIBRATOR;
		case WEAPON_VIBRATOR2: return WEAPON_MODEL_VIBRATOR2;
		case WEAPON_FLOWER: return WEAPON_MODEL_FLOWER;
		case WEAPON_CANE: return WEAPON_MODEL_CANE;
		case WEAPON_GRENADE: return WEAPON_MODEL_GRENADE;
		case WEAPON_TEARGAS: return WEAPON_MODEL_TEARGAS;
		case WEAPON_MOLOTOV: return WEAPON_MODEL_MOLOTOV;
		case WEAPON_COLT45: return WEAPON_MODEL_COLT45;
		case WEAPON_SILENCED: return WEAPON_MODEL_SILENCED;
		case WEAPON_DEAGLE: return WEAPON_MODEL_DEAGLE;
		case WEAPON_SHOTGUN: return WEAPON_MODEL_SHOTGUN;
		case WEAPON_SAWEDOFF: return WEAPON_MODEL_SAWEDOFF;
		case WEAPON_SHOTGSPA: return WEAPON_MODEL_SHOTGSPA;
		case WEAPON_UZI: return WEAPON_MODEL_UZI;
		case WEAPON_MP5: return WEAPON_MODEL_MP5;
		case WEAPON_AK47: return WEAPON_MODEL_AK47;
		case WEAPON_M4: return WEAPON_MODEL_M4;
		case WEAPON_TEC9: return WEAPON_MODEL_TEC9;
		case WEAPON_RIFLE: return WEAPON_MODEL_RIFLE;
		case WEAPON_SNIPER: return WEAPON_MODEL_SNIPER;
		case WEAPON_ROCKETLAUNCHER: return WEAPON_MODEL_ROCKETLAUNCHER;
		case WEAPON_HEATSEEKER: return WEAPON_MODEL_HEATSEEKER;
		case WEAPON_FLAMETHROWER: return WEAPON_MODEL_FLAMETHROWER;
		case WEAPON_MINIGUN: return WEAPON_MODEL_MINIGUN;
		case WEAPON_SATCHEL: return WEAPON_MODEL_SATCHEL;
		case WEAPON_BOMB: return WEAPON_MODEL_BOMB;
		case WEAPON_SPRAYCAN: return WEAPON_MODEL_SPRAYCAN;
		case WEAPON_FIREEXTINGUISHER: return WEAPON_MODEL_FIREEXTINGUISHER;
		case WEAPON_CAMERA: return WEAPON_MODEL_CAMERA;
		case WEAPON_NIGHTVISION: return WEAPON_MODEL_NIGHTVISION;
		case WEAPON_INFRARED: return WEAPON_MODEL_INFRARED;
		case WEAPON_PARACHUTE: return WEAPON_MODEL_PARACHUTE;
	}

	return -1;
}

int GetModelRefCounts(int iModel)
{
	uint16_t p = GetModelInfoByID(iModel)->m_numRefs;
	return p;
}

/**
 * @param pActor
 * @return
 */
int GamePool_Ped_GetIndex(sa::CPed* pActor)
{
	//spdlog::info("GamePool_Ped_GetIndex: 0x{:X}", reinterpret_cast<uintptr_t>(pActor));
	return Memory::callFunction<int>("_Z14GettPoolPedRefP4CPed", pActor);
}

/**
 * @param iID
 * @return
 */
sa::CVehicle* GamePool_Vehicle_GetAt(int iID)
{
	//spdlog::info("GamePool_Vehicle_GetAt: {}", iID);
	return Memory::callFunction<sa::CVehicle*>("_Z14GetPoolVehiclei", iID);
}

/**
 * @param iModel
 * @return
 */
float GetModelColSphereRadius(int iModel)
{
	sa::CBaseModelInfo* modelInfo = GetModelInfoByID(iModel);
	if (modelInfo) {
		sa::CColModel* colModel = modelInfo->m_pColModel;
		if (colModel != nullptr) {
			return colModel->m_boundSphere.m_fRadius;
		}
	}

	return 0.0f;
}

/**
 * @param iModel
 * @param vec
 */
void GetModelColSphereVecCenter(int iModel, sa::CVector* vec)
{
	sa::CBaseModelInfo* modelInfo = GetModelInfoByID(iModel);
	if (modelInfo) {
		sa::CColModel* colModel = modelInfo->m_pColModel;
		if (colModel != nullptr) {
			sa::CVector* v = &colModel->m_boundSphere.m_vecCenter;
			vec->x = v->x;
			vec->y = v->y;
			vec->z = v->z;
		}
	}
}

/**
 * @param rwObject
 */
void DestroyAtomicOrClump(RwObject* rwObject)
{
	if (rwObject) {
		int type = rwObject->type;

		if (type == 1) {
			// RpAtomicDestroy
			Memory::callFunction("_Z15RpAtomicDestroyP8RpAtomic", rwObject);

			void* parent = rwObject->parent;
			if (parent) {
				// RwFrameDestroy
				Memory::callFunction("_Z14RwFrameDestroyP7RwFrame", parent);
			}
		}
		else if (type == 2) {
			// RpClumpDestroy
			Memory::callFunction("_Z14RpClumpDestroyP7RpClump", rwObject);
		}
	}
}

void GamePrepareTrain(sa::CVehicle* pGtaVehicle)
{
	spdlog::info("GamePrepareTrain");

	sa::CPed* pDriver = pGtaVehicle->pDriver;

	if (pDriver) {
		if (pDriver->m_nPedType != 0 && pDriver->m_nPedType != 1) {
			pDriver->Destructor();
			pGtaVehicle->pDriver = nullptr;
		}
	}
}

void ProjectMatrix(sa::CVector* vecOut, sa::CMatrix* mat, sa::CVector* vecPos)
{
	vecOut->x = mat->m_up.x * vecPos->z + mat->m_forward.x * vecPos->y + mat->m_right.x * vecPos->x + mat->m_pos.x;
	vecOut->y = mat->m_up.y * vecPos->z + mat->m_forward.y * vecPos->y + mat->m_right.y * vecPos->x + mat->m_pos.y;
	vecOut->z = mat->m_up.z * vecPos->z + mat->m_forward.z * vecPos->y + mat->m_right.z * vecPos->x + mat->m_pos.z;
}

static sa::CVector g_axis[3] = {
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f}};

int RwMatrixRotate(RwMatrix* mat, int axis, float angle)
{
	return Memory::callFunction<int>("_Z14RwMatrixRotateP11RwMatrixTagPK5RwV3df15RwOpCombineType", mat, &g_axis[axis], angle, rwCOMBINEPRECONCAT);
}

void RwMatrixScale(RwMatrix* mat, sa::CVector* vecScale)
{
	mat->right.x *= vecScale->x;
	mat->right.y *= vecScale->x;
	mat->right.z *= vecScale->x;

	mat->up.x *= vecScale->y;
	mat->up.y *= vecScale->y;
	mat->up.z *= vecScale->y;

	mat->at.x *= vecScale->z;
	mat->at.y *= vecScale->z;
	mat->at.z *= vecScale->z;

	mat->flags &= 0xFFFDFFFC;
}

enum Flags {
	IDENTITY = 0x20000
};

enum Type {
	TYPENORMAL = 1,
	TYPEORTHOGONAL = 2,
	TYPEORTHONORMAL = 3,
	TYPEMASK = 3
};

void MatrixInvertOrthonormal(RwMatrix* dst, const RwMatrix* src)
{
	dst->right.x = src->right.x;
	dst->right.y = src->up.x;
	dst->right.z = src->at.x;
	dst->up.x = src->right.y;
	dst->up.y = src->up.y;
	dst->up.z = src->at.y;
	dst->at.x = src->right.z;
	dst->at.y = src->up.z;
	dst->at.z = src->at.z;
	dst->pos.x = -(src->pos.x * src->right.x + src->pos.y * src->right.y + src->pos.z * src->right.z);
	dst->pos.y = -(src->pos.x * src->up.x + src->pos.y * src->up.y + src->pos.z * src->up.z);
	dst->pos.z = -(src->pos.x * src->at.x + src->pos.y * src->at.y + src->pos.z * src->at.z);
	dst->flags = TYPEORTHONORMAL;
}

RwMatrix* MatrixInvertGeneral(RwMatrix* dst, const RwMatrix* src)
{
	float det, invdet;
	// calculate a few cofactors
	dst->right.x = src->up.y * src->at.z - src->up.z * src->at.y;
	dst->right.y = src->at.y * src->right.z - src->at.z * src->right.y;
	dst->right.z = src->right.y * src->up.z - src->right.z * src->up.y;
	// get the determinant from that
	det = src->up.x * dst->right.y + src->at.x * dst->right.z + dst->right.x * src->right.x;
	invdet = 1.0;
	if (det != 0.0f) {
		invdet = 1.0f / det;
	}
	dst->right.x *= invdet;
	dst->right.y *= invdet;
	dst->right.z *= invdet;
	dst->up.x = invdet * (src->up.z * src->at.x - src->up.x * src->at.z);
	dst->up.y = invdet * (src->at.z * src->right.x - src->at.x * src->right.z);
	dst->up.z = invdet * (src->right.z * src->up.x - src->right.x * src->up.z);
	dst->at.x = invdet * (src->up.x * src->at.y - src->up.y * src->at.x);
	dst->at.y = invdet * (src->at.x * src->right.y - src->at.y * src->right.x);
	dst->at.z = invdet * (src->right.x * src->up.y - src->right.y * src->up.x);
	dst->pos.x = -(src->pos.x * dst->right.x + src->pos.y * dst->up.x + src->pos.z * dst->at.x);
	dst->pos.y = -(src->pos.x * dst->right.y + src->pos.y * dst->up.y + src->pos.z * dst->at.y);
	dst->pos.z = -(src->pos.x * dst->right.z + src->pos.y * dst->up.z + src->pos.z * dst->at.z);
	dst->flags &= ~IDENTITY;
	return dst;
}

RwMatrix* RwMatrixInvert(RwMatrix* dst, const RwMatrix* src)
{
	if (src->flags & IDENTITY) {
		*dst = *src;
	}
	else if ((src->flags & TYPEMASK) == TYPEORTHONORMAL) {
		MatrixInvertOrthonormal(dst, src);
	}
	else {
		return MatrixInvertGeneral(dst, src);
	}
	return dst;
}

bool IsEntityPlaceable(sa::CEntity* pEntity)
{
	if (*(uintptr_t*) pEntity) {
		if (*(uintptr_t*) pEntity == g_saSym->Abs<uintptr_t>(addr::vptr$CPlaceable)) {
			return true;
		}
	}

	return false;
}

void CalcScreenCoors(const RwV3d* In, RwV3d* pResult, float pScaleX, float pScaleY, bool ClipFarPlane, bool ClipNearPlane)
{
	// CSprite::CalcScreenCoors(RwV3d const&,RwV3d*,float *,float *,bool,bool)
	Memory::callFunction("_ZN7CSprite15CalcScreenCoorsERK5RwV3dPS0_PfS4_bb", In, pResult, &pScaleX, &pScaleY, ClipFarPlane, ClipNearPlane);
}

// widget fix
uintptr_t* g_pWidgets;

sa::WidgetIDs GetWidgetTypeFromWidget(uintptr_t pWidget)
{
	if (!pWidget) return static_cast<sa::WidgetIDs>(-1);
	if (!g_pWidgets) return static_cast<sa::WidgetIDs>(-1);

	for (int i = 0; i < sa::NUM_WIDGETS; i++) {
		if (g_pWidgets[i] && pWidget == g_pWidgets[i]) {
			return static_cast<sa::WidgetIDs>(i);
		}
	}

	return static_cast<sa::WidgetIDs>(-1);
}

eWidgetState ProcessFixedWidget(uintptr_t pWidget)
{
	CPlayerPed* pPlayerPed = pGame->FindPlayerPed();

	sa::WidgetIDs widgetType = GetWidgetTypeFromWidget(pWidget);
	switch (widgetType) {
		case -1: return STATE_NONE;
			break;

		case sa::WidgetIDs::WIDGET_BUTTON_SWIM:
		case sa::WidgetIDs::WIDGET_BUTTON_DIVE:
			if (!ScriptCommand(&is_actor_in_the_water, pPlayerPed->m_gtaId)) {
				return STATE_DISABLED;
			}
			break;

		case sa::WidgetIDs::WIDGET_ATTACK:
			if (pPlayerPed->IsInPassengerDriveByMode()) {
				return STATE_NONE;
			}
			else if (pPlayerPed->IsInVehicle() && !pPlayerPed->IsAPassenger()) {
				return STATE_DISABLED;
			}
			break;

		case sa::WidgetIDs::WIDGET_BUTTON_SPRINT:
		case sa::WidgetIDs::WIDGET_SPRINT:
			if (pPlayerPed->IsInVehicle() || pPlayerPed->IsInJetpack() || ScriptCommand(&is_actor_in_the_water, pPlayerPed->m_gtaId)) {
				return STATE_DISABLED;
			}
			break;

		case sa::WidgetIDs::WIDGET_HORN:
		case sa::WidgetIDs::WIDGET_NITRO:
			if (!pPlayerPed->IsInVehicle()) {
				return STATE_DISABLED;
			}
			break;

		case sa::WidgetIDs::WIDGET_VEHICLE_SHOOT_LEFT:
		case sa::WidgetIDs::WIDGET_VEHICLE_SHOOT_RIGHT:
			if (!pPlayerPed->IsInVehicle() || pPlayerPed->IsAPassenger()) {
				return STATE_DISABLED;
			}
			break;

		case sa::WidgetIDs::WIDGET_ROCKET:
			if (pNetGame) {
				if (!pPlayerPed->IsInVehicle()) {
					return STATE_DISABLED;
				}
				else {
					if (!pPlayerPed->GetGtaVehicle()) return STATE_NONE;
					int model = pPlayerPed->GetGtaVehicle()->m_nModelIndex;
					if (pPlayerPed->IsAPassenger()) {
						return STATE_DISABLED;
					}
					// if is armed vehicle
					switch (model) {
						// Firetruck
						case 407:
							// hunter
						case 425:
							// Rhino Tank
						case 432:
							// Seasparrow
						case 447:
							// RC Baron
						case 464:
							// RC Raider
						case 465:
							// Rustler
						case 476:
							// Hydra
						case 520: return STATE_NONE;
							// block other non-armed vehicle enable shoot button
						default: return STATE_DISABLED;
					}
				}
			}
			break;

		case sa::WidgetIDs::WIDGET_DROP_CRANE:
		case sa::WidgetIDs::WIDGET_CRANE_UP:
		case sa::WidgetIDs::WIDGET_CRANE_DOWN:
			if (!pPlayerPed->IsInVehicle()) {
				return STATE_DISABLED;
			}
			break;

		case sa::WidgetIDs::WIDGET_ACCELERATE:
		case sa::WidgetIDs::WIDGET_BRAKE:
			if (!pPlayerPed->IsInVehicle() && !pPlayerPed->IsInJetpack()) {
				return STATE_DISABLED;
			}
			break;

		case sa::WidgetIDs::WIDGET_ENTER_CAR:
			if (pPlayerPed->IsInJetpack()) {
				return STATE_NONE;
			}

			if (pNetGame) {
				CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
				if (pVehiclePool) {
					// Get the vehicles near the player
					VEHICLEID vehicleId = pVehiclePool->FindNearestToLocalPlayerPed();
					if (vehicleId == INVALID_VEHICLE_ID) {
						return STATE_DISABLED;
					}
					else {
						CVehicle* pVehicle = pVehiclePool->GetAt(vehicleId);
						if (pVehicle) {
							// player is not in the car and the distance between the vehicle and the player > 10
							if (!pPlayerPed->IsInVehicle() && pVehicle->GetDistanceFromLocalPlayerPed() > 10.0f) {
								// Disable button display
								return STATE_DISABLED;
							}
						}
					}
				}
			}
			break;
	}

	return STATE_NONE;
}

/* =========== TexturedTextDraw ============= */

bool bTextDrawTextureSlotState[200];
RwTexture* TextDrawTexture[200];

void ResetTextDrawTextures()
{
	memset(bTextDrawTextureSlotState, 0, sizeof(bTextDrawTextureSlotState));
	memset(TextDrawTexture, 0, sizeof(TextDrawTexture));
}

int GetFreeTextDrawTextureSlot()
{
	for (int i = 0; i < 200; i++) {
		if (!bTextDrawTextureSlotState[i]) {
			bTextDrawTextureSlotState[i] = true;
			return i;
		}
	}

	return -1;
}

void DestroyTextDrawTexture(int index)
{
	if (index >= 0 && index < 200) {
		if (TextDrawTexture[index]) {
			RwTextureDestroy(TextDrawTexture[index]);
			TextDrawTexture[index] = nullptr;
			bTextDrawTextureSlotState[index] = false;
		}
	}
}

/**
 * @param texture
 * @param rect
 * @param dwColor
 * @param uv
 */
void DrawTextureUV(RwTexture* texture, sa::CRect* rect, uint32_t dwColor, float* uv)
{
	if (texture) {
		RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*) rwFILTERLINEAR);
		((sa::CSprite2d*) texture)->Draw(rect, dwColor, uv);
	}
}

/**
 * @param x
 * @param y
 * @param rect
 * @return
 */
bool IsPointInRect(float x, float y, sa::CRect* rect)
{
	if (x >= rect->left && x <= rect->right && y >= rect->bottom && y <= rect->top) {
		return true;
	}

	return false;
}

uintptr_t ModelInfoCreateInstance(int iModel)
{
	sa::CBaseModelInfo* modelInfo = GetModelInfoByID(iModel);
	if (modelInfo) {
		// CXXXModelInfo::CreateInstance(void)
		return Memory::callFunction<uintptr_t>(g_saSym->GetVmtFunction(modelInfo->vtable, 11), modelInfo);
	}

	return 0;
}

/**
 * @param rwObject
 */
void RenderClumpOrAtomic(uintptr_t rwObject)
{
	if (rwObject) {
		if (*(uint8_t*) rwObject == 1) {
			// Atomic
			spdlog::info("Render Atomic!");
			Memory::callFunction("_Z27AtomicDefaultRenderCallBackP8RpAtomic", rwObject);
		}
		else if (*(uint8_t*) rwObject == 2) {
			spdlog::info("Render Clump!");
			// rpClumpRender
			Memory::callFunction("_Z13RpClumpRenderP7RpClump", rwObject);
		}
	}
}

void ProcessLineOfSight(sa::CVector* vecStart, sa::CVector* vecEnd, sa::CColPoint* colPoint, sa::CEntity** refEntityPtr,
		bool bCheckBuildings, bool bCheckVehicles, int bCheckPeds, bool bCheckObjects, bool bCheckDummies, bool bSeeThroughStuff, bool bIgnoreSomeObjectsForCamera, bool bShootThroughStuff)
{
	// CWorld::ProcessLineOfSight(CVector const&,CVector const&,CColPoint &,CEntity *&,bool,bool,bool,bool,bool,bool,bool,bool)
	Memory::callFunction("_ZN6CWorld18ProcessLineOfSightERK7CVectorS2_R9CColPointRP7CEntitybbbbbbbb", vecStart, vecEnd, colPoint, refEntityPtr, bCheckBuildings, bCheckVehicles, bCheckPeds, bCheckObjects, bCheckDummies, bSeeThroughStuff, bIgnoreSomeObjectsForCamera, bShootThroughStuff);
}
