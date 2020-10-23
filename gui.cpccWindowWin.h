/*  *****************************************
 *  File:		cpccWindowWin.h
 *  Version:	see function getClassVersion()
 *	Purpose:	Portable (cross-platform), light-weight, library
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2013,2014 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license exists for closed source projects.
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
#include "gui.cpccDrawingToolsWinDC.h"

#pragma comment(lib,"msimg32.lib")	// needed for TransparentBlt()

// 
// https://docs.microsoft.com/en-us/windows/win32/gdi/painting-on-a-dc-that-spans-multiple-displays

/**
	windows version of the C++ portable window class
*/
class cpccWindowWin: public cpccWindowBase
{
private:
	HDC						m_WindowDC,
							m_DrawDC;
	cpccWinGDIMemoryDC*		m_renderBuffer;
	cpccDrawingToolsWinDC	m_dtool;
	enum cpccWindowWin_config	{ config_HasDoubleBuffer=true };

// check this: LockWindowUpdate(hwnd);

protected:		// ctors. 

	/// constructor is protected because this class should not be created alone, but only via its derived class, cpccWindow
	explicit cpccWindowWin(const HWND aWnd): cpccWindowBase(aWnd, m_dtool), m_renderBuffer(NULL), m_dtool(m_DrawDC)
	{
		// m_rect.top = m_rect.bottom = m_rect.left = m_rect.right = 0;
		m_DrawDC = m_WindowDC = GetDC(m_windowHandle);
		SetBkMode(m_WindowDC, TRANSPARENT);
		RECT windRect;
		GetWindowRect(m_windowHandle, &windRect); // in screen coordinates
		const int windWidth = windRect.right - windRect.left;
		const int windHeight = windRect.bottom - windRect.top;
		
		infoLog().addf(_T("cpccWindowWin() with hWND:%X, rect.top = %i, left:%i, bottom:%i, right:%i, width:%i, height:%i"), 
			m_windowHandle, windRect.top, windRect.left, windRect.bottom, windRect.right, windWidth, windHeight);

		if (config_HasDoubleBuffer)
		{
			// the last paramenter, "true" means take a screenshot here of the actual window contents and put them in the buffer
			m_renderBuffer = new cpccWinGDIMemoryDC(m_WindowDC, windWidth, windHeight, true);
			m_DrawDC = m_renderBuffer->dc();
		}
	}


	virtual ~cpccWindowWin()
	{
		if (m_renderBuffer)
			delete (m_renderBuffer);
		if (m_WindowDC)
			ReleaseDC(m_windowHandle, m_WindowDC);
	}

	void lockFocus(void) { } // used in OSX. Here, I might add something like BeginPaint()
	void unlockFocus(void) { } // used in OSX. Here, I might add something like EndPaint()


public:
	
	const HDC		getDrawDC(void) const { return m_DrawDC; }

	virtual void	useDblBuffer(const bool a)	
	{ 
		cpccWindowBase::useDblBuffer(a); 
		#pragma warning(suppress  : 6240)
		m_DrawDC = (a && config_HasDoubleBuffer) ? m_renderBuffer->dc() : m_WindowDC;
	}

	/*
		https://docs.microsoft.com/en-us/windows/win32/gdi/hmonitor-and-the-device-context

		Any function that returns a display device context (DC) normally returns a DC for the primary monitor. 
		To obtain the DC for another monitor, use the EnumDisplayMonitors function. Or, you can use the device name from 
		the GetMonitorInfo function to create a DC with CreateDC. 
		However, if the function, such as GetWindowDC or BeginPaint, gets a DC for a window that spans more than one 
		display, the DC will also span the two displays.
	*/

	/*
	void setOffset(const int offsetX, const int offsetY) override
	{
		m_dtool.m_offsetX = offsetX;
		m_dtool.m_offsetY = offsetY;
	}
	*/

	virtual void        bitBlitFrom(const int x, const int y, const HDC& srcContext, const int srcW, const int srcH, const cpccColor* transparentColor = NULL) override 
	{ 
		m_dtool.bitBlitFrom(x, y, srcContext, srcW, srcH, transparentColor);
	}

protected:		// functions ////////////////////////////////
				// they are protected because no one needs to call the directly. They are called from the base class' methods

	void 		flush(void) 
	{ 
		if ((!m_renderBuffer) || (!m_useDblBuffer))
			return;	// no need to flush, already drawn on the screen

		m_renderBuffer->blitTo(m_WindowDC);
		// BitBlt(m_WindowDC, m_rect.left, m_rect.top, m_rect.right-m_rect.left, m_rect.bottom-m_rect.top, m_DrawDC, 0, 0, SRCCOPY);
	}

	void getTextSize(const cpcc_char *txt, int *width, int *height)
	{	cpccTextParams params;
		params.fontName		= fontName.getCurrent().c_str();
		params.fontSize		= &fontSize.getCurrent();
		params.fontWeight	= fontWeight.getCurrent();
		params.fontQuality	= fontQuality.getCurrent();
		params.kerning		= &kerning.getCurrent();
		
		m_dtool.getTextSize(txt, params, width, height);
	}

	virtual cpccRecti   getBounds(void) const override;
	inline int 		getHeight(void)	const override { return getSize().h; };
	inline int 		getWidth(void) 	const override { return getSize().w; } ;
	virtual cpccSizei   getSize(void) const;
	inline int      getTop(void)	const override { return getTopLeft().y; }
	inline int      getLeft(void)	const override { return getTopLeft().x; }
	inline sPointi  getTopLeft(void)	const override;
	inline void 	fillWithColor(const cpccColor& c); // zero based or desktop window based?
	inline void		fillRectWithColor(const cpccRecti &r, const cpccColor& c)	{ m_dtool.fillRectWithColor(r, c); }
	inline void		fillEllipseWithColor(const int left, const int top, const int right, const int bottom, const cpccColor& c)
					{ m_dtool.fillEllipseWithColor( left, top, right, bottom, c); } 
	
	
protected:  // the xxxxxx_impl() functions. They should be called only from the anscenstor
	
	virtual void 			drawLine(const int x1, const int y1, const int x2, const int y2, const int width, const cpccColor &c)
	{
		{ m_dtool.drawLine(x1, y1, x2, y2, width, c); }
	}
	

	cpccColor	getPixel_impl(const int x, const int y)	const				{ return m_dtool.getPixel(x,y); }
	void		setPixel_impl(const int x, const int y, const cpccColor &c) { m_dtool.setPixel(x,y,c); }
		
	void 		textOut_impl(const int x, const int y, const cpcc_char *txt) 
	{
		cpccTextParams params;
		params.fontName		= fontName.getCurrent().c_str();
		params.fontSize		= &fontSize.getCurrent();
		params.fontWeight	= fontWeight.getCurrent();
		params.fontQuality	= fontQuality.getCurrent();
		params.color		= &drawColor.getCurrent();
		params.kerning		= &kerning.getCurrent();
		params.textAlign	= &textAlign.getCurrent();
		m_dtool.drawText(x, y, txt, params);
	}

	
};

// //////////////////////////////////
//
//		cpccWindowWin implementation
//
// //////////////////////////////////

inline cpccRecti   cpccWindowWin::getBounds(void) const
{
	cpccRecti rect;
	RECT windRect;
	GetWindowRect(m_windowHandle, &windRect); // in screen coordinates
	rect.top = windRect.top;
	rect.left = windRect.left;
	rect.height = windRect.bottom - windRect.top;
	rect.width = windRect.right - windRect.left;;
	return rect;
}


inline cpccSizei   cpccWindowWin::getSize(void) const
{
	cpccSizei size;
	RECT tmpRect;
	GetClientRect(m_windowHandle, &tmpRect);
	// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getclientrect
	// The left and top members are zero. The right and bottom members contain the width and height of the window.
	size.w = tmpRect.right;
	size.h = tmpRect.bottom;
	return size;
}


inline void 	cpccWindowWin::fillWithColor(const cpccColor& c) 
{ 
	RECT tmpRect;
	GetClientRect(m_windowHandle, &tmpRect);
	m_dtool.fillRectWithColor_impl_win(tmpRect, c.asCOLORREF());
}


inline sPointi      cpccWindowWin::getTopLeft(void)	const
{
	sPointi topLeft;
	RECT windRect;
	GetWindowRect(m_windowHandle, &windRect); // in screen coordinates
	topLeft.x = windRect.left;
	topLeft.y = windRect.top;
	return topLeft;
}
