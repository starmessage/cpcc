/*  *****************************************
 *  File:		cpccNumberWithBounds.h
  *	Purpose:	Portable (cross-platform), light-weight, library
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

#ifndef cpccNumberWithBounds_h
#define cpccNumberWithBounds_h
#define NOMINMAX



#include <algorithm>
#include <iostream>

#include "cpccTesting.h"


/*
	The current C++ standard does not allow float (i.e. real number) or 
	character string literals to be used as template non-type parameters.
	This is why I cannot have T m_min as a template parameter
*/
// Class to represent numbers that have specific numeric ranges. 
// E.g. a color component can vary from 0 to 1.
// The template parameters define the max and min limits for the number.
// cpccNumberWithBounds<unsigned char, 0, 255, 1> a;
// declares a char variable that can take values from 0 to 255
// cpccNumberWithBounds<float, 0, 5, 2> a;
// declares a float variable that can take values from 0 to 2.5
// The denominator is needed as a trick because float values cannot be used as template parameters
// Therefore, to pass 5.1 as a maximum value put m_max=51, m_denominator=10
template<typename T, const int m_min, const int m_max, const int m_denominator>
class cpccNumberWithBounds
{
	private: // data
		T m_value ;
		const T set(const T a)		{ return m_value =  applyLimits(a); }

	public:	// constructor
        cpccNumberWithBounds<T, m_min, m_max, m_denominator>(const T a=0) noexcept: m_value(a) { /* set(a); */ }

	public:	// functions
		static const T applyLimits(const T a)	
		{	if (a<m_min/m_denominator)
				return m_min/m_denominator;

			if (a>m_max/m_denominator)
				return m_max/m_denominator;
			return a;
		}

		static const T getMax() 	{ return m_max/m_denominator; }
		static const T getMin()  	{ return m_min/m_denominator; }
        //const T get()				{ return m_value; }

	public: // operators
		void operator =(const T a) 		{ set(a); }
		void operator -=(const T a) 	{ set( m_value - a); }
		void operator +=(const T a) 	{ set( m_value + a); }
		void operator *=(const float a) { set( m_value * a); }
		void operator /=(const float a) { if (a!=0.0f) set( m_value / a); }
		bool operator ==(const T a) 	{ return ( m_value == a); }
		// http://stackoverflow.com/questions/356950/c-functors-and-their-uses
		const T operator()(const T a)	{ return set(a); }
		const T operator()(void) 		{ return m_value; }
		inline operator T(void) const   { return m_value; } // convert to T

		
};

typedef cpccNumberWithBounds<float, 0, 1, 1>			cpccFloat0_1;
typedef cpccNumberWithBounds<unsigned char, 0, 255, 1>	cpccByte0_255;


// /////////////////////////////////////////////////////////////////////////////////////////////////
//
//		class cpccNumberWithBounds testing
//
// /////////////////////////////////////////////////////////////////////////////////////////////////


TEST_RUN(cpccNumberWithBounds_test, cpccTesting::singleton::getOutput())
{
    const bool skipThisTest = false;

    if (skipThisTest)
    {
        TEST_ADDNOTE("Test skipped");
        return;
    }

    cpccNumberWithBounds<float, 0, 1, 1>		f;
    f = 2.0f;
    // std::cout << "f:" << f();

    TEST_EXPECT((f == 1.0f) , "#9621a1: cpccNumberWithBounds");

    f = 1.33f;
    TEST_EXPECT((f() == 1.0f) , "#9621a2: cpccNumberWithBounds");
    TEST_EXPECT((f == 1.0f) , "#9621a3: cpccNumberWithBounds");

    float tmpFloat = 1.01f;
    f = tmpFloat;
    TEST_EXPECT((f() == 1.0f) , "#9621a4: cpccNumberWithBounds");


    f = 0.5f;
    TEST_EXPECT(f == 0.5f , "#9621b: cpccNumberWithBounds");
    f -= 1.0f;
    TEST_EXPECT(f == 0.0f , "#9621c: cpccNumberWithBounds");

    f = 0.1f;
    f *= 2.0f;

    float newf = f();
    TEST_EXPECT(newf == 0.2f , "#9621d: cpccNumberWithBounds");
}


#endif // cpccNumberWithBounds
