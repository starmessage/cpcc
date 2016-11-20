
/*  *****************************************
 *  File:		gui.cpccDrawingToolsAbstract.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight, graphic context drawing tools
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2013 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *  Download:	https://code.google.com/p/cpcc/
 *              https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */
 
 
#pragma once


#include "cpccColor.h"
#include "cpccUnicodeSupport.h"
#include "gui.cpccText.h"




/*
	the graphics context can be
	
	1. windows: 
	HDC
	
	
	2. mac: 
    https://developer.apple.com/library/mac/documentation/cocoa/Conceptual/CocoaDrawingGuide/DrawingEnviron/DrawingEnviron.html#//apple_ref/doc/uid/TP40003290-CH202-BBCJDGHJ
    In Cocoa, graphics contexts are represented by the NSGraphicsContext class and are used to 
    represent the following drawing destinations:
    - Windows (and their views)
    - Images (including bitmaps of all kinds)
    - Printers
    - Files (PDF, EPS)
    - OpenGL surfaces
    You can also create graphics contexts explicitly to handle drawing in special situations. 
    For example, one way to create a bitmap image is to create the bitmap canvas and then 
    create a graphics context that draws directly to that canvas.
 
    Getting a Quartz Graphics Context
    Before using any Quartz features, you need to obtain a Quartz graphics context (CGContextRef) for drawing. 
    For view-based drawing, you can get the context by sending a graphicsPort message to the current Cocoa 
    graphics context (NSGraphicsContext). This method returns a pointer that you can cast to a 
    CGContextRef data type and use in subsequent Quartz function calls.
 
 
	CGContextRef (coreGraphics context) / Quartz
	The CGContextRef opaque type represents a Quartz 2D drawing destination.
    In OS X, this is the Core Graphics context, a CGContextRef object (opaque type).
	https://developer.apple.com/library/mac/documentation/graphicsimaging/reference/CGContext/Reference/reference.html
	
 
    Quartz Images (CGImageRef)
    Quartz images are immutable data types that you use to manipulate bitmap data in Quartz. 
    Although NSImage is easier to use and provides automatic support for resolution independence, 
    you might need to create Quartz images if another API you are using expects them. 
    You can create a Quartz image by drawing into a NSBitmapImageRep object or Quartz bitmap context 
    and then extracting a CGImageRef from there.
 
    Quartz Layers (CGLayerRef)
    Quartz layers are a mutable alternative to Quartz images. You can draw to layers much like you 
    would draw to an NSImage object. You do so by creating a context, locking focus on that context, 
    drawing, and retrieving an image object from the results. Because they are implemented in video memory, 
    layers can be very efficient to use, especially if you need to draw the same image repeatedly.
 
	- CGBitmapContext / void *data = CGBitmapContextGetData()
	http://stackoverflow.com/questions/8189180/how-can-i-get-the-pixel-data-of-a-pixel-in-a-uiimage-or-cgimage
	
	we can also create our own context, namely a bitmap-based context with e.g. 
	CGBitmapContextCreate(). This function returns a context that we can then pass to 
	the CGContext functions to draw into that context, etc.
	
	- NSimage
	- NSBitmapImageRep
	- CGImageRef
        Regardless of how you create a CGImage object, you use the function CGContextDrawImage 
        to draw the image to a graphics context. Keep in mind that CGImage objects are immutable. 
        When you no longer need a CGImage object, release it by calling the function CGImageRelease.
 
	- CALayer 
	
	3. IOS:
	http://www.markj.net/iphone-uiimage-pixel-color/
	http://stackoverflow.com/questions/12770181/how-to-get-the-pixel-color-on-touch
	
	http://www.objc.io/issue-3/moving-pixels-onto-the-screen.html
	
	4. platform independent:
	- cpccPixelBuffer
	
*/


/**
	the cpccDrawingToolsAbstract class collects the common tools
	needed for drawing inside a cpccWindow class and a cpccImage class

*/
template<typename TNativeRect>
class cpcciDrawingTools
{
public:
    virtual ~cpcciDrawingTools() {}
    
public:		// functions
    virtual void 				fillEllipseWithColor(const int left, const int top, const int right, const int bottom, const cpccColor& c)=0;
	virtual void 				fillRectWithColor(const TNativeRect &r, const cpccColor& aColor) =0;
	virtual void 				drawText(int x, int y, const cpcc_char *text, const cpccTextParams& params) const =0;
	virtual void				getTextSize(const cpcc_char *txt, const cpccTextParams& params, int *width, int *height) const = 0;
	virtual cpccColor			getPixel(const int x, const int y) const =0;
	virtual void 				setPixel(const int x, const int y, const cpccColor &aColor)=0;
	virtual void 				drawLine(const int x1, const int y1, const int x2, const int y2, const int width, const cpccColor &aColor) = 0;
    // cannot be done abstract because one clild class does not need it
    // virtual void				bitBlitFrom(const int x, const int y, const TNativeSurface &srcContext, const int srcW, const int srcH, const cpccColor* transparentColor = NULL) = 0;
	
};

