#include "../UI.h"
#include "../../Game/Game.h"
#include "../../Net/NetGame.h"

extern UI* pUI;
extern Game* pGame;
extern NetGame* pNetGame;

/* DeathWindow */

DeathWindow::DeathWindow()
{
	m_pDeathWindow.clear();
}

void DeathWindow::record(const char* playername, unsigned int playercolor, const char* killername, unsigned int killercolor, uint8_t reason)
{
	if (!playername || !strlen(playername)) return;

	DeathWindowStruct* pPlayerKill = new DeathWindowStruct;
	pPlayerKill->playerName = playername;
	pPlayerKill->playercolor = playercolor;
	pPlayerKill->killerName = killername;
	pPlayerKill->killercolor = killercolor;
	pPlayerKill->reason = reason;

	if (m_pDeathWindow.size() >= 5) {
		m_pDeathWindow.pop_front();
	}

	m_pDeathWindow.push_back(pPlayerKill);
}

void DeathWindow::draw(ImGuiRenderer* renderer)
{
	if (pNetGame && pNetGame->GetTextDrawPool() && pNetGame->GetTextDrawPool()->GetState()) return;

	if (!m_pDeathWindow.empty()) {
		float fPosSprite = pUI->ScaleX(1480.0f);

		ImVec2 vecPos;
		vecPos.y = pUI->ScaleY(420.0f);

		for (auto& playerkill : m_pDeathWindow) {
			if (playerkill) {
				unsigned char PlayerColor[3];
				PlayerColor[0] = (playerkill->playercolor >> 8) & 0xFF;
				PlayerColor[1] = (playerkill->playercolor >> 16) & 0xFF;
				PlayerColor[2] = (playerkill->playercolor >> 24) & 0xFF;

				unsigned char KillerColor[3];
				KillerColor[0] = (playerkill->killercolor >> 8) & 0xFF;
				KillerColor[1] = (playerkill->killercolor >> 16) & 0xFF;
				KillerColor[2] = (playerkill->killercolor >> 24) & 0xFF;

				if (!playerkill->killerName.empty() && !playerkill->playerName.empty()) {
					vecPos.x = (fPosSprite - renderer->calculateTextSize(Encoding::cp2utf(playerkill->killerName), UISettings::fontSize() / 2).x) - 10.0f;
					renderer->drawText(vecPos, ImColor(KillerColor[0], KillerColor[1], KillerColor[2]), Encoding::cp2utf(playerkill->killerName), true, UISettings::fontSize() / 2);

					vecPos.x = fPosSprite;
					renderer->drawText(vecPos, 0xFF000000, "G", false, UISettings::fontSize() / 2, pUI->weapFont());
					renderer->drawText(vecPos, 0xFFFFFFFF, spriteIDForWeapon(playerkill->reason), false, UISettings::fontSize() / 2, pUI->weapFont());

					vecPos.x = fPosSprite + UISettings::fontSize() / 2 + 20.0f;
					renderer->drawText(vecPos, ImColor(PlayerColor[0], PlayerColor[1], PlayerColor[2]), Encoding::cp2utf(playerkill->playerName), true, UISettings::fontSize() / 2);
				}

				else if (playerkill->killerName.empty() && !playerkill->playerName.empty()) {
					vecPos.x = (fPosSprite - renderer->calculateTextSize(Encoding::cp2utf(playerkill->playerName), UISettings::fontSize() / 2).x) - 10.0f;
					renderer->drawText(vecPos, ImColor(PlayerColor[0], PlayerColor[1], PlayerColor[2]), Encoding::cp2utf(playerkill->playerName), true, UISettings::fontSize() / 2);

					uint32_t dwColor = 0xFFFFFFFF;
					if (playerkill->reason == 55) { dwColor = 0xFFFF0000; }
					else if (playerkill->reason == 56) dwColor = 0xFF0000FF;

					vecPos.x = fPosSprite;

					renderer->drawText(vecPos, 0xFF000000, "G", false, UISettings::fontSize() / 2, pUI->weapFont());
					renderer->drawText(vecPos, dwColor, spriteIDForWeapon(playerkill->reason), false, UISettings::fontSize() / 2, pUI->weapFont());
				}

				vecPos.y += UISettings::fontSize() / 2 + 5.0f;
			}
		}
	}
}

const char* DeathWindow::spriteIDForWeapon(uint8_t byteWeaponID)
{
	switch (byteWeaponID) {
		case WEAPON_FIST: return "%";
		case WEAPON_BRASSKNUCKLE: return "B";
		case WEAPON_GOLFCLUB: return ">";
		case WEAPON_NITESTICK: return "(";
		case WEAPON_KNIFE: return "C";
		case WEAPON_BAT: return "?";
		case WEAPON_SHOVEL: return "&";
		case WEAPON_POOLSTICK: return "\"";
		case WEAPON_KATANA: return "!";
		case WEAPON_CHAINSAW: return "1";
		case WEAPON_DILDO: return "E";
		case WEAPON_DILDO2: return "E";
		case WEAPON_VIBRATOR: return "E";
		case WEAPON_VIBRATOR2: return "E";
		case WEAPON_FLOWER: return "$";
		case WEAPON_CANE: return "#";
		case WEAPON_GRENADE: return "@";
		case WEAPON_TEARGAS: return "D";
		case WEAPON_COLT45: return "6";
		case WEAPON_SILENCED: return "2";
		case WEAPON_DEAGLE: return "3";
		case WEAPON_SHOTGUN: return "=";
		case WEAPON_SAWEDOFF: return "0";
		case WEAPON_SHOTGSPA: return "+";
		case WEAPON_UZI: return "I";
		case WEAPON_MP5: return "8";
		case WEAPON_AK47: return "H";
		case WEAPON_M4: return "5";
		case WEAPON_TEC9: return "7";
		case WEAPON_RIFLE: return ".";
		case WEAPON_SNIPER: return "A";
		case WEAPON_ROCKETLAUNCHER: return "4";
		case WEAPON_HEATSEEKER: return ")";
		case WEAPON_FLAMETHROWER: return "P";
		case WEAPON_MINIGUN: return "F";
		case WEAPON_SATCHEL: return "<";
		case WEAPON_BOMB: return ";";
		case WEAPON_SPRAYCAN: return "/";
		case WEAPON_FIREEXTINGUISHER: return ",";
		case WEAPON_PARACHUTE: return ":";
		case WEAPON_VEHICLE: return "L";
		case WEAPON_DROWN: return "J";
		case WEAPON_HELIBLADES: return "R";
		case WEAPON_EXPLOSION: return "Q";
		case WEAPON_COLLISION: return "K";
		case 55: return "N";
		case 56: return "N";
	}
	return "J";
}