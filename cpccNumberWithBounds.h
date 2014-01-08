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

#define cpccNumberWithBounds_DoSelfTest

#include <algorithm>
#include <iostream>
#include <assert.h>



template<typename T, const T &m_min, const T &m_max>
class cpccNumberWithBounds
{
	private: // data
		T m_value ;
	
	public:	// functions
		const T bound(const T a)	{ return std::max(m_min, std::min(m_max, a)); }
		const T& get() 				{ return m_value; }
		const T& getMax() 			{ return m_max; }
		const T& getMin() 			{ return m_min; }
		void  set(const T aValue)	{ 	m_value =  bound(aValue); }
		
	public: // operators
		void operator =(const T a) 		{ set(a); }
		void operator -=(const T a) 	{ set( m_value - a); }
		void operator +=(const T a) 	{ set( m_value + a); }
		void operator *=(const float a) { set( m_value * a); }
		void operator /=(const float a) { if (a!=0.0f) set( m_value / a); }
		bool operator ==(const T a) 	{ return ( m_value == a); }
		// http://stackoverflow.com/questions/356950/c-functors-and-their-uses
		const T& operator()(const T a)	{ return m_value = bound(a); }
		const T& operator()(void) 		{ return m_value; }
		
	#if defined(cpccNumberWithBounds_DoSelfTest)
		static void  selfTest(void);
	#endif
		
};


float fzero(0.0f), fone(1.0f);
typedef cpccNumberWithBounds<float, fzero, fone>		cpccFloat0_1;

unsigned char czero(0), c255(255);
typedef cpccNumberWithBounds<unsigned char, czero, c255>		cpccByte0_255;

/*
float float0_1_deleted(float a)
{
	cpccFloat0_1 v;
	v.set(a);
	return v.get();
}

const float bound0_1f(const float a)
{
	return std::min(0.0f, std::max(1.0f, a));
}
*/

#endif // cpccNumberWithBounds
