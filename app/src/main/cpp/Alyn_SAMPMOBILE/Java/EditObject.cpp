//
// Created by ALYN on 25-Jan-25.
//

#include "../Client.h"
#include "../Java/Java.h"
#include "../UI/UI.h"
#include "../Game/Game.h"
#include "../Net/NetGame.h"
#include "EditObject.h"

extern UI* pUI;
extern NetGame* pNetGame;

void EditObject::startEditAttachedObject(int slot)
{
	if (!pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->IsValidAttach(slot)) {
		pUI->chat()->addDebugMessage("Invalid attach slot %d", slot);
		return;
	}

	EditObject::m_id = slot;
	EditObject::m_type = TYPE_ATTACHED_OBJECT;
	EditObject::showGui(true);
}

void EditObject::startEditObject(uint16_t objectId)
{
	CObject* pObject = pNetGame->GetObjectPool()->GetAt(objectId);
	if (!pObject) {
		pUI->chat()->addDebugMessage("Invalid object id %d", objectId);
		return;
	}

	EditObject::m_id = objectId;
	EditObject::m_type = TYPE_OBJECT;
	EditObject::showGui(true);
}

void EditObject::showGui(bool show)
{
	g_java->showEditObject(show);
	m_visible = show;
}

void EditObject::cancelEdit()
{
	EditObject::m_id = -1;
	EditObject::showGui(false);
}

extern "C" JNIEXPORT void JNICALL Java_ro_alynsampmobile_game_ui_widgets_EditObject_Click(JNIEnv* env, jobject thiz, jint button, jboolean state)
{
	CPlayerPed* pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
	auto& attach = pPlayer->m_attachedObjects[EditObject::m_id].info;
	if (EditObject::m_type == EditObject::TYPE_ATTACHED_OBJECT) {
		if (!pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->IsValidAttach(EditObject::m_id)) {
			EditObject::showGui(false);
			return;
		}
	}

	CObject* pObject = nullptr;

	if (EditObject::m_type == EditObject::TYPE_OBJECT) {
		pObject = pNetGame->GetObjectPool()->GetAt(EditObject::m_id);
		if (!pObject) {
			pUI->chat()->addDebugMessage("Invalid object id %d", EditObject::m_id);
			EditObject::showGui(false);
			return;
		}
	}

	// lazy to fix this
	int BUTTON_LEFT_RIGHT = 1; // 0
	int BUTTON_UP_DOWN = 0; // 1
	int BUTTON_PUSH_PULL = 2;
	int BUTTON_SCALE = 3;
	int BUTTON_ROT_X = 4;
	int BUTTON_ROT_Y = 5;
	int BUTTON_ROT_Z = 6;

	float value = state ? 0.005f : -0.005f;
	float rotValue = state ? 1.0f : -1.0f;

	if (button == BUTTON_LEFT_RIGHT) {
		if (EditObject::m_type == EditObject::TYPE_ATTACHED_OBJECT) {
			attach.vecOffset.x += value;
		}
		if (EditObject::m_type == EditObject::TYPE_OBJECT) {
			RwMatrix matrix = pObject->m_entity->GetMatrix().ToRwMatrix();
			matrix.pos.x += value;
//			pObject->SetMatrix(matrix);
			pObject->TeleportTo(matrix.pos.x, matrix.pos.y, matrix.pos.z);
		}
	}
	if (button == BUTTON_UP_DOWN) {
		if (EditObject::m_type == EditObject::TYPE_ATTACHED_OBJECT) {
			attach.vecOffset.y += value;
		}
		if (EditObject::m_type == EditObject::TYPE_OBJECT) {
			RwMatrix matrix = pObject->m_entity->GetMatrix().ToRwMatrix();
			matrix.pos.y += value;
//			pObject->SetMatrix(matrix);
			pObject->TeleportTo(matrix.pos.x, matrix.pos.y, matrix.pos.z);
		}
	}
	if (button == BUTTON_PUSH_PULL) {
		if (EditObject::m_type == EditObject::TYPE_ATTACHED_OBJECT) {
			attach.vecOffset.z += value;
		}
		if (EditObject::m_type == EditObject::TYPE_OBJECT) {
			RwMatrix matrix = pObject->m_entity->GetMatrix().ToRwMatrix();
			matrix.pos.z += value;
//			pObject->SetMatrix(matrix);
			pObject->TeleportTo(matrix.pos.x, matrix.pos.y, matrix.pos.z);
		}
	}
	if (button == BUTTON_SCALE) {
		if (EditObject::m_type == EditObject::TYPE_ATTACHED_OBJECT) {
			attach.vecScale.x += value;
			attach.vecScale.y += value;
			attach.vecScale.z += value;
		}
		if (EditObject::m_type == EditObject::TYPE_OBJECT) {
			sa::CVector vecScale = pObject->GetScale();
			vecScale.x += value;
			vecScale.y += value;
			vecScale.z += value;
			pObject->SetScale(vecScale);
		}
	}
	if (button == BUTTON_ROT_X) {
		if (EditObject::m_type == EditObject::TYPE_ATTACHED_OBJECT) {
			attach.vecRot.x += rotValue;
		}
		if (EditObject::m_type == EditObject::TYPE_OBJECT) {
			float x, y, z;
			pObject->GetRotation(&x, &y, &z);
			x += rotValue;
			sa::CVector vRot = {x, y, z};
			pObject->SetRotation(&vRot);
		}
	}
	if (button == BUTTON_ROT_Y) {
		if (EditObject::m_type == EditObject::TYPE_ATTACHED_OBJECT) {
			attach.vecRot.y += rotValue;
		}
		if (EditObject::m_type == EditObject::TYPE_OBJECT) {
			float x, y, z;
			pObject->GetRotation(&x, &y, &z);
			y += rotValue;
			sa::CVector vRot = {x, y, z};
			pObject->SetRotation(&vRot);
		}
	}
	if (button == BUTTON_ROT_Z) {
		if (EditObject::m_type == EditObject::TYPE_ATTACHED_OBJECT) {
			attach.vecRot.z += rotValue;
		}
		if (EditObject::m_type == EditObject::TYPE_OBJECT) {
			float x, y, z;
			pObject->GetRotation(&x, &y, &z);
			z += rotValue;
			sa::CVector vRot = {x, y, z};
			pObject->SetRotation(&vRot);
		}
	}

	// update object
	if (pNetGame && EditObject::m_type == EditObject::TYPE_OBJECT) {
		if (EditObject::m_lastUpdate - GetTickCount() > 500) {
			RwMatrix matrix = pObject->m_entity->GetMatrix().ToRwMatrix();
			sa::CVector rot;
			pObject->GetRotation(&rot.x, &rot.y, &rot.z);

			RakNet::BitStream bsSend;
			bsSend.Write(EditObject::eResponse::EDIT_RESPONSE_UPDATE);
			bsSend.Write(EditObject::m_playerObj);
			bsSend.Write(EditObject::m_id);
			bsSend.Write(matrix.pos);
			bsSend.Write(rot);
			pNetGame->GetRakClient()->RPC(&RPC_EditObject, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);

			EditObject::m_lastUpdate = GetTickCount();
		}
	}
}

extern "C" JNIEXPORT void JNICALL Java_ro_alynsampmobile_game_ui_widgets_EditObject_Exit(JNIEnv* env, jobject thiz)
{
	if (pNetGame) {
		RakNet::BitStream bsSend;
		bsSend.Write((uint32_t) EditObject::eResponse::EDIT_RESPONSE_CANCEL);
		pNetGame->GetRakClient()->RPC(&RPC_CancelEditObject, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
	}

	EditObject::showGui(false);
}

extern "C" JNIEXPORT void JNICALL Java_ro_alynsampmobile_game_ui_widgets_EditObject_Save(JNIEnv* env, jobject thiz)
{
	CPlayerPed* pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
	auto& attach = pPlayer->m_attachedObjects[EditObject::m_id].info;

	if (pNetGame && EditObject::m_type == EditObject::TYPE_ATTACHED_OBJECT) {
		RakNet::BitStream bsSend;
		bsSend.Write(EditObject::eResponse::EDIT_RESPONSE_FINAL);
		bsSend.Write(EditObject::m_id);
		bsSend.Write(attach.iModel);
		bsSend.Write(attach.iBoneID);
		bsSend.Write(attach.vecOffset);
		bsSend.Write(attach.vecRot);
		bsSend.Write(attach.vecScale);
		bsSend.Write(0);
		bsSend.Write(0);
		pNetGame->GetRakClient()->RPC(&RPC_EditAttachedObject, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
	}
	if (pNetGame && EditObject::m_type == EditObject::TYPE_OBJECT) {
		CObject* pObject = pNetGame->GetObjectPool()->GetAt(EditObject::m_id);
		if (!pObject) {
			pUI->chat()->addDebugMessage("Invalid object id %d", EditObject::m_id);
			goto exit;
		}

		RwMatrix matrix = pObject->m_entity->GetMatrix().ToRwMatrix();
		sa::CVector rot;
		pObject->GetRotation(&rot.x, &rot.y, &rot.z);

		RakNet::BitStream bsSend;
		bsSend.Write(EditObject::eResponse::EDIT_RESPONSE_FINAL);
		bsSend.Write(EditObject::m_playerObj);
		bsSend.Write(EditObject::m_id);
		bsSend.Write(matrix.pos);
		bsSend.Write(rot);
		pNetGame->GetRakClient()->RPC(&RPC_EditObject, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
	}

exit:
	EditObject::showGui(false);
}
