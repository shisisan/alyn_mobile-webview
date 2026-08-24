#pragma once

#include "../../Memory/include/Memory.h"

namespace sa {
class CTxdStore {
public:
	static void PushCurrentTxd()
	{
		Memory::callFunction("_ZN9CTxdStore14PushCurrentTxdEv");
	}

	static void PopCurrentTxd()
	{
		Memory::callFunction("_ZN9CTxdStore13PopCurrentTxdEv");
	}

	static int32_t FindTxdSlot(char const* pName)
	{
		return Memory::callFunction<int32_t>("_ZN9CTxdStore11FindTxdSlotEPKc", pName);
	}

	static void SetCurrentTxd(int index, const char* dbOverride = 0)
	{
		Memory::callFunction("_ZN9CTxdStore13SetCurrentTxdEiPKc", index, dbOverride);
	}

	static int32_t AddTxdSlot(const char* pName, const char* dbName, bool keepCPU = false)
	{
		return Memory::callFunction<int32_t>("_ZN9CTxdStore10AddTxdSlotEPKcS1_b", pName, dbName, keepCPU);
	}

	static void AddRef(int index)
	{
		Memory::callFunction("_ZN9CTxdStore6AddRefEi", index);
	}

	static RwTexDictionary* GetTxdParent(RwTexDictionary* pTxd)
	{
		return Memory::callFunction<RwTexDictionary*>("_ZN9CTxdStore12GetTxdParentEP15RwTexDictionary", pTxd);
	}
};
}
