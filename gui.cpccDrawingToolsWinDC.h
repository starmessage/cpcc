
/*  *****************************************
 *  File:		gui.cpccDrawingToolsWinDC.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight, graphic context drawing tools
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2013 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com/cpcclibrary
 *  Download:	https://code.google.com/p/cpcc/
 *              https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */
 
 
#pragma once

#include <windows.h>
#include "gui.cpccDrawingToolsAbstract.h"
#include "gui.cpccWinGDIhelper.h"

#pragma comment(lib,"msimg32.lib")	// needed for TransparentBlt()


//////////////////////////////////////////////
//		cpccDrawingToolsWinDC
//////////////////////////////////////////////
class cpccDrawingToolsWinDC : public cpcciDrawingTools<RECT>
{
private:

protected:
	HDC &m_hDC;

public:		// constructor

	explicit cpccDrawingToolsWinDC(HDC &aDC): m_hDC(aDC) { }

public:		// data
	

public:		// functions

	void	fillEllipseWithColor(const int left, const int top, const int right, const int bottom, const cpccColor& aColor) override
	{
		cpccWinGDIBrush tmpBrush(m_hDC, aColor.asCOLORREF());
		Ellipse(m_hDC, left, top, right, bottom);
	}


	void 	fillRectWithColor(const RECT &r, const cpccColor& aColor)  override
	{
		// const COLORREF c = /* (DWORD) */ aColor.asColorref();
		cpccWinGDIBrush tmpBrush(m_hDC, aColor.asCOLORREF());
		
		// The brush identified by the hbr parameter may be either a handle to a logical brush or a color value. 
		// If specifying a handle to a logical brush, call CreatePatternBrush, or CreateSolidBrush to obtain the handle.
		// If specifying a color value for the hbr parameter, it must be one of the standard system colors (the value 1 must be added to the chosen color). 
		FillRect(m_hDC, &r , tmpBrush);
	}
	

	virtual void 		drawText(int x, int y, const cpcc_char *text, const cpccTextParams& params) const override
	{
		RECT tmpRect;
		tmpRect.left = tmpRect.right = x;
		tmpRect.top = tmpRect.bottom = y;

		if (params.color)
			SetTextColor(m_hDC, params.color->asCOLORREF());

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
			applyKerningPtr = new cpccFontKerningWin(m_hDC, (int) *params.kerning);
		
		cpccWinGDIFont useFont(m_hDC, params.fontName, params.fontSize, params.fontWeight, params.fontQuality);
		
		DrawText(m_hDC, text, -1, &tmpRect, alignment | DT_NOCLIP | DT_NOPREFIX);

		// restore the drawing tools
		if (params.kerning)
			delete applyKerningPtr; 
	}
	

	void getTextSize(const cpcc_char *txt, const cpccTextParams& params, int *width, int *height) const override
	{	RECT tmpRect = {0,0,0,0};

		cpccWinGDIFont useFont(m_hDC, params.fontName, params.fontSize, params.fontWeight, params.fontQuality);
		
		cpccFontKerningWin *applyKerningPtr=NULL;
		if (params.kerning)
			applyKerningPtr = new cpccFontKerningWin(m_hDC, (int) *params.kerning);
						
		DrawText(m_hDC, txt, -1, &tmpRect, DT_LEFT | DT_NOCLIP | DT_NOPREFIX | DT_CALCRECT);
		
		if (params.kerning)		// restore the drawing tools
			delete applyKerningPtr;

		if (width)
			*width = tmpRect.right - tmpRect.left;
		if (height)
			*height = tmpRect.bottom - tmpRect.top;
	}

    
	virtual cpccColor	getPixel(const int x, const int y) const  override
	{	
		if (!m_hDC)
			return cpccRed;
		
		COLORREF c = ::GetPixel(m_hDC, x, y);
		return cpccColor(GetRValue(c), GetGValue(c), GetBValue(c));
	}
	

	virtual void 		setPixel(const int x, const int y, const cpccColor &aColor) override
	{
		if (m_hDC)
			::SetPixel (m_hDC, x, y, aColor.asCOLORREF());
	}	
	

	inline virtual void amplifyPixel(const int x, const int y, const float xR, const float xG, const float xB)
	{
		if (!m_hDC)
			return;

		COLORREF c = ::GetPixel(m_hDC, x, y);
		int R = (int) (GetRValue(c) * xR),
			G = (int) (GetGValue(c) * xG),
			B = (int) (GetBValue(c) * xB);
		::SetPixel(m_hDC, x, y, RGB(std::min(255, R), std::min(255, G), std::min(255, B) ) );
	}


	virtual void		bitBlitFrom(const int x, const int y, const HDC &srcContext, const int srcW, const int srcH, const cpccColor* transparentColor=NULL) 
	{ 
		if (!m_hDC)
			return;

		if (!transparentColor)
			::BitBlt(m_hDC, x, y, srcW , srcH, srcContext, 0,0, SRCCOPY); 
		else // http://www.winprog.org/tutorial/transparency.html
			TransparentBlt(m_hDC, x, y, srcW , srcH, srcContext, 0, 0, srcW , srcH, transparentColor->asCOLORREF());
	}
	

	virtual void 		drawLine(const int x1, const int y1, const int x2, const int y2, const int width, const cpccColor &aColor)
	{
		if (!m_hDC)
			return;

		cpccWinGDIPen tmpPen(m_hDC, width, aColor.asCOLORREF());
		::MoveToEx(m_hDC, x1, y1, NULL);
		::LineTo(m_hDC, x2, y2);
	}
};

