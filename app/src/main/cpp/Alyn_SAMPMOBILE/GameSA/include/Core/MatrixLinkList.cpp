//
// Created by ALYN on 13-Feb-25.
//

#include "MatrixLinkList.h"
#include "../../Client.h"

namespace sa {
CMatrixLinkList gMatrixList;

void CMatrixLinkList::Initialize()
{
	//gMatrixList = *g_saSym->GetSymbol<CMatrixLinkList*>("gMatrixList");

	Memory::protectAddr(g_saSym->Abs(VER_x32 ? 0x6776AC : 0x84CD98));
	*g_saSym->Abs<CMatrixLinkList**>(VER_x32 ? 0x6776AC : 0x84CD98) = &gMatrixList;
}

CMatrixLinkList::CMatrixLinkList()
		: m_head(), m_tail(), m_allocatedListHead(), m_allocatedListTail(), m_freeListHead(), m_freeListTail()
{
	m_pObjects = nullptr;
}

void CMatrixLinkList::Init(int32 count)
{
	m_pObjects = new CMatrixLink[count];

	m_head.m_pNext = &m_tail;
	m_tail.m_pPrev = &m_head;

	m_freeListHead.m_pNext = &m_freeListTail;
	m_freeListTail.m_pPrev = &m_freeListHead;

	m_allocatedListHead.m_pNext = &m_allocatedListTail;
	m_allocatedListTail.m_pPrev = &m_allocatedListHead;

	for (int32 i = count - 1; i >= 0; --i) {
		auto& mat = m_pObjects[i];
		m_freeListHead.Insert(&mat);
	}
}

void CMatrixLinkList::Shutdown()
{
	delete[] m_pObjects;
	m_pObjects = nullptr;
}

CMatrixLink* CMatrixLinkList::AddToList1()
{
	if (IsFull()) {
		return nullptr;
	}

	auto pNextFree = m_freeListHead.m_pNext;
	CMatrixLinkList::MoveToList1(pNextFree);
	return pNextFree;
}

CMatrixLink* CMatrixLinkList::AddToList2()
{
	if (IsFull()) {
		return nullptr;
	}

	auto pNextFree = m_freeListHead.m_pNext;
	CMatrixLinkList::MoveToList2(pNextFree);
	return pNextFree;
}

void CMatrixLinkList::MoveToList1(CMatrixLink* mat)
{
	mat->Remove();
	m_head.Insert(mat);
}

void CMatrixLinkList::MoveToList2(CMatrixLink* mat)
{
	mat->Remove();
	m_allocatedListHead.Insert(mat);
}

void CMatrixLinkList::MoveToFreeList(CMatrixLink* mat)
{
	mat->Remove();
	m_freeListHead.Insert(mat);
}

int32 CMatrixLinkList::GetNumFree()
{
	int32 result = 0;
	auto cur = m_freeListHead.m_pNext;
	while (cur != &m_freeListTail) {
		cur = cur->m_pNext;
		++result;
	}

	return result;
}

int32 CMatrixLinkList::GetNumUsed1()
{
	int32 result = 0;
	auto cur = m_head.m_pNext;
	while (cur != &m_tail) {
		cur = cur->m_pNext;
		++result;
	}

	return result;
}

int32 CMatrixLinkList::GetNumUsed2()
{
	int32 result = 0;
	auto cur = m_allocatedListHead.m_pNext;
	while (cur != &m_allocatedListTail) {
		cur = cur->m_pNext;
		++result;
	}

	return result;
}
}
