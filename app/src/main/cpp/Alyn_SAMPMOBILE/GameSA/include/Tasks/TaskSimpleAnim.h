//
// Created by ALYN on 2024/8/18.
//
#pragma once

#include "../SA.h"
#include "TaskSimple.h"
#include "../Animation/AnimBlendAssociation.h"

namespace sa {
class CAnimBlendAssociation;

class CTaskSimpleAnim : public CTaskSimple {
public:
	CAnimBlendAssociation* m_pAnim;
	union {
		struct {
			bool m_bIsFinished : 1;
			bool m_bDontInterrupt : 1;
			bool m_bHoldLastFrame : 1;

			// These flags are used in CTaskSimpleRunAnim only
			bool m_bDontBlendOut : 1;

			// These flags are used in 	CTaskSimpleRunNamedAnim only
			bool m_bRunInSequence : 1;
			bool m_bOffsetAtEnd : 1;
			bool m_bOffsetAvailable : 1;
		};
		uint8_t m_nFlags;
	};
};
VALIDATE_SIZE(CTaskSimpleAnim, (VER_x32 ? 0x10 : 0x20));
}
