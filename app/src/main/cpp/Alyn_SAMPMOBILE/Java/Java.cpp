//
// Created by ALYN on 2024/8/25.
//
#include "Java.h"
#include "../../Client.h"
#include "../../UI/UI.h"
#include "../../Game/Game.h"
#include "../../Net/NetGame.h"

extern UI* pUI;
extern NetGame* pNetGame;

Java::Java(JNIEnv* env, jobject sampObj, jobject uiObj)
{
	spdlog::info("Initializing java..");

	if (!sampObj || !uiObj) {
		spdlog::error("Invalid java objects");
		return;
	}

	m_sampActivity = env->NewGlobalRef(sampObj);
	m_uiActivity = env->NewGlobalRef(uiObj);

	jclass sampClass = env->GetObjectClass(sampObj);
	if (!sampClass) {
		spdlog::error("SAMP class not found");
		return;
	}

	jclass uiClass = env->GetObjectClass(uiObj);
	if (!uiClass) {
		spdlog::error("UI class not found");
		return;
	}

	m_setPauseState = env->GetMethodID(uiClass, "setPauseState", "(Z)V");

	m_showLoadingScreen = env->GetMethodID(uiClass, "showLoadingScreen", "(Z)V");
	m_showKeyboard = env->GetMethodID(uiClass, "showKeyboard", "(Z)V");
	m_showDialog = env->GetMethodID(uiClass, "showDialog", "(IILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	m_showEditObject = env->GetMethodID(uiClass, "showEditObject", "(Z)V");

	m_showWantedLevel = env->GetMethodID(uiClass, "showWantedLevel", "(Z)V");
	m_setWantedLevel = env->GetMethodID(uiClass, "setWantedLevel", "(I)V");

	m_exitGame = env->GetMethodID(uiClass, "exitGame", "()V");

	env->DeleteLocalRef(sampClass);
	env->DeleteLocalRef(uiClass);
}

JNIEnv* Java::getEnv()
{
	if (!Client::javaVM()) {
		spdlog::error("No java vm");
		return nullptr;
	}

	JNIEnv* env;
	if (Client::javaVM()->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
		spdlog::error("Failed to get env");
		return nullptr;
	}

	return env;
}

jstring createJString(JNIEnv* env, const char* text)
{
	jclass strClass = env->FindClass("java/lang/String");
	jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
	jstring encoding = env->NewStringUTF("UTF-8");

	jbyteArray bytes = env->NewByteArray(strlen(text));
	env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*) text);
	return (jstring) env->NewObject(strClass, ctorID, bytes, encoding);
}

void Java::setPauseState(bool pause)
{
	JNIEnv* env = getEnv();
	if (!env) {
		spdlog::error("No env");
		return;
	}
	env->CallVoidMethod(m_uiActivity, m_setPauseState, pause);
}

void Java::showLoadingScreen(bool show)
{
	JNIEnv* env = getEnv();
	if (!env) {
		spdlog::error("No env");
		return;
	}
	env->CallVoidMethod(m_uiActivity, m_showLoadingScreen, show);
}

void Java::showKeyboard(bool show)
{
	JNIEnv* env = getEnv();
	if (!env) {
		spdlog::error("No env");
		return;
	}
	env->CallVoidMethod(m_uiActivity, m_showKeyboard, show);
}

void Java::showDialog(int dialog_id, int dialog_style, char* title, char* message, char* button1, char* button2)
{
	JNIEnv* env = getEnv();

	if (!env) {
		spdlog::error("No env");
		return;
	}

	jstring jtitle = createJString(env, title);
	jstring jmessage = createJString(env, message);
	jstring jbutton1 = createJString(env, button1);
	jstring jbutton2 = createJString(env, button2);

	env->CallVoidMethod(m_uiActivity, m_showDialog, dialog_id, dialog_style, jtitle, jmessage, jbutton1, jbutton2);
}

void Java::showEditObject(bool show)
{
	JNIEnv* env = getEnv();
	if (!env) {
		spdlog::error("No env");
		return;
	}

	env->CallVoidMethod(m_uiActivity, m_showEditObject, show);
}

void Java::showWantedLevel(bool show)
{
	JNIEnv* env = getEnv();
	if (!env) {
		spdlog::error("No env");
		return;
	}

	env->CallVoidMethod(m_uiActivity, m_showWantedLevel, show);
}

void Java::setWantedLevel(int level)
{
	JNIEnv* env = getEnv();
	if (!env) {
		spdlog::error("No env");
		return;
	}

	env->CallVoidMethod(m_uiActivity, m_setWantedLevel, level);
}

void Java::exitGame()
{
	JNIEnv* env = getEnv();
	if (!env) {
		spdlog::error("No env");
		return;
	}

	env->CallVoidMethod(m_uiActivity, m_exitGame);
}


const std::string EXPECTED_SIGNATURE(OBFUSCATE("89cb277c5cd1a90145acb3c2a89cea995bfbd3b3e5ebe2b180d2248efc4aa041"));

extern "C" JNIEXPORT jboolean JNICALL Java_ro_alynsampmobile_launcher_utils_SignatureChecker_nativeCheckSignature(JNIEnv* env, jclass clazz, jstring computedSignature)
{
	const char* computedSignatureCStr = env->GetStringUTFChars(computedSignature, nullptr);
	std::string computedSignatureStr(computedSignatureCStr);
	env->ReleaseStringUTFChars(computedSignature, computedSignatureCStr);


	return JNI_TRUE;
}

extern "C" JNIEXPORT void JNICALL Java_ro_alynsampmobile_game_SAMP_initializeSAMP(JNIEnv* env, jobject sampObj, jobject uiObj, jstring gameDir, jboolean isOffline)
{
	const char* gameDirCStr = env->GetStringUTFChars(gameDir, nullptr);
	std::string gameDirStr(gameDirCStr);
	env->ReleaseStringUTFChars(gameDir, gameDirCStr);



	Client::initialize(gameDirStr, isOffline);
	g_java = new Java(env, sampObj, uiObj);
}

extern "C" JNIEXPORT void JNICALL Java_ro_alynsampmobile_game_ui_UI_keyboardSend(JNIEnv* env, jobject obj, jbyteArray str)
{
	jboolean isCopy = true;
	jbyte* msg = env->GetByteArrayElements(str, &isCopy);
	jsize length = env->GetArrayLength(str);

	std::string str_string((char*) msg, length);

	if (pUI) {
		pUI->chat()->keyboardEvent(str_string);
	}

	env->ReleaseByteArrayElements(str, msg, JNI_ABORT);
}

extern "C" JNIEXPORT void JNICALL Java_ro_alynsampmobile_game_ui_UI_sendDialogResponse(JNIEnv* env, jobject obj, jint dialog_id, jint button_id, jint list_item, jbyteArray str)
{
	spdlog::info("Java_com_samp_game_ui_UI_sendDialogResponse");

	jboolean isCopy = true;

	jbyte* msg = env->GetByteArrayElements(str, &isCopy);
	jsize length = env->GetArrayLength(str);

	std::string szStr((char*) msg, length);

	if (pNetGame) {
		pNetGame->SendDialogResponse(dialog_id, button_id, list_item, (char*) szStr.c_str());
	}

	env->ReleaseByteArrayElements(str, msg, JNI_ABORT);
}
