/*  *****************************************
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
	

public:		// data

	// drawing parameters
	cpccStackWithDefault<cpccColor> 	bgColor,
										drawColor;
	
	// text (font+paragraph) parameters:
	cpccStackWithDefault<cpcc_string>	fontName;
	cpccStackWithDefault<int>			fontSize,
                                        textAlign;
	cpccStackWithDefault<float>			kerning;
	cpccStackWithDefault<eFontQuality>	fontQuality;
	cpccStackWithDefault<eFontWeight>	fontWeight;

	
public:  // constructor
	
	cpccWindowBase(const cpccNativeWindowHandle aWnd): 
		m_windowHandle(aWnd),
		bgColor(cpccGray), 
		drawColor(cpccYellow), 
		fontName(_T("Arial")), 
		fontSize(12),
		textAlign(-1),	// left
		kerning(0.0f),
		fontQuality(fqAntiAliased),
		fontWeight(fwNormal)
	{
		
	}

    virtual ~cpccWindowBase() { }
    
public:		// concrete functions

	cpccNativeWindowHandle 	getNativeWindowHandle(void) { return m_windowHandle; }

public:  // abstract functions

    virtual void 					flush(void)=0;
    virtual void					clear(void)=0;
    virtual void					fillWithColor(const cpccColor& c)=0;
	virtual void					fillRectWithColor(const cpccRecti &r, const cpccColor& c)=0;
    virtual void 					textOut(int x, int y, const cpcc_char *text, cpccCSS *aCssPtr)=0;
    virtual void					pushCss(cpccCSS *aCssPtr)=0;
	virtual void					popCss(cpccCSS* aCssPtr)=0;
    virtual cpccColor				getPixel(const int x, const int y)=0;
    virtual void 					setPixel(int x, int y, const cpccColor &c)=0;
    virtual int                     getWidth(void)=0;
    virtual int                     getHeight(void)=0;
    virtual void                    getTextSize(const cpcc_char *txt, int *width, int *height)=0;
   
};

