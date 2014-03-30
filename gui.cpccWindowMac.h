/*  *****************************************
 *  File:		cpccFileSystemMini.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight, file system library
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

#include "../../../../cpcc/cpccColor.h"
#include "../../../../cpcc/gui.cpccWindowBase.h"


class cpccWindowMacTextStylesMac
{
public:
    
	void setFont(NSMutableDictionary *aTextAttr, const cpcc_char *aFontName, const int aFontSize)
    {
        if (aTextAttr && aFontName)
        {
            /*
             // convert font
             NSFont* font = [NSFont fontWithName:@"Arial" size:30];
             font = [[NSFontManager sharedFontManager] convertFont:font toHaveTrait:NSFontItalicTrait];
             
             // create with traits and weight
             NSFont* font = [[NSFontManager sharedFontManager]  fontWithFamily:@"Arial" traits:NSFontItalicTrait weight:2 size:30];
             */
            
            NSString *fontName = [[[NSString alloc] initWithUTF8String:aFontName] autorelease];
            
            // works ok
            // [aTextAttr setObject:[NSFont fontWithName:fontName size:aFontSize ]  forKey:NSFontAttributeName];
            
            // https://developer.apple.com/library/mac/documentation/cocoa/reference/applicationkit/classes/NSFontManager_Class/Reference/Reference.html#//apple_ref/doc/uid/20000383-16096
            [aTextAttr setObject: [[NSFontManager sharedFontManager] fontWithFamily:fontName traits:0 weight:1 size:aFontSize] forKey:NSFontAttributeName] ;
        }
    }
    
  
    void setColor(NSMutableDictionary *aTextAttr, cpccColor *aColor)
    {
        if (aTextAttr && aColor)
            [aTextAttr setObject:aColor->asNSColor() forKey:NSForegroundColorAttributeName];
    }
    
    
    void setParagraph(NSMutableDictionary *aTextAttr, const int align)
    {
        if (aTextAttr)
        {
            NSMutableParagraphStyle *paragraphStyle = [[[NSParagraphStyle defaultParagraphStyle] mutableCopy]  autorelease];
            
            if (align==0)
                [paragraphStyle setAlignment:NSCenterTextAlignment];
            if (align==1)
                [paragraphStyle setAlignment:NSRightTextAlignment];
            if (align==-1)
                [paragraphStyle setAlignment:NSLeftTextAlignment];
            
            [aTextAttr setObject:paragraphStyle forKey:NSParagraphStyleAttributeName];
        }
    }
};

//////////////////////////////////////////////////////////////////////////


class cpccWindowMac: public cpccWindowBase
{
private:
	NSView*				m_windowHandle;
	CGContextRef 		m_drawSurface;
	NSRect				m_windowRect;
	TmioanTextStylesMac	m_styleHelper;
    const float         m_skewX, m_skewY;
protected:
	
public:  // constructor
	
	cpccWindowMac(NSView *aWnd): m_windowHandle(aWnd), m_skewX(0.0f), m_skewY(0.0f)
	{
		m_windowRect.size = [aWnd bounds].size;
		m_windowRect.origin  = [aWnd bounds].origin;
		// std::cout << "m_windowRect size w=" << m_windowRect.size.width << "\n";
	}

private:
    

    
public:  // functions
    
    cpccColor	getPixel(const int x, const int y)
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
        
        /*
        float colourValue = ([pixelColor greenComponent] + [pixelColor redComponent]+ [pixelColor blueComponent]) / 3.0f;
        std::cout << "getPixel("<< x << "," << y << ") newY:" << newY <<" result:" << colourValue << "\n";
		*/
        
        [m_windowHandle unlockFocus];

		cpccColor c;
        // std::cout << "getPixel result:" << (int) c.getBrightness() << "\n";
		c.fromNSColor(pixelColor);
		return c;
	}
	
	
    void		getTextSize(const cpcc_char *txt, int *width, int *height)
    {
        // http://stackoverflow.com/questions/2044981/determining-the-pixel-length-of-a-string-in-cocoa-mac-osx
        
        NSString *macString = [[[NSString alloc] initWithUTF8String:txt] autorelease];
		NSMutableDictionary *textAttrib = [[[NSMutableDictionary alloc] init] autorelease];
    
        m_styleHelper.setFont(textAttrib, fontName.getCurrent().c_str(), fontSize.getCurrent());
        NSSize tmpSize = [macString sizeWithAttributes: textAttrib];
		
        if (width)
            *width = tmpSize.width;
        
        if (height)
            *height = tmpSize.height;
    }
	
    
	NSView* 	getNativeWindowHandle(void) { return m_windowHandle; }
	
	void 		flush() { };
	

	int 		getHeight(void) { return m_windowRect.size.height; }
	int 		getWidth(void) 	{ return m_windowRect.size.width; }
	
	
	void		BitBilt_impl(int x, int y, NSImage *img, int w, int h, cpccColor *aTransparentColor)
	{
		if (img)
            [img drawAtPoint:NSMakePoint(x +m_skewX, y +m_skewY) fromRect:NSZeroRect operation: NSCompositeSourceOver fraction: 1.0f];
	}
    
	
protected:
	
	void 		fillWithColor_impl(const cpccColor& aColor)
	{
		//std::cout << "fillWithColor_impl()\n";
        NSColor *backgroundColor = aColor.asNSColor();
		[backgroundColor setFill];
		NSRectFill(m_windowRect);
	}
    
		
	void 		textOut_impl(int x, int y, const cpcc_char *text)
	{
		// https://developer.apple.com/library/mac/#documentation/graphicsimaging/conceptual/drawingwithquartz2d/dq_text/dq_text.html#//apple_ref/doc/uid/TP30001066-CH213-TPXREF101
		//	std::cout << "textOut(" << x << " , " << y << " , "<< text << ")";
		
		// Note: Stroke and fill colors do not affect the appearance of text.
        // To apply color to text, you must change the attributes associated with the text
        // https://developer.apple.com/library/mac/documentation/cocoa/conceptual/CocoaDrawingGuide/Color/Color.html
        
		NSString *macString = [[[NSString alloc] initWithUTF8String:text] autorelease];
		
		NSMutableDictionary *textAttrib = [[[NSMutableDictionary alloc] init] autorelease];
        
        m_styleHelper.setFont(textAttrib, fontName.getCurrent().c_str(), fontSize.getCurrent());
        m_styleHelper.setColor(textAttrib, & drawColor.getCurrent());
        int align = textAlign.getCurrent();
        m_styleHelper.setParagraph(textAttrib, align);
        
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
		[macString drawInRect:rect withAttributes:textAttrib];
        [m_windowHandle unlockFocus];
        
        // std::cout << "textOut_impl("<< text << ") isFlipped:" << (m_windowHandle.isFlipped? "Y\n" : "N\n" );
	}
	
	

};

