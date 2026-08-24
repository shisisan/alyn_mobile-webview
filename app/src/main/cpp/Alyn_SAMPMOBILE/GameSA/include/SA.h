#pragma once

#include "RenderWare/skeleton.h"

#include "Core/PtrNode.h"
#include "Core/Rect.h"

#include "Entity/Ped/PlayerPedData.h"
#include "Entity/Ped/PedIK.h"
#include "Entity/Vehicle/AutoMobile.h"
#include "Entity/Vehicle/Train.h"
#include "Entity/Vehicle/Plane.h"
#include "Entity/Vehicle/Bike.h"
#include "Entity/Object/Object.h"

#include "Model/BaseModelInfo.h"
#include "Model/AtomicModelInfo.h"
#include "Model/ClumpModelInfo.h"
#include "Model/PedModelInfo.h"

#include "Scripts/RunningScript.h"

#include "Tasks/Task.h"
#include "Tasks/TaskSimpleRunNamedAnim.h"

#include "Animation/AnimBlock.h"

#include "Scene.h"
#include "Sprite.h"
#include "GxtChar.h"
#include "TxdStore.h"
#include "TextureDatabaseRuntime.h"
#include "Cheat.h"
#include "Camera.h"
#include "Pool.h"
#include "EntryExit.h"
#include "PlayerInfo.h"
#include "Store.h"
#include "FileObjectInstance.h"
#include "FontDetails.h"

namespace sa {
extern RwGlobals* RwEngineInstance;
extern RsGlobalType* RsGlobal;
extern CScene* Scene;

void Initialize();
}
