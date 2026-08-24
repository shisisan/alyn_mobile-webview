//
// Created by ALYN on 13-Feb-25.
//

#include "Placeable.h"
#include "Core/MatrixLinkList.h"
#include "../../Client.h"

namespace sa {
void (* CPlaceable_InitMatrixArray)();
void CPlaceable::InitMatrixArray()
{
	gMatrixList.Init(CPlaceable::NUM_MATRICES_TO_CREATE);
}

void (* CPlaceable_ShutdownMatrixArray)();
void CPlaceable::ShutdownMatrixArray()
{
	gMatrixList.Shutdown();
}

CVector CPlaceable::GetRightVector()
{
	if (m_matrix) {
		return m_matrix->GetRight();
	}
	return {std::cos(m_placement.m_fHeading), std::sin(m_placement.m_fHeading), 0.0f};
}

CVector CPlaceable::GetForwardVector()
{
	if (m_matrix) {
		return m_matrix->GetForward();
	}
	return {-std::sin(m_placement.m_fHeading), std::cos(m_placement.m_fHeading), 0.0f};
}

CVector CPlaceable::GetUpVector()
{
	if (m_matrix) {
		return m_matrix->GetUp();
	}
	return {0.0f, 0.0f, 1.0f};
}

void CPlaceable::SetPosn(float x, float y, float z)
{
//    auto& pos = GetPosition();
//    pos.Set(x, y, z);
	((void (*)(CPlaceable*, float, float, float, bool)) (*(void**) (*(uintptr*) this + (VER_x32 ? 0x3C : 0x3C * 2))))(this, x, y, z, false);
}

void CPlaceable::SetPosn(const CVector& posn)
{
//    auto& pos = GetPosition();
//    pos = posn;
	((void (*)(CPlaceable*, float, float, float, bool)) (*(void**) (*(uintptr*) this + (VER_x32 ? 0x3C : 0x3C * 2))))(this, posn.x, posn.y, posn.z, false);
}

void CPlaceable::SetOrientation(float x, float y, float z)
{
	if (!m_matrix) {
		m_placement.m_fHeading = z;
		return;
	}

	CVector vecPos = m_matrix->GetPosition();
	m_matrix->SetRotate(x, y, z);
	m_matrix->GetPosition() += vecPos;
}

void CPlaceable::GetOrientationDeg(float& x, float& y, float& z)
{
	GetOrientation(x, y, z);

	x = x * 57.295776 * -1.0;
	y = y * 57.295776 * -1.0;
	z = z * 57.295776 * -1.0;
}

void CPlaceable::GetOrientation(float& x, float& y, float& z)
{
	if (!m_matrix) {
		z = m_placement.m_fHeading;
		return;
	}

	m_matrix->ConvertToEulerAngles(&x, &y, &z, 21);
}

void CPlaceable::SetHeading(float heading)
{
	if (m_matrix) {
		m_matrix->SetRotateZOnly(heading);
	}
	else {
		m_placement.m_fHeading = heading;
	}
}

float CPlaceable::GetHeading()
{
	if (!m_matrix) {
		return m_placement.m_fHeading;
	}

	const auto& vecForward = m_matrix->GetForward();
	return std::atan2(-vecForward.x, vecForward.y);
}

bool CPlaceable::IsWithinArea(float x1, float y1, float x2, float y2) const
{
	const auto& vecPos = GetPosition();
	if (x1 > x2) {
		std::swap(x1, x2);
	}

	if (y1 > y2) {
		std::swap(y1, y2);
	}

	return vecPos.x >= x1 && vecPos.x <= x2 && vecPos.y >= y1 && vecPos.y <= y2;
}

bool CPlaceable::IsWithinArea(float x1, float y1, float z1, float x2, float y2, float z2) const
{
	const auto& vecPos = GetPosition();
	if (x1 > x2) {
		std::swap(x1, x2);
	}

	if (y1 > y2) {
		std::swap(y1, y2);
	}

	if (z1 > z2) {
		std::swap(z1, z2);
	}

	return vecPos.x >= x1
			&& vecPos.x <= x2
			&& vecPos.y >= y1
			&& vecPos.y <= y2
			&& vecPos.z >= z1
			&& vecPos.z <= z2;
}

void CPlaceable::RemoveMatrix()
{
	const auto& vecForward = m_matrix->GetForward();
	auto fHeading = std::atan2(-vecForward.x, vecForward.y);

	m_placement.m_vPosn = m_matrix->GetPosition();
	m_placement.m_fHeading = fHeading;

	m_matrix->m_pOwner = nullptr;
	gMatrixList.MoveToFreeList(m_matrix);
	m_matrix = nullptr;
}

void CPlaceable::AllocateStaticMatrix()
{
	if (m_matrix) {
		return gMatrixList.MoveToList2(m_matrix);
	}

	if (gMatrixList.IsFull()) {
		gMatrixList.GetOldestLink()->m_pOwner->RemoveMatrix();
	}

	m_matrix = gMatrixList.AddToList2();
	m_matrix->m_pOwner = this;
}

void CPlaceable::AllocateMatrix()
{
	if (m_matrix) {
		return;
	}

	if (gMatrixList.IsFull()) {
		gMatrixList.GetOldestLink()->m_pOwner->RemoveMatrix();
	}

	m_matrix = gMatrixList.AddToList1();
	m_matrix->m_pOwner = this;
}

void CPlaceable::SetMatrix(CMatrix& matrix)
{
	if (!m_matrix) {
		if (matrix.GetUp().z == 1.0F) {
			auto& vecForward = matrix.GetForward();
			auto fHeading = std::atan2(-vecForward.x, vecForward.y);

			m_placement.m_vPosn = matrix.GetPosition();
			m_placement.m_fHeading = fHeading;
			return;
		}
		CPlaceable::AllocateMatrix();
	}

	*static_cast<CMatrix*>(m_matrix) = matrix;
}

float CPlaceable::GetRoll() const
{
	if (!m_matrix) {
		return 0.f;
	}

	const auto& right = m_matrix->GetRight();
	const auto xymag = CVector2D{right}.SquaredMagnitude(); // NOTE: We're using sqmag here because it doesn't matter, and we save a sqrt this way.
	return std::atan2(right.z, m_matrix->GetUp().z < 0.f ? -xymag : xymag);
}

constexpr float sq(float x) { return x * x; }

bool CPlaceable::IsPointInRange(const CVector& point, float range)
{
	return DistanceBetweenPointsSquared(point, GetPosition()) <= sq(range);
}

CMatrix& CPlaceable::GetMatrix()
{
	if (!m_matrix) {
		CPlaceable::AllocateMatrix();
		m_placement.UpdateMatrix(m_matrix);
	}

	return *m_matrix;
}

void CPlaceable::FreeStaticMatrix()
{
	gMatrixList.MoveToList1(m_matrix);
}

void CPlaceable::Initialize()
{
	Memory::hook("_ZN10CPlaceable15InitMatrixArrayEv", (void*) &CPlaceable::InitMatrixArray, (void**) &CPlaceable_InitMatrixArray);
	Memory::hook("_ZN10CPlaceable19ShutdownMatrixArrayEv", (void*) &CPlaceable::ShutdownMatrixArray, (void**) &CPlaceable_ShutdownMatrixArray);
}

CPlaceable::CPlaceable()
		: m_placement()
{
	m_matrix = nullptr;
}

CPlaceable::~CPlaceable()
{
	Memory::callFunction("_ZN10CPlaceableD2Ev", this);
}
}
