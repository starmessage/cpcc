/*  *****************************************
 *  File:		cpccWindowMac.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight library
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2013 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *				http://www.24hsoftware.com/portable-cpp-filesystem-library
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */

#pragma once


/* 	tutorial 
	/Users/cto/Documents/work/C-Proj/pxCore/examples/NativeDrawing/NativeDrawing.cpp
 

 A layer, represented by the CGLayerRef data type, is engineered for 
 optimal performance. 
 For this reason a layer is typically a better 
 choice for offscreen drawing than a bitmap graphics context is.
 
 All Quartz drawing functions draw to a graphics context. The graphics 
 context provides an abstraction of the destination, freeing you from 
 the details of the destination, such as its resolution. 
 You work in user space, and Quartz performs the necessary transformations 
 to render the drawing correctly to the destination. 
 
 When you use a CGLayer object for drawing, you also draw to a graphics context

 CGContextSynchronize()
 Marks a window context for update
 Similar to updateRect.
 */



#include <Carbon/Carbon.h>


#include "cpccColor.h"
#include "gui.cpccWindowBase.h"
#include "gui.cpccTextStylesMac.h"

#define DEBUGPARAM_DISABLE_TEXTDRAWING   false

//////////////////////////////////////////////////////////////////////////
// 		cpccWindowMac
//////////////////////////////////////////////////////////////////////////

class cpccWindowMac: public cpccWindowBase
{
private:
	NSRect				m_windowRect;
    const float         m_skewX, m_skewY;
	
protected:
	
protected:		// ctors. Protected because this class should not be created alone, but only via its derived class, cpccWindow
	
	cpccWindowMac(NSView *aWnd): cpccWindowBase(aWnd), m_skewX(0.0f), m_skewY(0.0f)
	{
		m_windowRect.size = [aWnd bounds].size;
		m_windowRect.origin  = [aWnd bounds].origin;
        [[NSGraphicsContext currentContext] setShouldAntialias:NO];
 		// std::cout << "m_windowRect size w=" << m_windowRect.size.width << "\n";
	}

    
public:  // functions
    
    void		getTextSize(const cpcc_char *txt, int *width, int *height)
    {
        // http://stackoverflow.com/questions/2044981/determining-the-pixel-length-of-a-string-in-cocoa-mac-osx
        
        NSString *macString = [[[NSString alloc] initWithUTF8String:txt] autorelease];
		NSMutableDictionary *textAttrib = [[[NSMutableDictionary alloc] init] autorelease];
    
        cpccTextStylesMac	styleHelper;
        styleHelper.setFont(textAttrib, fontName.getCurrent().c_str(), fontSize.getCurrent());
        styleHelper.setKerning(textAttrib, kerning.getCurrent());
        
        NSSize tmpSize = [macString sizeWithAttributes: textAttrib];
		
        std::cout << "getTextSize(" << txt << ")\n";
        
        if (width)
            *width = tmpSize.width;
        
        if (height)
            *height = tmpSize.height;
    }
	
	void 		flush() { };
	int 		getHeight(void) { return m_windowRect.size.height; }
	int 		getWidth(void) 	{ return m_windowRect.size.width; }
	

	void 	fillRectWithColor_impl(const NSRect &aRect, const NSColor* aColor)
	{
        //[m_windowHandle lockFocus];
		[aColor setFill];
		NSRectFill(aRect);
        //NSRectFill(m_windowRect);
        //[m_windowHandle unlockFocus];
	}
    
	
	void	fillRectWithColor(const cpccRecti &aRect, const cpccColor& aColor)
	{   //[m_windowHandle lockFocus];
        fillRectWithColor_impl(aRect.asNSRect(), aColor.asNSColor());
        //[m_windowHandle unlockFocus];
	}
	
    
    virtual void fillEllipseWithColor(const int left, const int top, const int right, const int bottom, const cpccColor& c)
    {
        NSColor *fillColor = c.asNSColor();
        [fillColor setFill];
        NSRect ellipseRect = NSMakeRect(left, top, right-left, bottom-top);
        [[NSBezierPath bezierPathWithOvalInRect:ellipseRect] fill];
    }

	
    void 		fillWithColor(const cpccColor& aColor)
	{
		//std::cout << "fillWithColor_impl()\n";
		fillRectWithColor_impl(m_windowRect, aColor.asNSColor());
		/*
        NSColor *backgroundColor = aColor.asNSColor();
		[backgroundColor setFill];
		NSRectFill(m_windowRect);
		*/
	}
	
protected:  // the xxxxxx_impl() functions. They should be called only from the anscenstor
    
    
    void setPixel_impl(const int x, const int y, const cpccColor &aColor)
	{
        // in OSX there is no pair to NSReadPixel, like the window's setPixel(x,y, color)
        // I use a fillRect with size of 1x1 pixels
        if (!m_windowHandle) return;
        
        [m_windowHandle lockFocus];
        NSColor *drawColor = aColor.asNSColor();
        [drawColor set];
        NSRectFill(NSMakeRect(x, y, 1.0, 1.0));
        [m_windowHandle unlockFocus];
	}
    
	
    cpccColor	getPixel_impl(const int x, const int y)
	{
		if (!m_windowHandle) return cpccGreen;
		
		// http://lists.apple.com/archives/cocoa-dev/2009/Jan/msg02014.html
		// NSReadPixel() will tell you the color of the pixel nearest to the given location,
		// in the current drawing context.
		
		[m_windowHandle lockFocus]; // NSReadPixel pulls data out of the current focused graphics context, so -lockFocus is necessary here.
        /*
         NSReadPixel(NSPoint passedPoint)
         https://developer.apple.com/library/mac/documentation/Cocoa/Reference/ApplicationKit/Miscellaneous/AppKit_Functions/Reference/reference.html#//apple_ref/c/func/NSReadPixel
         Because the passedPoint parameter is relative to the current coordinate system,
         if you wish to read a pixel from a specific view, you must convert points in the view’s
         coordinate system to the current coordinate system before calling this function.
         Alternatively, you can lock focus on the view and then specify the pixel coordinate in the view’s coordinate system.
         */
        
		NSColor * pixelColor = NSReadPixel(NSMakePoint(x +m_skewX, y + m_skewY)); //pixelPosition is of type NSPoint and has the value of the position where you want to read the color
   
        
        [m_windowHandle unlockFocus];
        
		cpccColor c;
        // std::cout << "getPixel result:" << (int) c.getBrightness() << "\n";
		c.fromNSColor(pixelColor);
		return c;
	}
	
	
	void 		textOut_impl(int x, int y, const cpcc_char *text)
	{
#if (!DEBUGPARAM_DISABLE_TEXTDRAWING)
		// https://developer.apple.com/library/mac/#documentation/graphicsimaging/conceptual/drawingwithquartz2d/dq_text/dq_text.html#//apple_ref/doc/uid/TP30001066-CH213-TPXREF101
		//	std::cout << "textOut(" << x << " , " << y << " , "<< text << ")";
		
		// Note: Stroke and fill colors do not affect the appearance of text.
        // To apply color to text, you must change the attributes associated with the text
        // https://developer.apple.com/library/mac/documentation/cocoa/conceptual/CocoaDrawingGuide/Color/Color.html
        
        if (fontQuality.getCurrent()==fqNonAntiAliased)
            [[NSGraphicsContext currentContext] setShouldAntialias: NO];
        
		NSString *macString = [[[NSString alloc] initWithUTF8String:text] autorelease];
		
		NSMutableDictionary *textAttrib = [[[NSMutableDictionary alloc] init] autorelease];
        
        cpccTextStylesMac	styleHelper;
        styleHelper.setFont(textAttrib, fontName.getCurrent().c_str(), fontSize.getCurrent());
        styleHelper.setColor(textAttrib, & drawColor.getCurrent());
        int align = textAlign.getCurrent();
        styleHelper.setParagraph(textAttrib, align);
        
        styleHelper.setKerning(textAttrib, kerning.getCurrent());
        
        // up to this point all the text attributes that affect the size must have been applied
        // we can leave for later the text attributes that affect the alignment
        NSRect rect;
        rect.size = [macString sizeWithAttributes: textAttrib];

        rect.origin.y = y + m_skewX;
        rect.origin.x = x + m_skewY;
		
        if (align==0)   // center
            rect.origin.x -= rect.size.width/2;
        if (align==1)   // right
            rect.origin.x -= rect.size.width;
        
        
        [m_windowHandle lockFocus];
        // You should only invoke this method when an NSView object has focus.
		[macString drawInRect:rect withAttributes:textAttrib ];
        [m_windowHandle unlockFocus];
        
        //std::cout << "textOut_impl() isFlipped:" << (m_windowHandle.isFlipped? "Y" : "N " ) << " x:" << rect.origin.x <<  " y:" << rect.origin.y << "\n";
        if (fontQuality.getCurrent()==fqNonAntiAliased)
            [[NSGraphicsContext currentContext] setShouldAntialias: YES];
#endif
	}
	
	

};

