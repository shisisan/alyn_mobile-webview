#pragma once

class RGBA {
public:
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;

public:
	RGBA(unsigned char red, unsigned char green, unsigned blue)
	{
		Set(red, green, blue, 255);
	}

	RGBA(unsigned char red, unsigned char green, unsigned blue, unsigned char alpha)
	{
		Set(red, green, blue, alpha);
	}

	RGBA(RGBA const& rhs)
	{
		Set(rhs);
	}

	RGBA(unsigned int intValue)
	{
		Set(intValue);
	}

	RGBA() { }

	void Set(unsigned char red, unsigned char green, unsigned blue)
	{
		R = red;
		G = green;
		B = blue;
	}

	void Set(unsigned char red, unsigned char green, unsigned blue, unsigned char alpha)
	{
		Set(red, green, blue);
		A = alpha;
	}

	void Set(RGBA const& rhs)
	{
		Set(rhs.R, rhs.G, rhs.B, rhs.A);
	}

	void Set(RGBA const& rhs, unsigned char alpha)
	{
		Set(rhs.R, rhs.G, rhs.B, alpha);
	}

	void Set(unsigned int intValue)
	{
		R = (intValue >> 24) & 0xFF;
		G = (intValue >> 16) & 0xFF;
		B = (intValue >> 8) & 0xFF;
		A = intValue & 0xFF;
	}

	RGBA ToRGB() const
	{
		return RGBA(R, G, B, 255);
	}

	unsigned int ToInt() const
	{
		return A | (B << 8) | (G << 16) | (R << 24);
	}

	unsigned int ToIntARGB() const
	{
		return B | (G << 8) | (R << 16) | (A << 24);
	}

	void Invert()
	{
		Set(255 - R, 255 - G, 255 - B);
	}

	RGBA Inverted() const
	{
		RGBA invertedColor = *this;
		invertedColor.Invert();
		return invertedColor;
	}

	bool operator ==(RGBA const& rhs) const
	{

	}

	RGBA& operator =(RGBA const& rhs)
	{
		A = rhs.A;
		B = rhs.B;
		G = rhs.G;
		R = rhs.R;
	}
};
