/*  *****************************************
 *  File:		math.cpccFixedPointNumber.h
 *	Purpose:	Portable (cross-platform), light-weight, class replacement for floats. 
 *				Fast real numbers.
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2014 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *  Download:	https://code.google.com/p/cpcc/
 *              https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#include "cpcc_selfTest.h"

#define real2integer(Xreal)     (Xreal * (1<<fBits) +0.5f )

/**
    Math:
	For -2.3, the integer and fractional parts are -3 and .7
 */
/// Fast float class. It uses internally integer arithmetics to speed up simple (+, -, *, /) calculations.
/// It can directly replace float and double variables.
/// T is the internal integer type to use.
/// fBits is the number of bits for the fractional part.
template <typename T, int fBits>
class cpccFastFloat
{
private:
	T m_value;
	inline const float getReal(void) const { return (float) m_value / (1 << fBits); }
    
public: // constructors
    cpccFastFloat(): m_value(0)  { }
    
    cpccFastFloat(const float a): m_value((T) real2integer(a))  { }
    cpccFastFloat(const long a): m_value(a << fBits)  { }
    cpccFastFloat(const int a): m_value(a << fBits)  { }
    cpccFastFloat(const cpccFastFloat<T, fBits>& a): m_value(a.m_value)  { }
    
    
public: // "copy" operators
    
    inline cpccFastFloat<T, fBits>& operator= (const cpccFastFloat<T, fBits>& a)
    {   m_value = a.m_value; return *this; }
    
    
    inline cpccFastFloat<T, fBits>& operator= (const float a)
    {   m_value = (T) real2integer(a); return *this; }
    
    
    inline cpccFastFloat<T, fBits>& operator= (const long a)
    {   m_value = (T) (a << fBits); return *this; }
    
    
    inline cpccFastFloat<T, fBits>& operator= (const int a)
    {   m_value = (T) (a << fBits); return *this; }
    
    
public: // "convert to" operators
    
    /// convert to float
    inline operator float(void) const   { return getReal(); }
    
 
public: // comparison operators
    
    inline const bool operator == (const cpccFastFloat<T, fBits>& a) const   { return m_value ==  a.m_value; }
    inline const bool operator == (const float a) const                             { return getReal() ==  a; }
    inline const bool operator  < (const cpccFastFloat<T, fBits>& a) const   { return m_value <   a.m_value;  }
    inline const bool operator  < (const float a) const								{ return getReal() <   a;  }
	inline const bool practicallyEqual(const float a)								{ return abs(m_value - (T) real2integer(a)) <= ((m_value + (T) real2integer(a))>> (fBits+3)); }
    inline const T	  getPracticallyEqualThresshold(const float a)					{ return ((m_value + (T) real2integer(a))>> (fBits+3)); }
    
	// the rest comparison operators will be automatically implemented by the helper class
    
    
public: // arithmetic operators
    
    inline cpccFastFloat<T, fBits>& operator += (const cpccFastFloat<T, fBits>& a)    {  m_value += a.m_value; return *this; }
    inline cpccFastFloat<T, fBits>& operator -= (const cpccFastFloat<T, fBits>& a)    {  m_value -= a.m_value; return *this; }
    inline cpccFastFloat<T, fBits>& operator *= (const cpccFastFloat<T, fBits>& a)
		{  m_value = (T) ((m_value * a.m_value) >> fBits); return *this; }
    inline cpccFastFloat<T, fBits>& operator /= (const cpccFastFloat<T, fBits>& a)
		{  m_value = (T) ((m_value << fBits ) / a.m_value ); return *this; }

    // the rest comparison operators will be automatically implemented by the helper class

public:  // other functions
    
    static const float getMinPossibleValue(void)
    {   cpccFastFloat<T, fBits> a;
        a.m_value = 1;
        return a;
    }
    
    static void selfTest(void)
    {
        float b = 3.14f, bb=5.1f;
        cpccFastFloat<T, fBits> a(3.14f), aa; // for 9 fractional digits, internal rep should be 1607,68 rounded to 1608
        aa = 5.1f;

		float minval = cpccFastFloat<T, fBits>::getMinPossibleValue();
        consolePut( "cpccFixedPointNumber. a:" << a << " b:" << b << " getMinimum:" << minval << " internal rep:" << a.m_value );
        
		assert(a.practicallyEqual(3.14f) && "#8515a: cpccFixedPointNumber::selfTest()");
		assert(a.practicallyEqual(b) && "#8515b: cpccFixedPointNumber::selfTest()");

		a = 2*a - 0.2f;
        b = 2*b - 0.2f;
        
		// float tmp=a;
		consolePut( "cpccFixedPointNumber of 6.08 =" << a );
		assert(a.practicallyEqual(b) && "#8515c: cpccFixedPointNumber::selfTest()");

		consolePut("cpccFixedPointNumber of 6.08 =" << a );
        assert(aa.practicallyEqual(bb) && "#8515d: cpccFixedPointNumber::selfTest()");
        a = a *aa;
        b = b *bb;
        
		consolePut( "cpccFixedPointNumber a=" << a << " a.val=" << a.m_value << " b=" << b );
		consolePut( "getPracticallyEqualThresshold(a)=" << a.getPracticallyEqualThresshold(b) );
        assert(a.practicallyEqual(b) && "#8515e: cpccFixedPointNumber::selfTest()");
    }
    
    
};


typedef cpccFastFloat<long, 13>    cpccFastFloat13; // 13 bits fractional part


