#include <GLES2/gl2.h>
#include "SA.h"
#include "Entity/Placeable.h"
#include "Core/Matrix.h"
#include "Core/MatrixLinkList.h"

namespace sa {
RwGlobals* RwEngineInstance;
RsGlobalType* RsGlobal;
CScene* Scene;

void Initialize()
{
	spdlog::info("Initializing SA...");

	RwEngineInstance = *g_saSym->GetSymbol<RwGlobals**>("RwEngineInstance");
	RsGlobal = g_saSym->GetSymbol<RsGlobalType*>("RsGlobal");
	Scene = g_saSym->GetSymbol<CScene*>("Scene");

	CMatrixLinkList::Initialize();
	CPlaceable::Initialize();
}
}
