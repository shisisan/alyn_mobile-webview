#include "Client.h"
#include "BackTrace.h"
#include "Settings.h"
#include "ClientCmds.h"
#include "Game/Patches.h"
#include "Game/Hooks.h"
#include "Java/Java.h"
#include "Java/MultiTouch.h"
#include "UI/UI.h"
#include "Game/Game.h"
#include "Game/AudioStream.h"
#include "Net/NetGame.h"
#include "Voice/Plugin.h"
#include "Crashlytics.h"
#include "cef/SAMPMobileCef.h"

JavaVM* Client::java_vm = nullptr;
std::string Client::game_dir;
bool Client::offline_mode = false;
std::string Client::samp_version;

Java* g_java = nullptr;
UI* pUI = nullptr;
Game* pGame = nullptr;
NetGame* pNetGame = nullptr;
PlayerTags* pPlayerTags = nullptr;
AudioStream* pAudioStream = nullptr;
SnapShotHelper* pSnapShotHelper = nullptr;
MaterialTextGenerator* pMaterialTextGenerator = nullptr;

SymUtils* g_saSym = new SymUtils();
JavaVM* g_vm = nullptr;

void init_logger(const std::string& gameDir)
{
	try {
		auto now = std::chrono::system_clock::now();
		auto in_time_t = std::chrono::system_clock::to_time_t(now);
		std::stringstream ss;
		ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d_%H-%M-%S");
		std::string dateTime = ss.str();

		// Define log file path client+date+time.log
		std::string path = gameDir + "logcat/client_" + dateTime + ".log";

		// Create Android (logcat) sink
		auto android_sink = std::make_shared<spdlog::sinks::android_sink_mt>("Alyn_SAMPMOBILE");

		// Create file sink
		auto max_size = 1048576 * 10; // Max file size 10MB
		auto max_files = 3; // Rotate files
		auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(path, max_size, max_files);

		// Combine both sinks
		std::vector<spdlog::sink_ptr> sinks{android_sink, file_sink};
		auto logger = std::make_shared<spdlog::logger>("Alyn_SAMPMOBILE", sinks.begin(), sinks.end());

		// Set log level and pattern
		logger->set_level(spdlog::level::info);
		logger->set_pattern("[%d-%m-%Y %H:%M:%S] [%^%l%$] %v");

		// Set as default logger
		spdlog::set_default_logger(logger);
	}
	catch (const spdlog::spdlog_ex& ex) {
		__android_log_print(ANDROID_LOG_ERROR, "Alyn_SAMPMOBILE", "Log initialization failed: %s", ex.what());
	}
}

void* startGame(void*)
{
	while (*g_saSym->GetSymbol<uint32_t*>("gGameState") != 7) {
		usleep(500);
	}

	pGame->StartGame();

	pthread_exit(nullptr);
}

void Client::initialize(const std::string& gameDir, bool offlineMode)
{
	// signal handler
	struct sigaction sig_action{};
	sig_action.sa_sigaction = [](int signal, siginfo_t* info, void* ctx) {
		dump_register(signal, info, ctx);
		dump_stack(2);
		exit(signal);
	};
	sigemptyset(&sig_action.sa_mask);
	sig_action.sa_flags = SA_SIGINFO;
	sigaction(SIGSEGV, &sig_action, nullptr);

	init_logger(gameDir);

	spdlog::info("Initializing Alyn_SAMPMOBILE client...");

	java_vm = g_vm;
	game_dir = gameDir;
	offline_mode = offlineMode;
	samp_version = "0.3.7";

	spdlog::info("Java VM: 0x{:X}", reinterpret_cast<uintptr_t>(java_vm));
	spdlog::info("Game version: {}", VER_x32 ? "GTA: SA v2.10 (armeabi-v7a)" : "GTA: SA v2.10 (arm64-v8a)");
	spdlog::info("Game directory: {}", game_dir);
	spdlog::info("Offline mode: {}", offline_mode);

	Settings::initialize();

	cef::setGamePath(game_dir.c_str());

	if (firebase::crashlytics::Initialize()) {
		spdlog::info("Crashlytics initialized");
		firebase::crashlytics::SetUserId(Settings::nick());
	}

	if (Settings::sampversion() == 0) {
		samp_version = "0.3.7";
	}
	else {
		samp_version = "0.3.7-R4";
	}

	spdlog::info("SA-MP version: {}", samp_version);

	spdlog::info("Loading libGTASA.so...");
	g_saSym->Open("libGTASA.so");

	if (!g_saSym->GetLibAddr()) {
		spdlog::error("Failed to load libGTASA.so!");
		return;
	}

	spdlog::info("libGTASA.so loaded at 0x{:X}", g_saSym->GetLibAddr());

	char str[100];
	sprintf(str, "0x%llx", g_saSym->GetLibAddr());
	firebase::crashlytics::SetCustomKey("libGTASA.so", str);

	sa::Initialize();
	LoadBassLibrary();

	Hooks::install();
	Patches::apply();

    cef::applyJavaVM(java_vm);

	MultiTouch::initialize();

	pGame = new Game();

	if (Settings::voice()) {
		Plugin::OnPluginLoad();
		Plugin::OnSampLoad();
	}

	pthread_t thread;
	pthread_create(&thread, nullptr, startGame, nullptr);

	// FIXME: causes crash on some devices - why?
	/*std::thread([]() {
		while (*g_saSym->GetSymbol<uint32_t*>("gGameState") != 7) {
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}

		pGame->StartGame();
	}).detach();*/
}

void Client::process()
{
	static bool gameInited = false;
	static bool netgameInited = false;

	if (!gameInited) {
		spdlog::info("Initializing game...");

		pPlayerTags = new PlayerTags();
		pAudioStream = new AudioStream();
		pSnapShotHelper = new SnapShotHelper();
		pMaterialTextGenerator = new MaterialTextGenerator();

		Patches::preApply();

		pGame->Initialize();
		pAudioStream->Initialize();

		pUI->splashscreen()->setVisible(false);
		pUI->chat()->setVisible(true);
		pUI->buttonpanel()->setVisible(true);

		g_java->showLoadingScreen(false);

		ClientCmds::initialize();

		if (Settings::voice()) {
			LogVoice("[dbg:samp:load] : module loading...");
			for (const auto& loadCallback : Samp::loadCallbacks) {
				if (loadCallback != nullptr) {
					loadCallback();
				}
			}
			Samp::loadStatus = true;
			LogVoice("[dbg:samp:load] : module loaded");
		}

		if (offline_mode) {
			pUI->buttonpanel()->setVisible(false);
			pUI->chat()->addInfoMessage("-> GTA: SA " + (VER_x32 ? std::string("v2.10 (armeabi-v7a)") : std::string("v2.10 (arm64-v8a)")) + " started");
			pUI->chat()->addInfoMessage("-> Type /j to start jetpack");
			pUI->chat()->addInfoMessage("-> Click on map to teleport anywhere");

			pGame->ToggleCJWalk(false); // so we can restore nop
			pGame->ToggleCJWalk(true);
			pGame->SetWorldTime(12, 0);
			pGame->AddToLocalMoney(100000000);
			pGame->GetCamera()->Restore();
			pGame->GetCamera()->SetBehindPlayer();
			pGame->FindPlayerPed()->SetModelIndex(271);
			pGame->FindPlayerPed()->SetHealth(1000.0f);
			pGame->FindPlayerPed()->SetArmour(1000.0f);

			RwMatrix mat = pGame->FindPlayerPed()->m_ped->GetMatrix().ToRwMatrix();
			mat.pos = {2090.6255, 1164.5679, 11.6484};
			pGame->FindPlayerPed()->m_ped->SetMatrix((sa::CMatrix&) mat);
			pGame->FindPlayerPed()->SetTargetRotation(72.8293);
			pGame->NewVehicle(411, mat.pos.x, mat.pos.y + 5.0f, mat.pos.z, 0.0f, false);

			if (!g_saSym->GetSymbol("_ZZ20Menu_SwitchOffToGamevE12bInitWidgets")) {
				spdlog::info("Widgets not inited");
				Memory::callFunction("_Z20Menu_SwitchOffToGamev");
			}
		}

		gameInited = true;
	}

	if (!netgameInited && !offline_mode) {
		spdlog::info("Initializing netgame...");
		pNetGame = new NetGame(Settings::ip(), Settings::port(), Settings::nick(), Settings::pass());
		netgameInited = true;
	}

	if (pNetGame) {
		pNetGame->Process();

		CTextDrawPool* pTextDrawPool = pNetGame->GetTextDrawPool();
		if (pTextDrawPool) {
			pTextDrawPool->Draw();
		}

		if (pAudioStream) {
			pAudioStream->Process();
		}

		if (g_java && !Game::IsGamePaused()) {
			g_java->showWantedLevel(true);
		}
	}
}

void Client::initializeUI()
{
	if (Settings::voice()) {
		for (const auto& deviceInitCallback : Render::deviceInitCallbacks) {
			if (deviceInitCallback != nullptr) {
				deviceInitCallback();
			}
		}
	}

	std::string fonts_path = std::string(Client::gameDir()) + "fonts/";
	pUI = new UI(ImVec2(sa::RsGlobal->screenWidth, sa::RsGlobal->screenHeight), fonts_path);
	pUI->initialize();
	pUI->performLayout();
}

jint JNI_OnLoad(JavaVM* vm, [[maybe_unused]] void* reserved)
{
	__android_log_print(ANDROID_LOG_INFO, "Alyn_SAMPMOBILE", "Alyn_SAMPMOBILE library loaded! Build time: " __DATE__ " " __TIME__);

	JNIEnv* env;
	if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
		return JNI_ERR;
	}

	g_vm = vm;

	return env->GetVersion();
}
