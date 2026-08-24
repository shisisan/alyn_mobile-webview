#pragma once

uint32_t GetTickCount();

sa::CPed* GamePool_FindPlayerPed();
sa::CPed* GamePool_Ped_GetAt(int slot);
int GamePool_Ped_GetIndex(sa::CPed* pActor);
sa::CObject* GamePool_Object_GetAt(int iID);
uint32_t GamePool_Vehicle_GetIndex(sa::CVehicle* pGtaVehicle);
sa::CVehicle* GamePool_Vehicle_GetAt(int iID);

int GetVehicleSubtype(sa::CVehicle* pGtaVehicle);

sa::CBaseModelInfo* GetModelInfoByID(int iModelID);
uint32_t ModelInfoGetFlags(int iModelID);
uint16_t ModelInfoGetReferenceCount(int iModelID);
bool IsExistInfoForModel(int iModelID);
bool IsValidModel(int iModelID);
bool IsValidPedModel(uint modelID);
int GetModelRefCounts(int iModel);
RwObject* GetModelRWObject(uint modelID);

RwTexture* LoadTextureFromTxd(const char* txdname, const char* texturename);
bool AddTxdToSlot(const char* szTxdName);

float FloatOffset(float f1, float f2);
float DegToRad(float fDegress);
float fixAngle(float angle);
float subAngle(float a1, float a2);

void SetPlayerPedPtrRecord(uint8_t bytePlayer, sa::CPed* pPed);
uint8_t FindPlayerNumFromPedPtr(sa::CPed* dwPedPtr);

int GameGetWeaponModelIDFromWeaponID(int iWeaponID);

int GetFreeTextDrawTextureSlot();
void ResetTextDrawTextures();
void DestroyTextDrawTexture(int index);

void DrawTextureUV(RwTexture* texture, sa::CRect* rect, uint32_t dwColor, float* uv);

bool IsPointInRect(float x, float y, sa::CRect* rect);

uintptr_t ModelInfoCreateInstance(int iModel);

void RenderClumpOrAtomic(uintptr_t rwObject);

float GetModelColSphereRadius(int iModel);

void GetModelColSphereVecCenter(int iModel, sa::CVector* vec);

void DestroyAtomicOrClump(RwObject* rwObject);

void GamePrepareTrain(sa::CVehicle* pGtaVehicle);

void GameResetStats();

void ProjectMatrix(sa::CVector* vecOut, sa::CMatrix* mat, sa::CVector* vecPos);
int RwMatrixRotate(RwMatrix* mat, int axis, float angle);
void RwMatrixScale(RwMatrix* mat, sa::CVector* vecScale);

void MatrixInvertOrthonormal(RwMatrix* dst, const RwMatrix* src);
RwMatrix* MatrixInvertGeneral(RwMatrix* dst, const RwMatrix* src);
RwMatrix* RwMatrixInvert(RwMatrix* dst, const RwMatrix* src);

void DefinedState();

void SetRenderWareCamera(RwCamera* camera);

void rpWorldAddCamera(RwCamera* camera);
void rpWorldAddLight(RpLight* rpLight);
void rpWorldRemoveLight(RpLight* rpLight);

int GetTaskTypeFromTask(sa::CTask** task);

bool IsEntityPlaceable(sa::CEntity* pEntity);

void CalcScreenCoors(const RwV3d* In, RwV3d* pResult, float pScaleX, float pScaleY, bool ClipFarPlane, bool ClipNearPlane);

// widget fix
enum eWidgetState {
	STATE_NONE,
	STATE_DISABLED
};

extern uintptr_t* g_pWidgets;

eWidgetState ProcessFixedWidget(uintptr_t pWidget);

void ProcessLineOfSight(sa::CVector* vecStart, sa::CVector* vecEnd, sa::CColPoint* colPoint, sa::CEntity** refEntityPtr,
		bool bCheckBuildings, bool bCheckVehicles, int bCheckPeds, bool bCheckObjects, bool bCheckDummies, bool bSeeThroughStuff, bool bIgnoreSomeObjectsForCamera, bool bShootThroughStuff);
