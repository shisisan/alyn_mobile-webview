#pragma once

class SnapShotHelper {
public:
	SnapShotHelper();

	RwTexture* CreatePedSnapShot(int iModel, uint32_t dwColor, sa::CVector* vecRot, float fZoom);
	RwTexture* CreateVehicleSnapShot(int iModel, uint32_t dwColor, sa::CVector* vecRot, float fZoom, int dwColor1, int dwColor2);
	RwTexture* CreateObjectSnapShot(int iModel, uint32_t dwColor, sa::CVector* vecRot, float fZoom);

private:
	void SetUpScene();

	RwCamera* m_camera;
	RpLight* m_light;
	RwFrame* m_frame;
	RwRaster* m_raster;
};