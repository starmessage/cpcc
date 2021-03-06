/*  *****************************************
 *  File:		cpccColor.h
 *  Version:	
 *	Purpose:	Portable (cross-platform), light-weight library
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2013 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *				http://www.24hsoftware.com/portable-cpp-filesystem-library
 *				https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */


#pragma once

#define NOMINMAX

#include "cpccTypes.h"
#include <algorithm>
#include <assert.h>
#include <iostream>

#ifdef _WIN32
	#include <windows.h>	// for COLORREF
    typedef COLORREF    TcpccNativeColor;

#endif

#ifdef __APPLE__
	#include <appkit/NSColor.h>	// for NSColor
    typedef NSColor *   TcpccNativeColor;
#endif



//#define applyMinMax(xLow, xHigh, x)		(((x>xHigh)?xHigh:x)<xLow)?xLow:((x>xHigh)?xHigh:x)



// new class
class cpccColor32
{
public:
    union
    {
        cpccDWORD data;

		struct
        {
#ifdef cpccLITTLEENDIAN 
            cpccBYTE a, b, g, r;
#else
			cpccBYTE r: 8, g: 8, b: 8, a: 8;
#endif
        };
    };
    
    cpccColor32(): data(0) { a = 255; }
    
    cpccColor32(unsigned long aHexColor) // do not use a CORORREF as aHexColor. COLORREF is defined as #00BBGGRR.
    {   b = aHexColor & 0xFF; aHexColor = aHexColor >> 8;
        g = aHexColor & 0xFF; aHexColor = aHexColor >> 8;
        r = aHexColor & 0xFF;
        a = 255;
    }
    
	
    cpccColor32(const cpccColor32 &c) { data = c.data; }
	cpccColor32(const unsigned char r_, const unsigned char g_, const unsigned char b_, const unsigned char a_=255 ) :
		r(r_), g(g_), b(b_), a(a_) {}
	   
    
	inline const cpccColor32 & operator =(const cpccColor32& c2)  { data = c2.data;  return(*this); }

    inline const bool operator ==(const cpccColor32& c2)  { return (c2.data == data); }
    inline const bool operator !=(const cpccColor32& c2)  { return (c2.data != data); }
	template <typename T> static const cpccBYTE applyLimits(const T x) 
	{
		if (x > 255) return 255;
		if (x < 0)	 return 0;
		return (cpccBYTE) x;
	}
    
    /*
    TcpccNativeColor toNativeColor(const cpccColor32 &c) const
    {
    #ifdef _WIN32
        return RGB(r, g, b);
    #elif __APPLE__
        return[NSColor colorWithDeviceRed: (r/255.0f) green: (g/255.0f) blue: (b/255.0f) alpha: a/255.0f];
    #endif
        return (TcpccNativeColor) 0;
    }
     */
    
    /*
    void fromNativeColor(const TcpccNativeColor &c)
    {
    #ifdef _WIN32
        r = GetRValue(c); g = GetGValue(c); b = GetBValue(c); a=255;
    #elif __APPLE__
        CGFloat aR, aG, aB, aA;
        [c getRed : &aR green : &aG blue : &aB alpha : &aA];
        r = aR * 255; g = aG * 255; b = aB * 255; a = aA * 255;
    #endif
    }
    */
    
    //     typedef unsigned long DWORD; // 32bit
    
#ifdef _WIN32
	/* definitions from WinGDI.h
	typedef DWORD   COLORREF;
	#define GetRValue(rgb)      (LOBYTE(rgb))
	#define GetGValue(rgb)      (LOBYTE(((WORD)(rgb)) >> 8))
	#define GetBValue(rgb)      (LOBYTE((rgb)>>16))
	#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
	*/
	inline const COLORREF asCOLORREF(void) const		{ return RGB(r, g, b);  }
	inline void fromCOLORREF(const COLORREF c) 	{	r = GetRValue(c); g = GetGValue(c); b = GetBValue(c); }

#endif

	inline cpccDWORD asDWORD(void) const { return data; }
	inline void fromDWORD(const cpccDWORD c) { data = c; }

#ifdef __APPLE__
    
	NSColor *asNSColor(void) const 	{ return[NSColor colorWithDeviceRed : (r/255.0f) green : (g/255.0f) blue : (b/255.0f) alpha : a/255.0f]; }

	void fromNSColor(NSColor *c)
	{
		CGFloat aR, aG, aB, aA;
		[c getRed : &aR green : &aG blue : &aB alpha : &aA];

		r = (cpccBYTE) ( aR * 255);
        g = (cpccBYTE) ( aG * 255);
        b = (cpccBYTE) ( aB * 255);
        a = (cpccBYTE) ( aA * 255);
	}


	CGColorRef asCGColorRef(void)
	{
		// You create a CGColor object by calling the function CGColorCreate, passing a CGColorspace object
		// and an array of floating-point values that specify the intensity values for the color.
		// The last component in the array specifies the alpha value

		return CGColorCreateGenericRGB(r/255.0, g/255.0, b/255.0, a/255.0);
	}


	/// get the color components from CGColorRef and put them in cpccColor
	void fromCGColorRef(CGColorRef c)
	{
		const CGFloat *components = CGColorGetComponents(c);
		int numComponents = (int) CGColorGetNumberOfComponents(c);
		if (numComponents >= 3)
		{
			r = 255 * components[0];
			g = 255 * components[1];
			b = 255 * components[2];
		}

		if (numComponents == 4)
			a = 255 * components[3];
		else
			a = 255;
	}
#endif

	inline const cpccBYTE getBrightness(void) const		{ return (r + g + b) / 3; }

    
    void amplifyComponents(const float xR, const float xG, const float xB)
    {
        r = applyLimits( r*xR );
		g = applyLimits( g*xG );
		b = applyLimits( b*xB );
    }


	cpccColor32 operator *(const float f) const
	{
		cpccColor32 c(*this);
		c.amplifyComponents(f, f, f);
		return c;
	}

    
    cpccColor32& operator *=(const float f)
    {
        amplifyComponents(f,f,f);
        return *this;
    }
    
    cpccColor32 operator +(const cpccColor32& c2)
    {
        cpccColor32 result;
        result.r = r + c2.r;
        result.g = g + c2.g;
        result.b = b + c2.b;
        return result;
    }
    
    static void selfTest(void)
    {
        std::cout << "cpccColor32_t::selfTest() starting\n";
        assert((sizeof(cpccDWORD)==4) && "#7614a cpccColor32_t: expecting data to be 4 bytes long");
        
        cpccColor32 c(0x00223344); // a is set to 255
        
        assert((c.r==0x22) && (c.g==0x33) && (c.b==0x44) &&  "#7614b cpccColor32 contructor problem from hex color");
        
        
        cpccColor32 c1(2,3,4, 255), c2(0x01020304); // a is set to 255
        
        std::cout << "SizeOf(cpccColor32_t)=" << sizeof(cpccColor32) << " sizeof(unsigned char)=" << sizeof(unsigned char) << " c1.data=" << c1.data << " c2.data=" << c2.data << "\n";
        
        assert((c1==c2) && "#7614c cpccColor32 contructor problem from hex color. Maybe a big/litte endian mismatch?");
    }
};


// #define USE_COLOR_V1
#ifdef USE_COLOR_V1

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
		T b, g, r, a;

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

		void amplifyComponents(const float x)
		{
			r = colorChannelTypes<T>::applyLimits(r*x);
			g = colorChannelTypes<T>::applyLimits(g*x);
			b = colorChannelTypes<T>::applyLimits(b*x);
		}

		
		void amplifyComponents(const float xR, const float xG, const float xB)
		{
			r = colorChannelTypes<T>::applyLimits(r*xR);
			g = colorChannelTypes<T>::applyLimits(g*xG);
			b = colorChannelTypes<T>::applyLimits(b*xB);
		}

		
		T getBrightness(void) const
		{
			T brightness = colorChannelTypes<T>::applyLimits(((r + g + b)/3.0f));
			return brightness;
		}

		
	public:		// operators ----------------------------------------	

		cpccColorT<T>& operator=( const cpccColorT<T> &aColor ) 
		{
			r = aColor.r;
			g = aColor.g;
			b = aColor.b;
			a = aColor.a;
			return * this;
		}

		cpccColorT<T>& operator=(const long &aColorAsLong)
		{
			b = aColorAsLong & 0xFF;
			aColorAsLong = aColorAsLong >> 8;
			g = aColorAsLong & 0xFF;
			aColorAsLong = aColorAsLong >> 8;
			r = aColorAsLong & 0xFF;
		}

		
		cpccColorT<T> operator *(const float f)
		{
			cpccColorT<T> tmp( *this );
			tmp.amplifyComponents(f,f,f);
			return tmp;
		}
		
		
		cpccColorT<T>& operator *=(const float f)
		{
			amplifyComponents(f,f,f);
			return *this;
		}


        inline const bool operator ==(const cpccColorT<T>& c2)
		{
			return (r==c2.r) && (g==c2.g) && (b==c2.b);
		}
        
		
        inline const bool operator !=(const cpccColorT<T>& c2)
		{
			return (! (*this==c2));
		}
        
	public:		// selftest ----------------------------------------

		static void selfTest();

	};




// default color class with 8 bit components
class 		cpccColor: public cpccColorT<cpccUint8_t>
{
public:
	cpccColor() : cpccColorT<cpccUint8_t>() { }


	cpccColor(cpccDWORD aColorHex)
	{		// The COLORREF RGB value in Win32 is defined as #00BBGGRR. 
			// so do not pass a COLORREF to this constructor
			b = aColorHex & 0xFF;
			aColorHex = aColorHex >> 8;
			g = aColorHex & 0xFF;
			aColorHex = aColorHex >> 8;
			r = aColorHex & 0xFF;
			a = 255;
	}

	
	cpccColor(cpccUint8_t aRed, cpccUint8_t aGreen, cpccUint8_t aBlue, cpccUint8_t aAlpha=255) : cpccColorT<cpccUint8_t>(aRed, aGreen, aBlue, aAlpha)
	{ }

	// this is a dangerous function. Someone might misunderstand that the floats are in the interval 0..1
	cpccColor(float aRed, float aGreen, float aBlue, float aAlpha=255) : cpccColorT<cpccUint8_t>((cpccUint8_t)aRed, (cpccUint8_t)aGreen, (cpccUint8_t)aBlue, (cpccUint8_t)aAlpha)
	{ }

	cpccColor(int aRed, int aGreen, int aBlue, int aAlpha=255) : cpccColorT<cpccUint8_t>((cpccUint8_t)aRed, (cpccUint8_t)aGreen, (cpccUint8_t)aBlue, (cpccUint8_t)aAlpha)
	{ }
	

#ifdef _WIN32
	const COLORREF asCOLORREF(void) const
	{	return	RGB(r,g,b); // return ((b << 16) | (g << 8) | r);
	}

#endif

#ifdef __APPLE__
    NSColor *asNSColor(void) const
    {
        // return [NSColor colorWithCalibratedRed:(r/255.0f) green:(g/255.0f) blue:(b/255.0f) alpha:1.0f];
        return [NSColor colorWithDeviceRed:(r/255.0f) green:(g/255.0f) blue:(b/255.0f) alpha:1.0f];

    }
    
    
    void fromNSColor(NSColor *c)
    {  	CGFloat aR, aG, aB, aA;
        [c getRed:&aR green:&aG blue:&aB alpha:&aA];
        
		r=aR*255;
		g=aG*255;
		b=aB*255;
		a=aA*255;
    }
	
	/*
	CGColorRef asCGColorRef(void)
	{
		// You create a CGColor object by calling the function CGColorCreate, passing a CGColorspace object 
		// and an array of floating-point values that specify the intensity values for the color. 
		// The last component in the array specifies the alpha value
		
		return CGColorCreateGenericRGB(r/255.0, g/255.0, b/255.0, a/255.0);
	}
	*/
    
    
	/// get the color components from CGColorRef and put them in cpccColor
	void fromCGColorRef(CGColorRef c)
    {  	
		const CGFloat *components = CGColorGetComponents(c);
		int numComponents = CGColorGetNumberOfComponents(c);
		if (numComponents >= 3)
		{	r	= 255*components[0];
			g 	= 255*components[1];
			b	= 255*components[2];
		}	
	
		if (numComponents == 4)
			a= 255*components[3];
			else
			a= 255;
    }
    
#endif


}; 

#else
    typedef 	cpccColor32				cpccColor;
#endif

// typedef 	cpccColorT<float>				cpccColorF;


static const cpccColor	cpccBlack(          0x00, 0x00, 0x00);
static const cpccColor	cpccWhite(          0xFF, 0xFF, 0xFF);
static const cpccColor	cpccGray(           0x80, 0x80, 0x80);
static const cpccColor	cpccBlue(           0x00, 0x00, 0xFF);
static const cpccColor	cpccGreen(          0x00, 0xFF, 0x00);
static const cpccColor	cpccDarkblue(       0x0A, 0x0A, 0x30);
static const cpccColor	cpccRed(            0xFF, 0x00, 0x00);
static const cpccColor	cpccYellow(         0xFF, 0xFF, 0x00);
static const cpccColor	cpccMagenta(        0xFF, 0x00, 0xFF);
static const cpccColor	cpccSilver(         0xC0, 0xC0, 0xC0);
static const cpccColor	cpccMaroon(         0x80, 0x00, 0x00);
static const cpccColor	cpccNavy(           0x00, 0x00, 0x80);
static const cpccColor	cpccCyan(           0x00, 0xFF, 0xFF);
static const cpccColor	cpccMidnightBlue( 	0x19, 0x19, 0x70);
static const cpccColor	cpccDarkBlue(       0x00, 0x00, 0x8B);
static const cpccColor	cpccAntiqueWhite( 	0xFA, 0xEB, 0xD7);
static const cpccColor	cpccLemonChiffon( 	0xFF, 0xFA, 0xCD);
static const cpccColor	cpccMoccasin(       0xFF, 0xE4, 0xB5);
static const cpccColor	cpccSlateGray(      0x70, 0x80, 0x90);
static const cpccColor	cpccBurlyWood(  	0xDE, 0xB8, 0x87);
static const cpccColor	cpccNavajoWhite(  	0xFF, 0xDE, 0xAD);
static const cpccColor	cpccOlive(          0x80, 0x80, 0x00);
static const cpccColor	cpccPurple(         0x80, 0x00, 0x80);
static const cpccColor	cpccSkyBlue(        0x87, 0xCE, 0xEB);
static const cpccColor	cpccSteelBlue(  	0x46, 0x82, 0xB4);
static const cpccColor	cpccSpringGreen(  	0x00, 0xFF, 0x7F);
static const cpccColor	cpccTomato(         0xFF, 0x63, 0x47);
static const cpccColor	cpccPapayaWhip(  	0xFF, 0xEF, 0xD5);
//static const cpccColor	cpccBurlyWood(		0xDE, 0xB8, 0x87);

