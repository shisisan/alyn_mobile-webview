//
// Created by ALYN on 2024/8/18.
//
#pragma once

#include "../SA.h"
#include "AnimBlendNode.h"
#include "AnimBlendHierarchy.h"

namespace sa {
enum eAnimationFlags {
	ANIMATION_DEFAULT = 0x0,
	ANIMATION_STARTED = 0x1,
	ANIMATION_LOOPED = 0x2,
	ANIMATION_FREEZE_LAST_FRAME = 0x4,
	ANIMATION_UNLOCK_LAST_FRAME = 0x8,  // Animation will be stuck on last frame, if not set
	ANIMATION_PARTIAL = 0x10, // TODO: Flag name is possibly incorrect? Following the usual logic (like `ANIMATION_MOVEMENT`), it should be `ANIMATION_GET_IN_CAR` (See  `RemoveGetInAnims`)
	ANIMATION_MOVEMENT = 0x20,
	ANIMATION_TRANSLATE_Y = 0x40,
	ANIMATION_TRANSLATE_X = 0x80,
	ANIMATION_WALK = 0x100,
	ANIMATION_200 = 0x200,
	ANIMATION_ADD_TO_BLEND = 0x400, // Possibly should be renamed to ANIMATION_IDLE, see `CPed::PlayFootSteps()`
	ANIMATION_800 = 0x800,
	ANIMATION_SECONDARY_TASK_ANIM = 0x1000,
	ANIMATION_FREEZE_TRANSLATION = 0x2000,
	ANIMATION_BLOCK_REFERENCED = 0x4000,
	ANIMATION_INDESTRUCTIBLE = 0x8000 // The animation is never destroyed if this flag is set, NO MATTER WHAT
};

enum eAnimBlendCallbackType : uint32_t {
	ANIM_BLEND_CALLBACK_NONE = 0,
	ANIM_BLEND_CALLBACK_FINISH = 1,
	ANIM_BLEND_CALLBACK_DELETE = 2
};

class CAnimBlendLink {
public:
	CAnimBlendLink* next{};
	CAnimBlendLink* prev{};

	CAnimBlendLink() = default;

	void Init()
	{
		next = nullptr;
		prev = nullptr;
	}

	void Prepend(CAnimBlendLink* link)
	{
		if (next) {
			next->prev = link;
		}
		link->next = next;
		link->prev = this;
		next = link;
	}

	void Remove()
	{
		if (prev) {
			prev->next = next;
		}
		if (next) {
			next->prev = prev;
		}
		Init();
	}
};

struct SClumpAnimAssoc {
	CAnimBlendLink m_Link;
	uint16 m_nNumBlendNodes;
	int16 m_nAnimGroup;
	CAnimBlendNode* m_pNodeArray;
	CAnimBlendHierarchy* m_pHierarchy;
	float m_fBlendAmount;
	float m_fBlendDelta;
	float m_fCurrentTime;
	float m_fSpeed;
	float m_fTimeStep;
	int16 m_nAnimId;
	uint16 m_nFlags; // TODO: use bitfield

	float GetBlendAmount(float weight) { return IsPartial() ? m_fBlendAmount : m_fBlendAmount * weight; }
	[[nodiscard]] bool IsRunning() const { return (m_nFlags & ANIMATION_STARTED) != 0; }
	[[nodiscard]] bool IsRepeating() const { return (m_nFlags & ANIMATION_LOOPED) != 0; }
	[[nodiscard]] bool IsPartial() const { return (m_nFlags & ANIMATION_PARTIAL) != 0; }
	[[nodiscard]] bool IsMoving() const { return (m_nFlags & ANIMATION_MOVEMENT) != 0; }
	[[nodiscard]] bool HasYTranslation() const { return (m_nFlags & ANIMATION_TRANSLATE_X) != 0; }
	[[nodiscard]] bool HasXTranslation() const { return (m_nFlags & ANIMATION_TRANSLATE_Y) != 0; }
	[[nodiscard]] bool IsIndestructible() const { return (m_nFlags & ANIMATION_INDESTRUCTIBLE) != 0; }
};

class CAnimBlendClumpData {
public:
	CAnimBlendLink m_list;
	union {
		int32 m_numFrames;
		int32 m_numBones;
	};
#if !VER_x32
	uint8 m_padd[4];
#endif
	float* m_pExtractedVelocity;
	AnimBlendFrameData* m_pFrameDataArray;

	AnimBlendFrameData* GetFrameDataByNodeId(unsigned int nodeId)
	{
		for (int i = 0; i < m_numFrames; i++) {
			AnimBlendFrameData* frameData = &m_pFrameDataArray[i];
			if (frameData->m_nNodeId == nodeId) {
				return frameData;
			}
		}
		return nullptr;
	}
};
VALIDATE_SIZE(CAnimBlendClumpData, (VER_x32 ? 0x14 : 0x28));

class CAnimBlendStaticAssociation {
public:
	uintptr vtable;
	uint16 m_nNumBlendNodes;
	int16 m_nAnimId;
	int16 m_nAnimGroup;
	uint16 m_nFlags;
	CAnimBlendSequence** m_pSequenceArray;
	CAnimBlendHierarchy* m_pHierarchy;

public:
	auto GetHashKey() const noexcept { return m_pHierarchy->m_hashKey; }
};
VALIDATE_SIZE(CAnimBlendStaticAssociation, (VER_x32 ? 0x14 : 0x20));

class CAnimBlendAssociation : public SClumpAnimAssoc {
public:
	eAnimBlendCallbackType m_nCallbackType;
	void (* m_pCallbackFunc)(CAnimBlendAssociation*, void*);
	void* m_pCallbackData;

public:
	virtual ~CAnimBlendAssociation();

	// NOTSA
	void SetFlag(eAnimationFlags flag, bool value = true)
	{
		if (value) {
			m_nFlags |= (int) flag;
		}
		else {
			m_nFlags &= ~(int) flag;
		}
	}

	static CAnimBlendAssociation* FromLink(CAnimBlendLink* link)
	{
		return (CAnimBlendAssociation*) ((char*) link - offsetof(CAnimBlendAssociation, m_Link));
	}

	void SetSpeed(float speed)
	{
		m_fSpeed = speed;
	}
};
VALIDATE_SIZE(CAnimBlendAssociation, (VER_x32 ? 0x3C : 0x60));
}
