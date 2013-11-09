//
// cpccColor.h
//

#pragma once

#define NOMINMAX

#include "cpccTypes.h"
#include <algorithm>


#ifdef _WIN32
	#include <windows.h>	// for COLORREF
#endif


typedef cpccUint32_t TmioanColorRGBA;

#define applyMinMax(xLow, xHigh, x)		(((x>xHigh)?xHigh:x)<xLow)?xLow:((x>xHigh)?xHigh:x)

template<typename T>
class colorChannelTypes	{  };



template <>		// Partial specialization
class colorChannelTypes<cpccUint8_t>
{
public:
	static cpccUint8_t		getLimitHigh(void)			{ return 255; }
	static cpccUint8_t		applyLimits(const float a)	{ return (cpccUint8_t) (applyMinMax( 0, 255, a)); }
	static cpccUint8_t		convertFrom(const float a)	{ return applyLimits(a*255.0f); }
};


template <>		// Partial specialization
class colorChannelTypes<float>
{
public:
	static float		getLimitHigh(void)					{ return 1.0f; }
	static float		applyLimits(const float a)			{ return applyMinMax(0.0f, 1.0f, a); }
	static float		convertFrom(const cpccUint8_t a)	{ return applyLimits(a/255.0f); }
};



template<typename T>
class cpccColorT 
	{
	public: // data
		T a, b, g, r;

		enum { config_RunSelfTest=true };
		
	public: // constructors


		cpccColorT(): r(0), g(0), b(0)
		{
			a= colorChannelTypes<T>::getLimitHigh();
		}
		
		
		cpccColorT(T red, T green, T blue)
			: r(red), g(green), b(blue)
		{
			a= colorChannelTypes<T>::getLimitHigh();
		}
		
		
		cpccColorT(T red, T green, T blue, T alpha)
			: r(red), g(green), b(blue), a(alpha)
		{ }
			

		cpccColorT( const cpccColorT<T> &src ) 
			: r( src.r ), g( src.g ), b( src.b ), a( src.a )
		{ }

		/*
		cpccColorT(cpccUint32_t aColorHex) : a(255)
		{	
			b = aColorHex & 0xFF;
			aColorHex = aColorHex >> 8;
			g = aColorHex & 0xFF;
			aColorHex = aColorHex >> 8;
			r = aColorHex & 0xFF;
		}
		*/


	public: // functions

	
	
		TmioanColorRGBA asARGB(void) 
		{
			return	((a << 24) | (r << 16) | (g << 8) | b);
		}
		

		TmioanColorRGBA asRGB(void) 
		{
			return	((r << 16) | (g << 8) | b);
		}
		



		void amplifyComponents(const float xR, const float xG, const float xB)
		{
			r = colorChannelTypes<T>::applyLimits(r*xR);
			g = colorChannelTypes<T>::applyLimits(g*xG);
			b = colorChannelTypes<T>::applyLimits(b*xB);
		}

		
		
	public:		// operators ----------------------------------------	

		cpccColorT<T>& operator=( const cpccColorT<T> &rhs ) 
		{
			r = rhs.r;
			g = rhs.g;
			b = rhs.b;
			a = rhs.a;
			return * this;
		}


		cpccColorT<T> operator *(const float f)
		{
			cpccColorT<T> tmp( *this );
			tmp.amplifyComponents(f,f,f);
			return tmp;
		}
		
		
		cpccColorT<T> operator *=(const float f)
		{
			amplifyComponents(f,f,f);
			return this;
		}


	public:		// selftest ----------------------------------------

		static void selfTest();

	};




// default color class with 8 bit components
class 		cpccColor: public cpccColorT<cpccUint8_t>
{
public:
	cpccColor() : cpccColorT<cpccUint8_t>() { }


	cpccColor(unsigned int aColorHex) 
	{	
			b = aColorHex & 0xFF;
			aColorHex = aColorHex >> 8;
			g = aColorHex & 0xFF;
			aColorHex = aColorHex >> 8;
			r = aColorHex & 0xFF;
			a = 255;
	}


	cpccColor(cpccUint8_t aRed, cpccUint8_t aGreen, cpccUint8_t aBlue, cpccUint8_t aAlpha=255) : cpccColorT<cpccUint8_t>(aRed, aGreen, aBlue, aAlpha)
	{ }


#ifdef _WIN32
	COLORREF asColorref(void)
	{
		return	((b << 16) | (g << 8) | r);
	}

#endif


}; 


typedef 	cpccColorT<float>				cpccColorF;

//	http://www.rapidtables.com/web/color/RGB_Color.htm

static const cpccColor	cpccBlack(   0x000000);
static const cpccColor	cpccGray(    0x808080);
static const cpccColor	cpccBlue(    0x0000FF);
static const cpccColor	cpccGreen(	0x00FF00);
static const cpccColor	cpccDarkblue(0x0A0A30);
static const cpccColor	cpccRed(     0xFF0000);
static const cpccColor	cpccYellow( 	0xFFFF00);
static const cpccColor	cpccMagenta(	0xFF00FF);
static const cpccColor	cpccSilver( 	0xC0C0C0);
static const cpccColor	cpccMaroon( 	0x800000);
static const cpccColor	cpccNavy( 	0x000080);
static const cpccColor	cpccMidnightBlue( 	0x191970);
static const cpccColor	cpccDarkBlue( 	0x00008B);
static const cpccColor	cpccAntiqueWhite( 	0xFAEBD7);
static const cpccColor	cpccLemonChiffon( 	0xFFFACD);
static const cpccColor	cpccMoccasin( 	0xFFE4B5);
static const cpccColor	cpccSlateGray( 	0x708090);