
/*  *****************************************
 *  File:		gui.cpccDrawingToolsNSBitmapImageRep.h
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
#include "gui.cpccDrawingToolsAbstract.h"
#include "gui.cpccTextStylesMac.h"


//////////////////////////////////////////////
//		cpccDrawingToolsNSBitmapImageRep
//////////////////////////////////////////////
typedef  NSBitmapImageRep *     TimgRepHandle;

class cpccDrawingToolsNSBitmapImageRep: public cpccDrawingToolsAbstract<TimgRepHandle , NSRect>
{
private:

protected:


public:  // constructor
    
	cpccDrawingToolsNSBitmapImageRep(TimgRepHandle &aRep): cpccDrawingToolsAbstract<TimgRepHandle, NSRect>(aRep) { }

public:		// data
	

public:		// functions
    
    virtual void		bitBlitFrom(const int x, const int y, const TimgRepHandle &srcContext, const int srcW, const int srcH, const cpccColor* transparentColor=NULL)
	{
		if (!m_drawContext || !srcContext)
			return;
        // todo:
    }
    
    
	virtual void 		fillRectWithColor(const NSRect &r, const cpccColor& aColor)
	{
        if (!m_drawContext)
			return;
        
        NSColor *c = aColor.asNSColor();
        for (int x=r.origin.x; x< r.origin.x + r.size.width; ++x)
			for (int y=r.origin.y; y< r.origin.y + r.size.height; ++x)
				[m_drawContext setColor: c atX:x y:y];
    }
	

	virtual void 		textOut(int x, int y, const cpcc_char *text, const cpccTextParams& params) const
	{
		// todo:
	}
	

	virtual const cpccVector2i	getTextSize(const cpcc_char *txt, const cpccTextParams& params) const 
	{
        if (!m_drawContext)
			return cpccVector2i(0,0);
        
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

    
	virtual cpccColor	getPixel(const int x, const int y) const 
	{
        if (!m_drawContext)
			return cpccGreen;
        NSColor* color = [m_drawContext colorAtX:x y:y];
        cpccColor result;
        result.fromNSColor(color);
		return result;
    }
	
    
	virtual void 		setPixel(const int x, const int y, const cpccColor &aColor)
	{
		if (!m_drawContext)
            return;
        [m_drawContext setColor: aColor.asNSColor() atX:x y:y];
	}
    
    
};

