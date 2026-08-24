//
// Created by ALYN on 25-Jan-25.
//

#ifndef SA_MP_MOBILE_EDITOBJECT_H
#define SA_MP_MOBILE_EDITOBJECT_H

#include <jni.h>

class EditObject {
public:
	enum eEditType {
		TYPE_NONE,
		TYPE_ATTACHED_OBJECT,
		TYPE_OBJECT
	};

	enum eResponse : uint32_t {
		EDIT_RESPONSE_CANCEL,
		EDIT_RESPONSE_FINAL,
		EDIT_RESPONSE_UPDATE
	};

	static inline int m_id = -1;
	static inline eEditType m_type = TYPE_NONE;
	static inline bool m_visible = false;
	static inline bool m_playerObj = false;
	static inline uint32_t m_lastUpdate = GetTickCount();

public:
	static void startEditAttachedObject(int slot);
	static void startEditObject(uint16_t objectId);
	static void showGui(bool show);
	static void cancelEdit();
};

#endif //SA_MP_MOBILE_EDITOBJECT_H
