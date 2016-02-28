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
#include "gui.cpccDrawingToolsMacOnFocused.h"



#define DEBUGPARAM_DISABLE_TEXTDRAWING   false

//////////////////////////////////////////////////////////////////////////
// 		cpccWindowMac
//////////////////////////////////////////////////////////////////////////

class cpccWindowMac: public cpccWindowBase
{
private:
	NSRect                          m_windowRect;
    const float                     m_skewX, m_skewY;
    cpccDrawingToolsMacOnFocused    dtool;
    
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
    
    void		getTextSize(const cpcc_char *txt, int *width, int *height) override
    {
        cpccTextParams  params;

        params.fontName = fontName.getCurrent().c_str();
        params.fontSize = &fontSize.getCurrent();
        params.kerning  = &kerning.getCurrent();
        
        dtool.getTextSize(txt, params, width, height);
    }
	
    
	void 		flush() override { };
	int 		getHeight(void) const override	{ return m_windowRect.size.height; }
	int 		getWidth(void) 	const override	{ return m_windowRect.size.width; }
    int         getTop(void)    const override	{ return m_windowRect.origin.y; }
    int         getLeft(void)   const override	{ return m_windowRect.origin.x; }
    void        lockFocus(void) override { [m_windowHandle lockFocus]; }
    void        unlockFocus(void) override { [m_windowHandle unlockFocus]; }
    
	void	fillRectWithColor(const cpccRecti &aRect, const cpccColor& aColor) override
	{
        dtool.fillRectWithColor(aRect.asNSRect(), aColor);
	}
	
    
    virtual void fillEllipseWithColor(const int left, const int top, const int right, const int bottom, const cpccColor& c) override
    { dtool.fillEllipseWithColor(left, top, right, bottom, c); }

	
    void 		fillWithColor(const cpccColor& aColor) override	{ dtool.fillRectWithColor(m_windowRect, aColor); }
    
	
protected:  // the xxxxxx_impl() functions. They should be called only from the anscenstor
    
    virtual void 				drawLine(const int x1, const int y1, const int x2, const int y2, const int width, const cpccColor &c) override
    {
            dtool.drawLine(x1, y1, x2, y2, width, c);
    }
    
    
    void setPixel_impl(const int x, const int y, const cpccColor &aColor)
	{
        if (!m_windowHandle) return;
        
        [m_windowHandle lockFocus];
        
        dtool.setPixel(x, y, aColor);
        //NSColor *drawColor = aColor.asNSColor();
        //[drawColor set];
        //NSRectFill(NSMakeRect(x, y, 1.0, 1.0));
        [m_windowHandle unlockFocus];
	}
    
	
    cpccColor	getPixel_impl(const int x, const int y)
	{
		if (!m_windowHandle) return cpccGreen;
		
		// http://lists.apple.com/archives/cocoa-dev/2009/Jan/msg02014.html
		// NSReadPixel() will tell you the color of the pixel nearest to the given location,
		// in the current drawing context.
		
		[m_windowHandle lockFocus];

        cpccColor c = dtool.getPixel(x +m_skewX, y + m_skewY);
        [m_windowHandle unlockFocus];
        
		// std::cout << "getPixel result:" << (int) c.getBrightness() << "\n";
		return c;
	}
	
	
	void 		textOut_impl(int x, int y, const cpcc_char *text)
	{
#if (!DEBUGPARAM_DISABLE_TEXTDRAWING)

        cpccTextParams  params;

        params.fontName = fontName.getCurrent().c_str();
        params.fontSize = &fontSize.getCurrent();
        params.kerning  = &kerning.getCurrent();
        params.color    = &drawColor.getCurrent();
        params.textAlign= &textAlign.getCurrent();
        
        if (fontQuality.getCurrent()==fqNonAntiAliased)
            [[NSGraphicsContext currentContext] setShouldAntialias: NO];

        [m_windowHandle lockFocus];
        // You should only invoke this method when an NSView object has focus.
        dtool.drawText(x, y, text, params);
        [m_windowHandle unlockFocus];

        if (fontQuality.getCurrent()==fqNonAntiAliased)
            [[NSGraphicsContext currentContext] setShouldAntialias: YES];

        /*
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
         */
#endif
	}
	
	

};

