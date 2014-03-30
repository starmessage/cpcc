/*  *****************************************
 *  File:		cpccWindowWin.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight, library
 *				portable window class
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

#include <Windows.h>
#include "cpccColor.h"
#include "gui.cpccWindowBase.h"
#include "gui.cpccrenderBufferWinGDI.h"
#include "gui.cpccFontWinGDI.h"

#pragma comment(lib,"msimg32.lib")	// needed for TransparentBlt()




class cpccWindowWin: public cpccWindowBase
{
private:
	HWND 				m_hWnd;
	RECT				m_rect;
	HDC					m_WindowDC,
						m_DrawDC;
	renderBufferWinGDI*	m_renderBuffer;
	
	enum	{ config_UseDoubleBuffer=true };

protected:

public:		// ctors ////////////////////////////////
	cpccWindowWin(const HWND aWnd): m_hWnd(aWnd), m_renderBuffer(NULL)
	{
		m_rect.top = m_rect.bottom = m_rect.left = m_rect.right = 0;
		m_DrawDC = m_WindowDC = GetDC(m_hWnd);
		SetBkMode(m_WindowDC, TRANSPARENT);
		GetWindowRect(m_hWnd, &m_rect);

		if (config_UseDoubleBuffer)
		{
			m_renderBuffer = new renderBufferWinGDI(m_WindowDC, m_rect.right - m_rect.left, m_rect.bottom - m_rect.top);
			m_DrawDC = m_renderBuffer->dc();
		}

	}


	~cpccWindowWin()
	{
		if (m_renderBuffer)
			delete (m_renderBuffer);
		if (m_WindowDC)
			ReleaseDC(m_hWnd, m_WindowDC);
	}


public:		// functions ////////////////////////////////
	HWND 		getNativeWindowHandle(void) { return m_hWnd; }
	// HDC			getDC(void)	{ return m_WindowDC; }

	void 		flush() 
	{ 
		if (!m_renderBuffer)
			return;	// no need to flush, already drawn on the screen

		BitBlt(m_WindowDC, m_rect.left, m_rect.top, m_rect.right-m_rect.left, m_rect.bottom-m_rect.top, 
			   m_DrawDC, 0, 0, SRCCOPY);
	}


	cpccColor	getPixel(const int x, const int y)
	{
		COLORREF c = ::GetPixel(m_DrawDC, x, y);
		cpccColor ret(c);
		return ret;
	}


	void getTextSize(const cpcc_char *txt, int *width, int *height)
	{	
		RECT tmpRect;
		tmpRect.right = tmpRect.left = 0;
		tmpRect.bottom = tmpRect.top =0;

		cpccFontWinGDI useFont(m_DrawDC, fontName.getCurrent().c_str(), fontSize.getCurrent(), fontWeight.getCurrent(), fontQuality.getCurrent());

		int oldSpacing=GetTextCharacterExtra(m_DrawDC);
		SetTextCharacterExtra(m_DrawDC, textSpacing.getCurrent()); // change the spacing of the characters
	
		DrawText(m_DrawDC, txt, -1, &tmpRect, DT_LEFT | DT_NOCLIP | DT_NOPREFIX | DT_CALCRECT);

		// restore the drawing tools
		SetTextCharacterExtra(m_DrawDC,oldSpacing);

		if (width)
			*width = tmpRect.right-tmpRect.left;

		if (height)
			*height = tmpRect.bottom-tmpRect.top;
	}


	int 		getHeight(void) { return m_rect.bottom - m_rect.top; }
	int 		getWidth(void) 	{ return m_rect.right - m_rect.left; }

	
	void		BitBilt_impl(int x, int y, HDC srcDC, int w, int h, cpccColor* transparentColor=NULL)
	{
		// http://www.winprog.org/tutorial/transparency.html
		if (!m_WindowDC)
			return;

		if (!transparentColor)
			::BitBlt(m_DrawDC, x, y, w , h, srcDC, 0,0, SRCCOPY); 
		else
			TransparentBlt(m_DrawDC, x, y, w, h, srcDC, 0, 0, w, h, transparentColor->asColorref());
	}
	
protected:

	// align: -1=left, 0=center, 1=right
	void 		textOut_impl(int x, int y, const cpcc_char *txt) 
	{
		RECT tmpRect;
		tmpRect.left = x;
		tmpRect.top = y;
		tmpRect.right = tmpRect.left;
		tmpRect.bottom = tmpRect.top;


		long alignment=DT_LEFT;
		switch (textAlign.getCurrent())
		{
			case 1: alignment=DT_RIGHT; break;
			case 0: alignment=DT_CENTER; break;
		}

		cpccFontWinGDI useFont(m_DrawDC, fontName.getCurrent().c_str(), fontSize.getCurrent(), fontWeight.getCurrent(), fontQuality.getCurrent());
		SetTextColor(m_DrawDC, drawColor.getCurrent().asColorref());

		int oldSpacing=GetTextCharacterExtra(m_DrawDC);
		SetTextCharacterExtra(m_DrawDC, textSpacing.getCurrent()); // change the spacing of the characters
		// apo starmsg.cpp: DrawText(CompatDC, msg,-1, &GlobalRect,DT_WORDBREAK); // να το κανω rect
		DrawText(m_DrawDC, txt, -1, &tmpRect, alignment | DT_NOCLIP | DT_NOPREFIX);

		// restore the drawing tools
		SetTextCharacterExtra(m_DrawDC,oldSpacing);
	}

	
	void 		fillWithColor_impl(const cpccColor &aColor) 
	{ 
		const COLORREF c = /* (DWORD) */ aColor.asColorref();

		// The brush identified by the hbr parameter may be either a handle to a logical brush or a color value. 
		// If specifying a handle to a logical brush, call CreatePatternBrush, or CreateSolidBrush to obtain the handle.
		// If specifying a color value for the hbr parameter, it must be one of the standard system colors (the value 1 must be added to the chosen color). 
		FillRect(m_DrawDC, &m_rect ,(HBRUSH) CreateSolidBrush(c));
	}
	

};

