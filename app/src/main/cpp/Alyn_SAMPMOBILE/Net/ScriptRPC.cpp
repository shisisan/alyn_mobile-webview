#include "../UI/UI.h"
#include "../Game/Game.h"
#include "../Game/AudioStream.h"
#include "NetGame.h"

extern UI* pUI;
extern Game* pGame;
extern NetGame* pNetGame;
extern AudioStream* pAudioStream;

void ScrSetGravity(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetGravity");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	float fGravity;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(fGravity);

	pGame->SetGravity(fGravity);
}

void ScrSetCameraPos(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetCameraPos");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	float fX, fY, fZ;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);

	pGame->GetCamera()->SetPosition(fX, fY, fZ, 0.0f, 0.0f, 0.0f);
}

void ScrSetCameraLookAt(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetCameraLookAt");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	float fX, fY, fZ;
	uint8_t byteType;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);
	bsData.Read(byteType);
	if (byteType < 1 || byteType > 2) {
		byteType = 2;
	}

	pGame->GetCamera()->LookAtPoint(fX, fY, fZ, byteType);
}

void ScrInterpolateCamera(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrInterpolateCamera");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	bool mode;
	sa::CVector vecFrom;
	sa::CVector vecTo;
	int iTime;
	uint8_t byteMode;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(mode);
	bsData.Read(vecFrom.x);
	bsData.Read(vecFrom.y);
	bsData.Read(vecFrom.z);
	bsData.Read(vecTo.x);
	bsData.Read(vecTo.y);
	bsData.Read(vecTo.z);
	bsData.Read(iTime);
	bsData.Read(byteMode);

	if (byteMode < 1 || byteMode > 2) {
		byteMode = 2;
	}

	if (iTime > 0) {
		pNetGame->GetPlayerPool()->GetLocalPlayer()->m_bSpectateProcessed = true;
		if (mode) {
			pGame->GetCamera()->InterpolateCameraPos(&vecFrom, &vecTo, iTime, byteMode);
		}
		else {
			pGame->GetCamera()->InterpolateCameraLookAt(&vecFrom, &vecTo, iTime, byteMode);
		}
	}
}

void ScrTogglePlayerSpectating(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrTogglePlayerSpectating");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint32_t dwToggle;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(dwToggle);

	spdlog::info("dwToggle: {}", dwToggle);

	pNetGame->GetPlayerPool()->GetLocalPlayer()->ToggleSpectating(dwToggle);
}

void ScrSetSpawnInfo(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetSpawnInfo");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYER_SPAWN_INFO spawnInfo;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read((char*) &spawnInfo, sizeof(PLAYER_SPAWN_INFO));

	pNetGame->GetPlayerPool()->GetLocalPlayer()->SetSpawnInfo(&spawnInfo);
}

void ScrAddGangZone(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrAddGangZone");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint16_t wZoneID;
	float minX, minY, maxX, maxY;
	uint32_t dwColor;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	CGangZonePool* pGangZonePool = pNetGame->GetGangZonePool();
	if (pGangZonePool) {
		bsData.Read(wZoneID);
		bsData.Read(minX);
		bsData.Read(minY);
		bsData.Read(maxX);
		bsData.Read(maxY);
		bsData.Read(dwColor);
		pGangZonePool->New(wZoneID, minX, minY, maxX, maxY, dwColor);
	}
}

void ScrGangZoneDestroy(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrGangZoneDestroy");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	CGangZonePool* pGangZonePool = pNetGame->GetGangZonePool();
	if (pGangZonePool) {
		uint16_t wZoneID;
		bsData.Read(wZoneID);
		pGangZonePool->Delete(wZoneID);
	}
}

void ScrGangZoneFlash(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrGangZoneFlash");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	CGangZonePool* pGangZonePool = pNetGame->GetGangZonePool();
	if (pGangZonePool) {
		uint16_t wZoneID;
		uint32_t dwColor;
		bsData.Read(wZoneID);
		bsData.Read(dwColor);
		pGangZonePool->Flash(wZoneID, dwColor);
	}
}

void ScrGangZoneStopFlash(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrGangZoneStopFlash");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	CGangZonePool* pGangZonePool = pNetGame->GetGangZonePool();
	if (pGangZonePool) {
		uint16_t wZoneID;
		bsData.Read(wZoneID);
		pGangZonePool->StopFlash(wZoneID);
	}
}

int iTotalObjects = 0;

void ScrCreateObject(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrCreateObject");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	OBJECTID ObjectID;
	int iModel;
	sa::CVector vecPos;
	sa::CVector vecRot;
	float fDrawDistance;
	uint8_t byteNoCameraCol;
	OBJECTID AttachedObjectID;
	VEHICLEID AttachedVehicleID;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ObjectID);
	bsData.Read(iModel);
	bsData.Read(vecPos.x);
	bsData.Read(vecPos.y);
	bsData.Read(vecPos.z);
	bsData.Read(vecRot.x);
	bsData.Read(vecRot.y);
	bsData.Read(vecRot.z);
	bsData.Read(fDrawDistance);
	bsData.Read(byteNoCameraCol);
	bsData.Read(AttachedVehicleID);
	bsData.Read(AttachedObjectID);

	sa::CVector vecAttachOffset;
	sa::CVector vecAttachRot;
	uint8_t bSyncRotation;

	if (AttachedObjectID != INVALID_OBJECT_ID || AttachedVehicleID != INVALID_VEHICLE_ID) {
		bsData.Read(vecAttachOffset.x);
		bsData.Read(vecAttachOffset.y);
		bsData.Read(vecAttachOffset.z);
		bsData.Read(vecAttachRot.x);
		bsData.Read(vecAttachRot.y);
		bsData.Read(vecAttachRot.z);
		bsData.Read(bSyncRotation);
	}

	CObjectPool* pObjectPool = pNetGame->GetObjectPool();
	pObjectPool->New(ObjectID, iModel, vecPos, vecRot, fDrawDistance);

	CObject* pObject = pObjectPool->GetAt(ObjectID);
	if (AttachedObjectID != INVALID_OBJECT_ID) {
		if (pObject) {
			pObject->SetAttachedObject(AttachedObjectID, &vecAttachOffset, &vecAttachRot, bSyncRotation);
		}
	}
	else if (AttachedVehicleID != INVALID_VEHICLE_ID) {
		if (pObject) {
			pObject->SetAttachedVehicle(AttachedVehicleID, &vecAttachOffset, &vecAttachRot);
		}
	}

	uint8_t byteMaterialsCount;
	bsData.Read(byteMaterialsCount);
	if (byteMaterialsCount > 0) {
		char txdname[256];
		char texturename[256];
		uint8_t byteType;
		uint8_t byteMaterialIndex;
		uint16_t MaterialModel;
		uint8_t byteLength;
		uint32_t dwColor;

		bsData.Read(byteType);

		if (byteType == MATERIAL_TYPE_TEXTURE) {
			bsData.Read(byteMaterialIndex);
			bsData.Read(MaterialModel);
			bsData.Read(byteLength);
			bsData.Read(txdname, byteLength);
			txdname[byteLength] = '\0';
			bsData.Read(byteLength);
			bsData.Read(texturename, byteLength);
			texturename[byteLength] = '\0';
			bsData.Read(dwColor);

			if (strlen(txdname) < 32 && strlen(texturename) < 32) {
				if (MaterialModel == 0xFFFF || MaterialModel > 20000) {
					MaterialModel = 0xFFFF;
				}

				CObject* pObject = pObjectPool->GetAt(ObjectID);
				if (pObject) {
					pObject->SetMaterial(MaterialModel, byteMaterialIndex, txdname, texturename, dwColor);
				}
			}
		}
		else if (byteType == MATERIAL_TYPE_TEXT) {
			uint8_t byteMaterialSize;
			char szFontName[32 + 1];
			uint8_t byteFontSize;
			uint8_t byteFontBold;
			uint32_t dwFontColor;
			uint32_t dwBackgroundColor;
			uint8_t byteAlign;
			char szText[2048 + 1];

			bsData.Read(byteMaterialSize);
			bsData.Read(byteLength);
			if (byteLength > sizeof(szFontName)) {
				return;
			}

			bsData.Read(szFontName, byteLength);
			szFontName[byteLength] = '\0';

			bsData.Read(byteFontSize);
			bsData.Read(byteFontBold);
			bsData.Read(dwFontColor);
			bsData.Read(dwBackgroundColor);
			bsData.Read(byteAlign);
			stringCompressor->DecodeString(szText, 2048 + 1, &bsData);

			char utf8[2048 + 1];
			strncpy(utf8, Encoding::cp2utf(szText).c_str(), 2048 + 1);

			pObject->SetMaterialText(byteMaterialIndex, byteMaterialSize, szFontName, byteFontSize, byteFontBold, dwFontColor, dwBackgroundColor, byteAlign, utf8);
		}
	}

	iTotalObjects++;
}

void ScrDestroyObject(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrDestroyObject");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	OBJECTID ObjectID;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ObjectID);

	iTotalObjects--;
	spdlog::info("DestroyObject; Total objects: {}", iTotalObjects);

	CObjectPool* pObjectPool = pNetGame->GetObjectPool();
	pObjectPool->Delete(ObjectID);
}

void ScrSetObjectMaterial(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetObjectMaterial");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	CObjectPool* pObjectPool = pNetGame->GetObjectPool();
	if (pObjectPool) {
		uint16_t wObjectId;

		bsData.Read(wObjectId);
		if (wObjectId < 0 || wObjectId >= MAX_OBJECTS) {
			return;
		}

		CObject* pObject = pObjectPool->GetAt(wObjectId);
		if (pObject) {
			uint8_t byteMaterialType;
			uint8_t byteMaterialIndex;
			uint8_t byteLength;

			bsData.Read(byteMaterialType);
			bsData.Read(byteMaterialIndex);
			switch (byteMaterialType) {
				case MATERIAL_TYPE_TEXTURE: {
					uint16_t wModelID;
					char szTxdName[32 + 1], szTexName[32 + 1];
					uint32_t dwColor;

					bsData.Read(wModelID);
					bsData.Read(byteLength);
					if (byteLength > sizeof(szTxdName)) {
						return;
					}

					bsData.Read(szTxdName, byteLength);
					szTxdName[byteLength] = '\0';

					bsData.Read(byteLength);
					if (byteLength > sizeof(szTexName)) {
						return;
					}

					bsData.Read(szTexName, byteLength);
					szTexName[byteLength] = '\0';

					bsData.Read(dwColor);

					pObject->SetMaterial(wModelID, byteMaterialIndex, szTxdName, szTexName,
							dwColor);
					break;
				}
				case MATERIAL_TYPE_TEXT: {
					uint8_t byteMaterialSize;
					char szFontName[32 + 1];
					uint8_t byteFontSize;
					uint8_t byteFontBold;
					uint32_t dwFontColor;
					uint32_t dwBackgroundColor;
					uint8_t byteAlign;
					char szText[2048 + 1];

					bsData.Read(byteMaterialSize);
					bsData.Read(byteLength);
					if (byteLength > sizeof(szFontName)) {
						return;
					}

					bsData.Read(szFontName, byteLength);
					szFontName[byteLength] = '\0';

					bsData.Read(byteFontSize);
					bsData.Read(byteFontBold);
					bsData.Read(dwFontColor);
					bsData.Read(dwBackgroundColor);
					bsData.Read(byteAlign);
					stringCompressor->DecodeString(szText, 2048 + 1, &bsData);

					char utf8[2048 + 1];
					strncpy(utf8, Encoding::cp2utf(szText).c_str(), 2048 + 1);

					pObject->SetMaterialText(byteMaterialIndex, byteMaterialSize, szFontName, byteFontSize, byteFontBold, dwFontColor, dwBackgroundColor, byteAlign, utf8);
					break;
				}
			}
		}
	}
}

void ScrRemoveBuilding(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrRemoveBuilding");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	int iModel;
	float fX, fY, fZ;
	float fRadius;

	bsData.Read(iModel);
	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);
	bsData.Read(fRadius);

	RemoveBuilding::add(iModel, {fX, fY, fZ}, fRadius);
}

void ScrSetPlayerSkin(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetPlayerSkin");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (!pPlayerPool) {
		return;
	}

	int iPlayerID;
	int iModel;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(iPlayerID);
	bsData.Read(iModel);

	if (IsValidPedModel(iModel)) {
		if (pPlayerPool->GetLocalPlayerID() == iPlayerID) {
			pPlayerPool->GetLocalPlayer()->GetPlayerPed()->SetModelIndex(iModel);
		}
		else {
			if (pPlayerPool->GetSlotState(iPlayerID)) {
				CRemotePlayer* pPlayer = pPlayerPool->GetAt(iPlayerID);
				if (!pPlayer || pPlayer->GetState() == PLAYER_STATE_NONE) {
					return;
				}
				CPlayerPed* pPlayerPed = pPlayer->GetPlayerPed();
				if (!pPlayerPed) {
					return;
				}
				pPlayerPed->SetModelIndex(iModel);
			}
		}
	}
	else {
		if (pUI) pUI->chat()->addDebugMessage("Warning: SetPlayerSkin %d isn't a valid ped model.", iModel);
	}
}

void ScrSetPlayerMapIcon(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetPlayerMapIcon");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint8_t byteIconID;
	sa::CVector vecPos;
	uint8_t byteType;
	uint32_t dwColor;
	uint8_t byteStyle;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(byteIconID);
	bsData.Read(vecPos.x);
	bsData.Read(vecPos.y);
	bsData.Read(vecPos.z);
	bsData.Read(byteType);
	bsData.Read(dwColor);
	bsData.Read(byteStyle);

	// fix crash (invalid GTASA map icons)
	if (byteIconID == 1 or byteIconID == 2 or byteIconID == 4 or byteIconID == 56) {
		byteIconID = 52;
	}

	pNetGame->SetMapIcon(byteIconID, vecPos.x, vecPos.y, vecPos.z, byteType, dwColor, byteStyle);
}

void ScrRemovePlayerMapIcon(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrRemovePlayerMapIcon");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint8_t byteIconID;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(byteIconID);

	pNetGame->DisableMapIcon(byteIconID);
}

void ScrShowNameTag(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrShowNameTag");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID PlayerID;
	uint8_t byteShowNameTag;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(PlayerID);
	bsData.Read(byteShowNameTag);

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (pPlayerPool->GetSlotState(PlayerID)) {
		CRemotePlayer* pPlayer = pPlayerPool->GetAt(PlayerID);
		if (pPlayer) {
			pPlayer->m_bShowNameTag = byteShowNameTag;
		}
	}
}

void ScrApplyPlayerAnimation(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrApplyPlayerAnimation");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID playerId;
	uint8_t byteAnimLibLen;
	uint8_t byteAnimNameLen;
	char szAnimLib[256];
	char szAnimName[256];
	float fS;
	bool opt1, opt2, opt3, opt4;
	int opt5;

	memset(szAnimLib, 0, 256);
	memset(szAnimName, 0, 256);

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(playerId);
	bsData.Read(byteAnimLibLen);
	bsData.Read(szAnimLib, byteAnimLibLen);
	bsData.Read(byteAnimNameLen);
	bsData.Read(szAnimName, byteAnimNameLen);
	bsData.Read(fS);
	bsData.Read(opt1);
	bsData.Read(opt2);
	bsData.Read(opt3);
	bsData.Read(opt4);
	bsData.Read(opt5);

	szAnimLib[byteAnimLibLen] = '\0';
	szAnimName[byteAnimNameLen] = '\0';

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	CPlayerPed* pPlayerPed = nullptr;

	if (pPlayerPool) {
		if (pPlayerPool->GetLocalPlayerID() == playerId) {
			pPlayerPed = pPlayerPool->GetLocalPlayer()->GetPlayerPed();
		}
		else if (pPlayerPool->GetSlotState(playerId)) {
			pPlayerPed = pPlayerPool->GetAt(playerId)->GetPlayerPed();
		}

		spdlog::info("ApplyAnimation: {}, {}", szAnimLib, szAnimName);

		if (pPlayerPed) {
			pPlayerPed->ApplyAnimation(szAnimName, szAnimLib, fS, opt1, opt2, opt3, opt4, opt5);
		}
	}
}

void ScrClearPlayerAnimations(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrClearPlayerAnimations");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	PLAYERID playerId;
	bsData.Read(playerId);
	RwMatrix mat;

	CPlayerPool* pPlayerPool = NULL;
	CPlayerPed* pPlayerPed = NULL;

	pPlayerPool = pNetGame->GetPlayerPool();

	if (pPlayerPool) {
		if (playerId == pPlayerPool->GetLocalPlayerID()) {
			pPlayerPed = pPlayerPool->GetLocalPlayer()->GetPlayerPed();
		}
		else {
			if (pPlayerPool->GetSlotState(playerId)) {
				pPlayerPed = pPlayerPool->GetAt(playerId)->GetPlayerPed();
			}
		}

		if (pPlayerPed) {
			mat = pPlayerPed->m_ped->GetMatrix().ToRwMatrix();
			pPlayerPed->TeleportTo(mat.pos.x, mat.pos.y, mat.pos.z);
		}
	}
}

void ScrSetPlayerHealth(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetPlayerHealth");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	float fHealth;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(fHealth);

	pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->SetHealth(fHealth);
}

void ScrGivePlayerWeapon(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrGivePlayerWeapon");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	int iWeapon;
	int iAmmo;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(iWeapon);
	bsData.Read(iAmmo);
	pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->GiveWeapon(iWeapon, iAmmo);
}

void ScrSetPlayerInterior(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetPlayerInterior");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint8_t byteInteriorId;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(byteInteriorId);

	pGame->FindPlayerPed()->SetInterior(byteInteriorId, true);
}

void ScrShowTextDraw(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrShowTextDraw");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CTextDrawPool* pTextDrawPool = pNetGame->GetTextDrawPool();
	if (pTextDrawPool == nullptr) {
		return;
	}

	uint16_t wTextDrawID;
	TEXT_DRAW_TRANSMIT textDrawTransmit;
	uint16_t wTextLength;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(wTextDrawID);
	bsData.Read((char*) &textDrawTransmit, sizeof(TEXT_DRAW_TRANSMIT));
	bsData.Read(wTextLength);

	char szText[MAX_TEXT_DRAW_LINE + 1];

	if (wTextLength < MAX_TEXT_DRAW_LINE) {
		bsData.Read(szText, wTextLength);
		szText[wTextLength] = '\0';

		pTextDrawPool->New(wTextDrawID, &textDrawTransmit, szText);
	}
	else {
		if (pUI) pUI->chat()->addInfoMessage("Warning: ignoring large TetDraw size=%u", wTextLength);
	}
}

void ScrHideTextDraw(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrHideTextDraw");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CTextDrawPool* pTextDrawPool = pNetGame->GetTextDrawPool();
	if (pTextDrawPool == nullptr) {
		return;
	}

	uint16_t wTextDrawID;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(wTextDrawID);

	pTextDrawPool->Delete(wTextDrawID);
}

void ScrTextDrawSetString(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrTextDrawSetString");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CTextDrawPool* pTextDrawPool = pNetGame->GetTextDrawPool();
	if (pTextDrawPool == nullptr) {
		return;
	}

	uint16_t wTextDrawID;
	uint16_t wTextLength;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(wTextDrawID);
	bsData.Read(wTextLength);

	if (wTextLength < MAX_TEXT_DRAW_LINE) {
		char szText[MAX_TEXT_DRAW_LINE + 1];
		bsData.Read(szText, wTextLength);
		szText[wTextLength] = '\0';

		CTextDraw* pTextDraw = pTextDrawPool->GetAt(wTextDrawID);
		if (pTextDraw) {
			pTextDraw->SetText(szText);
		}
	}
}

void ScrSelectTextDraw(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSelectTextDraw");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	bool bEnable = false;
	uint32_t dwColor = 0;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(bEnable);
	bsData.Read(dwColor);
	spdlog::info("SELECTTEXTDRAW: bEnable: {}, dwColor: {}", bEnable, dwColor);

	pNetGame->GetTextDrawPool()->SetSelectState(bEnable ? true : false, dwColor);
}

void ScrSetPlayerAmmo(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetPlayerAmmo");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CLocalPlayer* pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();

	uint8_t byteWeapon;
	uint16_t wAmmo;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(byteWeapon);
	bsData.Read(wAmmo);

	pLocalPlayer->GetPlayerPed()->SetAmmo(byteWeapon, wAmmo);
}

void ScrSetVehicleHealth(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetVehicleHealth");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	VEHICLEID VehicleID;
	float fHealth;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(VehicleID);
	bsData.Read(fHealth);

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (pVehiclePool) {
		CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
		if (pVehicle) {
			pVehicle->SetHealth(fHealth);
		}
	}
}

void ScrAttachTrailerToVehicle(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrAttachTrailerToVehicle");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	VEHICLEID TrailerID;
	VEHICLEID VehicleID;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(TrailerID);
	bsData.Read(VehicleID);

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) {
		return;
	}

	CVehicle* pTrailer = pVehiclePool->GetAt(TrailerID);
	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);

	if (pTrailer && pVehicle) {
		pVehicle->SetTrailer(pTrailer);
		pVehicle->AttachTrailer();
	}
}

void ScrDetachTrailerFromVehicle(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrDetachTrailerFromVehicle");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) {
		return;
	}

	VEHICLEID VehicleID;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(VehicleID);

	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
	if (pVehicle) {
		pVehicle->DetachTrailer();
		pVehicle->SetTrailer(nullptr);
	}
}

void ScrSetObjectPos(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetObjectPos");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	OBJECTID ObjectID;
	float fX;
	float fY;
	float fZ;
	float fUnused;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ObjectID);
	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);
	bsData.Read(fUnused);

	CObjectPool* pObjectPool = pNetGame->GetObjectPool();
	if (!pObjectPool) {
		return;
	}

	CObject* pObject = pObjectPool->GetAt(ObjectID);
	if (pObject) {
		pObject->TeleportTo(fX, fY, fZ);
	}
}

void ScrSetObjectRotation(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetObjectRotation");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	OBJECTID ObjectID;
	sa::CVector vecRot;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ObjectID);
	bsData.Read(vecRot.x);
	bsData.Read(vecRot.y);
	bsData.Read(vecRot.z);

	CObjectPool* pObjectPool = pNetGame->GetObjectPool();
	if (!pObjectPool) {
		return;
	}

	CObject* pObject = pObjectPool->GetAt(ObjectID);
	if (!pObject) {
		return;
	}

	pObject->SetRotation(&vecRot);
}

void ScrCreateExplosion(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrCreateExplosion");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	float fX, fY, fZ;
	uint32_t dwType;
	float fRadius;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);
	bsData.Read(dwType);
	bsData.Read(fRadius);

	ScriptCommand(&create_explosion_with_radius, fX, fY, fZ, dwType, fRadius);
}

void ScrSetVehicleNumberPlate(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetVehicleNumberPlate");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	VEHICLEID VehicleID;
	uint8_t byteLength;
	char szPlateName[32 + 1];
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(VehicleID);
	bsData.Read(byteLength);
	if (byteLength > 32) {
		return;
	}

	bsData.Read(szPlateName, byteLength);
	szPlateName[byteLength] = '\0';

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) {
		return;
	}

	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
	if (!pVehicle) {
		return;
	}

	pVehicle->SetPlateText(szPlateName);
}

#define SPECTATE_TYPE_NORMAL 1
#define SPECTATE_TYPE_FIXED 2
#define SPECTATE_TYPE_SIDE 3

void ScrSpectatePlayer(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSpectatePlayer");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID PlayerID;
	uint8_t byteMode;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(PlayerID);
	bsData.Read(byteMode);

	switch (byteMode) {
		case SPECTATE_TYPE_FIXED: byteMode = 15;
			break;
		case SPECTATE_TYPE_SIDE: byteMode = 14;
			break;
		default: byteMode = 4;
	}

	CLocalPlayer* pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
	pLocalPlayer->m_byteSpectateMode = byteMode;
	pLocalPlayer->SpectatePlayer(PlayerID);
}

void ScrSpectateVehicle(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSpectateVehicle");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	VEHICLEID VehicleID;
	uint8_t byteMode;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(VehicleID);
	bsData.Read(byteMode);

	switch (byteMode) {
		case SPECTATE_TYPE_FIXED: byteMode = 15;
			break;
		case SPECTATE_TYPE_SIDE: byteMode = 14;
			break;
		default: byteMode = 3;
	}

	CLocalPlayer* pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
	pLocalPlayer->m_byteSpectateMode = byteMode;
	pLocalPlayer->SpectateVehicle(VehicleID);
}

void ScrRemoveVehicleComponent(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrRemoveVehicleComponent");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) {
		return;
	}

	VEHICLEID VehicleID;
	uint16_t wComponent;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(VehicleID);
	bsData.Read(wComponent);

	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
	if (!pVehicle) {
		return;
	}

	pVehicle->RemoveComponent(wComponent);
}

void ScrAttachObjectToPlayer(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrAttachObjectToPlayer");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	OBJECTID ObjectID;
	PLAYERID PlayerID;
	float offsetX, offsetY, offsetZ;
	float rX, rY, rZ;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ObjectID);
	bsData.Read(PlayerID);
	bsData.Read(offsetX);
	bsData.Read(offsetY);
	bsData.Read(offsetZ);
	bsData.Read(rX);
	bsData.Read(rY);
	bsData.Read(rZ);

	CObjectPool* pObjectPool = pNetGame->GetObjectPool();
	if (!pObjectPool) {
		return;
	}
	CObject* pObject = pObjectPool->GetAt(ObjectID);
	if (!pObject) {
		return;
	}

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (!pPlayerPool) {
		return;
	}

	if (pPlayerPool->GetLocalPlayerID() == PlayerID) {
		CLocalPlayer* pLocalPlayer = pPlayerPool->GetLocalPlayer();
		ScriptCommand(&attach_object_to_actor,
				pObject->m_gtaId,
				pLocalPlayer->GetPlayerPed()->m_gtaId,
				offsetX, offsetY, offsetZ,
				rX, rY, rZ);
	}
	else {
		CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(PlayerID);
		ScriptCommand(&attach_object_to_actor,
				pObject->m_gtaId,
				pRemotePlayer->GetPlayerPed()->m_gtaId,
				offsetX, offsetY, offsetZ,
				rX, rY, rZ);
	}
}

void ScrSetPlayerWantedLevel(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetPlayerWantedLevel");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	uint8_t byteWantedLevel;
	bsData.Read(byteWantedLevel);

	if (pGame) {
		pGame->SetWantedLevel(byteWantedLevel);
	}
}

void ScrSetPlayerSpecialAction(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetPlayerSpecialAction");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint8_t byteSpecialAction;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(byteSpecialAction);

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (!pPlayerPool) {
		return;
	}

	pPlayerPool->GetLocalPlayer()->ApplySpecialAction(byteSpecialAction);
}

void ScrEnableStuntBonus(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrEnableStuntBonus");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	bool bEnable;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(bEnable);

	pGame->EnableStuntBonus(bEnable);
}

void ScrSetPlayerFightingStyle(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetPlayerFightingStyle");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID PlayerID;
	uint8_t byteStyle;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(PlayerID);
	bsData.Read(byteStyle);

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (!pPlayerPool) {
		return;
	}

	CPlayerPed* pPlayerPed = nullptr;

	if (PlayerID == pPlayerPool->GetLocalPlayerID()) {
		pPlayerPed = pPlayerPool->GetLocalPlayer()->GetPlayerPed();
	}
	else {
		CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(PlayerID);
		if (pRemotePlayer) {
			pPlayerPed = pRemotePlayer->GetPlayerPed();
		}
	}

	if (pPlayerPed) {
		pPlayerPed->SetFightingStyle(byteStyle);
	}
}

void ScrSetPlayerVelocity(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetPlayerVelocity");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	sa::CVector vecVelocity;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(vecVelocity.x);
	bsData.Read(vecVelocity.y);
	bsData.Read(vecVelocity.z);

	CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
	if (!pPlayerPed) {
		return;
	}

	if (pPlayerPed->IsOnGround()) {
		auto flags = pPlayerPed->GetStateFlags();
		flags.bIsStanding = !flags.bIsStanding;
		flags.bWasStanding = !flags.bWasStanding;
		pPlayerPed->SetStateFlags(flags);
	}

	pPlayerPed->m_ped->SetVelocity(vecVelocity);
}

void ScrSetVehicleVelocity(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetVehicleVelocity");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint8_t byteType;
	sa::CVector vecVelocity;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(byteType);
	bsData.Read(vecVelocity.x);
	bsData.Read(vecVelocity.y);
	bsData.Read(vecVelocity.z);

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) {
		return;
	}

	CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
	if (!pPlayerPed) {
		return;
	}

	if (pPlayerPed->IsInVehicle()) {
		sa::CVehicle* pGtaVehicle = pPlayerPed->GetGtaVehicle();
		VEHICLEID VehicleID = pVehiclePool->FindIDFromGtaPtr(pGtaVehicle);
		CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
		if (!pVehicle) {
			return;
		}

		if (byteType) {
			if (byteType == 1) {
				pVehicle->m_pVehicle->SetTurnSpeed(vecVelocity);
			}
		}
		else {
			pVehicle->m_pVehicle->SetVelocity(vecVelocity);
		}
	}
}

void ScrToggleWidescreen(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrToggleWidescreen");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint8_t byteToggle;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(byteToggle);
	//if (pUI) pUI->chat()->addDebugMessage("Widescreen = %d", byteToggle);
	ScriptCommand(&toggle_widescreen, byteToggle);
}

void ScrSetVehicleTireDamageStatus(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetVehicleTireDamageStatus");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	VEHICLEID VehicleID;
	uint8_t byteTireDamageStatus;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(VehicleID);
	bsData.Read(byteTireDamageStatus);

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) {
		return;
	}

	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
	if (!pVehicle) {
		return;
	}

	pVehicle->SetTireDamageStatus(byteTireDamageStatus);
}

void ScrSetPlayerTeam(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetPlayerTeam");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID PlayerID;
	uint8_t byteTeam;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(PlayerID);
	bsData.Read(byteTeam);

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (!pPlayerPool) {
		return;
	}

	CLocalPlayer* pLocalPlayer = pPlayerPool->GetLocalPlayer();
	if (!pLocalPlayer) {
		return;
	}

	if (PlayerID == pPlayerPool->GetLocalPlayerID()) {
		pLocalPlayer->m_byteTeam = byteTeam;
	}
	else {
		CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(PlayerID);
		if (pRemotePlayer) {
			pRemotePlayer->SetTeam(byteTeam);
		}
	}
}

void ScrSetPlayerName(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetPlayerName");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID PlayerID;
	uint8_t byteLen;
	char szName[28];
	uint8_t byteSuccess;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(PlayerID);
	bsData.Read(byteLen);
	if (byteLen > MAX_PLAYER_NAME) {
		return;
	}

	bsData.Read(szName, byteLen);
	szName[byteLen] = '\0';
	bsData.Read(byteSuccess);

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (!pPlayerPool) {
		return;
	}

	if (byteSuccess == 1) {
		pPlayerPool->SetPlayerName(PlayerID, szName);
	}

	if (pPlayerPool->GetLocalPlayerID() == PlayerID) {
		pPlayerPool->SetLocalPlayerName(szName);
	}
}

void ScrSetPlayerPos(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetPlayerPos");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	sa::CVector vecPos;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(vecPos.x);
	bsData.Read(vecPos.y);
	bsData.Read(vecPos.z);

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (!pPlayerPool) {
		return;
	}
	CLocalPlayer* pLocalPlayer = pPlayerPool->GetLocalPlayer();
	if (!pLocalPlayer) {
		return;
	}

	pLocalPlayer->DisableSurfing();
	pLocalPlayer->GetPlayerPed()->TeleportTo(vecPos.x, vecPos.y, vecPos.z);
}

void ScrSetPlayerPosFindZ(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetPlayerPosFindZ");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	sa::CVector vecPos;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(vecPos.x);
	bsData.Read(vecPos.y);
	bsData.Read(vecPos.z);

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (!pPlayerPool) {
		return;
	}

	CLocalPlayer* pLocalPlayer = pPlayerPool->GetLocalPlayer();
	if (!pLocalPlayer) {
		return;
	}

	vecPos.z = pGame->FindGroundZForCoord(vecPos.x, vecPos.y, vecPos.z) + 1.5f;
	pLocalPlayer->DisableSurfing();
	pLocalPlayer->GetPlayerPed()->TeleportTo(vecPos.x, vecPos.y, vecPos.z);
}

void ScrPutPlayerInVehicle(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrPutPlayerInVehicle");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	VEHICLEID VehicleID;
	uint8_t byteSeatID;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(VehicleID);
	bsData.Read(byteSeatID);

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) {
		return;
	}

	uint32_t dwGtaVehicleID = pVehiclePool->FindGtaIDFromID(VehicleID);
	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);

	if (dwGtaVehicleID && pVehicle) {
		pGame->FindPlayerPed()->PutDirectlyInVehicle(dwGtaVehicleID, byteSeatID);
	}
}

void ScrRemovePlayerFromVehicle(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrRemovePlayerFromVehicle");
	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (!pPlayerPool) {
		return;
	}

	CLocalPlayer* pLocalPlayer = pPlayerPool->GetLocalPlayer();
	if (!pLocalPlayer) {
		return;
	}

	pLocalPlayer->GetPlayerPed()->ExitCurrentVehicle();
}

void ScrSetPlayerColor(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetPlayerColor");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID PlayerID;
	uint32_t dwColor;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(PlayerID);
	bsData.Read(dwColor);

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (!pPlayerPool) {
		return;
	}

	if (pPlayerPool->GetLocalPlayerID() == PlayerID) {
		CLocalPlayer* pLocalPlayer = pPlayerPool->GetLocalPlayer();
		if (pLocalPlayer) {
			pLocalPlayer->SetPlayerColor(dwColor);
		}
	}
	else {
		CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(PlayerID);
		if (pRemotePlayer) {
			pRemotePlayer->SetPlayerColor(dwColor);
		}
	}
}

void ScrShowGameText(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrShowGameText");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	char szMsg[256];
	memset(szMsg, 0, sizeof(szMsg));

	int iSize;
	int iTime;
	int iLen;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(iSize);
	bsData.Read(iTime);
	bsData.Read(iLen);

	if (iLen < 0 || iLen > 200) {
		return;
	}

	bsData.Read(szMsg, iLen);
	szMsg[iLen] = '\0';
	pGame->DisplayGameText(szMsg, iTime, iSize);
}

void ScrSetVehiclePos(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetVehiclePos");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	VEHICLEID VehicleID;
	float fX;
	float fY;
	float fZ;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(VehicleID);
	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) {
		return;
	}

	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
	if (!pVehicle) {
		return;
	}

	pVehicle->TeleportTo(fX, fY, fZ);
}

void ScrSetVehicleZAngle(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetVehicleZAngle");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	VEHICLEID VehicleID;
	float fAngle;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(VehicleID);
	bsData.Read(fAngle);

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) {
		return;
	}

	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
	if (!pVehicle) {
		return;
	}

	pVehicle->SetZAngle(fAngle);
}

void ScrSetVehicleParams(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetVehicleParams");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	VEHICLEID VehicleID;
	uint8_t byteObjective;
	uint8_t byteDoorsLocked;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(VehicleID);
	bsData.Read(byteObjective);
	bsData.Read(byteDoorsLocked);

	spdlog::info("VehicleID: {}", VehicleID);
	spdlog::info("objective: {}, doors: {}", byteObjective, byteDoorsLocked);

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	pVehiclePool->AssignSpecialParamsToVehicle(VehicleID, byteObjective, byteDoorsLocked);
}

void ScrSetPlayerCameraBehindPlayer(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetPlayerCameraBehindPlayer");
	pGame->GetCamera()->SetBehindPlayer();
}

void ScrTogglePlayerControllable(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrTogglePlayerControllable");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint8_t byteControllable;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(byteControllable);

	pGame->FindPlayerPed()->TogglePlayerControllable(byteControllable);
}

void ScrPlayerPlaySound(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrPlayerPlaySound");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	int iSound;
	float fX, fY, fZ;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(iSound);
	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);

	// TODO: Implement
}

void ScrSetWorldBounds(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetWorldBounds");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	float bounds[4];
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(bounds[0]);
	bsData.Read(bounds[1]);
	bsData.Read(bounds[2]);
	bsData.Read(bounds[3]);

	pNetGame->m_netSet->worldBounds[0] = bounds[0];
	pNetGame->m_netSet->worldBounds[1] = bounds[1];
	pNetGame->m_netSet->worldBounds[2] = bounds[2];
	pNetGame->m_netSet->worldBounds[3] = bounds[3];
}

void ScrGivePlayerMoney(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrGivePlayerMoney");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	int iMoney;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(iMoney);

	pGame->AddToLocalMoney(iMoney);
}

void ScrSetPlayerFacingAngle(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetPlayerFacingAngle");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	float fRotation;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(fRotation);

	pGame->FindPlayerPed()->SetTargetRotation(fRotation);
}

void ScrResetPlayerMoney(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrResetPlayerMoney");
	pGame->ResetLocalMoney();
}

void ScrResetPlayerWeapons(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrResetPlayerWeapons");
	pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->ClearWeapons();
}

void ScrLinkVehicleToInterior(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrLinkVehicleToInterior");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	VEHICLEID VehicleID;
	uint8_t byteInterior;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(VehicleID);
	bsData.Read(byteInterior);

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) {
		return;
	}

	pVehiclePool->LinkToInterior(VehicleID, byteInterior);
}

void ScrSetPlayerArmour(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetPlayerArmour");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CLocalPlayer* pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();

	float fArmour;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(fArmour);
	pLocalPlayer->GetPlayerPed()->SetArmour(fArmour);
}

void ScrSetArmedWeapon(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetArmedWeapon");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint32_t dwWeapon;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(dwWeapon);
	if (dwWeapon >= 0 && dwWeapon <= 46) {
		CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
		if (pPlayerPed) {
			pPlayerPed->SetArmedWeapon(dwWeapon);
		}
	}
}

void ScrSetPlayerAttachedObject(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetPlayerAttachedObject");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	NEW_ATTACHED_OBJECT NewAttachedObject;
	memset(&NewAttachedObject, 0, sizeof(NEW_ATTACHED_OBJECT));

	PLAYERID PlayerID;
	int index;
	bool bCreate;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(PlayerID);
	bsData.Read(index);
	bsData.Read(bCreate);

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (pPlayerPool) {
		CPlayerPed* pPlayerPed = nullptr;

		if (pPlayerPool->GetLocalPlayerID() == PlayerID) {
			pPlayerPed = pPlayerPool->GetLocalPlayer()->GetPlayerPed();
		}
		else {
			CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(PlayerID);
			if (pRemotePlayer) {
				pPlayerPed = pRemotePlayer->GetPlayerPed();
			}
		}

		if (pPlayerPed) {
			if (bCreate) {
				bsData.Read((char*) &NewAttachedObject, sizeof(NEW_ATTACHED_OBJECT));
				pPlayerPed->SetAttachedObject(index, &NewAttachedObject);
			}
			else {
				pPlayerPed->RemoveAttachedObject(index);
			}
		}
	}
}

void ScrApplyActorAnimation(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrApplyActorAnimation");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CActorPool* pActorPool = pNetGame->GetActorPool();
	if (!pActorPool) {
		return;
	}

	char szAnimLib[256];
	char szAnimName[256];
	memset(szAnimLib, 0, 256);
	memset(szAnimName, 0, 256);

	PLAYERID ActorID;
	uint8_t byteAnimLibLen;
	uint8_t byteAnimNameLen;
	float fDelta;
	bool bLoop;
	bool bLockX;
	bool bLockY;
	bool bFreeze;
	int iTime;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ActorID);
	bsData.Read(byteAnimLibLen);
	bsData.Read(szAnimLib, byteAnimLibLen);
	bsData.Read(byteAnimNameLen);
	bsData.Read(szAnimName, byteAnimNameLen);
	bsData.Read(fDelta);
	bsData.Read(bLoop);
	bsData.Read(bLockX);
	bsData.Read(bLockY);
	bsData.Read(bFreeze);
	bsData.Read(iTime);

	szAnimLib[byteAnimLibLen] = '\0';
	szAnimName[byteAnimNameLen] = '\0';

	CActor* pActor = pActorPool->GetAt(ActorID);
	if (pActor) {
		pActor->ApplyAnimation(szAnimName, szAnimLib, fDelta, bLoop, bLockX, bLockY, bFreeze, iTime);
	}
}

void ScrClearActorAnimation(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrClearActorAnimation");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CActorPool* pActorPool = pNetGame->GetActorPool();
	if (!pActorPool) {
		return;
	}

	PLAYERID ActorID;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ActorID);

	CActor* pActor = pActorPool->GetAt(ActorID);
	if (pActor) {
		pActor->ClearAnimation();
	}
}

void ScrSetActorFacingAngle(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetActorFacingAngle");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CActorPool* pActorPool = pNetGame->GetActorPool();
	if (!pActorPool) {
		return;
	}

	PLAYERID ActorID;
	float fAngle;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ActorID);
	bsData.Read(fAngle);

	CActor* pActor = pActorPool->GetAt(ActorID);
	if (pActor) {
		pActor->SetFacingAngle(fAngle);
	}
}

void ScrSetActorPos(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetActorPos");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CActorPool* pActorPool = pNetGame->GetActorPool();
	if (!pActorPool) {
		return;
	}

	PLAYERID ActorID;
	sa::CVector vecPos;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ActorID);
	bsData.Read(vecPos.x);
	bsData.Read(vecPos.y);
	bsData.Read(vecPos.z);

	CActor* pActor = pActorPool->GetAt(ActorID);
	if (pActor) {
		pActor->TeleportTo(vecPos.x, vecPos.y, vecPos.z);
	}
}

void ScrSetActorHealth(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetActorHealth");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CActorPool* pActorPool = pNetGame->GetActorPool();
	if (!pActorPool) {
		return;
	}

	PLAYERID ActorID;
	float fHealth;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ActorID);
	bsData.Read(fHealth);

	CActor* pActor = pActorPool->GetAt(ActorID);
	if (pActor) {
		pActor->SetHealth(fHealth);
	}
}

void ScrPlayAudioStream(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrPlayAudioStream");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint8_t byteUrlLen;
	char szUrl[256];
	float fX, fY, fZ;
	float fRadius;
	uint8_t byteUsePos;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	memset(szUrl, 0, sizeof(szUrl));

	bsData.Read(byteUrlLen);
	bsData.Read(szUrl, byteUrlLen);
	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);
	bsData.Read(fRadius);
	bsData.Read(byteUsePos);

	if (pUI) {
		pUI->chat()->addInfoMessage("[Audio Stream]: %s", szUrl);
	}

	if (pAudioStream) {
		pAudioStream->Play(szUrl, fX, fY, fZ, fRadius, byteUsePos);
	}
}

void ScrStopAudioStream(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrStopAudioStream");

	if (pAudioStream) {
		pAudioStream->Stop(false);
	}
	// pAudioStream->Stop(true);
}

void ScrMoveObject(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrMoveObject");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	OBJECTID ObjectID;
	float fPad0, fPad1, fPad2;
	float fPosX, fPosY, fPosZ;
	float fSpeed;
	float fRotX, fRotY, fRotZ;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ObjectID);
	bsData.Read(fPad0);
	bsData.Read(fPad1);
	bsData.Read(fPad2);
	bsData.Read(fPosX);
	bsData.Read(fPosY);
	bsData.Read(fPosZ);
	bsData.Read(fSpeed);
	bsData.Read(fRotX);
	bsData.Read(fRotY);
	bsData.Read(fRotZ);

	CObjectPool* pObjectPool = pNetGame->GetObjectPool();
	CObject* pObject = pObjectPool->GetAt(ObjectID);
	if (pObject) {
		pObject->MoveTo(fPosX, fPosY, fPosZ, fSpeed, fRotX, fRotY, fRotZ);
	}
}

void ScrStopObject(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrStopObject");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	OBJECTID ObjectID;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ObjectID);

	CObjectPool* pObjectPool = pNetGame->GetObjectPool();
	CObject* pObject = pObjectPool->GetAt(ObjectID);

	if (pObject) {
		pObject->StopMoving();
	}
}

void ScrSetPlayerDrunkLevel(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetPlayerDrunkLevel");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	uint32_t dwLevel;
	bsData.Read(dwLevel);

	CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
	if (pPlayerPed) {
		pPlayerPed->SetDrunkLevel(dwLevel);
	}
}

void ScrSetPlayerDrunkVisuals(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetPlayerDrunkVisuals");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	uint32_t dwVisuals;
	bsData.Read(dwVisuals);

	ScriptCommand(&set_player_drunk_visuals, 0, dwVisuals);
}

void ScrSetPlayerDrunkHandling(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrSetPlayerDrunkHandling");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	uint32_t dwHandling;
	bsData.Read(dwHandling);

	ScriptCommand(&set_player_drunk_handling, 0, dwHandling);
}

void ScrDeathMessage(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrDeathMessage");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	PLAYERID playerId, killerId;
	uint8_t reason;

	bsData.Read(killerId);
	bsData.Read(playerId);
	bsData.Read(reason);

	std::string killername, playername;
	unsigned int killercolor, playercolor;
	killername.clear();
	playername.clear();

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (pPlayerPool) {
		if (pPlayerPool->GetLocalPlayerID() == playerId) {
			playername = pPlayerPool->GetLocalPlayerName();
			playercolor = pPlayerPool->GetLocalPlayer()->GetPlayerColorAsRGBA();
		}
		else {
			if (pPlayerPool->GetSlotState(playerId)) {
				playername = pPlayerPool->GetPlayerName(playerId);
				playercolor = pPlayerPool->GetAt(playerId)->GetPlayerColor();
			}
		}

		if (pPlayerPool->GetLocalPlayerID() == killerId) {
			killername = pPlayerPool->GetLocalPlayerName();
			killercolor = pPlayerPool->GetLocalPlayer()->GetPlayerColorAsRGBA();
		}
		else {
			if (pPlayerPool->GetSlotState(killerId)) {
				killername = pPlayerPool->GetPlayerName(killerId);
				killercolor = pPlayerPool->GetAt(killerId)->GetPlayerColor();
			}
		}
	}

	pUI->deathwindow()->record(playername.c_str(), UI::fixcolor(playercolor), killername.c_str(), killercolor, reason);
}

void ScrDisableVehicleCollisions(RPCParameters* rpcParams)
{
	spdlog::info("ScriptRPC: ScrDisableVehicleCollisions");
	auto Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	bool bDisable;
	bsData.Read(bDisable);

	pNetGame->m_netSet->disableVehicleCollisions = bDisable;
}

void RegisterScriptRPCs(RakClientInterface* pRakClient)
{
	spdlog::info("Registering script RPC's..");

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetMapIcon, ScrSetPlayerMapIcon);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrDisableMapIcon, ScrRemovePlayerMapIcon);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetWeaponAmmo, ScrSetPlayerAmmo);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetGravity, ScrSetGravity);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleHealth, ScrSetVehicleHealth);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrAttachTrailerToVehicle, ScrAttachTrailerToVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrDetachTrailerFromVehicle, ScrDetachTrailerFromVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrCreateObject, ScrCreateObject);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetObjectPos, ScrSetObjectPos);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetObjectRotation, ScrSetObjectRotation);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrDestroyObject, ScrDestroyObject);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrCreateExplosion, ScrCreateExplosion);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrShowNameTag, ScrShowNameTag);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrMoveObject, ScrMoveObject);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrStopObject, ScrStopObject);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrNumberPlate, ScrSetVehicleNumberPlate);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerSpectating, ScrTogglePlayerSpectating);
	// RPC_null - unused
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectatePlayer, ScrSpectatePlayer);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectateVehicle, ScrSpectateVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrRemoveComponent, ScrRemoveVehicleComponent);
	// RPC_ScrForceClassSelection - useless
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrAttachObjectToPlayer, ScrAttachObjectToPlayer);
	// RPC_ScrInitMenu
	// RPC_ScrShowMenu
	// RPC_ScrHideMenu
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerWantedLevel, ScrSetPlayerWantedLevel);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrShowTextDraw, ScrShowTextDraw);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrHideTextDraw, ScrHideTextDraw);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrTextDrawSetString, ScrTextDrawSetString);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrAddGangZone, ScrAddGangZone);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrRemoveGangZone, ScrGangZoneDestroy);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrFlashGangZone, ScrGangZoneFlash);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrStopFlashGangZone, ScrGangZoneStopFlash);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrApplyPlayerAnimation, ScrApplyPlayerAnimation);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrClearPlayerAnimations, ScrClearPlayerAnimations);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetSpecialAction, ScrSetPlayerSpecialAction);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrEnableStuntBonus, ScrEnableStuntBonus);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetFightingStyle, ScrSetPlayerFightingStyle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerVelocity, ScrSetPlayerVelocity);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleVelocity, ScrSetVehicleVelocity);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrToggleWidescreen, ScrToggleWidescreen);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleTireStatus, ScrSetVehicleTireDamageStatus);
	// RPC_150 ???
	// RPC_92 ???
	// RPC_ScrPlayCrimeReport
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetSpawnInfo, ScrSetSpawnInfo);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerTeam, ScrSetPlayerTeam);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerSkin, ScrSetPlayerSkin);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerName, ScrSetPlayerName);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPos, ScrSetPlayerPos);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPosFindZ, ScrSetPlayerPosFindZ);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerHealth, ScrSetPlayerHealth);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrPutPlayerInVehicle, ScrPutPlayerInVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrRemovePlayerFromVehicle, ScrRemovePlayerFromVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerColor, ScrSetPlayerColor);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrDisplayGameText, ScrShowGameText);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetInterior, ScrSetPlayerInterior);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraPos, ScrSetCameraPos);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraLookAt, ScrSetCameraLookAt);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehiclePos, ScrSetVehiclePos);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleZAngle, ScrSetVehicleZAngle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrVehicleParams, ScrSetVehicleParams);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraBehindPlayer, ScrSetPlayerCameraBehindPlayer);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerControllable, ScrTogglePlayerControllable);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrPlaySound, ScrPlayerPlaySound);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetWorldBounds, ScrSetWorldBounds);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrHaveSomeMoney, ScrGivePlayerMoney);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerFacingAngle, ScrSetPlayerFacingAngle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrResetMoney, ScrResetPlayerMoney);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrResetPlayerWeapons, ScrResetPlayerWeapons);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrGivePlayerWeapon, ScrGivePlayerWeapon);
	// RPC_64 - unused
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrLinkVehicle, ScrLinkVehicleToInterior);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerArmour, ScrSetPlayerArmour);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrDeathMessage, ScrDeathMessage);
	// RPC_ScrSetShopName

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerDrunkLevel, ScrSetPlayerDrunkLevel);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerDrunkVisuals, ScrSetPlayerDrunkVisuals);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerDrunkHandling, ScrSetPlayerDrunkHandling);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetArmedWeapon, ScrSetArmedWeapon);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerAttachedObject, ScrSetPlayerAttachedObject);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrPlayAudioStream, ScrPlayAudioStream);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrStopAudioStream, ScrStopAudioStream);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrRemoveBuilding, ScrRemoveBuilding);
	// RPC_ScrAttachCameraToObject
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrInterpolateCamera, ScrInterpolateCamera);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ClickTextDraw, ScrSelectTextDraw);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetObjectMaterial, ScrSetObjectMaterial);
	// RPC_ScrObjectNoCameraCol
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrApplyActorAnimation, ScrApplyActorAnimation);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrClearActorAnimations, ScrClearActorAnimation);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetActorFacingAngle, ScrSetActorFacingAngle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetActorPos, ScrSetActorPos);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetActorHealth, ScrSetActorHealth);
	// RPC_SetPlayerVirtualWorld - 03DL only

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrDisableVehicleCollisions, ScrDisableVehicleCollisions);
}

void UnregisterScriptRPCs(RakClientInterface* pRakClient)
{
	spdlog::info("Unregistering script RPC's..");

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetMapIcon);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrDisableMapIcon);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetWeaponAmmo);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetGravity);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetVehicleHealth);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrAttachTrailerToVehicle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrDetachTrailerFromVehicle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrCreateObject);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetObjectPos);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetObjectRotation);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrDestroyObject);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrCreateExplosion);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrShowNameTag);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrMoveObject);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrStopObject);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrNumberPlate);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerSpectating);
	// RPC_null - unused
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectatePlayer);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectateVehicle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrRemoveComponent);
	// RPC_ScrForceClassSelection - useless
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrAttachObjectToPlayer);
	// RPC_ScrInitMenu
	// RPC_ScrShowMenu
	// RPC_ScrHideMenu
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerWantedLevel);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrShowTextDraw);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrHideTextDraw);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrTextDrawSetString);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrAddGangZone);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrRemoveGangZone);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrFlashGangZone);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrStopFlashGangZone);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrApplyPlayerAnimation);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrClearPlayerAnimations);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetSpecialAction);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrEnableStuntBonus);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetFightingStyle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerVelocity);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetVehicleVelocity);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrToggleWidescreen);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetVehicleTireStatus);
	// RPC_150 ???
	// RPC_92 ???
	// RPC_ScrPlayCrimeReport
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetSpawnInfo);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerTeam);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerSkin);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerName);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPos);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPosFindZ);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerHealth);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrPutPlayerInVehicle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrRemovePlayerFromVehicle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerColor);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrDisplayGameText);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetInterior);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetCameraPos);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetCameraLookAt);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetVehiclePos);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetVehicleZAngle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrVehicleParams);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetCameraBehindPlayer);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerControllable);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrPlaySound);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetWorldBounds);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrHaveSomeMoney);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerFacingAngle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrResetMoney);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrResetPlayerWeapons);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrGivePlayerWeapon);
	// RPC_64 - unused
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrLinkVehicle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerArmour);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrDeathMessage);
	// RPC_ScrSetShopName

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerDrunkLevel);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerDrunkVisuals);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerDrunkHandling);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetArmedWeapon);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerAttachedObject);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrPlayAudioStream);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrStopAudioStream);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrRemoveBuilding);
	// RPC_ScrAttachCameraToObject
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrInterpolateCamera);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ClickTextDraw);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetObjectMaterial);
	// RPC_ScrObjectNoCameraCol
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrApplyActorAnimation);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrClearActorAnimations);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetActorFacingAngle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetActorPos);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetActorHealth);
	// RPC_SetPlayerVirtualWorld - 03DL only

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrDisableVehicleCollisions);
}
