//
// Created by shisisan on 21/08/2026.
//

#include "SAMPMobileCef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <time.h>
#include <jni.h>
#include "../Deps/RakNet/RakNet/RakClientInterface.h" // Must be included in CMAKE
#include "./Deps/RakNet/RakNet/BitStream.h" // Must be included in CMAKE

namespace cef
{
    static char g_szGamePath[512] = {0};
    static char g_szLogPath[512] = {0};
    static void* g_pRakClient = nullptr;
    static uint8_t g_nPacketId = 252;
    static bool g_bInitialized = false;

    static JavaVM*  g_pJavaVM = nullptr;
    static jclass    g_ClsCefClientManager = nullptr;

    static jmethodID g_MidOpenBrowser       = nullptr;
    static jmethodID g_MidCloseBrowser      = nullptr;
    static jmethodID g_MidShowBrowser       = nullptr;
    static jmethodID g_MidHideBrowser       = nullptr;
    static jmethodID g_MidSetUrl            = nullptr;
    static jmethodID g_MidSetFocus          = nullptr;
    static jmethodID g_MidDispatchServerEvt = nullptr;

    static SendPacketFn g_pfnSendPacket = nullptr;

    static JNIEnv* getJniEnv(bool* pDidAttach)
    {
        *pDidAttach = false;
        if (!g_pJavaVM) return nullptr;

        JNIEnv* env = nullptr;
        jint res = g_pJavaVM->GetEnv((void**)&env, JNI_VERSION_1_6);
        if (res == JNI_EDETACHED)
        {
            if (g_pJavaVM->AttachCurrentThread(&env, nullptr) != 0)
                return nullptr;
            *pDidAttach = true;
        }
        else if (res != JNI_OK)
        {
            return nullptr;
        }
        return env;
    }

    static void callJavaOpenBrowser(const char* szUrl)
    {
        bool didAttach = false;
        JNIEnv* env = getJniEnv(&didAttach);
        if (!env) { log("callJavaOpenBrowser: JavaVM not ready yet"); return; }

        if (g_ClsCefClientManager && g_MidOpenBrowser) {
            jstring jUrl = env->NewStringUTF(szUrl);
            env->CallStaticVoidMethod(g_ClsCefClientManager, g_MidOpenBrowser, jUrl);
            env->DeleteLocalRef(jUrl);
            if (env->ExceptionCheck()) { env->ExceptionDescribe(); env->ExceptionClear(); }
        } else {
            log("callJavaOpenBrowser: method not cached");
        }

        if (didAttach) g_pJavaVM->DetachCurrentThread();
    }

    static void callJavaCloseBrowser()
    {
        bool didAttach = false;
        JNIEnv* env = getJniEnv(&didAttach);
        if (!env) { log("callJavaCloseBrowser: JavaVM not ready yet"); return; }

        if (g_ClsCefClientManager && g_MidCloseBrowser) {
            env->CallStaticVoidMethod(g_ClsCefClientManager, g_MidCloseBrowser);
            if (env->ExceptionCheck()) { env->ExceptionDescribe(); env->ExceptionClear(); }
        } else {
            log("callJavaCloseBrowser: method not cached");
        }

        if (didAttach) g_pJavaVM->DetachCurrentThread();
    }

    static void callJavaShowBrowser()
    {
        bool didAttach = false;
        JNIEnv* env = getJniEnv(&didAttach);
        if (!env) { log("callJavaShowBrowser: JavaVM not ready yet"); return; }

        if (g_ClsCefClientManager && g_MidShowBrowser) {
            env->CallStaticVoidMethod(g_ClsCefClientManager, g_MidShowBrowser);
            if (env->ExceptionCheck()) { env->ExceptionDescribe(); env->ExceptionClear(); }
        } else {
            log("callJavaShowBrowser: method not cached");
        }

        if (didAttach) g_pJavaVM->DetachCurrentThread();
    }

    static void callJavaHideBrowser()
    {
        bool didAttach = false;
        JNIEnv* env = getJniEnv(&didAttach);
        if (!env) { log("callJavaHideBrowser: JavaVM not ready yet"); return; }

        if (g_ClsCefClientManager && g_MidHideBrowser) {
            env->CallStaticVoidMethod(g_ClsCefClientManager, g_MidHideBrowser);
            if (env->ExceptionCheck()) { env->ExceptionDescribe(); env->ExceptionClear(); }
        } else {
            log("callJavaHideBrowser: method not cached");
        }

        if (didAttach) g_pJavaVM->DetachCurrentThread();
    }

    static void callJavaSetUrl(const char* szUrl)
    {
        bool didAttach = false;
        JNIEnv* env = getJniEnv(&didAttach);
        if (!env) { log("callJavaSetUrl: JavaVM not ready yet"); return; }

        if (g_ClsCefClientManager && g_MidSetUrl) {
            jstring jUrl = env->NewStringUTF(szUrl);
            env->CallStaticVoidMethod(g_ClsCefClientManager, g_MidSetUrl, jUrl);
            env->DeleteLocalRef(jUrl);
            if (env->ExceptionCheck()) { env->ExceptionDescribe(); env->ExceptionClear(); }
        } else {
            log("callJavaSetUrl: method not cached");
        }

        if (didAttach) g_pJavaVM->DetachCurrentThread();
    }

    static void callJavaSetFocus(bool isFocused)
    {
        bool didAttach = false;
        JNIEnv* env = getJniEnv(&didAttach);
        if (!env) { log("callJavaSetFocus: JavaVM not ready yet"); return; }

        if (g_ClsCefClientManager && g_MidSetFocus) {
            env->CallStaticVoidMethod(g_ClsCefClientManager, g_MidSetFocus, (jboolean) isFocused);
            if (env->ExceptionCheck()) { env->ExceptionDescribe(); env->ExceptionClear(); }
        } else {
            log("callJavaSetFocus: method not cached");
        }

        if (didAttach) g_pJavaVM->DetachCurrentThread();
    }

    static void callJavaDispatchServerEvent(const char* szEventName, const char* szEventData)
    {
        bool didAttach = false;
        JNIEnv* env = getJniEnv(&didAttach);
        if (!env) { log("callJavaDispatchServerEvent: JavaVM not ready yet"); return; }

        if (g_ClsCefClientManager && g_MidDispatchServerEvt) {
            jstring jName = env->NewStringUTF(szEventName);
            jstring jData = env->NewStringUTF(szEventData);
            env->CallStaticVoidMethod(g_ClsCefClientManager, g_MidDispatchServerEvt, jName, jData);
            env->DeleteLocalRef(jName);
            env->DeleteLocalRef(jData);
            if (env->ExceptionCheck()) { env->ExceptionDescribe(); env->ExceptionClear(); }
        } else {
            log("callJavaDispatchServerEvent: method not cached");
        }

        if (didAttach) g_pJavaVM->DetachCurrentThread();
    }

    void setSendCallback(SendPacketFn fn)
    {
        g_pfnSendPacket = fn;
        log("cef::setSendCallback: registered (%p)", (void*)fn);
    }

    void applyJavaVM(JavaVM* vm)
    {
        g_pJavaVM = vm;
        log("cef::applyJavaVM: JavaVM registered (%p)", (void*)vm);

        JNIEnv* env = nullptr;
        if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
            log("cef::applyJavaVM: failed to get JNIEnv on registering thread");
            return;
        }

        jclass localCls = env->FindClass("com/samp/cef/CefClientManager");
        if (!localCls) {
            log("cef::applyJavaVM: FindClass(CefClientManager) FAILED");
            env->ExceptionClear();
            return;
        }

        g_ClsCefClientManager = (jclass) env->NewGlobalRef(localCls);
        env->DeleteLocalRef(localCls);

        g_MidOpenBrowser       = env->GetStaticMethodID(g_ClsCefClientManager, "staticOpenBrowser", "(Ljava/lang/String;)V");
        g_MidCloseBrowser      = env->GetStaticMethodID(g_ClsCefClientManager, "staticCloseBrowser", "()V");
        g_MidShowBrowser       = env->GetStaticMethodID(g_ClsCefClientManager, "staticShowBrowser", "()V");
        g_MidHideBrowser       = env->GetStaticMethodID(g_ClsCefClientManager, "staticHideBrowser", "()V");
        g_MidSetUrl            = env->GetStaticMethodID(g_ClsCefClientManager, "staticSetBrowserUrl", "(Ljava/lang/String;)V");
        g_MidSetFocus          = env->GetStaticMethodID(g_ClsCefClientManager, "staticSetBrowserFocus", "(Z)V");
        g_MidDispatchServerEvt = env->GetStaticMethodID(g_ClsCefClientManager, "staticDispatchServerEvent", "(Ljava/lang/String;Ljava/lang/String;)V");

        if (!g_MidOpenBrowser)       log("cef::applyJavaVM: GetStaticMethodID(staticOpenBrowser) FAILED");
        if (!g_MidCloseBrowser)      log("cef::applyJavaVM: GetStaticMethodID(staticCloseBrowser) FAILED");
        if (!g_MidShowBrowser)       log("cef::applyJavaVM: GetStaticMethodID(staticShowBrowser) FAILED");
        if (!g_MidHideBrowser)       log("cef::applyJavaVM: GetStaticMethodID(staticHideBrowser) FAILED");
        if (!g_MidSetUrl)            log("cef::applyJavaVM: GetStaticMethodID(staticSetBrowserUrl) FAILED");
        if (!g_MidSetFocus)          log("cef::applyJavaVM: GetStaticMethodID(staticSetBrowserFocus) FAILED");
        if (!g_MidDispatchServerEvt) log("cef::applyJavaVM: GetStaticMethodID(staticDispatchServerEvent) FAILED");

        log("cef::applyJavaVM: class+methods cached successfully");
    }

    void setGamePath(const char* szPath)
    {
        if (!szPath) return;

        snprintf(g_szGamePath, sizeof(g_szGamePath), "%s", szPath);
        snprintf(g_szLogPath, sizeof(g_szLogPath), "%s/SAMP/cef.log", szPath);

        char szDir[512];
        snprintf(szDir, sizeof(szDir), "%s/SAMP", szPath);
        mkdir(szDir, 0755);

        log("CEF Client initialized. Storage path: %s", szPath);
    }

    void initNetwork(void* pRakClient, uint8_t packetId)
    {
        g_pRakClient = pRakClient;
        g_nPacketId = packetId;
        g_bInitialized = true;

        log("CEF Network initialized. RakClient: %p, Packet ID: %d", pRakClient, packetId);
    }

    bool isInitialized()
    {
        return g_bInitialized;
    }

    void handleServerConnection()
    {
        if (!g_bInitialized) return;

        log("Sending LibraryInit signal to SA:MP Server...");

        uint8_t buffer[6];
        uint16_t packetId16 = (uint16_t) g_nPacketId;
        memcpy(buffer, &packetId16, 2);
        uint32_t rpcId = RPC_LibraryInit;
        memcpy(buffer + 2, &rpcId, 4);

        bool ok = g_pfnSendPacket && g_pfnSendPacket(buffer, sizeof(buffer), true, true);

        if (ok) log("RPC_LibraryInit handshake sent successfully.");
        else    log("RPC_LibraryInit handshake FAILED to send.");
    }

    void sendClientEvent(const char* szEventName, const char* szEventData)
    {
        if (!g_bInitialized || !szEventName || !szEventData) return;

        uint16_t nameLen = (uint16_t) strlen(szEventName);
        uint16_t dataLen = (uint16_t) strlen(szEventData);

        size_t totalSize = 2 + 4 + 2 + nameLen + 2 + dataLen;
        uint8_t* pBuffer = (uint8_t*) malloc(totalSize);
        if (!pBuffer) return;

        size_t offset = 0;
        uint16_t packetId16 = (uint16_t) g_nPacketId;
        memcpy(pBuffer + offset, &packetId16, 2); offset += 2;

        uint32_t rpcId = RPC_ClientEvent;
        memcpy(pBuffer + offset, &rpcId, 4); offset += 4;

        memcpy(pBuffer + offset, &nameLen, 2); offset += 2;
        memcpy(pBuffer + offset, szEventName, nameLen); offset += nameLen;

        memcpy(pBuffer + offset, &dataLen, 2); offset += 2;
        memcpy(pBuffer + offset, szEventData, dataLen); offset += dataLen;

        bool ok = g_pfnSendPacket && g_pfnSendPacket(pBuffer, totalSize, true, true);

        if (ok) log("ClientEvent sent successfully -> Event: '%s', DataLen: %u", szEventName, dataLen);
        else    log("ClientEvent FAILED to send -> Event: '%s'", szEventName);

        free(pBuffer);
    }

    void sendBrowserInit(bool isSuccess, int16_t errorCode)
    {
        if (!g_bInitialized) return;

        uint8_t buffer[9];
        size_t offset = 0;

        uint16_t packetId16 = (uint16_t) g_nPacketId;
        memcpy(buffer + offset, &packetId16, 2); offset += 2;

        uint32_t rpcId = RPC_BrowserInit;
        memcpy(buffer + offset, &rpcId, 4); offset += 4;

        uint8_t boolVal = isSuccess ? 1 : 0;
        memcpy(buffer + offset, &boolVal, 1); offset += 1;

        memcpy(buffer + offset, &errorCode, 2); offset += 2;

        bool ok = g_pfnSendPacket && g_pfnSendPacket(buffer, sizeof(buffer), true, true);

        if (ok) log("RPC_BrowserInit sent -> success=%d, errorCode=%d", (int) isSuccess, (int) errorCode);
        else    log("RPC_BrowserInit FAILED to send.");
    }

    // all RPC packets share the same header: u16 packetId, u32 rpcId.
    // the string payloads are prefixed with a u16 length field.
    void handlePacket(void* pPacket)
    {
        if (!pPacket || !g_bInitialized) return;

        Packet* pkt = (Packet*) pPacket;
        if (!pkt->data || pkt->length < 6) return;

        uint8_t* pData = pkt->data;

        uint16_t packetId = 0;
        memcpy(&packetId, pData, sizeof(uint16_t));
        if (packetId != g_nPacketId) return;

        uint32_t rpcId = 0;
        memcpy(&rpcId, pData + 2, sizeof(uint32_t));

        switch (rpcId)
        {
            case RPC_InitBrowser:
            {
                if (pkt->length < 8) { log("RPC_InitBrowser: packet too short"); break; }
                uint16_t urlLen = 0;
                memcpy(&urlLen, pData + 6, sizeof(uint16_t));
                char szUrl[512] = {0};
                if (urlLen < sizeof(szUrl) && (size_t)(8 + urlLen) <= pkt->length) {
                    memcpy(szUrl, pData + 8, urlLen);
                }
                log("RPC_InitBrowser -> URL: %s", szUrl);
                callJavaOpenBrowser(szUrl);
                break;
            }
            case RPC_DestroyBrowser:
                log("RPC_DestroyBrowser executed.");
                callJavaCloseBrowser();
                break;
            case RPC_ShowBrowser:
                log("RPC_ShowBrowser executed.");
                callJavaShowBrowser();
                break;
            case RPC_HideBrowser:
                log("RPC_HideBrowser executed.");
                callJavaHideBrowser();
                break;
            case RPC_SetBrowserUrl:
            {
                if (pkt->length < 8) { log("RPC_SetBrowserUrl: packet too short"); break; }
                uint16_t urlLen = 0;
                memcpy(&urlLen, pData + 6, sizeof(uint16_t));
                char szUrl[512] = {0};
                if (urlLen < sizeof(szUrl) && (size_t)(8 + urlLen) <= pkt->length) {
                    memcpy(szUrl, pData + 8, urlLen);
                }
                log("RPC_SetBrowserUrl -> URL: %s", szUrl);
                callJavaSetUrl(szUrl);
                break;
            }
            case RPC_ChangeBrowserFocus:
            {
                if (pkt->length < 7) { log("RPC_ChangeBrowserFocus: packet too short"); break; }
                uint8_t isFocused = pData[6];
                log("RPC_ChangeBrowserFocus -> focused: %d", isFocused);
                callJavaSetFocus(isFocused != 0);
                break;
            }
            case RPC_ServerEvent:
            {
                if (pkt->length < 8) { log("RPC_ServerEvent: packet too short (header)"); break; }

                uint16_t nameLen = 0;
                memcpy(&nameLen, pData + 6, sizeof(uint16_t));
                size_t off = 8;

                if (off + nameLen + 2 > pkt->length) { log("RPC_ServerEvent: packet too short (name)"); break; }

                char szEventName[64] = {0};
                if (nameLen < sizeof(szEventName)) {
                    memcpy(szEventName, pData + off, nameLen);
                }
                off += nameLen;

                uint16_t dataLen = 0;
                memcpy(&dataLen, pData + off, sizeof(uint16_t));
                off += 2;

                if (off + dataLen > pkt->length) { log("RPC_ServerEvent: packet too short (data)"); break; }

                char* szEventData = (char*) malloc(dataLen + 1);
                if (!szEventData) break;
                memcpy(szEventData, pData + off, dataLen);
                szEventData[dataLen] = '\0';

                log("RPC_ServerEvent -> name: %s, dataLen: %u", szEventName, dataLen);
                callJavaDispatchServerEvent(szEventName, szEventData);

                free(szEventData);
                break;
            }
            default:
                log("Unknown CEF RPC ID: %u", rpcId);
                break;
        }
    }

    void log(const char* fmt, ...)
    {
        if (g_szLogPath[0] == '\0') return;

        FILE* f = fopen(g_szLogPath, "a");
        if (!f) return;

        time_t rawtime;
        struct tm* timeinfo;
        char timebuf[32];

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(timebuf, sizeof(timebuf), "[%Y-%m-%d %H:%M:%S]", timeinfo);

        fprintf(f, "%s ", timebuf);

        va_list args;
        va_start(args, fmt);
        vfprintf(f, fmt, args);
        va_end(args);

        fprintf(f, "\n");
        fclose(f);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_samp_cef_CefClientManager_nativeSendClientEvent(JNIEnv* env, jobject, jstring eventName, jstring eventData)
{
    if (!eventName || !eventData) return;

    const char* szEventName = env->GetStringUTFChars(eventName, nullptr);
    const char* szEventData = env->GetStringUTFChars(eventData, nullptr);

    if (szEventName && szEventData) {
        cef::sendClientEvent(szEventName, szEventData);
    }

    if (szEventName) env->ReleaseStringUTFChars(eventName, szEventName);
    if (szEventData) env->ReleaseStringUTFChars(eventData, szEventData);
}

extern "C" JNIEXPORT void JNICALL
Java_com_samp_cef_CefClientManager_nativeOnBrowserInit(JNIEnv* env, jobject, jboolean isSuccess, jint errorCode)
{
    cef::log("nativeOnBrowserInit: success=%d, errorCode=%d", (int) isSuccess, (int) errorCode);
    cef::sendBrowserInit((bool) isSuccess, (int16_t) errorCode);
}