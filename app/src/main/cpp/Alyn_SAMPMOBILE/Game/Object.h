#pragma once

enum {
	MAX_MATERIALS = 16
};
enum eMaterialType {
	MATERIAL_TYPE_NONE = 0,
	MATERIAL_TYPE_TEXTURE = 1,
	MATERIAL_TYPE_TEXT = 2
};
enum eMaterialSize {
	MATERIAL_SIZE_32X32 = 10,
	MATERIAL_SIZE_64X32 = 20,
	MATERIAL_SIZE_64X64 = 30,
	MATERIAL_SIZE_128X32 = 40,
	MATERIAL_SIZE_128X64 = 50,
	MATERIAL_SIZE_128X128 = 60,
	MATERIAL_SIZE_256X32 = 70,
	MATERIAL_SIZE_256X64 = 80,
	MATERIAL_SIZE_256X128 = 90,
	MATERIAL_SIZE_256256 = 100,
	MATERIAL_SIZE_512X64 = 110,
	MATERIAL_SIZE_512X128 = 120,
	MATERIAL_SIZE_512X256 = 130,
	MATERIAL_SIZE_512X512 = 140
};
enum eMaterialTextAlign {
	MATERIAL_ALIGN_LEFT = 0,
	MATERIAL_ALIGN_CENTER = 1,
	MATERIAL_ALIGN_RIGHT = 2
};

typedef struct _MATERIAL_OBJECT {
	int iType;
	bool bTextureWasCreated;

	typedef struct _MATERIAL_TEXT {
		uint8_t byteMaterialSize;
		char szFontName[32];
		uint8_t byteFontSize;
		uint8_t byteFontBold;
		uint32_t dwFontColor;
		uint32_t dwBackgroundColor;
		uint8_t byteAlign;
		char szText[2048];
	} MATERIAL_TEXT;

	MATERIAL_TEXT TextInfo;
	RwTexture* pMaterialTexture;
	uint32_t dwMaterialColor;
} MATERIAL_OBJECT;

class CObject : public CEntity {
public:
	CObject(int iModel, sa::CVector vecPos, sa::CVector vecRot, float fDrawDistance, uint8_t bAttached);
	~CObject() override;

	void Process(float fElapsedTime);

	void TeleportTo(float fX, float fY, float fZ) override;

	void SetScale(sa::CVector vecScale);
	sa::CVector GetScale();
	void SetRotation(sa::CVector* vecRotation);
	void GetRotation(float* pfX, float* pfY, float* pfZ);
	void RotateMatrix(sa::CVector vecRot);
	void ApplyMoveSpeed();
	float DistanceRemaining(RwMatrix* matPos);

	void MoveTo(float fX, float fY, float fZ, float fSpeed, float fRotX, float fRotY, float fRotZ);
	void StopMoving();

	void SetAttachedObject(uint16_t ObjectID, sa::CVector* vecPos, sa::CVector* vecRot, bool bSyncRotation);
	void SetAttachedVehicle(uint16_t VehicleID, sa::CVector* vecPos, sa::CVector* vecRot);

	void AttachToVehicle(CVehicle* pVehicle);
	void AttachToObject(CObject* pObject);

	bool AttachedToMovingEntity();

	void SetMaterial(int iModel, uint8_t byteMaterialIndex, char* txdname, char* texturename, uint32_t dwColor);
	void SetMaterialText(uint8_t byteMaterialIndex, uint8_t byteMaterialSize, const char* szFontName, uint8_t byteFontSize, uint8_t byteFontBold, uint32_t dwFontColor, uint32_t dwBackgroundColor, uint8_t byteAlign, const char* szText);
	void GetMaterialTextSize(int iMatSize, int* iSizeX, int* iSizeY);
	void ProcessMaterialText();

	void TryChangeToCustomObjectMaterial();
	void ChangeToCustomObjectMaterial();
	void ChangeToOriginalObjectMaterial();

public:
	bool m_bHasCustomObjectMaterial;
	MATERIAL_OBJECT m_Material[MAX_MATERIALS];
	RwTexture* m_pLastObjectMaterialTexture[MAX_MATERIALS];
	RwRGBA m_dwLastObjectMaterialColor[MAX_MATERIALS];

	bool m_bObjectMaterialTextureChanged;
	bool m_bObjectMaterialColorChanged;
	unsigned int m_uiLastObjectGeometry;
	bool m_bAllowChangeObjectMaterial;
	RwObject* m_pLastObjectMaterialRwObject;

	/* attach */
	uint16_t m_AttachedVehicleID;
	uint16_t m_AttachedObjectID;
	sa::CVector m_vecAttachedPos;
	sa::CVector m_vecAttachedRot;
	bool m_bSyncRotation;

	bool m_bAttachedToPed;
	bool m_bForceRender;

	uint8_t m_byteMoving;

private:
	sa::CVector m_vecRotation;
	int m_iModel;

	float m_fMoveSpeed;
	bool m_bNeedRotate;
	RwMatrix m_matTarget;
	sa::CQuaternion m_quatStart;
	sa::CQuaternion m_quatTarget;
	sa::CVector m_vecRotationTarget;
	sa::CVector m_vecSubRotationTarget;
	float m_fDistanceToTargetPoint;
	uint32_t m_dwMoveTick;
};
