//
// Created by ALYN on 17-Jan-25.
//

#ifndef SC_MAIN_H
#define SC_MAIN_H

#include <android/log.h>
#include <cstdint>

extern "C" int hasTouchScreen;
extern "C" int EnterGameFromSC(void);
extern "C" int (* EnterGameFromSCFunc)();
extern unsigned char SigningOutfromApp;
extern "C" int _Z13SetJNEEnvFuncPFPvvE(int result);
extern "C" int cloudStartDownload(const char* a1);
extern "C" int cloudModAddWatch(char* a1);
extern "C" int cloudModFind(char* a1);
extern "C" int cloudIsBusy();
extern "C" int _Z15EnterSocialCLubv(uintptr_t* a1);
extern "C" int scmainUpdate(int result, int a2, int a3);
extern "C" int _Z18TelemetryDataFlushv(uintptr_t* a1);
extern "C" int cloudGetBufferLen();
extern "C" unsigned char* cloudModReset(char* a1);
extern "C" int GetCloudUploadResult();
extern "C" int cloudStartUpload(const char* a1, const char* a2, const void* a3, size_t a4);
extern "C" int cloudStartCheckMod(const char* a1);
extern "C" char* GetRockstarID();
extern "C" void _Z17TelemetryDataSendPKcS0_(const char* a1, const char* a2);
extern "C" int cloudGetFree();
extern "C" int cloudGetBufferPtr();
extern "C" int _Z12IsSCSignedInv(uintptr_t* a1);
extern "C" int _Z16ProfileStatsSendPKci(const char* a1, int a2);
extern "C" bool IsProfileStatsBusy();

#endif //SC_MAIN_H
