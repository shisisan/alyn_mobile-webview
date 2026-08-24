#pragma once

#include <cstring>

class CEntity;

class CVehicle : public CEntity {
public:
	CVehicle(int iType, float fX, float fY, float fZ, float fRotation, bool bPreloaded, bool bSiren);
	virtual ~CVehicle();

	int GetVehicleSubtype();
	void AddComponent(int iComponentID);
	void RemoveComponent(int iComponentID);
	void SetPaintJob(uint8_t bytePaintJobID);
	void SetColor(uint8_t iColor1, uint8_t iColor2);
	void LinkToInterior(int iInterior);
	void SetDamageStatus(uint32_t dwPanelDamageStatus, uint32_t dwDoorDamageStatus, uint8_t byteLightDamageStatus);
	void SetTireDamageStatus(uint8_t byteTireDamageStatus);
	void SetPlateText(const char* szPlateText) { strncpy(m_szPlateText, szPlateText, 32); }
	void SetZAngle(float fAngle);
	void ProcessMarkers();
	bool IsOccupied();

	void AttachTrailer();
	void DetachTrailer();
	void SetTrailer(CVehicle* pTrailer);

	bool IsRCVehicle();
	float GetHealth();
	void SetHealth(float fHealth);
	CVehicle* GetTrailer();
	CVehicle* GetTractor();
	float GetTrainSpeed();
	uint16_t GetHydraThrusters();
	float GetBikeLean();
	bool IsATrainPart();
	bool VerifyInstance();
	bool IsDriverLocalPlayer();
	void SetInvulnerable(bool bInv);
	bool HasSunk();
	bool HasADriver();

	void SetEngineState(bool state)
	{
		m_pVehicle->SetEngineStatus(state);
	}

	int GetEngineState()
	{
		return m_pVehicle->GetEngineStatus();
	}

	void SetLightState(bool state);

	int GetLightState()
	{
		//return m_pVehicle->GetLightsStatus();
		return m_bLightState;
	}

	void SetDoorState(int state);
	bool GetDoorState();

	void SetSirenOn(uint8_t state)
	{
		m_pVehicle->m_nVehicleFlags.bSirenOrAlarm = state;
	}

	bool IsSirenOn()
	{
		return m_pVehicle->m_nVehicleFlags.bSirenOrAlarm;
	}

	void SetHydraThrusters(int state);

	void SetAlarmState(uint16_t wAlarmState) { m_pVehicle->m_nAlarmState = wAlarmState; }
	uint16_t GetAlarmState() { return m_pVehicle->m_nAlarmState; }

	void SetComponentOpenState(int iDoor, int iComponent, float fDoorOpenRatio);

	void OpenWindow(uint8_t component);
	void CloseWindow(uint8_t component);

	void RemoveEveryoneFromVehicle();

	bool HasTurret()
	{
		if (!m_pVehicle) return false;

		int nModel = GetModelIndex();
		return (nModel == 432 ||    // Tank
				nModel == 564 ||    // RC Tank
				nModel == 407 ||    // Firetruck
				nModel == 601       // Swatvan
		);
	}

	bool IsATowTruck()
	{
		if (!m_pVehicle) return false;
		return GetModelIndex() == 525;
	}

	bool IsATrailer()
	{
		if (!m_pVehicle) return false;

		int nModel = GetModelIndex();
		return (nModel == 435 ||
				nModel == 450 ||
				nModel == 584 ||
				nModel == 591 ||
				nModel == 606 ||
				nModel == 607 ||
				nModel == 608 ||
				nModel == 610 ||
				nModel == 611
		);
	}

public:
	sa::CVehicle* m_pVehicle;
	CVehicle* m_pTrailer;
	bool m_bPreloaded;
	bool m_bIsInvulnerable;
	bool m_bIsLocked;
	bool m_bLightState;
	bool m_bDoorsLocked;
	uint8_t m_byteObjectiveVehicle;
	bool m_bSpecialMarkerEnabled;
	uint8_t m_byteColor1;
	uint8_t m_byteColor2;
	bool m_bHaveColor;
	bool m_bSiren;
	char m_szPlateText[32];
	uint32_t m_dwMarkerID;
};