/*  *****************************************
 *  File:		math.cpccVector.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight, multidimentional number
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2014 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license exists for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://code.google.com/p/cpcc/
 *              https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */


#pragma once

#include <math.h>
#include <assert.h>
#include <iostream>

#include "math.cpccRandom.h"
#include "cpccTesting.h"

 
// This class-template has 2 arguments <T,Number>.
template<typename T, int DIM>
class cpccVector
{
	
	// Any other type of cpccVector is a friend and can access the data[] array
	template<typename U, int D> friend class cpccVector;

protected: // data
	T data[DIM];

public: // Constructors and destructors
	cpccVector()
	{	zero();	// clear all data
	}
	

	cpccVector(const T &a, const T &b)
	{ 
		assert(DIM>=2 &&  "#4391: cpccVector: DIM < 2" );	
		data[0] = a; data[1] = b;
		zero(2); // zero the rest
	}
	
	cpccVector(const T &a, const T &b, const T &c)  
	{ 
		assert(DIM>=3 &&  "#4391: cpccVector: DIM < 3" );
		data[0] = a; data[1] = b; data[2] = c;
		zero(3); // zero the rest
	}
	
	
	cpccVector(const cpccVector<T, DIM>& a) {  memcpy(data, a.data, sizeof(data[0])* DIM);	}

	
	template<typename T2>
	cpccVector(const cpccVector<T2, DIM>& a)
	{	for (int i=0; i<DIM; i++)
			data[i] = (T) a.data[i];
	}
	
public: 	// functions
	inline void zero(const unsigned int from=0)
	{	for (unsigned int i=from; i<DIM; ++i)
			data[i] = 0;
	}


public: 	// member aliases

	// get + set functions
	inline T& x() { return data[0]; }
	inline T& y() { assert(DIM>=2 &&  "#4397: cpccVector: DIM < 2" ); return data[1]; }
	inline T& z() { assert(DIM>=3 &&  "#4398: cpccVector: DIM < 3" ); return data[2]; }
	
	// const versions for getting the values
	inline const T getX() const { return data[0]; }
	inline const T getY() const { assert(DIM>=2 &&  "#4397: cpccVector: DIM < 2" ); return data[1]; }
	inline const T getZ() const { assert(DIM>=3 &&  "#4398: cpccVector: DIM < 3" ); return data[2]; }
	
public: // Overloaded operators

	/*
	// reference by index
	T& operator [] ( const int idx ) 
	{
		if ( 0 <= idx && idx < DIM )
			return data[ idx ];
		
		assert( "#cpccVector: index out of range." );	
		return data[0];
	}
	*/
	
	
    inline const bool operator==(const cpccVector<T, DIM>& aVector)const
	{
		for (int i=0; i<DIM; i++)
			if (data[i] != aVector.data[i])
				return false;
		return true;
    }

	
    inline const bool operator!=(const cpccVector<T, DIM>& aVector)const
	{
		return (! (*this==aVector));
	}
	

 	template<typename T2>
    inline cpccVector<T, DIM>&         operator*=(const T2& Scalar)
	{
		for (int i=0; i<DIM; i++)
			data[i] = (T) (Scalar * data[i]);
			
		return (*this);
	}
	
	

    inline cpccVector<T, DIM>&         operator/=(const float & Scalar)
	{
		if  (Scalar == 0)
			{
			assert(false && "#2841: cpccVector Div by zero.");
			return (*this);
			}
			 
		for (int i=0; i<DIM; i++)
			 data[i] = (T) (data[i] / Scalar);

		return (*this);
	}
		
	
	template<typename T2>
	inline cpccVector<T, DIM>&         operator+=(const cpccVector<T2, DIM>& aNumber)
	{
		for (int i=0; i<DIM; i++)
			data[i] += (T) aNumber.data[i];
			
		return (*this);
	}
	

	template<typename T2>
	inline cpccVector<T, DIM>&          operator-=(const cpccVector<T2, DIM>& aNumber)
	{
		for (int i=0; i<DIM; i++)
			data[i] -= (T) aNumber.data[i];
			
		return (*this);
	}


    template<typename T2> 
	inline cpccVector<T, DIM>&          operator=(const cpccVector<T2, DIM>& other)
    {
        for (int i=0; i<DIM; i++)
			data[i] = (T) other.data[i];
        return (*this);
    }
    
    
	template<typename T2>
	inline const cpccVector<T, DIM>    operator+(const cpccVector<T2, DIM>& aNumber)const
	{
		cpccVector<T, DIM> result(*this);
		result += aNumber;
		return result;
	}
	
	
	template<typename T2> 
    inline const cpccVector<T, DIM>    operator-(const cpccVector<T2, DIM>& aNumber)const
	{
		cpccVector<T, DIM> result(*this);
		result -= aNumber;
		return result;
	}

	
	template<typename T2>
	inline const cpccVector<T, DIM> operator*(const T2 Scalar)const
	{
		cpccVector<T, DIM> result(*this);
		for (int i=0; i<DIM; i++)
			result.data[i] = (T) (Scalar * result.data[i]);
		return result;
	}

	
	inline const cpccVector<T, DIM> operator/(const float & Scalar)const
	{
		assert(Scalar !=0 && "#9652 division by zero in cpccVector <T, DIM> operator/()");
		cpccVector<T, DIM> result(*this);
		return result *= 1/Scalar;
	}
		
public: // Public functions

	 
	inline const int getDim(void) { return DIM; }
	
	
    inline const bool isZero(void)
	{ 
		for (int i=0; i<DIM; i++)
			if ( data[i]!= 0 )
				return false;
		return true;
	}
	
	
    inline const T  getMagnitudeSquared(void)const
	{
		T result=(T)0;
		for (int i=0; i<DIM; ++i)
			result += data[i]*data[i];
		return result;
	}
	
	
	inline const double   getMagnitude(void)const 
	{
		return sqrt(getMagnitudeSquared()); 
	}

	
	void	setMagnitude(const float &mag)
	{	
		if (this->isZero()) 
				x()=mag;
			else
				this *= mag/(this->getMagnitude());
	}
	
	
	inline void    normalize(void)
	{
		setMagnitude(1.0f);
	}


	inline const T distanceSquaredFrom(const cpccVector<T, DIM> &Q)      // squared distance (more fast than the normal distance)
	{
		T result=(T)0;
		for (int i=0; i<DIM; ++i)
			{ T tmp=data[i]-Q.data[i]; result += tmp*tmp; }
		return result;
	}
	
	inline const double distanceFrom(const cpccVector<T, DIM> &Q)     { return sqrt(distanceSquaredFrom(Q)); }


	inline void randomizeBetween(const cpccVector<T, DIM> &A, const cpccVector<T, DIM> &B)      
	{
		for (int i=0; i<DIM; ++i)
			data[i] = cpccRandom::random(A.data[i], B.data[i]);
	}


	const double getAzimuthalAngle_inRadians(void) const	{ return (this->isZero()) ? 0.0 : atan2(y() , x()); }
	const double getAzimuthalAngle_inDegrees(void) const	{ return (180.0*getAzimuthalAngle_inRadians()/3.141592653589793238); }
	
	void	setAzimuthalAngle_inRadians(const float angleInRad)
	{	
		if (this->isZero())
			return;	
		double length = getMagnitude();
		x() = length * cos(angleInRad);
		y() = length * sin(angleInRad);
	}


	static void selfTest(void)
	{
		

	}
 
 };		// end of class cpccVector


 // /////////////////////////////////////////////////////////////////////////////////////////////////
//
//		class cpccKeyValue testing
//
// /////////////////////////////////////////////////////////////////////////////////////////////////


TEST_RUN(cpccVector_test, cpccTesting::sharedObjects::outputStream())
{
    const bool skipThisTest = false;

    if (skipThisTest)
    {
        TEST_ADDNOTE("Test skipped");
        return;
    }

    // cpccVector<int, 3>::selfTest();
    cpccVector<int, 3> a3d;

    TEST_EXPECT(a3d.isZero(), _T("#4231a: cpccVector::selfTest isZero"));

    a3d.x() = 10;

    TEST_EXPECT(!a3d.isZero(), _T("#4231b: cpccVector::selfTest isZero"));

    a3d.y() = 15;

    cpccVector<int, 3> b3d(20, 30, 0);
    TEST_EXPECT(a3d != b3d, _T("#4231c: cpccVector::selfTest equal"));

    a3d *= 2;
    TEST_EXPECT(a3d == b3d, _T("#4231d: cpccVector::selfTest not equal"));

    cpccVector<int, 3> c3d(a3d);
    TEST_EXPECT(a3d == c3d, _T("#4231e: cpccVector::selfTest not equal"));

    c3d.z() = 4;
    TEST_EXPECT(a3d != c3d, _T("#4231f: cpccVector::selfTest equal"));

    a3d.x() = 3; a3d.y() = 4; a3d.z() = 0;
    TEST_EXPECT(a3d.getMagnitude() == 5, _T("#4231g: cpccVector::selfTest magnitude problem"));

    a3d.x() = 4; a3d.y() = 4; a3d.z() = 2;
    TEST_EXPECT(a3d.getMagnitude() == 6, _T("#4231f: cpccVector::selfTest magnitude problem"));

    b3d = a3d;
    TEST_EXPECT(a3d.distanceFrom(b3d) == 0, _T("#4231h: cpccVector::selfTest distanceFrom() problem"));

    c3d = cpccVector<int, 3>(5, 6, 7);
    b3d -= c3d;
    TEST_EXPECT(a3d.distanceSquaredFrom(b3d) == 110, _T("#4231j: cpccVector::selfTest distanceSquaredFrom() problem"));

    c3d = cpccVector<int, 3>(9, 6, 3);
    a3d = c3d / 3.0f;
    TEST_EXPECT((a3d.x() == 3 && a3d.y() == 2 && a3d.z() == 1), _T("#4231k: cpccVector::selfTest division problem"));

}


