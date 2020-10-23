
/*  *****************************************
 *  File:		gui.cpccDrawingToolsCGContext.h
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
 
This_file_is_not_used
 
#pragma once

#include "cpccColor.h"
#include "gui.cpccDrawingToolsAbstract.h"

// see also:
// CGBitmapContext derived from CGContextRef

// I will need this to draw lines
// CGContextRef 		m_drawSurface;
// e.g.
//  myCGContext = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
// 	CGContextSetShouldAntialias(myCGContext, false);
//	CGContextBeginPath(myCGContext);
//	CGContextMoveToPoint(myCGContext, x0, myHeight - y0);
//	CGContextAddLineToPoint(myCGContext, x1, myHeight - y1);
//	CGContextStrokePath(myCGContext);
//	CGContextSetShouldAntialias(myCGContext, true);


// ////////////////////////////////////////////
//		cpccDrawingToolsCGContext
// ////////////////////////////////////////////
class cpccDrawingToolsCGContext: public cpccDrawingToolsAbstract<CGContextRef, CGRect>
{
private:

protected:


public:  // constructor
	cpccDrawingToolsCGContext(CGContextRef &aCtx): cpccDrawingToolsAbstract<CGContextRef, CGRect>(aCtx) { }

public:		// data
	

public:		// functions
	
	virtual void 		fillRectWithColor(const CGRect &r, const cpccColor& aColor) override
	{
        // CGContextSetFillColorWithColor(CGContextRef c, CGColorRef color)
        CGContextSetRGBFillColor (m_drawContext, aColor.a/255.0, aColor.r/255.0, aColor.g/255.0, aColor.b/255.0);// 3
        CGContextFillRect (m_drawContext, r);
    }
	

	virtual void 		textOut(int x, int y, const cpcc_char *text, const cpccTextParams& params) const
	{
		/*
		Drawing Text
			CGContextGetTextMatrix
			CGContextGetTextPosition
			CGContextSetCharacterSpacing
			CGContextSetFont
			CGContextSetFontSize
			CGContextSetTextDrawingMode
			CGContextSetTextMatrix
			CGContextSetTextPosition
			
			CGContextSelectFont 		Deprecated in iOS 7.0
			CGContextShowText 			Deprecated in iOS 7.0
			CGContextShowTextAtPoint 	Deprecated in iOS 7.0 
		*/
		
		if (params.kerning)
			CGContextSetCharacterSpacing(m_drawContext, *params.kerning);
		
		const cpcc_char *fontname = (params.fontName)? params.fontName : "Arial";
		CGContextSelectFont(m_drawContext, fontname, (params.fontSize)? *params.fontSize : 10.0f, kCGEncodingFontSpecific);
        
		//CGContextSetFontSize(m_drawContext, (params.fontSize)? *params.fontSize : 10.0f);
		
		//CGContextSetTextDrawingMode(ctx, kCGTextFillStroke);
		CGContextShowTextAtPoint(m_drawContext, x, y, text, 9);
	}
	

	virtual const cpccVector2i	getTextSize(const cpcc_char *txt, const cpccTextParams& params) const 
	{
        return cpccVector2i(0,0);
	}

    
	virtual cpccColor	getPixel(const int x, const int y) const 
	{	
		if (!m_drawContext)
			return cpccRed;
        return cpccBlack;
    }
	
    
	virtual void 		setPixel(const int x, const int y, const cpccColor &aColor)
	{
		if (!m_drawContext)
            return;
        
        CGContextSetRGBFillColor (m_drawContext, aColor.a/255.0, aColor.r/255.0, aColor.g/255.0, aColor.b/255.0);
        CGContextFillRect (m_drawContext, CGRectMake( x, y, 1.0, 1.0));
	}
	

	virtual void		bitBlit(const int x, const int y, const CGContextRef &srcContext, const int srcW, const int srcH, cpccColor* transparentColor=NULL)
	{ 
		if (!m_drawContext)
			return;

    }
	
};

