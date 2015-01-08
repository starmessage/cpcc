/*  *****************************************
 *  File:		math.cpccRect.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight, multidimentional number typedefs
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


#pragma once

#ifdef _WIN32
	#include <Windows.h>
#endif

#include "math.cpccVectorTypes.h"



/**
 Rectangle C++ class with lots of convenience functions
 Depends on cpccVector
 Constructors from RECT (WINDOWS) and NSRect (SOX)
 Convert to RECT and NSRect
 */
template<typename T>
class cpccRect
{
public:		// data
	cpccVector<T, 2>	topLeft,
						size;

public:		// constructors
	cpccRect() { }

	cpccRect(const T &l, const T &t, const T &r, const T &b): topLeft(l,t), size(r-l, b-t) { }

#ifdef _WIN32
	cpccRect(const RECT &r):	// construct from a Windows RECT
		topLeft.data[0](r.left),
		topLeft.data[1](r.top),
		size.data[0](r.right - r.left),
		size.data[1](r.bottom - r.top)
		{ }
#endif

public:		// operators
	inline const bool operator==(const cpccRect<T>& aRect)const  	{ return (topLeft==aRect.topLeft) && (size==aRect.size);   }
	inline cpccRect<T>& operator=(const cpccRect<T>& aRect)			{ topLeft=aRect.topLeft;  size=aRect.size; return (*this); };
	//template<typename T2>
	//inline cpccRect<T> operator=(const cpccRect<T2>& aRect)			{ topLeft= aRect.topLeft;  size=aRect.size; return (*this); };
	
   
public:		// functions
	inline cpccRect<T> &fromTLBR(const T &t, const T &l, const T &b, const T &r) { return fromXYWH(l, t, r - l, b - t); }

	inline cpccRect<T> &fromXYWH(const T &x, const T &y, const T &w, const T &h)
	{	topLeft.y() = y;	topLeft.x() = x;
		size.y()	= h;	size.x()	= w;
		return *this;
	}


#ifdef _WIN32
	const RECT asRECT() const
	{	RECT r;
		r.left		= topLeft.getX();
		r.top		= topLeft.getY();
		r.right		= r.left + size.getX();
		r.bottom	= r.top + size.getY();
		return r;
	}

	inline cpccRect<T> &fromRECT(const RECT &r)  {	return fromTLBR( r.top, r.left, r.bottom, r.right); }
#endif

#ifdef __APPLE__
    const NSRect asNSRect() const  { return NSMakeRect(topLeft.getX(), topLeft.getY(), size.getX(), size.getY()); }
#endif
    
public:		// convenience functions

	inline const T getRight(void) 	const { return topLeft.getX() + size.getX(); }
	inline const T getBottom(void) 	const { return topLeft.getY() + size.getY(); }
	inline const T getTop(void) 	const { return topLeft.getY(); }
	inline const T getLeft(void) 	const { return topLeft.getX(); }

	inline T&	x(void) 		{ return topLeft.x(); };
	inline T&	y(void) 		{ return topLeft.y(); };
	inline T&	width(void) 	{ return size.x(); };
	inline T&	height(void) 	{ return size.y(); };
	
};


typedef	cpccRect<int>	cpccRecti;
typedef	cpccRect<float>	cpccRectf;

static const cpccRecti	cpccRectZero;

