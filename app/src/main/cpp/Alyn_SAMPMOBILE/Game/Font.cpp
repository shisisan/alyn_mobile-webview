#include "../Client.h"
#include "Font.h"

void CFont::StringToAscii(const char* str, uint16_t* ascii)
{
	const char* ptr = str;
	while (*ptr) {
		unsigned char ch1 = *ptr;
		if (ch1 < 0x80) {
			*ascii++ = ch1;
		}
		else if (ch1 >= 0xC0 && ch1 <= 0xDF) {
			unsigned char ch2 = ptr[1];
			*ascii++ = ((ch1 & 0x1F) << 6) | (ch2 & 0x3F);
			ptr++;
		}
		else if (ch1 >= 0xE0 && ch1 <= 0xEF) {
			unsigned char ch2 = ptr[1];
			unsigned char ch3 = ptr[2];
			*ascii++ = ((ch1 & 0x0F) << 12) | ((ch2 & 0x3F) << 6) | (ch3 & 0x3F);
			ptr += 2;
		}
		else if (ch1 >= 0xF0 && ch1 <= 0xF7) {
			unsigned char ch2 = ptr[1];
			unsigned char ch3 = ptr[2];
			unsigned char ch4 = ptr[3];
			*ascii++ = ((ch1 & 0x07) << 18) | ((ch2 & 0x3F) << 12) | ((ch3 & 0x3F) << 6) | (ch4 & 0x3F);
			ptr += 3;
		}
		else {
			// Invalid character
			ptr++;
		}
		ptr++;
	}
	*ascii = 0; // Terminate with zero
}

void CFont::AsciiToGxtCharU8(uint16_t* ascii, uint16_t* gxt)
{
	while (*ascii) {
		int asciiValue = static_cast<int>(*ascii);

		bool found = false;
		for (auto& i : g_asciiMap) {
			if (i[0] == asciiValue) {
				*gxt++ = static_cast<uint16_t>(i[1]);
				found = true;
				break;
			}
		}
		if (!found) {
			// handle unknown characters
			*gxt++ = 63; // Fill unsupported text with question marks
		}

		ascii++;
	}
	*gxt = 0; // Terminate with zero
}

void CFont::AsciiToGxtChar(const char* ascii, uint16_t* gxt)
{
	if (ascii) {
		uint8_t currentChar = (uint8_t) *ascii;
		if (*ascii) {
			const char* nextCharPointer = ascii + 1;
			do {
				*gxt++ = currentChar;
				currentChar = (uint8_t) *nextCharPointer++;
			}
			while (currentChar);
		}
		*gxt = 0;
	}
}

void CFont::SetScale(float x, float y)
{
	sa::CFontDetails& fontDetails = *g_saSym->GetSymbol<sa::CFontDetails*>("_ZN5CFont7DetailsE");
	fontDetails.ScaleX = x;
	fontDetails.ScaleY = y;
}

void CFont::SetColor(RGBA* dwColor)
{
	Memory::callFunction("_ZN5CFont8SetColorE5CRGBA", dwColor);
}

void CFont::SetJustify(uint8_t just)
{
	Memory::callFunction("_ZN5CFont10SetJustifyEh", just);
}

void CFont::SetOrientation(uint8_t orientation)
{
	Memory::callFunction("_ZN5CFont14SetOrientationEh", orientation);
}

void CFont::SetWrapX(float wrap)
{
	Memory::callFunction("_ZN5CFont8SetWrapxEf", wrap);
}

void CFont::SetCentreSize(float size)
{
	Memory::callFunction("_ZN5CFont13SetCentreSizeEf", size);
}

void CFont::SetBackground(uint8_t bBackground, uint8_t bOnlyText)
{
	Memory::callFunction("_ZN5CFont13SetBackgroundEhh", bBackground, bOnlyText);
}

void CFont::SetBackgroundColor(RGBA* dwColor)
{
	Memory::callFunction("_ZN5CFont18SetBackgroundColorE5CRGBA", dwColor);
}

void CFont::SetProportional(uint8_t prop)
{
	Memory::callFunction("_ZN5CFont15SetProportionalEh", prop);
}

void CFont::SetDropShadowPosition(uint8_t pos)
{
	Memory::callFunction("_ZN5CFont21SetDropShadowPositionEa", pos);
}

void CFont::SetDropColor(RGBA* dwColor)
{
	Memory::callFunction("_ZN5CFont12SetDropColorE5CRGBA", dwColor);
}

void CFont::PrintString(float fX, float fY, const char* szText)
{
	auto gxt_string = new uint16_t[800];
	//AsciiToGxtChar(szText, gxt_string);

	uint16_t szAscii[801];
	StringToAscii(Encoding::cp2utf(szText).c_str(), szAscii);
	AsciiToGxtCharU8(szAscii, gxt_string);

	// CFont::PrintString
	Memory::callFunction("_ZN5CFont11PrintStringEffPt", fX, fY, gxt_string);

	delete gxt_string;

	// CFont::RenderFontBuffer
	Memory::callFunction("_ZN5CFont16RenderFontBufferEv");
}

void CFont::SetFontStyle(uint8_t style)
{
	Memory::callFunction("_ZN5CFont12SetFontStyleEh", style);
}

void CFont::SetEdge(uint8_t edge)
{
	Memory::callFunction("_ZN5CFont7SetEdgeEa", edge);
}





