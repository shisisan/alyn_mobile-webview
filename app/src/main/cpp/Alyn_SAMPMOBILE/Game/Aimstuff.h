#pragma once

extern uint8_t* pbyteCameraMode;
extern float* pfCameraExtZoom;
extern uint8_t* pbyteCurrentPlayer;
extern uint16_t* wCameraMode2;

#pragma pack(push, 1)
typedef struct _CAMERA_AIM {
	float f1x, f1y, f1z;        // m_vecFront
	float pos1x, pos1y, pos1z;  // m_vecSource
	float pos2x, pos2y, pos2z;  // m_vecSourceBeforeLookBehind
	float f2x, f2y, f2z;        // m_vecUp
} CAMERA_AIM;
#pragma pack(pop)

void GameAimSyncInit();
CAMERA_AIM* GameGetInternalAim();
float GameGetAspectRatio();

/* CameraMode */
uint8_t GameGetLocalPlayerCameraMode();
void GameSetPlayerCameraMode(uint8_t byteMode, uint8_t bytePlayerNumber);
uint8_t GameGetPlayerCameraMode(uint8_t bytePlayerNumber);

/* Aim */
CAMERA_AIM* GameGetRemotePlayerAim(uint8_t bytePlayerNumber);
void GameSetLocalPlayerAim();
void GameStoreLocalPlayerAim();
void GameSetRemotePlayerAim(uint8_t bytePlayerNumber);
void GameStoreRemotePlayerAim(uint8_t bytePlayerNumber, CAMERA_AIM* pAim);

/*ExtZoom & AspectRatio*/
float GameGetLocalPlayerCameraExtZoom();
void GameSetLocalPlayerCameraExtZoomAndAspect();
void GameSetRemotePlayerCameraExtZoomAndAspect(uint8_t bytePlayerNumber);
void GameStoreLocalPlayerCameraExtZoomAndAspect();
void GameSetPlayerCameraExtZoomAndAspect(uint8_t bytePlayerNumber, float fExtZoom, float fAspectRatio);

void calculateAimVector(sa::CVector* vec1, sa::CVector* vec2);