/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

/*
    R* terminology      Our terminology
    JustIndex           Index
    Index               Id
    Ref                 Handle/Ref
*/
#pragma once

namespace sa {
union tPoolObjectFlags {
	struct {
		unsigned char nId : 7;
		bool bEmpty : 1;
	};
private:
	unsigned char nValue;
public:
	unsigned char IntValue() { return nValue; }
};

template<class A, class B = A> class CPool {
public:
	B* m_aStorage;
	tPoolObjectFlags* m_aFlags;
	int m_nSize;
	int m_nFreeIndex;
	bool m_bOwnsArrays;
	bool m_bDealWithNoMemory;
	uint16 pad;

	// by qq1198
	CPool(int count, int size)
	{
		m_aStorage = (void*) new char[size * count];
		m_aFlags = reinterpret_cast<tPoolObjectFlags*>(new uint8_t[count]);
		m_nSize = count;
		m_nFreeIndex = -1;
		m_bOwnsArrays = false;
		m_bDealWithNoMemory = true;

		for (size_t i = 0; i < count; i++) {
			((uint8_t*) m_aFlags)[i] |= 0x80;
			((uint8_t*) m_aFlags)[i] &= 0x80;
		}
	}

	// Default constructor for statically allocated pools
	CPool()
	{
		// Remember to call CPool::Init to fill in the fields!
		m_aStorage = nullptr;
		m_aFlags = nullptr;
		m_nSize = 0;
		m_bOwnsArrays = false;
	}

	// Initializes pool
	CPool(int nSize, const char* pPoolName)
	{
		m_aStorage = static_cast<B*>(operator new(sizeof(B) * nSize));
		m_aFlags = static_cast<tPoolObjectFlags*>(operator new(sizeof(tPoolObjectFlags) * nSize));
		m_nSize = nSize;
		m_nFreeIndex = -1;
		m_bOwnsArrays = true;
		for (int i = 0; i < nSize; ++i) {
			m_aFlags[i].bEmpty = true;
			m_aFlags[i].nId = 0;
		}
	}

	~CPool()
	{
		Flush();
	}

	// Initialises a pool with preallocated
	// To be called one-time-only for statically allocated pools.
	void Init(int nSize, void* pObjects, void* pInfos)
	{
		// Since we statically allocated the pools we do not deallocate.
		assert(this->m_aStorage == nullptr);
		m_aStorage = static_cast<B*>(pObjects);
		m_aFlags = static_cast<tPoolObjectFlags*>(pInfos);
		m_nSize = nSize;
		m_nFreeIndex = -1;
		m_bOwnsArrays = false;
		for (int i = 0; i < nSize; ++i) {
			m_aFlags[i].bEmpty = true;
			m_aFlags[i].nId = 0;
		}
	}

	// Shutdown pool
	void Flush()
	{
		if (m_bOwnsArrays) {
			operator delete(m_aStorage);
			operator delete(m_aFlags);
		}
		m_aStorage = nullptr;
		m_aFlags = nullptr;
		m_nSize = 0;
		m_nFreeIndex = 0;
	}

	// Clears pool
	void Clear()
	{
		for (int i = 0; i < m_nSize; i++)
			m_aFlags[i].bEmpty = true;
	}

	// Returns if specified slot is free (0x404940)
	bool IsFreeSlotAtIndex(int idx)
	{
		return m_aFlags[idx].bEmpty;
	}

	// Returns slot index for this object
	int GetIndex(A* pObject)
	{
		return reinterpret_cast<B*>(pObject) - m_aStorage;
	}

	// Returns pointer to object by slot index
	A* GetAt(int nIndex)
	{
		return !IsFreeSlotAtIndex(nIndex) ? (A*) &m_aStorage[nIndex] : nullptr;
	}

	// Marks slot as free / used (0x404970)
	void SetFreeAt(int idx, bool bFree)
	{
		m_aFlags[idx].bEmpty = bFree;
	}

	// Set new id for slot (0x54F9F0)
	void SetIdAt(int idx, unsigned char id)
	{
		m_aFlags[idx].nId = id;
	}

	// Get id for slot (0x552200)
	unsigned char GetIdAt(int idx)
	{
		return m_aFlags[idx].nId;
	}

	// Allocates object
	A* New()
	{
		bool bReachedTop = false;
		do {
			if (++m_nFreeIndex >= m_nSize) {
				if (bReachedTop) {
					m_nFreeIndex = -1;
					return nullptr;
				}
				bReachedTop = true;
				m_nFreeIndex = 0;
			}
		}
		while (!m_aFlags[m_nFreeIndex].bEmpty);
		m_aFlags[m_nFreeIndex].bEmpty = false;
		++m_aFlags[m_nFreeIndex].nId;
		return &m_aStorage[m_nFreeIndex];
	}

	// Allocates object at a specific index from a SCM handle (ref) (0x59F610)
	void CreateAtRef(int nRef)
	{
		nRef >>= 8;
		m_aFlags[nRef].bEmpty = false;
		++m_aFlags[nRef].nId;
		m_nFreeIndex = 0;
		while (!m_aFlags[m_nFreeIndex].bEmpty)
			++m_nFreeIndex;
	}

	// (0x5A1C00)
	A* New(int nRef)
	{
		A* result = &m_aStorage[nRef >> 8];
		CreateAtRef(nRef);
		return result;
	}

	// Deallocates object
	void Delete(A* pObject)
	{
		int nIndex = reinterpret_cast<B*>(pObject) - m_aStorage;
		m_aFlags[nIndex].bEmpty = true;
		if (nIndex < m_nFreeIndex) {
			m_nFreeIndex = nIndex;
		}
	}

	// Returns SCM handle (ref) for object (0x424160)
	int GetRef(A* pObject)
	{
		return (GetIndex(pObject) << 8) + m_aFlags[GetIndex(pObject)].IntValue();
	}

	// Returns pointer to object by SCM handle (ref)
	A* GetAtRef(int ref)
	{
		int nSlotIndex = ref >> 8;
		return nSlotIndex >= 0 && nSlotIndex < m_nSize && m_aFlags[nSlotIndex].IntValue() == (ref & 0xFF) ? reinterpret_cast<A*>(&m_aStorage[nSlotIndex]) : nullptr;
	}

	// (0x54F6B0)
	unsigned int GetNoOfUsedSpaces()
	{
		unsigned int counter = 0;
		for (int i = 0; i < m_nSize; ++i) {
			if (!IsFreeSlotAtIndex(i)) {
				++counter;
			}
		}
		return counter;
	}

	unsigned int GetNoOfFreeSpaces()
	{
		return m_nSize - GetNoOfUsedSpaces();
	}

	// (0x54F690)
	unsigned int GetObjectSize()
	{
		return sizeof(B);
	}

	// (0x5A1CD0)
	bool IsObjectValid(A* obj)
	{
		int slot = GetIndex(obj);
		return slot >= 0 && slot < m_nSize && !IsFreeSlotAtIndex(slot);
	}
};
VALIDATE_SIZE(CPool<int32>, (VER_x32 ? 0x14 : 0x20));
}

