#pragma once

#define INVALID_OBJECT_ID    0xFFFF

class CObjectPool {
public:
	CObjectPool();
	~CObjectPool();

	bool New(OBJECTID ObjectID, int iModel, sa::CVector vecPos, sa::CVector vecRot, float fDrawDistance);
	bool Delete(OBJECTID ObjectID);

	void Process();

	CObject* GetAt(OBJECTID ObjectID)
	{
		if (ObjectID <= MAX_OBJECTS && m_bObjectSlotState[ObjectID]) {
			return m_pObjects[ObjectID];
		}

		return nullptr;
	}

	OBJECTID FindIDFromGtaPtr(sa::CEntity* pGtaObject);

	CObject* FindObjectFromGtaPtr(sa::CEntity* pGtaObject);

	void ProcessMaterialText();

public:
	int m_iObjectCount;
	bool m_bObjectSlotState[MAX_OBJECTS];
	CObject* m_pObjects[MAX_OBJECTS];
};