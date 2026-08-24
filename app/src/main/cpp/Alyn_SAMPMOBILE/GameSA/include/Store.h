//
// Created by ALYN on 2024/8/28.
//

#ifndef SA_MP_MOBILE_STORE_H
#define SA_MP_MOBILE_STORE_H

#include "SA.h"

namespace sa {
template<typename ObjectType, size_t Capacity>
class CStore {
public:
	size_t m_nCount;
	ObjectType m_aObjects[Capacity];

public:
	CStore()
	{
		m_nCount = 0;
	}

	ObjectType& GetItemAtIndex(size_t index)
	{
		return m_aObjects[index];
	}

	inline ObjectType& AddItem()
	{
		auto& obj = m_aObjects[m_nCount];
		++m_nCount;
		return obj;
	}
};
}

#endif //SA_MP_MOBILE_STORE_H
