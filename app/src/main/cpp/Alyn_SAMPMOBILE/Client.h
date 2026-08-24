#pragma once

#include <jni.h>
#include <thread>
#include <string>
#include <array>
#include <vector>
#include <mutex>
#include <future>
#include <sstream>
#include <fstream>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/android_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <encoding.h>
#include <Memory.h>
#include <Gloss.h>
#include <SymUtils.h>
#include <SA.h>
#include <RenderWare/rw.h>
#include <Encoding.h>
#include <obfuscator.h>

#include "Addr.h"

class Client {
public:
	Client() = default;

	static void initialize(const std::string& gameDir, bool offlineMode);
	static void process();
	static void initializeUI();

	static JavaVM* javaVM() { return java_vm; }
	static char* gameDir() { return (char*) game_dir.c_str(); }
	static bool offlineMode() { return offline_mode; }
	static std::string sampVersion() { return samp_version; }

private:
	static JavaVM* java_vm;
	static std::string game_dir;
	static bool offline_mode;
	static std::string samp_version;
};

extern SymUtils* g_saSym;
