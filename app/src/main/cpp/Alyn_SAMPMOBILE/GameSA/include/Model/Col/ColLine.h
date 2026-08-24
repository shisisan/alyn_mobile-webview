//
// Created by ALYN on 2024/8/17.
//

#pragma once

#include "../../SA.h"

namespace sa {
class CColLine {
public:
	CVector m_vecStart;
	float m_fStartSize;
	CVector m_vecEnd;
	float m_fEndSize;

};
VALIDATE_SIZE(CColLine, 0x20);
}
