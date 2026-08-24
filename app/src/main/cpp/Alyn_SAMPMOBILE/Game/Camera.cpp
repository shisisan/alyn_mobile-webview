//
// Created by ALYN on 2024/8/15.
//

#include "Game.h"

CCamera::CCamera()
{
	sa::CCamera& TheCamera = sa::getTheCamera();
	m_matPos = reinterpret_cast<RwMatrix*>(&TheCamera.m_mCameraMatrix);
}

void CCamera::SetBehindPlayer()
{
	ScriptCommand(&lock_camera_position, 0);
	ScriptCommand(&restore_camera_to_user);
	ScriptCommand(&set_camera_behind_player);
	ScriptCommand(&restore_camera_jumpcut);
}

void CCamera::SetPosition(float fX, float fY, float fZ, float fRotationX, float fRotationY, float fRotationZ)
{
	ScriptCommand(&restore_camera_to_user);
	ScriptCommand(&set_camera_position, fX, fY, fZ, fRotationX, fRotationY, fRotationZ);
}

void CCamera::LookAtPoint(float fX, float fY, float fZ, int iType)
{
	ScriptCommand(&restore_camera_to_user);
	ScriptCommand(&point_camera, fX, fY, fZ, iType);
}

/**
 * @param vecFrom
 * @param vecTo
 * @param iTime
 * @param byteMode
 */
void CCamera::InterpolateCameraPos(sa::CVector* vecFrom, sa::CVector* vecTo, int iTime, uint8_t byteMode)
{
	ScriptCommand(&restore_camera_to_user);
	ScriptCommand(&lock_camera_position, 1);
	ScriptCommand(&set_camera_pos_time_smooth, vecFrom->x, vecFrom->y, vecFrom->z, vecTo->x, vecTo->y, vecTo->z, iTime, byteMode);
}

/**
 * @param vecFrom
 * @param vecTo
 * @param iTime
 * @param byteMode
 */
void CCamera::InterpolateCameraLookAt(sa::CVector* vecFrom, sa::CVector* vecTo, int iTime, uint8_t byteMode)
{
	ScriptCommand(&lock_camera_target_point, 1);
	ScriptCommand(&point_camera_transverse, vecFrom->x, vecFrom->y, vecFrom->z, vecTo->x, vecTo->y, vecTo->z, iTime, byteMode);
}

void CCamera::Restore()
{
	ScriptCommand(&restore_camera_jumpcut);
}

void CCamera::GetMatrix(RwMatrix* Matrix)
{
	*Matrix = *m_matPos;
}
