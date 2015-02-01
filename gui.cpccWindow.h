
/*  *****************************************
 *  File:		cpccWindow.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight, window drawing class
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2013 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *  Download:	https://github.com/starmessage/cpcc
 *				https://code.google.com/p/cpcc/              
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */
 
 
#pragma once


#include "gui.cpccCSS.h"
#include "io.cpccLog.h"

#if defined(__APPLE__) 
	#include "gui.cpccWindowMac.h"
	typedef cpccWindowMac	windowImpl;

#elif defined(_WIN32) 
	#include "gui.cpccWindowWin.h"
	typedef cpccWindowWin	windowImpl;

#else
	#error #4367: Unknown platform for cpccWindow
#endif




class cpccWindow: public windowImpl 
{
private:
	
	
protected:
	
public:  // constructor
	
	cpccWindow(const cpccNativeWindowHandle wHandle): windowImpl(wHandle) 
	{
		if (!wHandle)
			infoLog().add(_T("Error: cpccWindow(wHandle) with NIL wHandle"));
		else
			infoLog().addf( _T("cpccWindow(wHandle:%X) constructor start"), (long) wHandle );
	}
    

public:		// data


public:		// functions
	
	void clear(void)
	{
		// to debug: in some cases, this is called twice during the screensaver run under MAC
		// infoLog().add(_T("cpccWindow.clear()"));
		fillWithColor(bgColor.getCurrent());
	}

	

    void	pushCss(cpccCSS *aCssPtr)
	{
		if (!aCssPtr)
			return;

		if (!aCssPtr->textAlign.isNull())
		{
			int alignment = -1; // left
			switch (aCssPtr->textAlign)
			{
				case cpccCSS::taRight:	alignment=1; break;
				case cpccCSS::taCenter:	alignment=0; break;
                case cpccCSS::taNone:
				case cpccCSS::taLeft: ;
			}
			textAlign.push(alignment);
		}

		// text color
		if (!aCssPtr->color.isNull())
			drawColor.push( aCssPtr->color);

        // text font
        if (!aCssPtr->fontName.isNull())
            fontName.push(aCssPtr->fontName);
            
        if (!aCssPtr->fontSize.isNull())
            fontSize.push(aCssPtr->fontSize);
	}


	void	popCss(cpccCSS* aCssPtr)
	{
		if (!aCssPtr)
			return;

		// restore drawing parameters
		if (!aCssPtr->textAlign.isNull())
			textAlign.pop();
        
		if (!aCssPtr->fontName.isNull())
			fontName.pop();
                          
		if (!aCssPtr->fontSize.isNull())
			fontSize.pop();
        
		if (!aCssPtr->color.isNull())
			drawColor.pop();
	}


	void 	textOut(int x, int y, const cpcc_char *text, cpccCSS *aCssPtr)
	{
		// prepare drawing tools
		pushCss(aCssPtr);

		// at last, do the drawing
        textOut_impl(x, y, text);
        
		popCss(aCssPtr);
	}
    
	
	cpccColor	getPixel(const int x, const int y)
	{
		if (x<0 || x>= (int) getWidth()) 
			return cpccBlack;

		if (y<0 || y>=(int) getHeight()) 
			return cpccBlack;

		return getPixel_impl(x, y);
	}


	void setPixel(int x, int y, const cpccColor &aColor)
	{
		if (x<0 || x>= (int) getWidth()) 
			return;

		if (y<0 || y>=(int) getHeight()) 
			return;
		
		setPixel_impl(x, y, aColor);
	}
};

