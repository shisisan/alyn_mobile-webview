#ifndef SAMP_SYMUTILS_H
#define SAMP_SYMUTILS_H

class SymUtils {

public:
	SymUtils() = default;
	~SymUtils() = default;
private:
	std::unordered_map<const char*, uintptr_t> cached_symbols;
public:
	GHandle handle = nullptr;
	uintptr_t libAddr = 0;
	char libName[250];
public:
	inline void Open(const char* libName)
	{
		if (!handle) handle = GlossOpen(libName);
		sprintf(this->libName, "%s", libName);
		if (!handle) {
			spdlog::error("Open: {} Handle: 0x{:X} - Failed", libName, reinterpret_cast<uintptr_t>(handle));
		}
		else {
			spdlog::info("Open: {} Handle: 0x{:X}", libName, reinterpret_cast<uintptr_t>(handle));
		}
	}
	inline uintptr_t GetLibAddr()
	{
		if (!libAddr) libAddr = GlossGetLibBias(libName);
		return libAddr;
	}
	template<typename Ret = void*>
	inline Ret Abs(uintptr_t addr)
	{
		return (Ret) (GetLibAddr() + addr);
	}
	inline uintptr_t ToLibAddr(uintptr_t addr)
	{
		return addr - GetLibAddr();
	}
	template<typename T=void*>
	inline T GetSymbol(const char* name)
	{
		if (cached_symbols.contains(name)) return (T) cached_symbols[name];
		if (!handle) {
			spdlog::error("GetSymbol: Invalid handle! Handle: 0x{:X} Name: {}", reinterpret_cast<uintptr_t>(handle), name);
			return 0;
		}
		uintptr_t symbolAddr = GlossSymbol(handle, name);
		if (!symbolAddr) {
			spdlog::error("GetSymbol: Invalid address! Address: 0x{:X} Name: {}", symbolAddr, name);
			return 0;
		}
		cached_symbols.emplace(name, symbolAddr);
		return (T) symbolAddr;
	}

	template<typename Ret = void*>
	inline Ret GetVmtFunction(const char* symbolName, int index, bool isPtr = false)
	{
		if (index < 1) {
			index = 1;
		}
		auto address = GetSymbol<uintptr_t>(symbolName);
		if (!address) {
			spdlog::error("getVmtFunction: Invalid address! Address: 0x{:X} Index: {} IsPtr: {}", address, index, isPtr);
			return Ret();
		}
		int offset = (index + 1) * (int) sizeof(void*);
		return isPtr ? (Ret) (address + offset) : *(Ret*) (address + offset);
	}

	template<typename Ret = void*>
	inline Ret GetVmtFunction(uintptr_t address, int index, bool isPtr = false)
	{
		if (index < 0) {
			index = 0;
		}
		if (!address) {
			spdlog::error("getVmtFunction: Invalid address! Address: 0x{:X} Index: {} IsPtr: {}", address, index, isPtr);
			return Ret();
		}
		int offset = index * (int) sizeof(void*);
		return isPtr ? (Ret) (address + offset) : *(Ret*) (address + offset);
	}
	template<typename Ret = void, typename... Args>
	inline Ret CallFunction(const char* symName, Args... args)
	{
		auto address = GetSymbol<uintptr_t>(symName);
		if (!address) {
			spdlog::error("callFunction: Invalid address! Address: 0x{:X}", address);
			return Ret();
		}
		return reinterpret_cast<Ret(*)(Args...)>(address)(args...);
	}
	template<typename Ret = void, typename... Args>
	inline Ret CallFunction(uintptr_t address, Args... args)
	{
		if (!address) {
			spdlog::error("callFunction: Invalid address! Address: 0x{:X}", address);
			return Ret();
		}
		return reinterpret_cast<Ret(*)(Args...)>(address)(args...);
	}

	template<typename Ret = void, typename... Args>
	inline Ret CallVmtFunction(uintptr_t address, int index, Args... args)
	{
		if (!address) {
			spdlog::error("CallVmtFunction: Invalid address! Address: 0x{:X} Index: {}", address, index);
			return Ret();
		}
		auto vmtAddr = GetVmtFunction<uintptr_t>(address, index);
		if (!vmtAddr) {
			spdlog::error("CallVmtFunction: Invalid vmtAddr! Address: 0x{:X} Index: {}", address, index);
			return Ret();
		}
		return reinterpret_cast<Ret(*)(Args...)>(vmtAddr)(args...);
	}
};

#endif //SAMP_SYMUTILS_H
