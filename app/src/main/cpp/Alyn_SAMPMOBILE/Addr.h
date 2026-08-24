#include <cstdint>
#include <type_traits>

class addr {
public:
	// hooks
	static constexpr uintptr_t CModelInfo_ms_modelInfoPtrs = !VER_x32 ? 0xB666E0 : 0x91DCB8;
	static constexpr uintptr_t ms_atomicModelStore = !VER_x32 ? 0xA06668 : 0x820738;
	static constexpr uintptr_t ms_pedModelStore = !VER_x32 ? 0xB5CB80 : 0x915FC8;
	static constexpr uintptr_t CPed_ProcessControl = !VER_x32 ? 0x598730 : 0x4A2540;
	static constexpr uintptr_t CPed_UpdatePosition_BranchBL1 = !VER_x32 ? CPed_ProcessControl + 0x5EC : CPed_ProcessControl + 0x552;
	static constexpr uintptr_t CWidget_setEnabled = !VER_x32 ? 0x3730B0 : 0x2B3CAC;
	static constexpr uintptr_t pedRots = !VER_x32 ? 0x655A38 : 0x539C16;
	static constexpr uintptr_t CTaskComplexEnterCarAsDriver_Branch1 = !VER_x32 ? 0x4EF504 : 0x40AC80;
	static constexpr uintptr_t CTaskComplexLeaveCar_Branch1 = !VER_x32 ? 0x4EE3C0 : 0x409A8E;
	static constexpr uintptr_t CTaskComplexLeaveCar_Branch2 = !VER_x32 ? 0x4EEE00 : 0x40A864;
	static constexpr uintptr_t CWeapon_FireInstantHit_branch1 = !VER_x32 ? 0x700E64: 0x5DBBBA;
	static constexpr uintptr_t CWeapon_FireInstantHit_branch2 = !VER_x32 ? 0x700EFC: 0x5DBC12;
	// hooks end

	// game
	static constexpr uintptr_t ClockFormat = !VER_x32 ? 0x73E1DA : 0x2BD618;
	static constexpr uintptr_t CTheScripts_bDisplayHud = !VER_x32 ? 0x9FF3A8 : 0x819D88;
	static constexpr uintptr_t CHud_bScriptDontDisplayRadar = !VER_x32 ? 0xC20DFC : 0x991FD8;
	static constexpr uintptr_t ToggleCJWalk = !VER_x32 ? 0x5C3970 : 0x4C5F6A;
	static constexpr uintptr_t SetGravity = !VER_x32 ? 0x75044C : 0x3FE810;
	// game end

	// patches
	static constexpr uintptr_t Menu_SwitchOffToGame_StartGameScreen_OnNewGameCheck = !VER_x32 ? 0x365F10 : 0x2A72C8;
	static constexpr uintptr_t _ZN6CWorld7PlayersE_ptr = !VER_x32 ? 0x84E7A8 : 0x6783C0;
	static constexpr uintptr_t _rwOpenGLRasterCreate_patch = !VER_x32 ? 0x23FDE0 : 0x1AE8DE;
	static constexpr uintptr_t CPlayerPed_ProcessAnimGroups_patch = !VER_x32 ? 0x5C3970 : 0x4C5F6A;
	static constexpr uintptr_t CRealTimeShadowManager_ReturnRealTimeShadow_patch1 = !VER_x32 ? 0x4E054C : 0x3FCD84;
	static constexpr uintptr_t CRealTimeShadowManager_ReturnRealTimeShadow_patch2 = !VER_x32 ? 0x4E0598 : 0x3FCDC4;
	static constexpr uintptr_t CPlayerPed_CPlayerPed_patch = !VER_x32 ? 0x5C0BC4 : 0x4C36E2;
	static constexpr uintptr_t CAEVehicleAudioEntity_Service_patch1 = !VER_x32 ? 0x674610 : 0x553E96;
	static constexpr uintptr_t CAEVehicleAudioEntity_Service_patch2 = !VER_x32 ? 0x682C1C : 0x561AC2;
	static constexpr uintptr_t CAEVehicleAudioEntity_Service_patch3 = !VER_x32 ? 0x68DD0C : 0x56BED4;
	static constexpr uintptr_t CAEVehicleAudioEntity_Service_patch4 = !VER_x32 ? 0x6A0A68 : 0x57D0C4;
	static constexpr uintptr_t CAudioEngine_StartLoadingTune_patch = !VER_x32 ? 0x70A3D0 : 0x5E4966;
	static constexpr uintptr_t RwCameraEndUpdate_patch = !VER_x32 ? 0x4D9254 : 0x3F6CDC;
	static constexpr uintptr_t CBike_ProcessAI_patch = !VER_x32 ? 0x6859BC : 0x564D30;
	static constexpr uintptr_t CMessages_AddBigMessage_patch = !VER_x32 ? 0x4F07DC : 0x40BF26;
	static constexpr uintptr_t CRadar_DrawCoordBlip_patch = !VER_x32 ? 0x52522C : 0x43FE5A;
	static constexpr uintptr_t CRadar_DrawEntityBlip_patch = !VER_x32 ? 0x525E14 : 0x4409AE;
	static constexpr uintptr_t FindPlayerInfoForThisPlayerPed_patch = !VER_x32 ? 0x59BAD0 : 0x4A5398;
	// patches end

	// vtables
	static constexpr uintptr_t vptr$CPlaceable = !VER_x32 ? 0x830098 : 0x667D14;
	static constexpr uintptr_t vptr$CAutomobile = !VER_x32 ? 0x83BB50 : 0x66D678;
	static constexpr uintptr_t vptr$CBike = !VER_x32 ? 0x83BE40 : 0x66D7F0;
	static constexpr uintptr_t vptr$CBmx = !VER_x32 ? 0x83C070 : 0x66D908;
	static constexpr uintptr_t vptr$CBoat = !VER_x32 ? 0x83C2A0 : 0x66DA20;
	static constexpr uintptr_t vptr$CHeli = !VER_x32 ? 0x83C4C8 : 0x66DB34;
	static constexpr uintptr_t vptr$CMonsterTruck = !VER_x32 ? 0x83C718 : 0x66DC5C;
	static constexpr uintptr_t vptr$CPlane = !VER_x32 ? 0x83C968 : 0x66DD84;
	static constexpr uintptr_t vptr$CQuadBike = !VER_x32 ? 0x83CBB8 : 0x66DEAC;
	static constexpr uintptr_t vptr$CTrailer = !VER_x32 ? 0x83CE08 : 0x66DFD4;
	static constexpr uintptr_t vptr$CTrain = !VER_x32 ? 0x83D058 : 0x66E0FC;
	static constexpr uintptr_t vptr$CPedModelInfo = !VER_x32 ? 0x82F310 : 0x667658;
	static constexpr uintptr_t vptr$CTaskSimpleJetPack = !VER_x32 ? 0x83A6C0 : 0x66CC28;
	static constexpr uintptr_t vptr$CTaskSimpleHandsUp = !VER_x32 ? 0x82C0A0 : 0x6657F0;
	// vtables end

	// start of vtables
	static constexpr uintptr_t start$CTaskAllocatorKillThreatsDriveby = !VER_x32 ? 0x83B9D8 : 0x66D5BC;
	static constexpr uintptr_t start$CTaskSimpleUninterruptable = !VER_x32 ? 0x82B7F8 : 0x6653DC;
	// start of vtables end
};


