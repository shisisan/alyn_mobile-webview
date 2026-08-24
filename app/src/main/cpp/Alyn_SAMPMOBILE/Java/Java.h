//
// Created by ALYN on 2024/8/25.
//

#ifndef SA_MP_MOBILE_JAVA_H
#define SA_MP_MOBILE_JAVA_H

#include <jni.h>

class Java {
public:
	Java(JNIEnv* env, jobject sampObj, jobject uiObj);
	JNIEnv* getEnv();

	void setPauseState(bool pause);

	void showLoadingScreen(bool show);
	void showKeyboard(bool show);
	void showDialog(int dialog_id, int dialog_style, char* title, char* message, char* button1, char* button2);
	void showEditObject(bool show);

	void showWantedLevel(bool show);
	void setWantedLevel(int level);

	void exitGame();

	jobject sampActivity() { return m_sampActivity; }
	jobject uiActivity() { return m_uiActivity; }

private:
	jobject m_sampActivity;
	jobject m_uiActivity;

	jmethodID m_setPauseState;

	jmethodID m_showLoadingScreen;
	jmethodID m_showKeyboard;
	jmethodID m_showDialog;
	jmethodID m_showEditObject;

	jmethodID m_showWantedLevel;
	jmethodID m_setWantedLevel;

	jmethodID m_exitGame;
};

extern Java* g_java;

#endif //SA_MP_MOBILE_JAVA_H
