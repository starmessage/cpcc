
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

#include "gui.cpccDrawingToolsMacOnFocused.h"


//////////////////////////////////////////////
//		cpccDrawingToolsNSBitmapImageRep
//////////////////////////////////////////////
typedef  NSBitmapImageRep *     TimgRepHandle;

class cpccDrawingToolsNSBitmapImageRep: public cpccDrawingToolsMacOnFocused
{
private:

protected:
	TimgRepHandle &m_imgRep;

public:  // constructor
    
	cpccDrawingToolsNSBitmapImageRep(TimgRepHandle &aRep): m_imgRep(aRep) { }
    
    virtual ~cpccDrawingToolsNSBitmapImageRep() {}

public:		// data
	

public:		// functions
    
    virtual void		bitBlitFrom(const int x, const int y, const TimgRepHandle &srcContext, const int srcW, const int srcH, const cpccColor* transparentColor=NULL)
	{
		if (!m_imgRep || !srcContext)
			return;
        // todo:
    }
    
    
	virtual void fillRectWithColor(const NSRect &r, const cpccColor& aColor) override
	{
        if (!m_imgRep)
			return;
        
        NSColor *c = aColor.asNSColor();
        for (int x=r.origin.x; x< r.origin.x + r.size.width; ++x)
			for (int y=r.origin.y; y< r.origin.y + r.size.height; ++x)
				[m_imgRep setColor: c atX:x y:y];
    }
	

	   
	virtual cpccColor getPixel(const int x, const int y) const override
	{
        if (!m_imgRep)
			return cpccGreen;
        NSColor* color = [m_imgRep colorAtX:x y:y];
        cpccColor result;
        result.fromNSColor(color);
		return result;
    }
	
    
	virtual void setPixel(const int x, const int y, const cpccColor &aColor) override
	{
		if (!m_imgRep)
            return;
        [m_imgRep setColor: aColor.asNSColor() atX:x y:y];
	}
    
    
    virtual void amplifyPixel(const int x, const int y, const float xR, const float xG, const float xB ) 
    {
        if (!m_imgRep)
            return;
        NSColor* color = [m_imgRep colorAtX:x y:y];
        NSColor* newColor = [NSColor colorWithDeviceRed : ([color redComponent] * xR)  green : ([color greenComponent] * xG) blue : ([color blueComponent] * xB) alpha : [color alphaComponent]]; 
    
        [m_imgRep setColor: newColor atX:x y:y];
    }
    
};

