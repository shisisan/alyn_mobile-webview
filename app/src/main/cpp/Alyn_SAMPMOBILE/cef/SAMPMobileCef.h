//
// Created by shisisan on 21/08/2026.
//

#ifndef SAMP_MOBILE_CEF_H
#define SAMP_MOBILE_CEF_H

#include <stdint.h>
#include <stdbool.h>
#include <jni.h>

namespace cef
{
    enum CefRpcId
    {
        RPC_InitBrowser = 1,
        RPC_DestroyBrowser,
        RPC_ShowBrowser,
        RPC_HideBrowser,
        RPC_SetBrowserUrl,
        RPC_ChangeBrowserFocus,
        RPC_ServerEvent,

        RPC_LibraryInit = 8,
        RPC_BrowserInit = 9,
        RPC_ClientEvent = 10
    };

    typedef bool (*SendPacketFn)(const uint8_t* data, size_t len, bool highPriority, bool reliable);

    void setSendCallback(SendPacketFn fn);
    void applyJavaVM(JavaVM* vm);
    void setGamePath(const char* szPath);

    void initNetwork(void* pRakClient, uint8_t packetId);
    void handlePacket(void* pPacket);
    void handleServerConnection();
    void sendClientEvent(const char* szEventName, const char* szEventData);
    void sendBrowserInit(bool isSuccess, int16_t errorCode);

    bool isInitialized();
    void log(const char* fmt, ...);
}

#endif