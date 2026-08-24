//
// Created by ALYN on 2024/8/18.
//
#pragma once

#include <span>

#include "../SA.h"

namespace sa {
class CAnimBlendHierarchy {
public:
	uint32 m_hashKey;
	CAnimBlendSequence* m_pSequences;
	uint16 m_nSeqCount;
	bool m_bRunningCompressed;
	bool m_bKeepCompressed;
	int32 m_nAnimBlockId;
	float m_fTotalTime;
	CLink<CAnimBlendHierarchy*>* m_Link;

public:
	auto GetSequences() const { return std::span{m_pSequences, (size_t) m_nSeqCount}; }

private:
	CAnimBlendHierarchy* Constructor()
	{
		CAnimBlendHierarchy();
		return this;
	}
	CAnimBlendHierarchy* Destructor()
	{
		this->CAnimBlendHierarchy::~CAnimBlendHierarchy();
		return this;
	}
};
VALIDATE_SIZE(CAnimBlendHierarchy, (VER_x32 ? 0x18 : 0x28));
}
