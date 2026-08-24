#pragma once

#include "Core/TDBArray.h"
#include "Enums/TextureDatabaseFormat.h"
#include "../../Memory/include/Memory.h"
#include "RenderWare/rwcore.h"

namespace sa {
class TextureDatabaseRuntime {
public:
	static void Register(TextureDatabaseRuntime* toRegister)
	{
		Memory::callFunction("_ZN22TextureDatabaseRuntime8RegisterEPS_", toRegister);
	}

	static void Unregister(TextureDatabaseRuntime* toUnregister)
	{
		Memory::callFunction("_ZN22TextureDatabaseRuntime10UnregisterEPS_", toUnregister);
	}

	static RwTexture* GetTexture(const char* name)
	{
		return Memory::callFunction<RwTexture*>("_ZN22TextureDatabaseRuntime10GetTextureEPKc", name);
	}

	static TextureDatabaseRuntime* Load(const char* withName, bool fullyLoad, TextureDatabaseFormat forcedFormat)
	{
		return Memory::callFunction<TextureDatabaseRuntime*>("_ZN22TextureDatabaseRuntime4LoadEPKcb21TextureDatabaseFormat", withName, fullyLoad, forcedFormat);
	}

	static TextureDatabaseRuntime* GetDatabase(const char* withName)
	{
		return Memory::callFunction<TextureDatabaseRuntime*>("_ZN22TextureDatabaseRuntime11GetDatabaseEPKc", withName);
	}

	static TDBArray<TextureDatabaseRuntime*>& registered() { return *g_saSym->GetSymbol<TDBArray<TextureDatabaseRuntime*>*>("_ZN22TextureDatabaseRuntime10registeredE"); }
};
}
