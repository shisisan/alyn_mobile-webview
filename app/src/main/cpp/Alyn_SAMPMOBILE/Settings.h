#pragma once

#include <imgui.h>
#include <RakNet/SocketLayer.h>

class Settings {
public:
	static void initialize();

	// server
	static char* host() { return m_host; }
	static int port() { return m_port; }
	static char* pass() { return m_pass; }

	static char* ip()
	{
		char* ip;
		if (m_host[0] < '0' || m_host[0] > '2') {
			ip = (char*) SocketLayer::Instance()->DomainNameToIP(m_host);
		}
		else {
			ip = m_host;
		}
		if (!ip) {
			return m_host;
		}
		return ip;
	}

	// settings
	static char* nick() { return m_nick; }
	static int sampversion() { return m_sampversion; }
	static bool newinterface() { return m_newinterface; }
	static bool systemkeyboard() { return m_systemkeyboard; }
	static bool timestamp() { return m_timestamp; }
	static bool fastconnect() { return m_fastconnect; }
	static bool voice() { return m_voice; }
	static bool displayfps() { return m_displayfps; }
	static int fpslimit() { return m_fpslimit; }
	static int chatstrings() { return m_chatstrings; }
	static float fontsize() { return m_fontsize; }
	static ImVec2 chatpos() { return m_chatpos; }
	static ImVec2 chatsize() { return m_chatsize; }

private:
	static bool m_initialized;

	// server
	static char m_host[64];
	static int m_port;
	static char m_pass[64];

	// settings
	static char m_nick[30];
	static int m_sampversion;
	static bool m_newinterface;
	static bool m_systemkeyboard;
	static bool m_timestamp;
	static bool m_fastconnect;
	static bool m_voice;
	static bool m_displayfps;
	static int m_fpslimit;
	static int m_chatstrings;
	static float m_fontsize;
	static ImVec2 m_chatpos;
	static ImVec2 m_chatsize;
};
