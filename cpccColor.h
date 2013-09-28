//
// cpccColor.h
//

#pragma once

#define NOMINMAX

#include "cpccTypes.h"
#include <algorithm>


typedef cpccUint32_t TmioanColorRGBA;



template<typename T>
class colorChannelTypes	{  };


template <>		// Partial specialization
class colorChannelTypes<cpccUint8_t>
{
public:
	static cpccUint8_t		getLimitHigh(void)			{ return 255; }
	static cpccUint8_t		applyLimits(const float a)	{ return (cpccUint8_t) std::min( 255.0f, std::max(0.0f, a)); }
	static cpccUint8_t		convertFrom(const float a)	{ return applyLimits(a*255.0f); }
};


template <>		// Partial specialization
class colorChannelTypes<float>
{
public:
	static float		getLimitHigh(void)					{ return 1.0f; }
	static float		applyLimits(const float a)			{ return std::min( 1.0f, std::max(0.0f, a)); }
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
		};
		
		
		cpccColorT(T red, T green, T blue)
			: r(red), g(green), b(blue)
		{
			a= colorChannelTypes<T>::getLimitHigh();
		};
		
		
		cpccColorT(T red, T green, T blue, T alpha)
			: r(red), g(green), b(blue), a(alpha)
			{ };
			

		cpccColorT( const cpccColorT<T> &src ) 
			: r( src.r ), g( src.g ), b( src.b ), a( src.a )
			{ };

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
		};
		
		TmioanColorRGBA asRGB(void) 
		{
			return	((r << 16) | (g << 8) | b);
		};
		
		
		void amplifyComponents(const float xR, const float xG, const float xB)
		{
			r = colorChannelTypes<T>::applyLimits(r*xR);
			g = colorChannelTypes<T>::applyLimits(g*xG);
			b = colorChannelTypes<T>::applyLimits(b*xB);
		};

		
		
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


typedef 	cpccColorT<float>				cpccColorF;
typedef 	cpccColorT<cpccUint8_t>			cpccColor; // default

static const cpccColor						cpccBlack( 0, 0, 0);
static const cpccColor						cpccGray(127,127,127);
static const cpccColor						cpccBlue(0,0,255);
static const cpccColor						cpccDarkblue(10,10,50);
static const cpccColor						cpccRed(255,0,0);

