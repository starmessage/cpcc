
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

#pragma comment(lib,"msimg32.lib")	// needed for TransparentBlt(), GradientFill

//////////////////////////////////////////////
//		cpccDrawingToolsWinDC
//////////////////////////////////////////////
class cpccDrawingToolsWinDC : public cpcciDrawingTools /* <RECT> */
{
private:

protected:
	HDC &m_hDC;
	

public:		// constructor

	explicit cpccDrawingToolsWinDC(HDC &aDC): m_hDC(aDC) { }

private:		// data
	int m_offsetX = 0, m_offsetY = 0;

public:		// functions

	void	fillEllipseWithColor(const int left, const int top, const int right, const int bottom, const cpccColor& aColor) override
	{
		cpccWinGDIBrush tmpBrush(m_hDC, aColor.asCOLORREF());
		Ellipse(m_hDC, m_offsetX + left, m_offsetY + top, m_offsetX + right, m_offsetY + bottom);
	}

	void 	fillRectWithColor_impl_win(const RECT& r, const COLORREF& aColorRef)
	{
		cpccWinGDIBrush tmpBrush(m_hDC, aColorRef);
		
		// The brush identified by the hbr parameter may be either a handle to a logical brush or a color value. 
		// If specifying a handle to a logical brush, call CreatePatternBrush, or CreateSolidBrush to obtain the handle.
		// If specifying a color value for the hbr parameter, it must be one of the standard system colors (the value 1 must be added to the chosen color). 
		FillRect(m_hDC, &r, tmpBrush);
	}

	void 	fillRectWithColor(const cpccRecti &r, const cpccColor& aColor)  override
	{
		RECT drawRect;
		drawRect.left	= r.left + m_offsetX;
		drawRect.right	= r.getRight() + m_offsetX;
		drawRect.top	= r.top + m_offsetY;
		drawRect.bottom = r.getBottom() + m_offsetY;
		
		fillRectWithColor_impl_win(drawRect, aColor.asCOLORREF());
	}


	void 	fillRectWithGradientColorV(const cpccRecti &r, const cpccColor& aTopColor, const cpccColor& aBottomColor)  override
	{
		// https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-gradientfill
		// https://docs.microsoft.com/en-us/windows/win32/gdi/drawing-a-shaded-rectangle
		// Create an array of TRIVERTEX structures that describe 
		
		// positional and color values for each vertex. For a rectangle, 
		// only two vertices need to be defined: upper-left and lower-right. 
		TRIVERTEX vertex[2] ; 
		// its color components are in COLOR16 format.
		// The color information of each channel is specified as a value from 0x0000 to 0xff00. 
		// Color16Value(x) = 256*x. That way Color16Value(255) would be 256*255 or 0xff00, giving you full range.
		// https://www.gamedev.net/forums/topic/551258-question-about-win32-and-c-data-types/
		vertex[0].x     = r.left + m_offsetX;
		vertex[0].y     = r.top + m_offsetY;
		vertex[0].Red	= aTopColor.r << 8; // 256* aTopColor.r;
		vertex[0].Green = aTopColor.g << 8;
		vertex[0].Blue  = aTopColor.b << 8;
		vertex[0].Alpha = 0x0000;

		vertex[1].x     = r.getRight() + m_offsetX;
		vertex[1].y     = r.getBottom() + m_offsetY;
		vertex[1].Red   = aBottomColor.r << 8;
		vertex[1].Green = aBottomColor.g << 8;
		vertex[1].Blue  = aBottomColor.b << 8;
		vertex[1].Alpha = 0x0000;

		// Create a GRADIENT_RECT structure that 
		// references the TRIVERTEX vertices. 
		GRADIENT_RECT gRect;
		gRect.UpperLeft  = 0;
		gRect.LowerRight = 1;

		GradientFill(m_hDC, vertex, 2, &gRect, 1, GRADIENT_FILL_RECT_V);
	}


	virtual void 		drawText(int x, int y, const cpcc_char *text, const cpccTextParams& params) const override
	{
		RECT tmpRect;
		tmpRect.left = tmpRect.right = m_offsetX + x;
		tmpRect.top = tmpRect.bottom = m_offsetY + y;

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
		
		COLORREF c = ::GetPixel(m_hDC, m_offsetX + x, m_offsetY + y);
		return cpccColor(GetRValue(c), GetGValue(c), GetBValue(c));
	}
	

	virtual void 		setPixel(const int x, const int y, const cpccColor &aColor) override
	{
		if (m_hDC)
			::SetPixel (m_hDC, m_offsetX + x, m_offsetY + y, aColor.asCOLORREF());
	}	
	

	inline virtual void amplifyPixel(const int x, const int y, const float xR, const float xG, const float xB)
	{
		if (!m_hDC)
			return;

		COLORREF c = ::GetPixel(m_hDC, m_offsetX + x, m_offsetY+ y);
		int R = (int) (GetRValue(c) * xR),
			G = (int) (GetGValue(c) * xG),
			B = (int) (GetBValue(c) * xB);
		::SetPixel(m_hDC, x, y, RGB(std::min(255, R), std::min(255, G), std::min(255, B) ) );
	}


	virtual void		bitBlitFrom(const int x, const int y, const HDC &srcContext, const int srcW, const int srcH, const cpccColor* transparentColor=NULL) 
	{ 
		if (!m_hDC)
			return;

		// infoLog().addf(_T("bitBlitFrom, m_offsetX=%i, m_offsetY=%i"), m_offsetX, m_offsetY);

		if (!transparentColor)
			::BitBlt(m_hDC, m_offsetX + x, m_offsetY + y, srcW , srcH, srcContext, 0,0, SRCCOPY);
		else // http://www.winprog.org/tutorial/transparency.html
			TransparentBlt(m_hDC, m_offsetX  + x, m_offsetY + y, srcW , srcH, srcContext, 0, 0, srcW , srcH, transparentColor->asCOLORREF());
			
	}
	

	virtual void 		drawLine(const int x1, const int y1, const int x2, const int y2, const int width, const cpccColor &aColor)
	{
		if (!m_hDC)
			return;

		cpccWinGDIPen tmpPen(m_hDC, width, aColor.asCOLORREF());
		::MoveToEx(m_hDC, m_offsetX + x1, m_offsetY + y1, NULL);
		::LineTo(m_hDC, m_offsetX + x2, m_offsetY + y2);
	}
};

