﻿/*  *****************************************
 *  File:		cpccWindowBase.h
 *  Version:	
 *	Purpose:	Portable (cross-platform), light-weight library
 *				to draw into native windows (HWND on Windows, NSView on MAC)
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2014 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *				http://www.24hsoftware.com/portable-cpp-filesystem-library
 *				https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */


#pragma once

#include "cpccColor.h"
#include "cpccUnicodeSupport.h"
#include "cpccStackWithDefault.h"
#include "math.cpccRect.h"
#include "gui.cpccCSS.h"
#include "gui.cpccText.h"
#include "gui.cpccDrawingToolsAbstract.h"


/*
 
 ideas from http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4073.pdf
 -----------------------
 The choice of type for each native handle type is left to the implementer.
 Implementers should note that a native handle need not be a one-to-one mapping to an underlying type; 
 it could be a struct that provides access to several native objects, for example, or 
 a struct containing an enum type identifier and a union of the
 different types if several different possible types would make sense.
 Further, implementers need not provide any native handles. 
 If they feel that their users will have no need of them, they can choose not to provide them; 
 this proposal simply allows for their existence and creates a standard typedef name for the type,
 some_class::native_handle_type, and a signature for the member function to retrieve it,
 some_class::native_handle_type
 some_class::native_handle().
 
 The surface type exists as a representation of an object that will be drawn to
 (a “render target” in graphics parlance). This object might be tied to a display surface,
 it might be block of memory with no connection to a display system, or it might even be a file or 
 some sort of output port.
 What defines it from the user’s perspective is that drawing operations are performed up on it.
 
 To analogize to the real world, 
 the surface is like a sheet of paper, 
 a pattern is like a rubber stamp, 
 a pattern used for masking is like a stencil, 
 a stroke operation is like tracing, etc. 
 The context in this analogy is... the person’s hands? That’s a reasonable fit, but a person can switch
 to another piece of paper and start drawing there; our context object could not do that.
 
 Surface VS Context
 So the context type was just a collection of functionality designed to operate on a specific surface,
 to store the necessary state to do so, and in some cases to create that state.
 It makes more sense to move those drawing operations and that state data to the surface type itself, 
 since it will ultimately need to be applied there anyway.
 The state data that a context created, objects of type path, 
 are now immutable objects that are generated by a factory class.
 For these reasons, the context type was eliminated and its functionality was merged into the surface type.
 
 */


#if defined(__APPLE__)
	#include <Cocoa/Cocoa.h>
	// Window handle is NSWindow (void*) on Mac OS X - Cocoa
	//typedef NSWindow* NativeWindowHandle;
	typedef NSView* cpccNativeWindowHandle;
	typedef NSView* cpccNativeWindowDrawHandle;
	//typedef void* cpccNativeWindowHandle;
	
	// CGRect
	//NSRect structure, defined in NSGeometry.h. 
	// typedef NSRect  NativeRect;
    typedef NSImage* cpccNativeDrawSurfaceHandle;


#elif defined(_WIN32)
	#include <Windows.h>
	// Window handle is HWND (HWND__*) on Windows
	typedef HWND    cpccNativeWindowHandle;
	typedef HDC		cpccNativeWindowDrawHandle;
    typedef HDC     cpccNativeDrawSurfaceHandle;

	// typedef RECT NativeRect;
#else
	#error Unknown platform for cpccNativeWindowHandle and cpccNativeDrawSurfaceHandle
#endif



/**
	Do your window drawing against this abstract class.
	Construct somewhere the cpccWindow which is a descendant and 
	creates automatically the Win or OSX version of the class.
	To draw image, use the cpccImage's draw function that takes as 
	a parameter the window to draw to.
*/
class cpccWindowBase
{
protected:
	cpccNativeWindowHandle				m_windowHandle;
	bool								m_useDblBuffer;

public:		// data

	// drawing parameters
	cpccStackWithDefault<cpccColor> 	bgColor,
										drawColor;
	
	// text (font+paragraph) parameters:
	cpccStackWithDefault<cpcc_string>	fontName;
	cpccStackWithDefault<int>			fontSize;
	cpccStackWithDefault<eTextAlign>    textAlign;
	cpccStackWithDefault<float>			kerning;
	cpccStackWithDefault<eFontQuality>	fontQuality;
	cpccStackWithDefault<eFontWeight>	fontWeight;

	
public:  // constructor
	
	cpccWindowBase(const cpccNativeWindowHandle aWnd): 
		m_windowHandle(aWnd),
		m_useDblBuffer(true),
		bgColor(cpccPapayaWhip),
		drawColor(cpccYellow), 
		fontName(_T("Arial")), 
		fontSize(12),
		textAlign(taLeft),	// left
		kerning(0.0f),
		fontQuality(fqAntiAliased),
		fontWeight(fwNormal)
	{
		
	}

    virtual ~cpccWindowBase() { }
    
public:		// concrete functions

	cpccNativeWindowHandle 	getNativeWindowHandle(void) { return m_windowHandle; }

public:  // abstract functions

    virtual void 			flush(void)=0;
    virtual void			clear(void)=0;
    virtual void			fillWithColor(const cpccColor& c)=0;
	virtual void			fillRectWithColor(const cpccRecti &r, const cpccColor& c)=0;
    virtual void			fillEllipseWithColor(const int left, const int top, const int right, const int bottom, const cpccColor& c)=0;
	virtual void			fillCircleWithColor(const int centerX, const int centerY, const int r, const cpccColor& c) { fillEllipseWithColor(centerX - r, centerY - r, centerX + r, centerY + r, c); };
	virtual void 			drawText(int x, int y, const cpcc_char *text, cpccCSS *aCssPtr)=0;
	virtual void 			drawLine(const int x1, const int y1, const int x2, const int y2, const int width, const cpccColor &c) = 0;
    virtual void			pushCss(cpccCSS *aCssPtr)=0;
	virtual void			popCss(cpccCSS* aCssPtr)=0;
    virtual cpccColor		getPixel(const int x, const int y)=0;
    virtual void 			setPixel(int x, int y, const cpccColor &c)=0;
    virtual int				getWidth(void)	const =0;
    virtual int             getHeight(void)	const =0;
	virtual int             getTop(void)	const =0;
    virtual int             getLeft(void)	const =0;

	virtual int             getBottom(void) const { return getTop() + getHeight(); };
	virtual int             getRight(void)	const { return getLeft() + getWidth(); };;
    virtual void            getTextSize(const cpcc_char *txt, int *width, int *height)=0;
	virtual void			useDblBuffer(const bool a)	{ m_useDblBuffer = a; }
};

