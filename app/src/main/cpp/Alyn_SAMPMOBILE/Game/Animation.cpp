#include "Animation.h"
#include "../Client.h"

const char* GetAnimByIdx(int idx)
{
	if (idx < 0 || idx >= 1843) {
		return nullptr;
	}
	return &g_szAnimBlockNames[idx][0];
}

int GetAnimIdxByName(const char* szName)
{
	for (int i = 0; i < 1843; i++) {
		if (!strcasecmp(szName, g_szAnimBlockNames[i])) {
			return i;
		}
	}
	return -1;
}
