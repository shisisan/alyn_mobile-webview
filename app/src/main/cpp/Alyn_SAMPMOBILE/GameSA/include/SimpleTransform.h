#pragma once

#include "Core/Vector.h"
#include "RenderWare/rwplcore.h"

namespace sa {
class CSimpleTransform {
public:
	CSimpleTransform()
			: m_vPosn(), m_fHeading(0.0F) { }

public:
	CVector m_vPosn; // SA name m_translate
	float m_fHeading;

	void UpdateRwMatrix(RwMatrix* out);
	void Invert(const CSimpleTransform& base);
	void UpdateMatrix(class CMatrix* out);
};
VALIDATE_SIZE(CSimpleTransform, 0x10);
}
