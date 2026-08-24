#pragma once

class CCamera {
public:
	CCamera();
	~CCamera() = default;

	void SetBehindPlayer();
	void SetPosition(float fX, float fY, float fZ, float fRotationX, float fRotationY, float fRotationZ);
	void LookAtPoint(float fX, float fY, float fZ, int iType);
	void Restore();
	void InterpolateCameraPos(sa::CVector* vecFrom, sa::CVector* vecTo, int iTime, uint8_t byteMode);
	void InterpolateCameraLookAt(sa::CVector* vecFrom, sa::CVector* vecTo, int iTime, uint8_t byteMode);

	void GetMatrix(RwMatrix* Matrix);

public:
	RwMatrix* m_matPos;
};
