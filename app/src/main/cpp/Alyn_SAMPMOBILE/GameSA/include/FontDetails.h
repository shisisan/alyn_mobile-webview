//
// Created by ALYN on 2024/9/8.
//

#ifndef SAMP_MOBILE_FONTDETAILS_H
#define SAMP_MOBILE_FONTDETAILS_H

#include "SA.h"

namespace sa {
struct CRGBA : RwRGBA {
};

class CFontDetails {
public:
	CRGBA Color;
	float ScaleX;
	float ScaleY;
	float Slope;
	float SlopeRefX;
	float SlopeRefY;
	bool8 Justify;
	bool8 Centre;
	bool8 RightJustify;
	bool8 Background;
	bool8 BackgroundOutline;
	bool8 Proportional;
	bool8 Shadow;
	uint8 __pad;
	float AlphaFade;
	CRGBA BGColor;
	float WrapEnd;
	float Centrex;
	float RightJustifyWrap;
	uint8 Style;
	uint8 ExtraFont;
	Int8 DropAmount;
	CRGBA DropColor;
	Int8 EdgeAmount;
	Int8 EdgeSpace;
	uint8 __pad2[3];
};
VALIDATE_SIZE(CFontDetails, 0x40);
}

#endif //SAMP_MOBILE_FONTDETAILS_H
