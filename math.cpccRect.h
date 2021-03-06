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
#elif defined(__APPLE__)
    #include <cocoa/cocoa.h>
#endif


#include "data.cpccWideCharSupport.h"


/**
 Rectangle C++ class with lots of convenience functions
 Depends on cpccVector
 Constructors from RECT (WINDOWS) and NSRect (OS X)
 Convert to RECT and NSRect
 */
template<typename T>
class cpccRect
{
public:		// data
	T	top, left, width, height;

public:		// constructors
	cpccRect() : 
			// x(left), y(top), 
			top(static_cast<T>(0)), left(static_cast<T>(0)), width(static_cast<T>(0)), height(static_cast<T>(0))
	{}

	cpccRect(const T &l, const T &t, const T &r, const T &b) : 
			// x(left), y(top), 
			top(t), left(l), width(r-l), height(b-t)
	{}

#ifdef _WIN32
	explicit cpccRect(const RECT &r):	// construct from a Windows RECT
		// x(left), y(top),
		left(r.left), top(r.top), width(r.right - r.left), height(r.bottom - r.top)
	{ }
#endif
    
    virtual ~cpccRect() {}

public:		// operators
	inline const bool operator==(const cpccRect<T>& aRect)const { return (top==aRect.top) && (left==aRect.left) && (width==aRect.width) && (height==aRect.height);   }
	inline cpccRect<T>& operator=(const cpccRect<T>& aRect)			{ top=aRect.top;  left=aRect.left; width=aRect.width;  height=aRect.height; return (*this); };
	
	template<typename T2>
	inline cpccRect<T>& operator=(const cpccRect<T2>& aRect)			{ top = (T)aRect.top;  left = (T)aRect.left; width = (T)aRect.width;  height = (T)aRect.height; return (*this); };

    template<typename Tnumber>
    inline cpccRect<T> operator* (const Tnumber f)
    {   cpccRect<T> result(*this);
        result.top = f * top;  result.left = f * left; result. width = f * width;  result.height = f * height;
        return result;
    };

    
public:		// functions
	inline cpccRect<T> &fromTLBR(const T &t, const T &l, const T &b, const T &r) { return fromXYWH(l, t, r - l, b - t); }

	inline cpccRect<T> &fromXYWH(const T &aX, const T &aY, const T &aW, const T &aH)
	{	top = aY;	left = aX; 	height = aH;	 width	= aW;
		return *this;
	}


#ifdef _WIN32
	const RECT asRECT() const
	{	RECT r;
		r.left		= left;
		r.top		= top;
		r.right		= getRight();
		r.bottom	= getBottom();
		return r;
	}

	inline cpccRect<T> &fromRECT(const RECT &r)  {	return fromTLBR( r.top, r.left, r.bottom, r.right); }
#endif

#ifdef __APPLE__
    const NSRect asNSRect() const  { return NSMakeRect(left, top, width, height); }
#endif
    
public:		// convenience functions
	inline virtual const T		getBottom(void) const	{ return top + height; }
	inline virtual const T		getRight(void)	const	{ return left + width; }
	// inline virtual const void	setBottom(const T v)	{ height = v - top; }
	// inline virtual const void	setRight(const T v)		{ width = v - left; }
	inline void			setXY(const T aX, const T aY) { left = aX; top = aY; }

	void moveBy(const T dX, const T dY) { left += dX; top += dY; }

	inline const bool   overlapsWithRect(const cpccRect<T> &aRect) const
	{
		if (getBottom() < aRect.top) return false;
		if (getRight() < aRect.left) return false;
		if (top > aRect.getBottom()) return false;
		if (left > aRect.getRight()) return false;
		return true;
	}


	inline const bool   isContainedInRect(const cpccRect<T> &aContainerRect) const
	{
		if (left < aContainerRect.left) return false;
		if (top < aContainerRect.top) return false;
		if (getBottom() > aContainerRect.getBottom()) return false;
		if (getRight() > aContainerRect.getRight()) return false;
		return true;
	}

	std::basic_string<TCHAR> asString(void) const
	{
		std::basic_string<TCHAR> result(_T("Top:"));
		result += TOSTRING_WN(top);
		result += _T(", Left:") + TOSTRING_WN(left);
		result += _T(", Bottom:") + TOSTRING_WN(getBottom());
		result += _T(", Right:") + TOSTRING_WN(getRight());
		result += _T(", Width:") + TOSTRING_WN(width);
		result += _T(", Height:") + TOSTRING_WN(height);

		return result;
	}
};


typedef	cpccRect<int>	cpccRecti;
typedef	cpccRect<float>	cpccRectf;

static const cpccRecti	cpccRectZero;

// //////////////////////////////////
//
//  cpccRect implementation
//
// //////////////////////////////////




/* old 
template<typename T>
class cpccRect
{
public:		// data
	cpccVector<T, 2>	topLeft,
		size;

public:		// constructors
	cpccRect() { }

	cpccRect(const T &l, const T &t, const T &r, const T &b) : topLeft(l, t), size(r - l, b - t) { }

#ifdef _WIN32
	cpccRect(const RECT &r) :	// construct from a Windows RECT
		topLeft.data[0](r.left),
		topLeft.data[1](r.top),
		size.data[0](r.right - r.left),
		size.data[1](r.bottom - r.top)
	{ }
#endif

public:		// operators
	inline const bool operator==(const cpccRect<T>& aRect)const  	{ return (topLeft == aRect.topLeft) && (size == aRect.size); }
	inline cpccRect<T>& operator=(const cpccRect<T>& aRect)			{ topLeft = aRect.topLeft;  size = aRect.size; return (*this); };
	//template<typename T2>
	//inline cpccRect<T> operator=(const cpccRect<T2>& aRect)			{ topLeft= aRect.topLeft;  size=aRect.size; return (*this); };


public:		// functions
	inline cpccRect<T> &fromTLBR(const T &t, const T &l, const T &b, const T &r) { return fromXYWH(l, t, r - l, b - t); }

	inline cpccRect<T> &fromXYWH(const T &x, const T &y, const T &w, const T &h)
	{
		topLeft.y() = y;	topLeft.x() = x;
		size.y() = h;	size.x() = w;
		return *this;
	}


#ifdef _WIN32
	const RECT asRECT() const
	{
		RECT r;
		r.left = topLeft.getX();
		r.top = topLeft.getY();
		r.right = r.left + size.getX();
		r.bottom = r.top + size.getY();
		return r;
	}

	inline cpccRect<T> &fromRECT(const RECT &r)  { return fromTLBR(r.top, r.left, r.bottom, r.right); }
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

*/
