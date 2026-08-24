#include "Game.h"
#include <GLES2/gl2.h>

extern Game* pGame;

SnapShotHelper::SnapShotHelper()
{
	m_camera = nullptr;
	m_light = nullptr;
	m_frame = nullptr;
	m_raster = nullptr;

	SetUpScene();
}

void SnapShotHelper::SetUpScene()
{
	m_light = RpLightCreate(2);
	if (!m_light) return;

	RwRGBAReal rwColor = {1.0f, 1.0f, 1.0f, 1.0f};
	RpLightSetColor(m_light, &rwColor);

	m_camera = RwCameraCreate();
	m_frame = RwFrameCreate();
	if (!m_camera || !m_frame) return;

	RwV3d v = {0.0f, 0.0f, 50.0f};
	RwFrameTranslate(m_frame, &v, (RwOpCombineType) rwCOMBINEPRECONCAT);

	v = {1.0f, 0.0f, 0.0f};
	RwFrameRotate(m_frame, &v, 90.0f, (RwOpCombineType) rwCOMBINEPRECONCAT);

	m_raster = RwRasterCreate(256, 256, 0, rwRASTERTYPEZBUFFER);
	m_camera->zBuffer = m_raster;

	RwObjectHasFrameSetFrame(m_camera, m_frame);

	RwCameraSetFarClipPlane(m_camera, 300.0f);
	RwCameraSetNearClipPlane(m_camera, 0.01f);

	RwV2d view = {0.5f, 0.5f};
	RwCameraSetViewWindow(m_camera, &view);
	RwCameraSetProjection(m_camera, rwPERSPECTIVE);

	rpWorldAddCamera(m_camera);
}

/**
 * @param iModel
 * @param dwColor
 * @param vecRot
 * @param fZoom
 * @return
 */
RwTexture* SnapShotHelper::CreatePedSnapShot(int iModel, uint32_t dwColor, sa::CVector* vecRot, float fZoom)
{
	spdlog::info("CreatePedSnapShot: iModel: {}, dwColor: {}, vecRot: {} {} {}, fZoom: {}", iModel, dwColor, vecRot->x, vecRot->y, vecRot->z, fZoom);

	RwRaster* raster = RwRasterCreate(256, 256, 32, rwRASTERFORMAT8888 | rwRASTERTYPECAMERATEXTURE);
	if (!raster) return nullptr;

	RwTexture* bufferTexture = RwTextureCreate(raster);
	if (!bufferTexture) return nullptr;

	auto pPed = new CActor(0, 0.0f, 0.0f, 0.0f, 0.0f);
	if (!pPed) {
		spdlog::error("CreatePedSnapShot: Failed to create ped");
		return nullptr;
	}

	float posZ = iModel == 162 ? 50.15f : 50.05f;
	float posY = fZoom * -2.25f;
	pPed->TeleportTo(0.0f, posY, posZ);
	pPed->SetModelIndex(iModel);
	pPed->SetGravityProcessing(false);
	pPed->SetCollisionChecking(false);

	RwMatrix mat = pPed->m_actor->GetMatrix().ToRwMatrix();

	if (vecRot->x != 0.0f) RwMatrixRotate(&mat, 0, vecRot->x);
	if (vecRot->y != 0.0f) RwMatrixRotate(&mat, 1, vecRot->y);
	if (vecRot->z != 0.0f) RwMatrixRotate(&mat, 2, vecRot->z);

	pPed->UpdateMatrix(mat);

	m_camera->frameBuffer = raster;
	SetRenderWareCamera(m_camera);

	RwCameraClear(m_camera, (RwRGBA*) &dwColor, 3);
	RwCameraBeginUpdate(m_camera);

	rpWorldAddLight(m_light);

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*) true);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*) true);
	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*) rwSHADEMODENASHADEMODE);
	RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*) 0);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*) rwCULLMODENACULLMODE);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*) false);
	DefinedState();

	pPed->Add();

	if (pPed->m_actor) {
		if (pPed->m_actor->m_pRpClump) {
			// RpAnimBlendClumpUpdateAnimations
			Memory::callFunction("_Z32RpAnimBlendClumpUpdateAnimationsP7RpClumpfb", pPed->m_actor->m_pRpClump, 100.0f, true);
		}
	}

	pPed->Render();

	RwCameraEndUpdate(m_camera);

	rpWorldRemoveLight(m_light);

	pPed->Remove();
	if (pPed) {
		delete pPed;
		pPed = nullptr;
	}

	++bufferTexture->refCount;
	return bufferTexture;
}

/**
 * @param iModel
 * @param dwColor
 * @param vecRot
 * @param fZoom
 * @param dwColor1
 * @param dwColor2
 * @return
 */
RwTexture* SnapShotHelper::CreateVehicleSnapShot(int iModel, uint32_t dwColor, sa::CVector* vecRot, float fZoom, int dwColor1, int dwColor2)
{
	spdlog::info("CreateVehicleSnapShot: iModel: {}, dwColor: {}, vecRot: {} {} {}, fZoom: {}, dwColor1: {}, dwColor2: {}", iModel, dwColor, vecRot->x, vecRot->y, vecRot->z, fZoom, dwColor1, dwColor2);

	RwRaster* raster = RwRasterCreate(256, 256, 32, rwRASTERFORMAT8888 | rwRASTERTYPECAMERATEXTURE);
	if (!raster) return nullptr;

	RwTexture* bufferTexture = RwTextureCreate(raster);
	if (!bufferTexture) return nullptr;

	if (iModel == 570) { iModel = 538; }
	else if (iModel == 569) iModel = 537;

	auto pVehicle = new CVehicle(iModel, 0.0f, 0.0f, 50.0f, 0.0f, false, false);
	if (!pVehicle) {
		spdlog::error("CreateVehicleSnapShot: Failed to create vehicle");
		return nullptr;
	}

	pVehicle->SetGravityProcessing(false);
	pVehicle->SetCollisionChecking(false);

	float fRadius = GetModelColSphereRadius(iModel);
	float posY = (-1.0 - (fRadius + fRadius)) * fZoom;

	if (pVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_BOAT) {
		posY = -5.5 - fRadius * 2.5;
	}

	pVehicle->TeleportTo(0.0f, posY, 50.0f);

	if (dwColor1 != 0xFFFF && dwColor2 != 0xFFFF) {
		pVehicle->SetColor(dwColor1, dwColor2);
	}

	RwMatrix mat = pVehicle->m_pVehicle->GetMatrix().ToRwMatrix();

	if (vecRot->x != 0.0f) RwMatrixRotate(&mat, 0, vecRot->x);
	if (vecRot->y != 0.0f) RwMatrixRotate(&mat, 1, vecRot->y);
	if (vecRot->z != 0.0f) RwMatrixRotate(&mat, 2, vecRot->z);

	pVehicle->UpdateMatrix(mat);

	m_camera->frameBuffer = raster;
	SetRenderWareCamera(m_camera);

	RwCameraClear(m_camera, (RwRGBA*) &dwColor, 3);
	RwCameraBeginUpdate(m_camera);

	rpWorldAddLight(m_light);

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*) true);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*) true);
	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*) rwSHADEMODENASHADEMODE);
	RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*) 0);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*) rwCULLMODENACULLMODE);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*) false);
	DefinedState();

	pVehicle->Add();
	pVehicle->Render();

	RwCameraEndUpdate(m_camera);

	rpWorldRemoveLight(m_light);

	pVehicle->Remove();
	if (pVehicle) {
		delete pVehicle;
		pVehicle = nullptr;
	}

	++bufferTexture->refCount;
	return bufferTexture;
}

/**
 * @param iModel
 * @param dwColor
 * @param vecRot
 * @param fZoom
 * @return
 */
RwTexture* SnapShotHelper::CreateObjectSnapShot(int iModel, uint32_t dwColor, sa::CVector* vecRot, float fZoom)
{
	spdlog::info("CreateObjectSnapShot: iModel: {}, dwColor: {}, vecRot: {} {} {}, fZoom: {}", iModel, dwColor, vecRot->x, vecRot->y, vecRot->z, fZoom);

	RwRaster* raster = RwRasterCreate(256, 256, 32, rwRASTERFORMAT8888 | rwRASTERTYPECAMERATEXTURE);
	if (!raster) return nullptr;

	RwTexture* bufferTexture = RwTextureCreate(raster);
	if (!bufferTexture) return nullptr;

	if (iModel == 1373 || iModel == 3118 || iModel == 3552 || iModel == 3553) {
		iModel = 18631;
	}

	bool bNeedRemoveModel = false;
	if (!pGame->IsModelLoaded(iModel)) {
		bNeedRemoveModel = true;
		pGame->RequestModel(iModel);
		pGame->LoadRequestedModels();
		while (!pGame->IsModelLoaded(iModel)) usleep(1);
	}

	auto pRwObject = reinterpret_cast<RwObject*>(ModelInfoCreateInstance(iModel));
	if (!pRwObject) {
		spdlog::error("CreateObjectSnapShot: Failed to create object");
		return nullptr;
	}

	float fRadius = GetModelColSphereRadius(iModel);

	sa::CVector vecCenter = {0.0f, 0.0f, 0.0f};
	GetModelColSphereVecCenter(iModel, &vecCenter);

	auto parent = static_cast<RwFrame*>(pRwObject->parent);
	if (!parent) return nullptr;

	sa::CVector v = {-vecCenter.x, (-0.1f - fRadius * 2.25f) * fZoom, 50.0f - vecCenter.z};
	RwFrameTranslate(parent, &v, rwCOMBINEPRECONCAT);

	if (iModel == 18631) {
		v.x = 0.0f;
		v.y = 0.0f;
		v.z = 1.0f;
		RwFrameRotate(parent, &v, 180.0f, rwCOMBINEPRECONCAT);
	}
	else {
		if (vecRot->x != 0.0f) {
			v.x = 1.0f;
			v.y = 0.0f;
			v.z = 0.0f;
			RwFrameRotate(parent, &v, vecRot->x, rwCOMBINEPRECONCAT);
		}

		if (vecRot->y != 0.0f) {
			v.x = 0.0f;
			v.y = 1.0f;
			v.z = 0.0f;
			RwFrameRotate(parent, &v, vecRot->y, rwCOMBINEPRECONCAT);
		}

		if (vecRot->z != 0.0f) {
			v.x = 0.0f;
			v.y = 0.0f;
			v.z = 1.0f;
			RwFrameRotate(parent, &v, vecRot->z, rwCOMBINEPRECONCAT);
		}
	}

	m_camera->frameBuffer = raster;
	SetRenderWareCamera(m_camera);

	RwCameraClear(m_camera, (RwRGBA*) &dwColor, 3);
	RwCameraBeginUpdate(m_camera);

	rpWorldAddLight(m_light);

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*) true);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*) true);
	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*) rwSHADEMODENASHADEMODE);
	RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*) nullptr);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*) rwCULLMODENACULLMODE);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*) false);
	DefinedState();

	RenderClumpOrAtomic(reinterpret_cast<uintptr_t>(pRwObject));

	RwCameraEndUpdate(m_camera);

	rpWorldRemoveLight(m_light);

	DestroyAtomicOrClump(pRwObject);

	if (bNeedRemoveModel) {
		pGame->RemoveModel(iModel, false);
	}

	++bufferTexture->refCount;
	return bufferTexture;
}
