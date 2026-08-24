//
// Created by ALYN on 2024/8/28.
//

#ifndef SA_MP_MOBILE_FILEOBJECTINSTANCE_H
#define SA_MP_MOBILE_FILEOBJECTINSTANCE_H

#include "SA.h"

namespace sa {
class CFileObjectInstance {
public:
	CVector m_vecPosition;
	CQuaternion m_qRotation;
	int32 m_nModelId;
	union {
		struct { // CFileObjectInstanceType
			uint32 m_nAreaCode : 8;
			uint32 m_bRedundantStream : 1;
			uint32 m_bDontStream : 1; // Merely assumed, no countercheck possible.
			uint32 m_bUnderwater : 1;
			uint32 m_bTunnel : 1;
			uint32 m_bTunnelTransition : 1;
			uint32 m_nReserved : 19;
		};
		uint32 m_nInstanceType;
	};
	int32 m_nLodInstanceIndex; // -1 - without LOD model
};

VALIDATE_SIZE(CFileObjectInstance, 0x28);
}

#endif //SA_MP_MOBILE_FILEOBJECTINSTANCE_H
