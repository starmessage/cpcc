
/*  *****************************************
 *  File:		gui.cpccDrawingToolsWinDC.h
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

#include <windows.h>
#include "gui.cpccDrawingToolsAbstract.h"
// #include "gui.cpccFontWinGDI.h"
#include "gui.cpccWinGDIhelper.h"

#pragma comment(lib,"msimg32.lib")	// needed for TransparentBlt()


//////////////////////////////////////////////
//		cpccDrawingToolsWinDC
//////////////////////////////////////////////
class cpccDrawingToolsWinDC: public cpccDrawingToolsAbstract<HDC, RECT>
{
private:

protected:


public:  // constructor
	cpccDrawingToolsWinDC(HDC &aDC): cpccDrawingToolsAbstract<HDC, RECT>(aDC) { }

public:		// data
	

public:		// functions

	void	fillEllipseWithColor(const int left, const int top, const int right, const int bottom, const cpccColor& aColor)
	{
		cpccWinGDIBrush tmpBrush(m_drawContext, aColor.asColorref());
		Ellipse(m_drawContext, left, top, right, bottom);
	}


	void 		fillRectWithColor(const RECT &r, const cpccColor& aColor) 
	{
		// const COLORREF c = /* (DWORD) */ aColor.asColorref();
		cpccWinGDIBrush tmpBrush(m_drawContext, aColor.asColorref());
		
		// The brush identified by the hbr parameter may be either a handle to a logical brush or a color value. 
		// If specifying a handle to a logical brush, call CreatePatternBrush, or CreateSolidBrush to obtain the handle.
		// If specifying a color value for the hbr parameter, it must be one of the standard system colors (the value 1 must be added to the chosen color). 


		FillRect(m_drawContext, &r , tmpBrush);
	}
	

	virtual void 		textOut(int x, int y, const cpcc_char *text, const cpccTextParams& params) const
	{
		RECT tmpRect;
		tmpRect.left = tmpRect.right = x;
		tmpRect.top = tmpRect.bottom = y;

		if (params.color)
			SetTextColor(m_drawContext, params.color->asColorref());

		long alignment=DT_LEFT;
		if (params.textAlign)
		{
			switch (*params.textAlign)
			{
				case 1: alignment=DT_RIGHT; break;
				case 0: alignment=DT_CENTER; break;
			}
		}

		cpccFontKerningWin *applyKerningPtr=NULL;
		if (params.kerning)
			applyKerningPtr = new cpccFontKerningWin(m_drawContext, (int) *params.kerning);
		
		cpccWinGDIFont useFont(m_drawContext, params.fontName, params.fontSize, params.fontWeight, params.fontQuality);
		

		DrawText(m_drawContext, text, -1, &tmpRect, alignment | DT_NOCLIP | DT_NOPREFIX);

		// restore the drawing tools
		if (params.kerning)
			delete applyKerningPtr; 
	}
	

	virtual const cpccVector2i	getTextSize(const cpcc_char *txt, const cpccTextParams& params) const 
	{	RECT tmpRect = {0,0,0,0};

		cpccWinGDIFont useFont(m_drawContext, params.fontName, params.fontSize, params.fontWeight, params.fontQuality);
		
		cpccFontKerningWin *applyKerningPtr=NULL;
		if (params.kerning)
			applyKerningPtr = new cpccFontKerningWin(m_drawContext, (int) *params.kerning);
						
		DrawText(m_drawContext, txt, -1, &tmpRect, DT_LEFT | DT_NOCLIP | DT_NOPREFIX | DT_CALCRECT);
		
		if (params.kerning)		// restore the drawing tools
			delete applyKerningPtr;

		return cpccVector2i(tmpRect.right-tmpRect.left, tmpRect.bottom-tmpRect.top);
	}

    
	virtual cpccColor	getPixel(const int x, const int y) const 
	{	
		if (!m_drawContext)
			return cpccRed;
		
		COLORREF c = ::GetPixel(m_drawContext, x, y);
		return cpccColor(GetRValue(c), GetGValue(c), GetBValue(c));
	}
	
	virtual void 		setPixel(const int x, const int y, const cpccColor &aColor)
	{
		if (m_drawContext)
			::SetPixel (m_drawContext, x, y, aColor.asColorref());
	}	
	

	virtual void		bitBlitFrom(const int x, const int y, const HDC &srcContext, const int srcW, const int srcH, const cpccColor* transparentColor=NULL) 
	{ 
		if (!m_drawContext)
			return;

		if (!transparentColor)
			::BitBlt(m_drawContext, x, y, srcW , srcH, srcContext, 0,0, SRCCOPY); 
		else // http://www.winprog.org/tutorial/transparency.html
			TransparentBlt(m_drawContext, x, y, srcW , srcH, srcContext, 0, 0, srcW , srcH, transparentColor->asColorref());
	}
	
};

