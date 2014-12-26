
/*  *****************************************
 *  File:		gui.cpccDrawingToolsMacOnFocused.h
 *	Purpose:	Portable (cross-platform), light-weight, graphic drawing tools
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2013 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */
 
 
#pragma once

#include "cpccColor.h"
#include "gui.cpccDrawingToolsAbstract.h"
#include "gui.cpccTextStylesMac.h"


//////////////////////////////////////////////
//		cpccDrawingToolsMacOnFocused
//  Asume the proper destination has already a lockFocus
//////////////////////////////////////////////


class cpccDrawingToolsMacOnFocused: public cpccDrawingToolsAbstract<NSRect>
{
private:

protected:
	

public:  // constructor
    
	
public:		// data
	

public:		// functions
    
    virtual cpccColor			getPixel(const int x, const int y) const
    {
        // not implemented
        return cpccMaroon;
    }
    
    
    virtual void 				setPixel(const int x, const int y, const cpccColor &aColor)
    {
        // not implemented
    }
    
    
    void 				fillEllipseWithColor(const int left, const int top, const int right, const int bottom, const cpccColor& c)
    {
        NSColor *fillColor = c.asNSColor();
        [fillColor setFill];
        NSRect ellipseRect = NSMakeRect(left, top, right-left, bottom-top);
        [[NSBezierPath bezierPathWithOvalInRect:ellipseRect] fill];
    }
    
    
    void fillRectWithColor(const NSRect &r, const cpccColor& aColor)
    {
        [aColor.asNSColor() setFill];
        NSRectFill(r);
    }
    
    
    

    void 		textOut(int x, int y, const cpcc_char *text, const cpccTextParams& params) const
	{
        // https://developer.apple.com/library/mac/#documentation/graphicsimaging/conceptual/drawingwithquartz2d/dq_text/dq_text.html#//apple_ref/doc/uid/TP30001066-CH213-TPXREF101
        //	std::cout << "textOut(" << x << " , " << y << " , "<< text << ")";
        
        // Note: Stroke and fill colors do not affect the appearance of text.
        // To apply color to text, you must change the attributes associated with the text
        // https://developer.apple.com/library/mac/documentation/cocoa/conceptual/CocoaDrawingGuide/Color/Color.html
        
		NSString *macString = [[[NSString alloc] initWithUTF8String:text] autorelease];
		NSMutableDictionary *textAttrib = [[[NSMutableDictionary alloc] init] autorelease];
		
        cpccTextStylesMac	styleHelper;
		
		if (params.kerning)
			styleHelper.setKerning(textAttrib, *params.kerning);
			
		if ((params.fontName) || (params.fontSize))
			styleHelper.setFont(textAttrib, (params.fontName)? params.fontName : "Arial", (params.fontSize)? *params.fontSize : 10.0f);
      	
        styleHelper.setColor(textAttrib, (params.color)? params.color : &cpccBurlyWood );
        
        int align = (params.textAlign) ? *params.textAlign : -1;
        styleHelper.setParagraph(textAttrib, align);
        
        NSRect rect;
        rect.size = [macString sizeWithAttributes: textAttrib];
        
        rect.origin.y = y;
        rect.origin.x = x;
        
        if (align==0)   // center
            rect.origin.x -= rect.size.width/2;
        if (align==1)   // right
            rect.origin.x -= rect.size.width;
        
		[macString drawInRect:rect withAttributes:textAttrib ];
	}
	

	virtual const cpccVector2i	getTextSize(const cpcc_char *txt, const cpccTextParams& params) const 
	{
		NSString *macString = [[[NSString alloc] initWithUTF8String:txt] autorelease];
		NSMutableDictionary *textAttrib = [[[NSMutableDictionary alloc] init] autorelease];
		
        cpccTextStylesMac	styleHelper;
		
		if (params.kerning)
			styleHelper.setKerning(textAttrib, *params.kerning);
			
		if ((params.fontName) || (params.fontSize))
			styleHelper.setFont(textAttrib, (params.fontName)? params.fontName : "Arial", (params.fontSize)? *params.fontSize : 10.0f);
      
        NSSize tmpSize = [macString sizeWithAttributes: textAttrib];
        return cpccVector2i(tmpSize.width, tmpSize.height);
	}

  
    
};

