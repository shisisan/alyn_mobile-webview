#pragma once

#include "../UI/ImGuiRenderer.h"

#pragma pack(push, 1)
typedef struct _TEXT_LABEL {
	char text[2048 + 1];
	uint32_t dwColor;
	sa::CVector vecPos;
	float fDistance;
	uint8_t bTestLOS;
	PLAYERID playerId;
	VEHICLEID vehicleId;
	sa::CVector vecOffsetCoords;
} TEXT_LABEL;
#pragma pack(pop)

class CTextLabelPool {
public:
	CTextLabelPool();
	~CTextLabelPool();

	bool GetSlotState(uint16_t wLabelId)
	{
		if (wLabelId < MAX_TEXT_LABELS && m_bSlotUsed[wLabelId]) {
			return true;
		}

		return false;
	}

	void NewLabel(uint16_t wLabelId, TEXT_LABEL* label);
	void ClearLabel(uint16_t wLabelId);

	void Render(ImGuiRenderer* renderer);

private:
	void Draw(ImGuiRenderer* renderer, sa::CVector vecPos, const char* text, uint32_t dwColor);

private:
	TEXT_LABEL m_TextLabels[MAX_TEXT_LABELS];
	bool m_bSlotUsed[MAX_TEXT_LABELS];
};