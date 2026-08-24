//
// Created by ALYN on 2024/8/30.
//
#include "Game.h"
#include "../UI/UI.h"

extern UI* pUI;
extern Game* pGame;
extern SnapShotHelper* pSnapShotHelper;

extern RwTexture* TextDrawTexture[];

CTextDraw::CTextDraw(TEXT_DRAW_TRANSMIT* pTextDrawTransmit, const char* szText)
{
	memset(&m_TextDrawData, 0, sizeof(TEXT_DRAW_DATA));

	m_TextDrawData.fLetterWidth = pTextDrawTransmit->fLetterWidth;
	m_TextDrawData.fLetterHeight = pTextDrawTransmit->fLetterHeight;

	m_TextDrawData.dwLetterColor = pTextDrawTransmit->dwLetterColor;
	m_TextDrawData.byteUnk12 = 0;
	m_TextDrawData.byteCentered = pTextDrawTransmit->byteCenter;
	m_TextDrawData.byteBox = pTextDrawTransmit->byteBox;

	m_TextDrawData.fLineWidth = pTextDrawTransmit->fLineWidth;
	m_TextDrawData.fLineHeight = pTextDrawTransmit->fLineHeight;

	m_TextDrawData.dwBoxColor = pTextDrawTransmit->dwBoxColor;
	m_TextDrawData.byteProportional = pTextDrawTransmit->byteProportional;
	m_TextDrawData.dwBackgroundColor = pTextDrawTransmit->dwBackgroundColor;
	m_TextDrawData.byteShadow = pTextDrawTransmit->byteShadow;
	m_TextDrawData.byteOutline = pTextDrawTransmit->byteOutline;
	m_TextDrawData.byteAlignLeft = pTextDrawTransmit->byteLeft;
	m_TextDrawData.byteAlignRight = pTextDrawTransmit->byteRight;
	m_TextDrawData.dwStyle = pTextDrawTransmit->byteStyle;

	m_TextDrawData.fX = pTextDrawTransmit->fX;
	m_TextDrawData.fY = pTextDrawTransmit->fY;

	m_TextDrawData.dwParam1 = 0xFFFFFFFF;
	m_TextDrawData.dwParam2 = 0xFFFFFFFF;
	m_TextDrawData.byteSelectable = pTextDrawTransmit->byteSelectable;
	m_TextDrawData.wModelID = pTextDrawTransmit->wModelID;
	m_TextDrawData.vecRot.x = pTextDrawTransmit->vecRot.x;
	m_TextDrawData.vecRot.y = pTextDrawTransmit->vecRot.y;
	m_TextDrawData.vecRot.z = pTextDrawTransmit->vecRot.z;
	m_TextDrawData.fZoom = pTextDrawTransmit->fZoom;
	m_TextDrawData.wColor1 = pTextDrawTransmit->wColor1;
	m_TextDrawData.wColor2 = pTextDrawTransmit->wColor2;
	m_TextDrawData.bHasKeyCode = false;
	m_TextDrawData.iTextureSlot = -1;
	SetText(szText);

	if (m_TextDrawData.dwStyle == 4) {
		m_TextDrawData.iTextureSlot = GetFreeTextDrawTextureSlot();
		LoadTexture();
	}

	m_TextDrawData.bHasRectArea = false;
	m_rectArea.left = 0.0f;
	m_rectArea.right = 0.0f;
	m_rectArea.bottom = 0.0f;
	m_rectArea.top = 0.0f;
	m_bHovered = false;
	m_dwHoverColor = 0;
}

CTextDraw::~CTextDraw()
{
	DestroyTextDrawTexture(m_TextDrawData.iTextureSlot);
}

/**
 * @param szText
 */
void CTextDraw::SetText(const char* szText)
{
	memset(m_szText, 0, 800);
	strncpy(m_szText, szText, 800);
	m_szText[800] = 0;

	if (m_TextDrawData.dwStyle == 4 && m_TextDrawData.iTextureSlot != -1) {
		DestroyTextDrawTexture(m_TextDrawData.iTextureSlot);
		LoadTexture();
	}
}

void CTextDraw::LoadTexture()
{
	char txdname[64 + 1];
	memset(txdname, 0, sizeof(txdname));
	char texturename[64 + 1];
	memset(texturename, 0, sizeof(texturename));

	char* szTexture = strchr(m_szText, ':');
	if (szTexture == nullptr) return;

	if (strlen(m_szText) < 64 && strchr(m_szText, '\\') == nullptr && strchr(m_szText, '/') == nullptr) {
		strncpy(txdname, m_szText, (size_t) (szTexture - m_szText));
		strcpy(texturename, ++szTexture);

		spdlog::info("LOADTEXTURE: txdname: {}, texture: {}", txdname, texturename);

		if (!strcasecmp(txdname, "ld_shtr") && !strcasecmp(texturename, "ship")) {
			strcpy(texturename, "ship_ld_shtr");
		}
		else if (!strcasecmp(txdname, "ld_spac") && !strcasecmp(texturename, "shoot")) {
			strcpy(texturename, "shoot_LD_SPAC");
		}
		else if (!strcasecmp(txdname, "ld_spac") && !strcasecmp(texturename, "white")) {
			strcpy(texturename, "white_LD_SPAC");
		}
		else {
			static char* texture_samp[] = {
					"smokeii_3", "back2", "back3", "back4", "back5",
					"back6", "back7", "back8", "back8_right", "back8_top",
					"intro1", "intro2", "intro3", "intro4", "duality", "health", "thrustg",
					"ric1", "ric2", "ric3", "ric4", "ric5", "load0uk", "loadsc0", "loadsc1",
					"loadsc10", "loadsc11", "loadsc12", "loadsc13", "loadsc14", "loadsc2",
					"loadsc3", "loadsc4", "loadsc5", "loadsc6", "loadsc7", "loadsc8",
					"loadsc9", "eax", "nvidia", "title_pc_eu", "title_pc_us", "loadscuk",
					"tyre64a", "wheel_alloy64", "wheel_classic64", "wheel_lighttruck64",
					"wheel_lightvan64", "wheel_offroad64", "wheel_saloon64", "wheel_smallcar64",
					"wheel_sport64", "wheel_truck64", "whee_rim64", "outro"};

			for (auto& i : texture_samp) {
				if (!strcasecmp(i, texturename)) {
					char buf[64];
					sprintf(buf, "%s_%s", texturename, txdname);
					strcpy(texturename, buf);
				}
			}
		}

		if (m_TextDrawData.iTextureSlot != -1) {
			TextDrawTexture[m_TextDrawData.iTextureSlot] = LoadTextureFromTxd(txdname, texturename);
		}
	}
}

void CTextDraw::Draw()
{
	if (m_TextDrawData.iTextureSlot != -1 || m_TextDrawData.dwStyle == 4) {
		DrawTextured();
	}
	else {
		DrawDefault();
	}
}

RGBA DWORD2RGBA(uint32_t dwColor)
{
	RGBA tmp;

	tmp.R = dwColor & 0xFF;
	dwColor >>= 8;
	tmp.G = dwColor & 0xFF;
	dwColor >>= 8;
	tmp.B = dwColor & 0xFF;
	dwColor >>= 8;
	tmp.A = dwColor & 0xFF; /* dwColor >>= 8; */

	return tmp;
}

std::string ReplaceKey(const std::string& input)
{
	static const std::unordered_map<std::string, std::string> keyMap = {
			{"~k~~PED_ANSWER_PHONE~", "TAB"},
			{"~k~~VEHICLE_FIREWEAPON_ALT~", "ALT GR / LCTRL / NUM0"},
			{"~k~~PED_DUCK~", "C"},
			{"~k~~VEHICLE_HORN~", "H / CAPSLOCK"},
			{"~k~~PED_FIREWEAPON~", "LCTRL / LMB"},
			{"~k~~VEHICLE_FIREWEAPON~", "LALT"},
			{"~k~~PED_SPRINT~", "SPC"},
			{"~k~~VEHICLE_ACCELERATE~", "W"},
			{"~k~~VEHICLE_ENTER_EXIT~", "ENTER"},
			{"~k~~PED_JUMPING~", "LSHIFT"},
			{"~k~~VEHICLE_BRAKE~", "S"},
			{"~k~~VEHICLE_LOOKRIGHT~", "E"},
			{"~k~~PED_LOCK_TARGET~", "RMB"},
			{"~k~~VEHICLE_HANDBRAKE~", "SPC"},
			{"~k~~VEHICLE_LOOKLEFT~", "Q"},
			{"~k~~PED_LOOKBEHIND~", "NUM1 / MMB"},
			{"~k~~VEHICLE_LOOKBEHIND~", "2"},
			{"~k~~TOGGLE_SUBMISSIONS~", "2"},
			{"~k~~SNEAK_ABOUT~", "LALT"},
			{"~k~~VEHICLE_TURRETUP~", "NUM8"},
			{"~k~~VEHICLE_TURRETDOWN~", "NUM2"},
			{"~k~~VEHICLE_TURRETLEFT~", "NUM4"},
			{"~k~~VEHICLE_TURRETRIGHT~", "NUM6"},
			{"~k~~CONVERSATION_YES~", "Y"},
			{"~k~~CONVERSATION_NO~", "N"},
			{"~k~~GROUP_CONTROL_BWD~", "H"},
			{"~k~~GROUP_CONTROL_FWD~", "G"},
			{"~k~~GO_FORWARD~", "UP"},
			{"~k~~VEHICLE_STEERUP~", "UP"},
			{"~k~~GO_BACK~", "DOWN"},
			{"~k~~VEHICLE_STEERDOWN~", "DOWN"},
			{"~k~~GO_LEFT~", "LEFT"},
			{"~k~~VEHICLE_STEERLEFT~", "LEFT"},
			{"~k~~GO_RIGHT~", "RIGHT"},
			{"~k~~VEHICLE_STEERRIGHT~", "RIGHT"}
	};

	std::string output = input;

	for (const auto& pair : keyMap) {
		std::string::size_type pos = 0;
		while ((pos = output.find(pair.first, pos)) != std::string::npos) {
			output.replace(pos, pair.first.length(), pair.second);
			pos += pair.second.length();
		}
	}

	return output;
}

void CTextDraw::DrawDefault()
{
	if (!m_szText || !strlen(m_szText)) return;

	int iScreenWidth = sa::RsGlobal->screenWidth;
	int iScreenHeight = sa::RsGlobal->screenHeight;
	float fHorizHudScale = 1.0f / 640.0f;
	float fVertHudScale = 1.0f / 448.0f;

	float fScaleY = (float) iScreenHeight * fVertHudScale * m_TextDrawData.fLetterHeight * 0.5f;
	float fScaleX = (float) iScreenWidth * fHorizHudScale * m_TextDrawData.fLetterWidth;

	CFont::SetScale(fScaleX, fScaleY);

	if (m_bHovered) {
		//uint32_t color = __builtin_bswap32(m_dwHoverColor | (0x000000FF));
		RGBA color = DWORD2RGBA(m_dwHoverColor);
		CFont::SetColor(&color);
	}
	else {
		RGBA letterColor = DWORD2RGBA(m_TextDrawData.dwLetterColor);
		CFont::SetColor(&letterColor);
	}

	CFont::SetJustify(0);

	if (m_TextDrawData.byteAlignRight) {
		CFont::SetOrientation(2);
	}
	else if (m_TextDrawData.byteCentered) {
		CFont::SetOrientation(0);
	}
	else {
		CFont::SetOrientation(1);
	}

	float fLineWidth = iScreenWidth * m_TextDrawData.fLineWidth * fHorizHudScale;
	CFont::SetWrapX(fLineWidth);

	float fLineHeight = iScreenWidth * m_TextDrawData.fLineHeight * fHorizHudScale;
	CFont::SetCentreSize(fLineHeight);

	CFont::SetBackground(m_TextDrawData.byteBox, 0);

	RGBA boxColor = DWORD2RGBA(m_TextDrawData.dwBoxColor);
	CFont::SetBackgroundColor(&boxColor);

	CFont::SetProportional(m_TextDrawData.byteProportional);

	RGBA backgroundColor = DWORD2RGBA(m_TextDrawData.dwBackgroundColor);
	CFont::SetDropColor(&backgroundColor);

	if (m_TextDrawData.byteOutline) {
		CFont::SetEdge(m_TextDrawData.byteOutline);
	}
	else {
		CFont::SetDropShadowPosition(m_TextDrawData.byteShadow);
	}

	CFont::SetFontStyle(m_TextDrawData.dwStyle);

	float fUseY = iScreenHeight - ((448.0 - m_TextDrawData.fY) * (iScreenHeight * fVertHudScale));
	float fUseX = iScreenWidth - ((640.0 - m_TextDrawData.fX) * (iScreenWidth * fHorizHudScale));

	std::string text = ReplaceKey(m_szText);
	CFont::PrintString(fUseX, fUseY, text.c_str());

	CFont::SetEdge(0);

	if (m_TextDrawData.byteAlignRight) {
		m_rectArea.left = fUseX - (fLineWidth - fUseX);
		m_rectArea.right = fUseX;
		m_rectArea.bottom = fUseY;
		m_rectArea.top = fUseY + fLineHeight;
	}
	else if (m_TextDrawData.byteCentered) {
		m_rectArea.left = fUseX - (fLineHeight * 0.5f);
		m_rectArea.right = m_rectArea.left + fLineHeight;
		m_rectArea.bottom = fUseY;
		m_rectArea.top = fUseY + fLineWidth;
	}
	else {
		m_rectArea.left = fUseX;
		m_rectArea.right = fLineWidth - fUseX + fUseX;
		m_rectArea.bottom = fUseY;
		m_rectArea.top = fUseY + fLineHeight;
	}

	m_TextDrawData.bHasRectArea = true;
}

void CTextDraw::DrawTextured()
{
	float scaleX = sa::RsGlobal->screenWidth * (1.0f / 640.0f);
	float scaleY = sa::RsGlobal->screenHeight * (1.0f / 448.0f);

	m_rectArea.left = m_TextDrawData.fX * scaleX;
	m_rectArea.bottom = m_TextDrawData.fY * scaleY;
	m_rectArea.right = (m_TextDrawData.fX + m_TextDrawData.fLineWidth) * scaleX;
	m_rectArea.top = (m_TextDrawData.fY + m_TextDrawData.fLineHeight) * scaleY;

	static float uv_reflected[8] = {
			0.0f, 1.0f,
			1.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f};

	static float uv_normal[8] = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f};

	m_TextDrawData.bHasRectArea = true;

	if (m_bHovered) {
		uint32_t dwReversed = __builtin_bswap32(m_dwHoverColor | (0x000000FF));
		DrawTextureUV(TextDrawTexture[m_TextDrawData.iTextureSlot], &m_rectArea, dwReversed, m_TextDrawData.dwStyle == 5 ? uv_reflected : uv_normal);
	}
	else {
		DrawTextureUV(TextDrawTexture[m_TextDrawData.iTextureSlot], &m_rectArea, m_TextDrawData.dwLetterColor, m_TextDrawData.dwStyle == 5 ? uv_reflected : uv_normal);
	}
}

void CTextDraw::SnapshotProcess()
{
	if (m_TextDrawData.dwStyle != 5 || m_TextDrawData.iTextureSlot != -1) {
		return;
	}

	RwTexture* snapshot = nullptr;

	// PED MODEL
	if (IsValidPedModel(m_TextDrawData.wModelID)) {
		if (!IsExistInfoForModel(m_TextDrawData.wModelID)) {
			m_TextDrawData.wModelID = 0;
		}

		snapshot = pSnapShotHelper->CreatePedSnapShot(
				m_TextDrawData.wModelID,
				m_TextDrawData.dwBackgroundColor,
				&m_TextDrawData.vecRot,
				m_TextDrawData.fZoom);
	}
		// VEHICLE MODEL
	else if (m_TextDrawData.wModelID >= 400 && m_TextDrawData.wModelID <= 611) {
		snapshot = pSnapShotHelper->CreateVehicleSnapShot(
				m_TextDrawData.wModelID,
				m_TextDrawData.dwBackgroundColor,
				&m_TextDrawData.vecRot,
				m_TextDrawData.fZoom,
				m_TextDrawData.wColor1,
				m_TextDrawData.wColor2
		);
	}
		// OBJECT MODEL
	else if ((m_TextDrawData.wModelID <= 320 || m_TextDrawData.wModelID > 373) && m_TextDrawData.wModelID < 615) {
		if (m_TextDrawData.wModelID >= 0) return;
		if (!IsValidModel(m_TextDrawData.wModelID)) {
			m_TextDrawData.wModelID = 18631;
		}

		snapshot = pSnapShotHelper->CreateObjectSnapShot(
				m_TextDrawData.wModelID,
				m_TextDrawData.dwBackgroundColor,
				&m_TextDrawData.vecRot,
				m_TextDrawData.fZoom
		);
	}
	else {
		if (!IsExistInfoForModel(m_TextDrawData.wModelID)) {
			m_TextDrawData.wModelID = 18631;
		}

		snapshot = pSnapShotHelper->CreateObjectSnapShot(
				m_TextDrawData.wModelID,
				m_TextDrawData.dwBackgroundColor,
				&m_TextDrawData.vecRot,
				m_TextDrawData.fZoom
		);
	}

	if (snapshot) {
		m_TextDrawData.iTextureSlot = GetFreeTextDrawTextureSlot();
		TextDrawTexture[m_TextDrawData.iTextureSlot] = snapshot;
	}
}
