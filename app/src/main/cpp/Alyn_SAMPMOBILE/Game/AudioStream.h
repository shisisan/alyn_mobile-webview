#pragma once

#include <bass.h>

class AudioStream {
public:
	AudioStream();

	bool Initialize();

	void Process();

	bool Play(const char* szUrl, float fX, float fY, float fZ, float fRadius, bool bUsePos);
	bool Stop(bool bWaitThread);

private:
	bool m_bInited;
};