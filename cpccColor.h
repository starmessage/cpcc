//
// cpccColor.h
//

#pragma once

#include "cpccTypes.h"

typedef cpccUint32_t TmioanColorRGBA;


#define applyMinMax(xLow, xHigh, x)		(((x>xHigh)?xHigh:x)<xLow)?xLow:((x>xHigh)?xHigh:x)

/*
template<typename Tlimit>
Tlimit getLimitHigh(void)
{
	return 0;
}


template<>
(float) getLimitHigh<float>(void)
{
	return 1.0f;
}
*/

template<typename T>
class cpccColorT 
	{
	public: // data
		T a, b, g, r;
		
	public: // constructors


		cpccColorT(): r(0), g(0), b(0)
			{
			a=getLimitHigh();
			};
		
		
		cpccColorT(T red, T green, T blue)
			: r(red), g(green), b(blue)
			{
			a=getLimitHigh();
			};
		
		
		cpccColorT(T red, T green, T blue, T alpha)
			: r(red), g(green), b(blue), a(alpha)
			{ };
			

		cpccColorT( const cpccColorT<T> &src ) 
			: r( src.r ), g( src.g ), b( src.b ), a( src.a )
			{ };


	public: // functions


		virtual T	getLimitHigh(void)=0;
		/*
		template<>
		static float		getLimitHigh(void) { return 1.0f; };
		
		template<>
		static cpccUint8_t	getLimitHigh(void) { return 255; };
		*/

		
		cpccColorT<T> operator=( const cpccColorT<T> &rhs ) 
		{
			r = rhs.r;
			g = rhs.g;
			b = rhs.b;
			return * this;
		}
	
	
		TmioanColorRGBA asARGB(void) 
		{
			return	((a << 24) | (r << 16) | (g << 8) | b);
		};
		
		TmioanColorRGBA asRGB(void) 
		{
			return	((r << 16) | (g << 8) | b);
		};
		
		
		void amplifyComponents(float xR, float xG, float xB)
		{
			r = applyMinMax(0, 255, (int) (r * xR));
			g = applyMinMax(0, 255, (int) (g * xG));
			b = applyMinMax(0, 255, (int) (b * xB));
		};

		
		/*
		TmioanColor operator *(float f)
		{
			changeBrightness(f,f,f);
			return this
		}
		*/
	};

/*
template <>		// Partial specialization
class cpccColorT<float>
{
public:
    static float			getLimitHigh(void) { return 1.0f; }
};
*/


class cpccColorF	: public cpccColorT<float>
{
public:
    float			getLimitHigh(void) { return 1.0f; }
};

class cpccColor	: public cpccColorT<cpccUint8_t>
{
public:
    cpccUint8_t			getLimitHigh(void) { return 255; }
};


/*
template <>		// Partial specialization
class cpccColorT<cpccUint8_t>
{
public:
	cpccUint8_t				a, b, g, r;

    static cpccUint8_t		getLimitHigh(void) { return 255; }

	cpccColorT(cpccUint32_t aColorHex) : a(255)
	{	
		b = aColorHex & 0xFF;
		aColorHex = aColorHex >> 8;
		g = aColorHex & 0xFF;
		aColorHex = aColorHex >> 8;
		r = aColorHex & 0xFF;
	}

};
*/



// typedef 	cpccColorT<float>				cpccColorF;
// typedef 	cpccColorT<cpccUint8_t>			cpccColor; // default

static const cpccColor						cpccBlack(0,0,0);
static const cpccColor						cpccGray(127,127,127);
static const cpccColor						cpccBlue(0,0,255);
static const cpccColor						cpccDarkblue(10,10,50);
static const cpccColor						cpccRed(255,0,0);


