#pragma once

#include "Matrix.h"

namespace sa {
class CPlaceable;

class CMatrixLink : public CMatrix {
public:
	CMatrixLink()
			: CMatrix() { }
	CMatrixLink(float fScale)
			: CMatrix() { SetScale(fScale); }

public:
	CPlaceable* m_pOwner;
	CMatrixLink* m_pPrev;
	CMatrixLink* m_pNext;

public:
	void Insert(CMatrixLink* pWhere)
	{
		pWhere->m_pNext = m_pNext;
		m_pNext->m_pPrev = pWhere;

		pWhere->m_pPrev = this;
		m_pNext = pWhere;
	}

	void Remove()
	{
		m_pNext->m_pPrev = m_pPrev;
		m_pPrev->m_pNext = m_pNext;
	}
};
VALIDATE_SIZE(CMatrixLink, (VER_x32 ? 0x54 : 0x68));
}
