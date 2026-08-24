//
// Created by ALYN on 29-Jan-25.
//

#ifndef SA_MP_MOBILE_ANIMBLOCK_H
#define SA_MP_MOBILE_ANIMBLOCK_H

#include "Base.h"

struct CAnimBlock {
	char m_name[16];
	bool m_loaded;
	uint8_t padding;
	uint16_t m_numRefs;
	uint32_t m_animIndex;
	uint32_t m_numAnims;
	uint32_t m_firstAssocGroup;
};
VALIDATE_SIZE(CAnimBlock, 0x20);

#endif //SA_MP_MOBILE_ANIMBLOCK_H
