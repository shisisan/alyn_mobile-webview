#include "../UI.h"
#include "../../Client.h"
#include "../../Game/Game.h"
#include "../../Net/NetGame.h"

extern UI* pUI;
extern Game* pGame;
extern NetGame* pNetGame;

void SwapPlayerInfo(PLAYER_SCORE_INFO* psi1, PLAYER_SCORE_INFO* psi2)
{
	PLAYER_SCORE_INFO plrinf;
	memcpy(&plrinf, psi1, sizeof(PLAYER_SCORE_INFO));
	memcpy(psi1, psi2, sizeof(PLAYER_SCORE_INFO));
	memcpy(psi2, &plrinf, sizeof(PLAYER_SCORE_INFO));
}

Scoreboard::Scoreboard()
		: m_offset(0), m_sorted(false), m_players(0), m_playerCount(0)
{

}

bool isAnyScrollBarActive()
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	ImGuiID active_id = ImGui::GetActiveID();
	return (active_id && (active_id == ImGui::GetScrollbarID(window, ImGuiAxis_X) || active_id == ImGui::GetScrollbarID(window, ImGuiAxis_Y)));
}

void scrollWhenDragging(bool x, bool y)
{
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 windowSize = ImGui::GetWindowSize();

	if (!isAnyScrollBarActive() &&
	    io.MousePos.x >= windowPos.x && io.MousePos.x <= windowPos.x + windowSize.x &&
	    io.MousePos.y >= windowPos.y && io.MousePos.y <= windowPos.y + windowSize.y) {
		ImVec2 mouse_delta = io.MouseDelta;
		if (x && mouse_delta.x != 0.0f && mouse_delta.x <= 100.0f) ImGui::SetScrollX(ImGui::GetScrollX() + -mouse_delta.x);
		if (y && mouse_delta.y != 0.0f && mouse_delta.y <= 100.0f) ImGui::SetScrollY(ImGui::GetScrollY() + -mouse_delta.y);
	}
}

void Scoreboard::draw(ImGuiRenderer* renderer)
{
	processUpdating();

	if (!m_players) return;

	if (!visible()) {
		if (m_players) {
			memset(m_players, 0, m_playerCount * sizeof(PLAYER_SCORE_INFO));
			free(m_players);
		}

		m_players = 0;
	}

	PLAYERID endplayer = m_playerCount;
	char szPlayerCount[30], szPlayerId[30], szScore[30], szPing[30];

	sprintf(szPlayerCount, "Players: %d", m_playerCount);
	uint16_t wPlayerTextSize = ImGui::CalcTextSize(szPlayerCount).x;

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.13f, 0.13f, 0.13f, 0.70f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.75f, 0.75f, 0.75f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 0.80f));

	ImGui::Begin("Scoreboard", nullptr,
	             ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

	ImGui::Columns(2, "ScoreboardInfo", false);
	ImGui::Text("%s", Encoding::cp2utf(pNetGame->m_hostName).c_str());
	ImGui::SetColumnWidth(-1, size().x - (wPlayerTextSize + (ImGui::GetStyle().ItemSpacing.x * 2)));
	ImGui::NextColumn();
	ImGui::Text("%s", szPlayerCount);
	ImGui::SetColumnWidth(-1, size().x / 100 + wPlayerTextSize + ImGui::GetStyle().ItemSpacing.x);
	ImGui::Columns();

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);

	ImGui::Columns(4, "ScoreboardHeader", false);
	ImGui::TextColored(ImVec4(0.57f, 0.65f, 0.78f, 1.1f), " id");
	ImGui::SetColumnWidth(-1, size().x * 0.1);
	ImGui::NextColumn();
	ImGui::TextColored(ImVec4(0.57f, 0.65f, 0.78f, 1.1f), " name");
	ImGui::SetColumnWidth(-1, size().x * 0.45);
	ImGui::NextColumn();
	ImGui::TextColored(ImVec4(0.57f, 0.65f, 0.78f, 1.1f), " score");
	ImGui::SetColumnWidth(-1, size().x * 0.2);
	ImGui::NextColumn();
	ImGui::TextColored(ImVec4(0.57f, 0.65f, 0.78f, 1.1f), " ping");
	ImGui::SetColumnWidth(-1, size().x * 0.25);
	ImGui::Columns();

	ImGui::BeginChild("ScoreboardChild", ImVec2(-1, (size().y - (UISettings::fontSize() / 2) * 3) - ImGui::GetStyle().ItemSpacing.y), true,
	                  ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NoSavedSettings);

	ImGui::Columns(4, "ScoreboardBody", false);
	for (uint32_t line = m_offset; line < endplayer; line++) {
		int tmpTabId = 0;
		if (tmpTabId == 0) {
			ImGui::PushID(tmpTabId + line);

			std::stringstream ss;
			ss << line + tmpTabId;
			std::string s = ss.str();

			std::string itemid = "##" + s;
			bool is_selected = (m_selectedItem == line);

			if (ImGui::Selectable(itemid.c_str(), is_selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick, ImVec2(0.0f, (UISettings::fontSize() / 2) + ImGui::GetStyle().ItemSpacing.y))) {
				if (m_lastSelectedItem == m_selectedItem) {
					RakNet::BitStream bsSend;
					bsSend.Write(m_players[line].dwID);
					bsSend.Write(0); // 0 from scoreboard
					pNetGame->GetRakClient()->RPC(&RPC_ClickPlayer, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);

					if (visible()) setVisible(false);
				}

				m_lastSelectedItem = line;
			}

			if (ImGui::IsItemHovered()) {
				m_selectedItem = line;
			}

			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}

			ImGui::SameLine();

			ImGui::PopID();
		}

		sprintf(szPlayerId, "%d", m_players[line].dwID);
		sprintf(szScore, "%d", m_players[line].iScore);
		sprintf(szPing, "%d", m_players[line].dwPing);

		ImGui::TextColored(UI::fixcolor(m_players[line].dwColor), "%s", szPlayerId);
		ImGui::SetColumnWidth(-1, size().x / 100 * 10);
		ImGui::NextColumn();

		ImGui::TextColored(UI::fixcolor(m_players[line].dwColor), "%s", m_players[line].szName);
		ImGui::SetColumnWidth(-1, size().x / 100 * 45);
		ImGui::NextColumn();

		ImGui::TextColored(UI::fixcolor(m_players[line].dwColor), "%s", szScore);
		ImGui::SetColumnWidth(-1, size().x / 100 * 20);
		ImGui::NextColumn();

		ImGui::TextColored(UI::fixcolor(m_players[line].dwColor), "%s", szPing);
		ImGui::SetColumnWidth(-1, size().x / 100 * 25);
		ImGui::NextColumn();
	}

	ImGui::EndColumns();

	scrollWhenDragging(false, true);

	ImGui::EndChild();

	ImGui::SetWindowSize(ImVec2(size().x, size().y));
	ImGui::SetWindowPos(position());
	ImGui::End();

	ImGui::PopStyleColor(3);
}

void Scoreboard::processUpdating()
{
	if (pNetGame) {
		if ((GetTickCount() - m_tickProcessingUpdate) > 2000) {
			m_tickProcessingUpdate = GetTickCount();

			// Get player list
			pNetGame->UpdatePlayerScoresAndPings();

			CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
			PLAYERID playercount = pPlayerPool->GetTotalPlayers();
			m_playerCount = playercount;

			if (m_offset > (playercount - 20)) m_offset = (playercount - 20);
			if (m_offset < 0) m_offset = 0;

			m_players = (PLAYER_SCORE_INFO*) malloc(playercount * sizeof(PLAYER_SCORE_INFO));
			memset(m_players, 0, playercount * sizeof(PLAYER_SCORE_INFO));
			m_players[0].dwID = pPlayerPool->GetLocalPlayerID();
			m_players[0].szName = pPlayerPool->GetLocalPlayerName();
			m_players[0].iScore = pPlayerPool->GetLocalPlayerScore();
			m_players[0].dwPing = pPlayerPool->GetLocalPlayerPing();
			m_players[0].dwColor = pPlayerPool->GetLocalPlayer()->GetPlayerColorAsRGBA();
			PLAYERID i = 1, x;
			for (x = 0; x < MAX_PLAYERS; x++) {
				if (!pPlayerPool->GetSlotState(x)) {
					continue;
				}

				CRemotePlayer* pPlayer = pPlayerPool->GetAt(x);
				if (pPlayer && pPlayer->IsNPC()) {
					continue;
				}

				m_players[i].dwID = x;
				m_players[i].szName = pPlayerPool->GetPlayerName(x);
				m_players[i].iScore = pPlayerPool->GetPlayerScore(x);
				m_players[i].dwPing = pPlayerPool->GetPlayerPing(x);
				m_players[i].dwColor = pPlayerPool->GetAt(x)->GetPlayerColor();
				m_players[i].iState = (int) pPlayerPool->GetAt(x)->GetState();
				i++;
			}

			if (m_sorted) {
				for (i = 0; i < playercount - 1; i++) {
					for (PLAYERID j = 0; j < playercount - 1 - i; j++) {
						if (m_players[j + 1].iScore > m_players[j].iScore) {
							SwapPlayerInfo(&m_players[j], &m_players[j + 1]);
						}
					}
				}
			}
		}
	}
}